---
layout: default
---

What is it?
-----------

The Amsterdam Compiler Kit is a venerable piece of software that dates
back to the early 1980s. It was originally written by <a
href="http://www.cs.vu.nl/%7East">Andrew Tanenbaum</a> and <a
href="http://www.cs.vu.nl/%7Eceriel">Ceriel Jacobs</a> as a commercial
product; for many years it was also used as Minix' native toolchain. After
eventually failing as a commercial project, it was made open source under a
BSD license in 2003 when it looked like it was going to be abandoned and the
code lost.

The ACK contains compilers for ANSI C, K&R C, Pascal, Modula-2, Occam
1, and a primitive Basic. It contains code generators for a large number of
architectures, mostly 8 and 16 bit machines; there are also a set of generic
optimisation, linker and librarian tools. Each language comes with its own
runtime, so if you're a C programmer you also get a libc. Compared to gcc, it
is far smaller, faster and easier to port.

This project currently hosts two versions of the ACK.

  - ACK 5.6 is an incremental update of Vrije Universiteit's last release.
    This has had minimum fixes necessary to make it run on modern Linux
    machines; unfortunately, the build system is designed for the Unixes of
    yesteryear and is not really up to modern standards. This version is the
    most complete, but is probably not particularly useful as very few of the
    platforms it targets exist any more. This version is provided for
    reference and archival purposes and to provide a backup if the 6.0
    release should fail.

  - ACK 6.0 is a ground-up reworking of the whole compiler suite, with a
    lot of the more archaic features removed. It is intended to produce a
    modern compiler toolchain with which actually useful work can be done,
    and is where all the new development happens. Currently, not all of 5.6's
    features have been ported, but those that have work considerably more
    reliably.

What architectures does it support?
-----------------------------------

It contains assembler and linker support for: 6500, 6800, 6805, 6809, ARM, i80,
Z80, Z8000, i86, i386, 68000, 68020, NS32016, S2650, SPARC, VAX, PDP11 and
VideoCore IV.

It contains code generator support for: 6500, ARM, i80, Z80, Z8000, i86, i386,
68000, 68020, NS32016, SPARC, VAX4, PDP11 and VideoCore IV.

It supports these platforms: pc86, linux386, linux68k, linuxppc, linuxmips,
cpm, rpi (VideoCore IV), pdpv7, msdos86 and msdos386.

However, that doesn't necessarily mean that the code is *working*. There is a
lot of half-finished code in the repository. There is *tested* support for:
i86, i386, 68000, i80, and VideoCore IV.

If you're interested in one of the other architectures, please ask on [the
mailing list](http://sourceforge.net/mail/?group_id=130811). Some code
archaeology will be required to enable a new target.

What's the generated code quality like?
---------------------------------------

Honestly? Not great.

The ACK was designed in an era when processor were CISC memory-centric
architectures with very small numbers of registers. For these architectures, it
produces middling good results, although modern compilers do far better (at the
expense of being enormously larger and more complex).

However, for register-centric architectures, it's pretty much a dead loss. The
register allocator cannot make effective use of large numbers of registers and
the underlying architecture requires nearly all operations to touch memory. It
is very easy to port to a new architecture... but the generated code is
terrible.

I, dtrg, cannot honestly recommend using the ACK for production code on modern
architectures unless as a stop-gap measure or unless the other benefits of the
ACK (e.g. having a very lightweight turnkey toolchain is valuable to you)
outweigh the code quality.

Haven't I seen this before somewhere?
-------------------------------------

Quite likely. The ACK has been used as the standard Minix compiler for
years. While the ACK was still commercial, this was done by distributing
binaries; when it get opened, a version was forked off and is now used as
part of the Minix base build. You can find <a
href="https://gforge.cs.vu.nl/plugins/scmsvn/viewcvs.php/trunk/bigports/ackpack/?root=minix">Minix's
version here</a>. This is an extremely stripped down variant that supports
only the Minix platform on the i386 and i86 architectures and was done by <a
href="http://www.laurasia.com.au/ack">Michael Kennett</a>.

In addition, the original 5.5 release is still available on <a
href="http://www.cs.vu.nl/ack/">the Vrije Universiteit ACK page</a>.

There may also be other versions elsewhere. The ACK is BSD licensed and as
a result if people want to fork the codebase and use it elsewhere, they don't
even need to ask, or indeed tell anyone. If you know of any other uses of the
compiler, please let me (dtrg) know --- I'd like to put in a link.

What's the involvement of Andrew Tanenbaum, Ceriel Jacobs and Vrije Universiteit?
---------------------------------------------------------------------------------

They have no official involvement.

They're aware that I, dtrg, am doing this, and are quite happy with it and
maintain an interest, but are not actively participating in the project. (Due
to being busy people with other things to do.) I, dtrg, have nothing to do
with Vrije Universiteit and have never even been to the Netherlands.

