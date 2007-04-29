<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  <title>The Amsterdam Compiler Kit</title>
</head>

<body>
<h1>Introduction</h1>

<p>The Amsterdam Compiler Kit is a cross-platform compiler and toolchain
suite that is small, portable, extremely fast, and extremely flexible. It
targets a number of low-end machines including the Z80, 8086 and 80386, but
there are many other code generators available. It supports several
languages, including ANSI C, Pascal and Modula-2, and contains integrated
runtime libraries including a libc.</p>

<p>The ACK runs on Unix systems; most development happens on Linux.</p>

<p>For more information, see the <a href="about.html">About the ACK</a>
page.</p>

<h1>Getting it</h1>

<p>You can download any release of the ACK from the <a
href="http://sourceforge.net/project/showfiles.php?group_id=130811">Sourceforge
download site</a>. Installation instructions are provided in the source
package.</p>

<h1>Documentation</h1>

<p>Some fairly heavy maintenance is currently being done on the ACK, which
means the documentation is not in very good shape. Most of the documentation
is in the form of man pages, which are supplied with the source; however, a
number of <a href="olddocs.html">rather old white papers</a> are also
available, that may give some insight as to the design and architecture of
the system.</p>

<p>If you have queries, your best bit is probably to <a
href="http://sourceforge.net/mail/?group_id=130811">join the mailing
list</a>.</p>

<h1>News</h1>

<h2>2007-04-29</h2>

<p>I've just uploaded 6.0pre3 to the <a
href="http://sourceforge.net/project/showfiles.php?group_id=130811">download
page</a>. Lots of bugfixes, and we now support compilation for CP/M using the
8080 code generator.</p>

<p>Other things that are new: better optimisation, slightly lighter-weight
binaries, better ANSI C headers, and floating point works on the pc86 and
linux386 platforms (although only if you have an FPU).</p>

<h2>2007-04-25</h2>

<p>I'm pleased to announce 6.0pre2 has just hit the <a
href="http://sourceforge.net/project/showfiles.php?group_id=130811">download
page</a>. i386 Linux binaries now have limited support.</p>

<p>This version has some substantial internal changes to simplify things all
round. Syscalls are no longer routed through libmon and are considerably
simpler and more lightweight to implement and use. The K&amp;R C compiler has
been removed, as it does nothing that the ANSI C compiler doesn't do better.
The built-in libc has been heavily ANSIfied and trimmed back to remove a lot
of the ancient Unixisms.</p>

<p>The distribution size is now under a megabyte.</p>

<h2>2007-02-25</h2>

<p>Finally, <i>finally</i>, after several years work, we finally have an
initial release of the ACK 6.0! </p>

<p>This is a preview release, and is extremely limited. It supports on
platform, which will generate PC bootable floppy disk images containing 8086
machine code. However, it does support ANSI C, K&amp;R C, Pascal, Modula-2,
Basic and Occam, and should form a good basis for evaluation and further
development. Let me know what you think. </p>

<p>This is known to build cleanly on Ubuntu Edgy Linux and OpenBSD 4.0, both
on i386. However, it hasn't had a lot of testing otherwise; Sourceforge have
shut down their compile farm service, and I don't have access to many
machines. Bug reports are extremely welcome. </p>

<p>You can get it <a
href="http://sourceforge.net/project/showfiles.php?group_id=130811">get it
from the Sourceforge download page</a>. </p>

<p>(Note that the ACK 5.6, using the old build mechanism, still remains
available for those that wish to use it; it's in the 6.0pre1 download area as
a previous version.) </p>

<h2>2006-02-04</h2>

<p>LLgen has just been released as a seperate package!</p>

<p>LLgen is a LL(1) parser generator, quite similar to yacc or bison, that
can generate recursive descent parsers from Extended Context-Free grammars
(which makes it quite a bit more useful than yacc or bison). The ACK uses it
extensively, but as it's a standalone component, I've decided that it would
be useful to distribute this separately. LLgen's input files are almost
identical to yacc's, so if you can use yacc and have been getting frustrated
with its limitations, LLgen is for you.</p>

<p>I've rewritten the build system and overhauled the source so it compiles
cleanly with gcc; it should work fine on modern systems (and extremely
quickly). The package contains full documentation, and LLgen is, like the
ACK, licensed under the new-style BSD license.</p>

<p>You can <a
href="http://sourceforge.net/project/showfiles.php?group_id=130811">get it
from the Sourceforge download page</a> right next to the ACK.</p>

<h2>2005-06-25</h2>

<p>We make our first release!</p>

<p>Version 5.6 of the ACK has now hit Sourceforge and is <a
href="http://sourceforge.net/project/showfiles.php?group_id=130811">ready to
download</a>. This version has been tweaked and fiddled with until it
compiles, very nearly cleanly, on Linux systems (all that were available for
testing): it should work without too much hassle on most other platforms.
This release contains full documentation, all the front ends, all the back
ends, all the libraries, and all the binary conversion tools, and should
actually be useful. Don't forget to <a
href="http://sourceforge.net/mail/?group_id=130811">join the mailing list</a>
if you want to use it! </p>

<h2>2005-06-10</h2>

<p>CVS repository goes live!</p>

<p>After great efforts by Ceriel Jacobs, to which I am extremely grateful, we
now have a complete copy of the original development repository. The original
distribution mechanism doesn't quite work on modern machines, but I'm working
on it, and in the meantime it makes fascinating browsing --- I believe we may
now have some of the oldest timestamps on SourceForge. (Several of the files
were last touched in 1985!)</p>

<p>For casual perusal it's probably simplest <a
href="http://cvs.sourceforge.net/viewcvs.py/tack/Ack/">to use CVSWeb</a>.
Alternatively, check out the <tt>Ack</tt> module --- full instructions on the
<a href="http://sourceforge.net/cvs/?group_id=130811">project CVS page</a>.
</p>

<h2>2005-02-14</h2>

<p>Initial version of this web page set up.</p>

<h2>2005-02-11</h2>

<p>Converted the troff documentation into (dodgy) HTML and PDF files; see the
documentation section for the list. The man pages haven't been converted yet,
but at least the main documentation is available. </p>
</body>
</html>
