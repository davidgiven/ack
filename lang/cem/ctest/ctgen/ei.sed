1i\
/* A sample sed script to show the use of the 'ops' file.\
   ops is converted into a test program for local integers \
*/
/LSTART/d
s/FN/main/
/ISTART/c\
	int x=255 , y= -256 ;
/[XYZS]/s/.*/& ; printf("%s#","&"@) ;/
/Z1/s/#/ %d&/
/Z1/s/@/, Z1&/
/Z2/s/#/ %d&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 255 ; /
/Z2/s/^/Z2 = 255 ; /
/[XYZS]/s/^/	/
s/X/4/g
s/Y/5/g
s/S/15/g
s/Z1/x/g
s/Z2/y/g
s/#/\\n/
s/@//
