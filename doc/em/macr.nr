.LP
.if n \{\
.nr LL 78
.ll 78 \}
.tr ~ 
.\" below are three simple macros to get the drawings right
.\" added by Dick Grune
.de Dr				\" Drawing $1 (size)
.sp 1
.ne \\$1
.na
.ft CW				\" constant spacing
.lg 0				\" no ligatures
..
.de Df				\" Drawing Footer
.br
.sp 1
.ft R
.ce 1000
.lg 1
..
.de De				\" Drawing End $1 (lines)
.br
.ft R
.lg 1
.ce 0
.ad
.sp \\$1
..
.\" macro for exponents, added by Ceriel Jacobs
.de Ex				\" Exponent $1 $2 [$3]
\\$1\v'-0.5m'\s-2\\$2\s+2\v'0.5m'\\$3
..
.\" QQ is like PP, but without space
.				\" use .PP, with PD 0.
.de QQ
.nr xx \\n(PD
.nr PD 0
.PP
.nr PD \\n(xx
..
.nr N1 0
.nr N2 0
.nr N3 0
.nr N4 0
.nr N5 0
.nr A5 0
.af A5 A
.de P1
.nr N2 0
.nr N1 \\n(N1+1
.ds Tl "\\n(N1. \\$1
.Ca 0
.sp
.LP
\\fB\\n(N1.  \\$1\\fP
.sp
..
.de P2
.nr N3 0
.nr N2 \\n(N2+1
.ds Tl "\\n(N1.\\n(N2 \\$1
.ne 5
.Ca 2
.sp
.LP
\\fB\\n(N1.\\n(N2  \\$1\fP
..
.de P3
.nr N4 0
.nr N3 \\n(N3+1
.ds Tl "\\n(N1.\\n(N2.\\n(N3 \\$1
.Ca 4
.LP
\\fI\\n(N1.\\n(N2.\\n(N3  \\$1\fP
..
.de P4
.nr N4 \\n(N4+1
.ds Tl "\\n(N1.\\n(N2.\\n(N3.\\n(N4 \\$1
.ne 5
.Ca 6
.LP
\\fI\\n(N1.\\n(N2.\\n(N3.\\n(N4  \\$1\fP
..
.de AP
.nr N5 \\n(N5+1
.nr A5 \\n(N5
.ds Tl "\\n(A5. \\$1
.ne 5
.Ca 0
.LP
\\fB\\n(A5.  \\$1\\fP
.sp
..
.de Ca
.da Cc
.if \\$1=0 \!.sp \\\\n(PDu
\!\l\&\\$1n\ \&\\*(Tl \l\&|\\\\n(LLu-\w\&\ \\n(PN\&u.\&\ \\n(PN
\!.br
.da
..
.de Ct
.Cc
.rm Cc
..
.de PT
.lt \\n(LLu
.pc %
.nr PN \\n%-1
.if \\n(PN%2=1 .tl '''\\n(PN'
.if (\\n(PN%2=0)&(\\n(PN) .tl '\\n(PN'''
.lt \\n(.lu
..
