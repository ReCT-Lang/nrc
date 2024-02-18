# Binder
The binder is a simple step in the compiler process. It has a simple job - Making sure everything exist when you
try to use it(so you don't call an invalid function).

It is also possible to do some basic type checking, but we won't for now.

The first step is to build a tree of all objects & their access modifiers etc