tested:		last
	set -x ;\
		for i in `awk '{for(i=\$$1;i<=127;i++)print i}' last ` ;\
		do \
		  echo $$i; \
		  echo $$i >last; \
		  select $$i tests > test.e; \
		  ack test.e; \
		  a.out \
		  : ok; \
		done
		rm -f test.e a.out
		>tested

last:		tests test.h select
		echo 0 >last

select:		select.c
		cc -O -i -o select select.c
