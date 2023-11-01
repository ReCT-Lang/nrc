# ReCT Design
The ReCT language has always had a bit of an identity struggle, so here's my(Jimmy's/khhs') inital proposal. Expect a bit of a ramble :)

I personally really like how ReCT works as a basis, the low-boilerplate classes and overall python-esqueness of it made it a treat.
Then rgoc came around and started dabbling in pointers and that made me happy too, but it started to struggle a bit in how it was made, as it tried
to be too much at once.

So, here's my main things that I'd like in the language(features, if you will):
- Functions - duh, we all know how ReCT works, it handles functions really well.
- Classes - ReCT is OOP-ish. I'd like classes to be reference types(i.e, nullable) and GC/ARC'd, basically, keep 'em memory safe.
- Structs - Welcome new addition, idea comes from C#, basically, they're classes but passed by value and thus don't need GC'd. They also allow
    more unsafe operations.
- External Functions - In order to work together with anything else, we need to be able to import stuff, this can be done with something like an "extern" keyword, just
    disable classes and work alongside the C naming stuff and we'll be fine(basically, runtime dlopen)
- A clear distinction of the set keyword - Always bothered me, you never knew what you'd be getting when "set" was used. Let's add some clear rules and new keyboard where needed.
- Clear arrays - What's wrong with C-style arrays? Can't I just `make` an `int[5]`, does it NEED to be `make array int(6)`, it just adds unnecessary clutter.
- Generics - Speaking of which, generic would be cool-ish. They're a mess to work with unless you have a high-level runtime, but it should hopefully be doable, see my proposal for how.
- A good build system - The way ReCT did builds and everything else *worked* but not much more. We need to settle for either JS or C# style builds. No arcaic C solution(linkers be damned)
- Unsafe functionality - Pointers are what make me happy when I work with C. If I don't need pointers, I don't do C, otherwise I do C. It's that simple. Pointers could either be forcefully hacked into the standard library(honestly, I don't mind as long as we get operator overloads to make 'em seamless), or we could add complete language-wide support, either works for me. Should be a proposal for it.

Please reach out to me(if you're reading this, you're prolly in the ~~ByteSpace~~ReCT Discord, and I am too) with any ideas, this document is expected to grow over time.

The more individual proposals are in their own markdown files
