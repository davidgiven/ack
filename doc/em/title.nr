.LP
\&
.sp 10
.ce 4
DESCRIPTION OF A MACHINE
ARCHITECTURE FOR  USE WITH
BLOCK  STRUCTURED  LANGUAGES
.sp 6
.ce 4
Andrew S. Tanenbaum
Hans  van  Staveren
Ed G. Keizer
Johan  W. Stevenson\v'-0.5m'*\v'0.5m'
.sp 2
.ce
August 1983
.sp 2
.ce
Informatica Rapport IR-81
.sp 13
Abstract
.sp 2
.ti +5
EM is a family of intermediate languages
designed for producing portable compilers.
A program called \fBfront end\fP
translates source programs to EM.
Another program, \fBback end\fP,
translates EM to the assembly language of the target machine.
Alternatively, the EM program can be assembled to a highly
efficient binary format for interpretation.
This document describes the EM languages in detail.
.sp 4
\v'-0.5m'*\v'0.5m' Present affiliation: NV Philips, Eindhoven
