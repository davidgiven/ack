1i\
 /*  ops is converted into a test program for local characters \
*/
/ISTART/d
s/FN/main/
/LSTART/c\
	char x=10 , y= 0100 ;
/[XYZS]/s/.*/& ; printf("%s#","&"@) ;/
/Z1/s/#/ %d&/
/Z1/s/@/, Z1&/
/Z2/s/#/ %d&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 10 ; /
/Z2/s/^/Z2 = 0100 ; /
/[XYZS]/s/^/	/
s/X/'0'/g
s/Y/'1'/g
s/S/4/g
s/Z1/x/g
s/Z2/y/g
s/#/\\n/
s/@//
