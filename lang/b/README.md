A B Compiler
============

abc is a compiler for the [B Programming Language](http://en.wikipedia.org/wiki/B_(programming_language)) that targets x86\_32 processors. It is currently tested under Linux but should work (or at least be easily ported) to other UNIX-like systems. The code is based on [an early C compiler (last1120c)](http://www.cs.bell-labs.com/who/dmr/primevalC.html) by Dennis Ritchie.

Documentation
-------------

* [The Programming Language B](http://9p.io/cm/cs/who/dmr/bintro.html)

* [B Reference by Ken Thompson](http://9p.io/cm/cs/who/dmr/kbman.html) describes a presumably earlier variant of B, which is slightly different from the one described above. The compiler cannot understand it, but I plan to implement a compatibility mode (the differences are minor).

Implementation
--------------

Since B was first implemented for machines with word addressing, some hacking was required to make it work on the byte addressed x86. Addresses filled in by the linker are always byte addresses, so pointers to these addresses are collectively stored at the end of the .data section and are then converted to word addresses at runtime, before main() is called.

The generated assembly is *very* inefficient, not even constant expressions are reduced at compile time. Also I/O is currently not buffered.

How to use
----------

The installation requires a little configuration:
'abc' is a frontend for the actual compiler which feels somewhat like gcc (it also handles assembling and linking). Before you can use it, set it's BDIR variable to the directory of the B compiler.
In the Makefile, change the directory of the 'install' rule to wherever you want your 'abc' file to reside.
Then type

	make install libs

which compiles the compiler 'b', installs the 'abc' frontend and compiles the B runtime and library (brt.o and lib.o).

To compile and link a B program, simply type

	abc -o outfile file1.b [file2.b ...]

If you want to compile and assemble only:

	abc -c file1.b [file2.b ...]

or generate only the assembly:

	abc -S file1.b [file2.b ...]

Examples of B programs are in the 'examples' directory, they are mostly from Brian Kernighan's tutorial.

Bugs
----

Since command line parameters aren't passed word-aligned, B can't handle them easily. brt.s copies the strings to another location and aligns them, the space is not dynamically allocated however and only 256 bytes are available by default.

The library is incomplete but has some of the most important functions.

I have only tested the compiler on an x86\_64 gentoo system.
