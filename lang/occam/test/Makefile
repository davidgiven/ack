.SUFFIXES: .ocm

.ocm.o:
	ack -c $<

ALL =	Huffman aatob copy key lifegame matmul sort use_prnt xxtoy

all:		$(ALL)

install:	all

cmp:		all

clean:
	rm -f *.o $(ALL) nohup.out

pr:
	@pr Makefile *.ocm

opr:
	make pr | opr

Huffman:	Huffman.o
		ack -.ocm -o Huffman Huffman.o
use_prnt:	use_prnt.o
		ack -.ocm -o use_prnt use_prnt.o
xxtoy:	xxtoy.o
		ack -.ocm -o xxtoy xxtoy.o
aatob:	aatob.o
		ack -.ocm -o aatob aatob.o
copy:	copy.o
		ack -.ocm -o copy copy.o
key:	key.o
		ack -.ocm -o key key.o
lifegame:	lifegame.o
		ack -.ocm -o lifegame lifegame.o
matmul:	matmul.o
		ack -.ocm -o matmul matmul.o
sort:	sort.o
		ack -.ocm -o sort sort.o
