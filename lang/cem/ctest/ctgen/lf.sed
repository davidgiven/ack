/ISTART/d
s/FN/main/
/LSTART/c\
	float x=3.1415 , y= 1e-7 ;
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
/Z2/s/#/ %e&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 3.1415 ; /
/Z2/s/^/Z2 = 1e-7 ; /
/[XYZS]/s/^/	/
s/X/.4e-5/g
s/Y/.3e-5/g
s/Z1/x/g
s/Z2/y/g
s/#/\\n/
s/@//
