.so /usr/lib/tmac/tmac.kun
.SS 10
.if n .LL 78
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
.ft CW				\" constant spacing
.lg 0				\" no ligatures
..
.de Df				\" Drawing Footer
.N 1
.FT P
.CS
.lg 1
..
.de De				\" Drawing End $1 (lines)
.Df				\" if it hasn't happened yet
.CE
.AD
.N \\$1
..
