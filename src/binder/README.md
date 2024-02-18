# Binder
The binder is a simple step in the compiler process. It has a simple job - Making sure everything exist when you
try to use it(so you don't call an invalid function).

Here's the step-by-step process:
- Build an object tree(class/struct/variable definitions etc.)
- Validate the object tree(no classes in structs for example)
- Validate statement blocks(type checking, access modifiers)