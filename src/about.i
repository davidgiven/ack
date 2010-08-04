<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  <title>About the ACK</title>
</head>

<body>
<h1>What is it?</h1>

<p>The Amsterdam Compiler Kit is a venerable piece of software that dates
back to the early 1980s. It was originally written by <a
href="http://www.cs.vu.nl/%7East">Andrew Tanenbaum</a> and <a
href="http://www.cs.vu.nl/%7Eceriel">Ceriel Jacobs</a> as a commercial
product; for many years it was also used as Minix' native toolchain. After
eventually failing as a commercial project, it was made open source under a
BSD license in 2003 when it looked like it was going to be abandoned and the
code lost.</p>

<p>The ACK contains compilers for ANSI C, K&amp;R C, Pascal, Modula-2, Occam
1, and a primitive Basic. It contains code generators for a large number of
architectures, mostly 8 and 16 bit machines; there are also a set of generic
optimisation, linker and librarian tools. Each language comes with its own
runtime, so if you're a C programmer you also get a libc. Compared to gcc, it
is far smaller, faster and easier to port.</p>

<p>This project currently hosts two versions of the ACK.</p>
<ul>
  <li>ACK 5.6 is an incremental update of Vrije Universiteit's last release.
    This has had minimum fixes necessary to make it run on modern Linux
    machines; unfortunately, the build system is designed for the Unixes of
    yesteryear and is not really up to modern standards. This version is the
    most complete, but is probably not particularly useful as very few of the
    platforms it targets exist any more. This version is provided for
    reference and archival purposes and to provide a backup if the 6.0
    release should fail.</li>
  <li>ACK 6.0 is a ground-up reworking of the whole compiler suite, with a
    lot of the more archaic features removed. It is intended to produce a
    modern compiler toolchain with which actually useful work can be done,
    and is where all the new development happens. Currently, not all of 5.6's
    features have been ported, but those that have work considerably more
    reliably.</li>
</ul>

<h1>What architectures does it support?</h1>

<p>The following architectures are supported.</p>

<table border="0">
  <tbody>
    <tr>
      <th></th>
      <th>5.6</th>
      <th>6.0</th>
    </tr>
    <tr>
      <td>6500</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>6800</td>
      <td>-</td>
      <td></td>
    </tr>
    <tr>
      <td>6805</td>
      <td>-</td>
      <td></td>
    </tr>
    <tr>
      <td>6809</td>
      <td>-</td>
      <td></td>
    </tr>
    <tr>
      <td>ARM</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>i80</td>
      <td>+</td>
      <td>+</td>
    </tr>
    <tr>
      <td>Z80</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>Z8000</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>i86</td>
      <td>+</td>
      <td>+F</td>
    </tr>
    <tr>
      <td>i386</td>
      <td>+</td>
      <td>+F</td>
    </tr>
    <tr>
      <td>68000</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>68020</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>NS32016</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>s2650</td>
      <td>-</td>
      <td></td>
    </tr>
    <tr>
      <td>SPARC</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>VAX4</td>
      <td>+</td>
      <td></td>
    </tr>
    <tr>
      <td>PDP11</td>
      <td>+</td>
      <td></td>
    </tr>
  </tbody>
</table>

<p>+ indicates that the particular architecture is supported by the code
generator and assembler; - indicates that it is supported by the assembler
only.</p>

<p>F indicates that the architecture has floating point support (if there is
an FPU).</p>

<p>Note: all the code generators <em>run</em> on 6.0, but as there are no
platforms yet that use them they are not currently shipped. If you wish to
use one, please ask and it can very easily be arranged. (Due to the way the
ACK works, it is not really possible to produce code targeted at a particular
architecture without also targeting it at a platform.)</p>

<h1>What platforms does it support?</h1>

<p>For 5.6:</p>

<table border="0">
  <tbody>
    <tr>
      <td>6500</td>
      <td>BBC Microcomputer</td>
    </tr>
    <tr>
      <td>ARM</td>
      <td>Acorn Archimedes</td>
    </tr>
    <tr>
      <td>i80</td>
      <td>Hermac, Nascom</td>
    </tr>
    <tr>
      <td>Z80</td>
      <td>Hermac, Nascom, limited CP/M</td>
    </tr>
    <tr>
      <td>Z8000</td>
      <td>(board)</td>
    </tr>
    <tr>
      <td>i86</td>
      <td>PC/IX</td>
    </tr>
    <tr>
      <td>i386</td>
      <td>Xenix v3, Xenix SysV</td>
    </tr>
    <tr>
      <td>68000</td>
      <td>16 bit: MinixST; 32-bit: SunOS, Mantra, PMDS</td>
    </tr>
    <tr>
      <td>68020</td>
      <td>SysV</td>
    </tr>
    <tr>
      <td>NS32016</td>
      <td>(board)</td>
    </tr>
    <tr>
      <td>SPARC</td>
      <td>Solaris, SunOS</td>
    </tr>
    <tr>
      <td>VAX4</td>
      <td>BSD4.2</td>
    </tr>
    <tr>
      <td>PDP11</td>
      <td>UNIX v7</td>
    </tr>
  </tbody>
</table>

<p>"(board)" signifies that the port was done to a bare development board
with no operating system other than a monitor.</p>

<p>For 6.0:</p>

<table border="0">
  <tbody>
    <tr>
      <td>i80</td>
      <td>cpm (CP/M 8080 executables)</td>
    </tr>
    <tr>
      <td>i86</td>
      <td>pc86 (bootable floppy disk images)</td>
    </tr>
    <tr>
      <td>i386</td>
      <td>linux386 (ELF Linux executables)</td>
    </tr>
  </tbody>
</table>

<h1>Haven't I seen this before somewhere?</h1>

<p>Quite likely. The ACK has been used as the standard Minix compiler for
years. While the ACK was still commercial, this was done by distributing
binaries; when it get opened, a version was forked off and is now used as
part of the Minix base build. You can find <a
href="https://gforge.cs.vu.nl/plugins/scmsvn/viewcvs.php/trunk/bigports/ackpack/?root=minix">Minix's
version here</a>. This is an extremely stripped down variant that supports
only the Minix platform on the i386 and i86 architectures and was done by <a
href="http://www.laurasia.com.au/ack">Michael Kennett</a>.</p>

<p>In addition, the original 5.5 release is still available on <a
href="http://www.cs.vu.nl/ack/">the Vrije Universiteit ACK page</a>.</p>

<p>There may also be other versions elsewhere. The ACK is BSD licensed and as
a result if people want to fork the codebase and use it elsewhere, they don't
even need to ask, or indeed tell anyone. If you know of any other uses of the
compiler, please let me (dtrg) know --- I'd like to put in a link.</p>

<h1>What's the involvement of Andrew Tanenbaum, Ceriel Jacobs and Vrije
Universiteit?</h1>

<p>They have no official involvement.</p>

<p>They're aware that I, dtrg, am doing this, and are quite happy with it and
maintain an interest, but are not actively participating in the project. (Due
to being busy people with other things to do.) I, dtrg, have nothing to do
with Vrije Universiteit and have never even been to Holland.</p>
</body>
</html>
