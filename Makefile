cmp:           # compile everything and compare
	(cd etc  ; make cmp )
	(cd util ; make cmp )
	(cd lang ; make cmp )
	(cd mach ; make cmp )

install:         # compile everything to machine code
	(cd etc  ; make install )
	(cd util ; make install )
	(cd lang/cem ; make install )
	(cd mach ; make install )
	(cd lang/pc ; make install )

clean:        # remove all non-sources, except boot-files
	(cd doc ; make clean )
	(cd man ; make clean )
	(cd h   ; make clean )
	(cd etc  ; make clean )
	(cd util ; make clean )
	(cd lang ; make clean )
	(cd mach ; make clean )

opr:            # print all sources
	make pr | opr

pr:             # print all sources
	@( pr Makefile ; \
	  (cd doc ; make pr ) ; \
	  (cd man ; make pr ) ; \
	  (cd h ; make pr ) ; \
	  (cd etc  ; make pr ) ; \
	  (cd lang ; make pr ) ; \
	  (cd util ; make pr ) ; \
	  (cd mach ; make pr ) \
	)
