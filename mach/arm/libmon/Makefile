all:

install:	
		../../install libros_s.a tail_ros

cmp:
		-../../compare libros_s.a tail_ros

clean:

opr :
	make pr | opr

pr:
	@arch pv libros_s.a | pr -h `pwd`/libros_s.a
