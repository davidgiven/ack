1i\
 /*  ops is converted into a test program for longs \
*/
/LSTART/d
s/FN/main/
/ISTART/c\
	long x=100234 , y= -301 , z= 0 ;
/[XYZS]/s/.*/& ; printf("%s#","&"@) ;/
/Z1/s/#/ %ld&/
/Z1/s/@/, Z1&/
/Z2/s/#/ %ld&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 10 ; /
/Z2/s/^/Z2 = 0100 ; /
/[XYZS]/s/^/	/
s/X/x/g
s/Y/16329/g
s/S/9/g
s/Z1/z/g
s/Z2/y/g
s/#/\\n/
s/@//
