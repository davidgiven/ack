d=../..
h=$d/h

cmp:          
		(cd comp ; make cmp)

install:      
		(cd comp ; make install)

clean:
		-rm -f *.o *.old
		(cd comp ; make clean)

opr:
		make pr ^ opr

pr:
		@(cd comp ; make pr)
		@(cd libpc ; make pr)
