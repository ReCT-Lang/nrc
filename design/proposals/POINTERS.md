# Pointers
All right! Pointers, pointers, pointers...

How should we deal with pointers?

Well, it ain't that hard, we can stay with the classical old rect-style `int pointer myVariable`(or however variabled worked), or we could do c-style `int* myVariable`, and I don't quite know which one to do this with.

Either way, pointers should only work on structs(see my struct proposals for how *this* works), just to avoid any memory management issues, and we should by default, language-wise only allow scoped pointers, which in short means that ***YOU CANNOT STORE THEM***, that's a job for something like arrays. This simply avoids all the headaches we get with trying to memory-manage a language with pointers.

If the user wants to get memory unsafe and whatnot, they're free to link to the C standard library(somehow) and just use malloc and free, but then they're knowingly voiding the memory-safe warranty, and they can no longer complain to us if their program leaks(their garanty is also voided, of course, no replacements once C gets mixed in).

In short, let's copy C#. It's just better that way.

Hold on, just realized we need to somehow allow storage of pointers... Please get in touch with me(khhs) if you've got any ideas.
