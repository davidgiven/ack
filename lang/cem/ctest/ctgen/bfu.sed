/ISTART/c\
/* test bit fields */\
struct bfs {\
	unsigned int bf1:1 ;\
	unsigned int bf2:4 ;\
	unsigned int bf3:8 ;\
	unsigned int bf4:16 ;\
} bfs ;
s/FN/main/
/LSTART/c\
	bfs.bf1=1 ; bfs.bf2=8 ;
/[XYZS]/s/.*/& ; printf("%s#","&"@) ;/
/Z1/s/#/ %d&/
/Z1/s/@/, Z1&/
/Z2/s/#/ %d&/
/Z2/s/@/, Z2&/
/Z1/s/^/Z1 = 255 ; /
/Z2/s/^/Z2 = 3 ; /
/[XYZS]/s/^/	/
s/X/bfs.bf1/g
s/Y/bfs.bf2/g
s/S/1/g
s/Z1/bfs.bf3/g
s/Z2/bfs.bf4/g
s/#/\\n/
s/@//
