ACK=ack
tested:		last
	set -x ;\
		for i in `awk '{for(i=\$$1;i<=151;i++)print i}' last ` ;\
		do \
		  echo $$i; \
		  echo $$i >last; \
		  select $$i tests > test.e; \
		  $(ACK) test.e; \
		  a.out \
		  : ok; \
		done
		rm -f test.e a.out
		>tested

last:		tests test.h select
		echo 0 >last

select:		select.c
		$(CC) -O -o select select.c
