DEST=/usr/lib/tmac/tmac.kun

install:
	for i in  "" 1 2 3 4 5 6 7 8 ;\
	do ;\
		cp tmac.q$i $DEST$i ;\
	done

cmp:
	for i in  "" 1 2 3 4 5 6 7 8 ;\
	do ;\
		cmp tmac.q$i $DEST$i ;\
	done

pr:
	@for i in  "" 1 2 3 4 5 6 7 8 ;\
	do ;\
		pr tmac.q$i ;\
	done

opr:
	make pr | opr

clean:
	@:
