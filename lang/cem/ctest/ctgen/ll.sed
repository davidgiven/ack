1i\
 /*  ops is converted into a test program for longs \
*/
/ISTART/d
s/FN/main/
/LSTART/c\
	long x=100234 , y= -301 ;
/[XYZS]/s/.*/& ; printf("%s#","&"@) ;/
/Z1/s/#/ %D&/
/Z1/s/@/, Z1&/
/Z2/s/#/ %D&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 10 ; /
/Z2/s/^/Z2 = 0100 ; /
/[XYZS]/s/^/	/
s/X/40000/g
s/Y/30000/g
s/S/9/g
s/Z1/x/g
s/Z2/y/g
s/#/\\n/
s/@//
