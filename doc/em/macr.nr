.if n .so /usr/lib/tmac/tmac.kun
.if t .so /usr/lib/ditroff/tmac/tmac.kun
.SS 10
.RP
.MS T E
\!.TL '%'''
.ME
.MS T O
\!.TL '''%'
.ME
.MS B
.sp 1
.ME
.SM S1 B
.SM S2 B
.\" below are three simple macros to get the drawings right
.\" added by Dick Grune
.de Dr				\" Drawing $1 (size)
.N 1
.NE \\$1
.NA
.cs 1 18			\" constant spacing
.lg 0				\" no ligatures
.ss 18				\" bug in troff
..
.de Df				\" Drawing Footer
.br
.cs 1
.ss 12
.lg 1
..
.de De				\" Drawing End $1 (lines)
.Df				\" if it hasn't happened yet
.AD
.N \\$1
..
