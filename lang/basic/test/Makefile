SRC=	bull.b buzzword.b checker.b creator.b grafiek.b gunner.b learn.b\
	opg1.b opg2.b opg3.b opg4.b opg5.b opg6.b som4.b test01.b test02.b\
	test03.b test04.b test05.b test06.b test07.b test08.b test09.b\
	test10.b test11.b test12.b test13.b test14.b test15.b test16.b\
	test17.b test19.b test20.b test21.b test22.b test23.b\
	test24.b test25.b test26.b test27.b test28.b test29.b test30.b\
	test31.b test32.b test33.b test34.b test35.b

#FLOATS = -fp
FLOATS =

head:	
	for i in $(SRC) ; do fp=$(FLOATS); export fp; runcmp $$i ; done
clean:
	rm -f *.b.[rx] *.[dekmos]
