//
//  swiftdizzle.h
//  swiftdizzle
//
//  Created by Derek Selander on 2/10/18.
//  Copyright © 2018 Razeware. All rights reserved.
//


#import <objc/runtime.h>

BOOL rebind_swiftClass(const char *replacementClassName, const char *replacementMethod, const char *className, const char *method, BOOL shouldReplaceBoth);
