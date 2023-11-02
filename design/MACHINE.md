# Virtual Machine Design Proposals

So, first off, let's list the requirements for the VM:
- Somewhat low-level - We don't want a x86 emulator, but we don't want the JVM, I'd say it should be possible to strike a nice balance.
- Simple - No supermassive stuff, just a buncha basic instructions like JMP, CMP and whatever else one would need
- Stack Based? - Stack based VMs are easy to write, and should be possible to make work, but registers etc would also be really handy.
- Capable of loading C - We'd like to open up C standard libraries and what have you. The machine doesn't need to provide much functionality if we do that.
- At least 32-bit - 16 or 8 bit machines are cumbersome and not really useful, I'd say we try to roll 32 or 64 bit stuff.
- Green Threads - Multithreading is really cool, and if we could get that running it'd be nice. To keep cross-platform easy, green threads are best(we do thread stuff in the VM, thus only one CPU core), now multiple cores are also useful, but that's for later.
- Cross-platform - Duh...

I don't have any ideas for an instruction set as of right now, but hopefully I'll be bringing some in soon-ish:tm:
