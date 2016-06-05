---
layout: default
---

Introduction
------------

The Amsterdam Compiler Kit is a cross-platform compiler and toolchain
suite that is small, portable, extremely fast, and extremely flexible. It
targets a number of low-end machines including the Z80, 8086 and 80386, but
there are many other code generators available. It supports several
languages, including ANSI C, Pascal and Modula-2, and contains integrated
runtime libraries including a libc.

The toolchain is mainly known as being the default toolchain for [Minix 1 and
2](http://minix1.woodhull.com/); with a bit of effort it gives you a complete
ANSI C toolchain which will run in a 64/64kB address space on an 8086.

The ACK runs on Unix systems; most development happens on Linux.

For more information, see the [About the ACK](about.html) page.

Getting it
----------

Want to use it? See [the Github release page](https://github.com/davidgiven/ack/releases/latest).

Want to develop for it? See [the Github repository](https://github.com/davidgiven/ack).

The release package is pruned and simplified to remove a lot of the non-working
code (so unless you're interested in said code, I would recommend it).
Installation instructions are in the README.

Note that I recommend that if you want to *use* the ACK, you get the source
tarball rather than a Github snapshot --- it's edited and pruned and easier to
work with than a checkout from Git.

Documentation
-------------

Some fairly heavy maintenance is currently being done on the ACK, which means
the documentation is not in very good shape. Most of the documentation is in
the form of man pages, which are supplied with the source; however, a number of
[rather old white papers](olddocs.html) are also available, that may give some
insight as to the design and architecture of the system.

If you have queries, your best bit is probably to [join the mailing
list](http://sourceforge.net/mail/?group_id=130811).

New news
--------

### 2016-06-05

After... far too many... years, I've finally pushed out another release,
6.0pre5.  Work hasn't stopped in the mean time; it's just been sporadic and
unreleased. The big news is that there's a mostly-working VideoCore IV code
generator which will make binaries you can run on the Raspberry Pi GPU, but
there's been lots of other changes.

As always, get it (and the proper release notes) from [the Github release
page](https://github.com/davidgiven/ack/releases/latest).

### 2015-06-18

Because SourceForge has been acting... strange, I've moved everything to Github.
Yes, now you can use git, and SVN too. hg is still supported via the hg-git extension.

tl;dr:

	git clone https://github.com/davidgiven/ack.git
	hg clone git+ssh://git@github.com:davidgiven/ack.git
	svn co https://github.com/davidgiven/ack
    
### 2011-02-11

ACK development (such as it is) has moved from the CVS repository to a new
Mercurial repository. The whole CVS repository has been imported, so the entire
revision history (branches and tags included) is available. I did have to make
a few fixes due to corruption in the CVS repository, but these only affect
files which no longer exist; it is possible that checkouts of very old
revisions may have some broken files. Instructions on how to access the
Mercurial repository can be found on the [Sourceforge project
page](http://sourceforge.net/projects/tack).

The old CVS repository will not be going away and will remain as a reference
but will no longer be modified.

The [very first
checkin](http://tack.hg.sourceforge.net/hgweb/tack/tack/rev/4bea19e501ed) in
the ACK project</a> happened at 13:42 UTC on Thursday, May 17 1984. This is
older than CVS itself.
 
Obsolete news
-------------

Wow, I've been working on this for a while. Everything here is old and obsolete
and completely irrelevant to anything other than historical value. However, as
the main purpose of this project is historical preservation, I feel
uncomfortable about deleting it... enjoy. (I *have* removed broken and obsolete
links.)

### 2010-08-08

I've just uploaded 6.0pre4 to the download page.  This contains some minor
fixes to sort out some bitrot associated with new Linux systems, that was
preventing compilation.

### 2007-04-29

I've just uploaded 6.0pre3 to the download page.
Lots of bugfixes, and we now support compilation for CP/M using the
8080 code generator.

Other things that are new: better optimisation, slightly lighter-weight
binaries, better ANSI C headers, and floating point works on the pc86 and
linux386 platforms (although only if you have an FPU).

### 2007-04-25

I'm pleased to announce 6.0pre2 has just hit the download page.
i386 Linux binaries now have limited support.

This version has some substantial internal changes to simplify things all
round. Syscalls are no longer routed through libmon and are considerably
simpler and more lightweight to implement and use. The K&R C compiler has
been removed, as it does nothing that the ANSI C compiler doesn't do better.
The built-in libc has been heavily ANSIfied and trimmed back to remove a lot
of the ancient Unixisms.

The distribution size is now under a megabyte.

### 2007-02-25

Finally, *finally*, after several years work, we finally have an
initial release of the ACK 6.0!

This is a preview release, and is extremely limited. It supports on
platform, which will generate PC bootable floppy disk images containing 8086
machine code. However, it does support ANSI C, K&R C, Pascal, Modula-2,
Basic and Occam, and should form a good basis for evaluation and further
development. Let me know what you think.

This is known to build cleanly on Ubuntu Edgy Linux and OpenBSD 4.0, both
on i386. However, it hasn't had a lot of testing otherwise; Sourceforge have
shut down their compile farm service, and I don't have access to many
machines. Bug reports are extremely welcome.

(Note that the ACK 5.6, using the old build mechanism, still remains
available for those that wish to use it; it's in the 6.0pre1 download area as
a previous version.)

### 2006-02-04

LLgen has just been released as a seperate package!

LLgen is a LL(1) parser generator, quite similar to yacc or bison, that
can generate recursive descent parsers from Extended Context-Free grammars
(which makes it quite a bit more useful than yacc or bison). The ACK uses it
extensively, but as it's a standalone component, I've decided that it would
be useful to distribute this separately. LLgen's input files are almost
identical to yacc's, so if you can use yacc and have been getting frustrated
with its limitations, LLgen is for you.

I've rewritten the build system and overhauled the source so it compiles
cleanly with gcc; it should work fine on modern systems (and extremely
quickly). The package contains full documentation, and LLgen is, like the
ACK, licensed under the new-style BSD license.

### 2005-06-25

We make our first release!

Version 5.6 of the ACK has now hit Sourceforge and is ready to download.
This version has been tweaked and fiddled with until it
compiles, very nearly cleanly, on Linux systems (all that were available for
testing): it should work without too much hassle on most other platforms.
This release contains full documentation, all the front ends, all the back
ends, all the libraries, and all the binary conversion tools, and should
actually be useful. Don't forget to <a
href="http://sourceforge.net/mail/?group_id=130811">join the mailing list</a>
if you want to use it!

### 2005-06-10

CVS repository goes live!

After great efforts by Ceriel Jacobs, to which I am extremely grateful, we
now have a complete copy of the original development repository. The original
distribution mechanism doesn't quite work on modern machines, but I'm working
on it, and in the meantime it makes fascinating browsing --- I believe we may
now have some of the oldest timestamps on SourceForge. (Several of the files
were last touched in 1985!)

### 2005-02-14

Initial version of this web page set up.

### 2005-02-11

Converted the troff documentation into (dodgy) HTML and PDF files; see the
documentation section for the list. The man pages haven't been converted yet,
but at least the main documentation is available.

