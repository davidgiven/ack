d=../..
h=$d/h

cmp:          
		(cd pem ; make cmp)

install:      
		(cd pem ; make install)

clean:
		-rm -f *.o *.old
		(cd pem ; make clean)

opr:
		make pr ^ opr

pr:
		@(cd pem ; make pr)
		@(cd libpc ; make pr)
