# Generics

Generics are really nice on some ends, and a pain in others. Luckily for us, we can use a little something known as copying someone else's homework, and do it all Java-style.

Basically, the JVM doesn't know jack-shit about what a generic is, that's all handled compile-time. Once you compile the program, it's all just `objects` being cast back and forth, which works quite well, with only some overhead.

We can even one-up Java and add support for better type-constraints and thus allow stuff like `make T();`(which you can't do in Java). We can also one-up C# if we really dare to.

I really need to expand onto this, but you should hopefully ge the gist. Feel free to contact me(as always)
