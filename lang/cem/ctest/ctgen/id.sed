1i\
#ifndef NOFLOAT
/LSTART/d
s/FN/main/
/ISTART/c\
	double x=3.1415; int y= -20 ;
/[^&]& /d
/[^|]| /d
/>>/d
/<</d
/%/d
/\^/d
/~/d
/&=/d
/|=/d
/[XYZS]/s/.*/& ; printf("%s#","&"@) ;/
/Z1/s/#/ %e&/
/Z1/s/@/, Z1&/
/Z2/s/#/ %d&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 3.1415 ; /
/Z2/s/^/Z2 = -20 ; /
/[XYZS]/s/^/	/
s/X/(-40)/g
s/Y/300/g
s/Z1/x/g
s/Z2/y/g
s/#/\\n/
s/@//
$a\
#else\
main() { return 0; }\
#endif
