# swiftdizzle
Swift method swizzling/method introspection for Swift instance methods

Why should Objective-C's method Swizzling or C's DYLD interpose, or C's lazy stubs `__DATA.__la_symbol_ptr` get all the introspection fun? **swiftdizzle** is a nice little C function that can introspect pure Swift functions at runtime.


![yoink example](https://github.com/DerekSelander/swiftdizzle/raw/master/media/ex.png)


In the above example, take note that `viewDidLoad()` is calling `someFunction()`, but `omagerdFunction()` is called instead. The potential infinite recursion of `omagerdFunction()` calling itself will actually call `someFunction()` instead.

## How to use swiftdizzle

TODO

## How does swiftdizzle work?

TODO
