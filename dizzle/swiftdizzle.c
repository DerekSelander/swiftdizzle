//
//  swiftdizzle.c
//  swiftdizzle
//
//  Created by Derek Selander on 2/10/18.
//  Copyright © 2018 Razeware. All rights reserved.
//

#import "swiftdizzle.h"
#import <stdio.h>
#import <dlfcn.h>
#import <unistd.h>
#import <string.h>

#if __LP64__
  #define FAST_DATA_MASK          0x00007ffffffffff8UL
#else 
  #define FAST_DATA_MASK        0xfffffffcUL
#endif

#define FAST_IS_SWIFT           (1UL<<0)

typedef struct class_ro_t {
  uint32_t flags;
  uint32_t instanceStart;
  uint32_t instanceSize;
#ifdef __LP64__
  uint32_t reserved;
#endif
  
  const uint8_t * ivarLayout;
  
  const char * name;
  void * baseMethodList; // method_list_t
  void * baseProtocols;  //protocol_list_t
  void * ivars; //ivar_list_t
  uint8_t * weakIvarLayout; // weakIvarLayout
  void *baseProperties; // property_list_t
  
} class_ro_t;

typedef struct class_rw_t {
  uint32_t flags;
  uint32_t version;
  
  const class_ro_t *ro;
  
  void* methods;        // redefined from method_array_t
  void* properties;   // redefined from property_array_t
  void* protocols;    // redefined from protocol_array_t
  
  struct dsobjc_class*   firstSubclass;
  struct dsobjc_class* nextSiblingClass;
  
  char *demangledName;
  
} class_rw_t;

typedef struct dsswift_class {
  struct dsobjc_class *isa;
  struct dsobjc_class *superclass;
  void *_buckets;
  void *maskAndOccupied;
  uintptr_t bits;
  uint32_t flags;
  uint32_t instanceAddressPoint;
  uint32_t instanceSize;
  uint16_t instanceAlignMask;
  uint16_t runtimeReservedBits;
  uint32_t classSize;
  uint32_t classAddressPoint;
  uintptr_t *typeDescriptor;
  uintptr_t *ivarDestroyer;
  uintptr_t *methods;
} dsswift_class;

BOOL rebind_swiftClass(const char *replacementClassName, const char *replacementMethod, const char *className, const char *method, BOOL shouldReplaceBoth) {
  Class cls = objc_getClass(className);
  if (!cls) {
    dprintf(STDERR_FILENO, "Couldn't find class for \"%s\"\n", className);
    return YES;
  }
  
  Class repl_cls = objc_getClass(replacementClassName);
  if (!repl_cls) {
    dprintf(STDERR_FILENO, "Couldn't find replacement class for \"%s\"\n", replacementClassName);
    return YES;
  }
  
  dsswift_class *swiftcls =  (dsswift_class*)cls;
  if (!(swiftcls->bits & FAST_IS_SWIFT)) {
    dprintf(STDERR_FILENO, "Class \"%s\" is not a swift class\n", className);
    return YES;
  }
  
  dsswift_class *repl_swiftcls =  (dsswift_class*)repl_cls;
  if (!(swiftcls->bits & FAST_IS_SWIFT)) {
    dprintf(STDERR_FILENO, "replacement class \"%s\" is not a swift class\n", replacementClassName);
    return YES;
  }
  
  
  uintptr_t* (^getAddressFromClass)(dsswift_class*, const char*) = ^uintptr_t*(dsswift_class* c, const char *method ){
    
    unsigned long methodsAddress = (unsigned long)&c->methods;
    unsigned long endAddress = (unsigned long)c + c->classSize - c->classAddressPoint;
    size_t size = (endAddress - methodsAddress) / sizeof(uintptr_t*);
    
    for (int i = 0; i < size; i++) {
      Dl_info info = {};
      uintptr_t * ptr = (uintptr_t*)methodsAddress;
      dladdr((void *)ptr[i], &info);
      if (info.dli_sname == NULL) {
        continue;
      }
      if (strcmp(info.dli_sname, method) == 0) {
        return &ptr[i];
      }
    }
    return NULL;
  };
  
  uintptr_t *repl_method_addr =  getAddressFromClass(swiftcls, replacementMethod);
  if (!repl_method_addr) {
    dprintf(STDERR_FILENO, "Couldn't find replacement method \"%s\" for class \"%s\"\n", replacementMethod, replacementClassName);
    return YES;
  }
  
  uintptr_t *method_addr =  getAddressFromClass(repl_swiftcls,  method);
  if (!method_addr) {
    dprintf(STDERR_FILENO, "Couldn't find method \"%s\" for class \"%s\"\n", method, className);
    return YES;
  }
  

  if (repl_method_addr && method_addr) {
    uintptr_t tmp = *method_addr;
    *method_addr = *repl_method_addr;
    
    if (shouldReplaceBoth) {
      *repl_method_addr = tmp;
    }
  }

  
  return NO;
  
}
