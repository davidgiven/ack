# include "setjmp.h"
# include <stdio.h>
# ifndef NOSIGNAL
# include <signal.h>
#endif /* not NOSIGNAL */

/*
 * setjmptest
 * A program to test setjmp(III) and longjmp,
 * in particular with respect to register variables
 */

int	whichtest;
int	nerrors;

main() {
	jmp_buf envm;
	register int i;

	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	if (nerrors) return nerrors;

	i = 1;
	if (setjmp(envm) == 0) {
		i = 2;
		longjmp(envm, 1);
	}
	else {
		if (i == 2) {
			printf("Setjmp/longjmp work ok, even with register variables\n");
		}
		else if (i == 1) {
			printf("WARNING: The setjmp/longjmp of this machine restore register variables\n\
to the value they had at the time of the \"setjmp\"\n");
		}
		else {
			printf("Aha, I just found one last error\n");
			return 1;
		}
	}
	return 0;
}

e(n) {
	nerrors++;
	fprintf(stderr,"Error %d in test %d\n",n,whichtest);
}

test1() {
	register p;

	printf("TEST 1 : one integer register variable\n");
	whichtest = 1;
	p = 200;
	garbage();
	if (p != 200) e(1);
}

test2() {
	register p,q;

	printf("TEST 2 : two integer register variables\n");
	whichtest = 2;
	p = 200; q = 300;
	garbage();
	if (p != 200) e(1);
	if (q != 300) e(2);
}
	
test3() {
	register p,q,r;

	printf("TEST 3 : three integer register variables\n");
	whichtest = 3;
	p = 200; q = 300; r = 400;
	garbage();
	if (p != 200) e(1);
	if (q != 300) e(2);
	if (r != 400) e(3);
}
	
char	buf[512];

test4() {
	register char *p;

	printf("TEST 4 : one pointer register variable\n");
	whichtest = 4;
	p = &buf[100];
	garbage();
	if (p != &buf[100]) e(1);
}

test5() {
	register char *p,*q;

	printf("TEST 5 : two pointer register variables\n");
	whichtest = 5;
	p = &buf[100]; q = &buf[200];
	garbage();
	if (p != &buf[100]) e(1);
	if (q != &buf[200]) e(2);
}

test6() {
	register char *p,*q,*r;

	printf("TEST 6 : three pointer register variables\n");
	whichtest = 6;
	p = &buf[100]; q = &buf[200]; r = &buf[300];
	garbage();
	if (p != &buf[100]) e(1);
	if (q != &buf[200]) e(2);
	if (r != &buf[300]) e(3);
}

jmp_buf env;

/*	return address of local variable.
	This way we can check that the stack is not polluted.
*/
char *
addr() {
	char a;

	return &a;
}

garbage() {
	register	i,j,k;
	register char	*p,*q,*r;
	char *a, *tmp;
	int t;

	p = &buf[300];
	q = &buf[400];
	r = &buf[500];
	i = 10; j = 20; k = 30;
	switch(setjmp(env)) {
	case 0:
		a = addr();
		longjmp(env,1);
		break;
	case 1:
		if (i != 10) e(11);
		if (j != 20) e(12);
		if (k != 30) e(13);
		if (p != &buf[300]) e(14);
		if (q != &buf[400]) e(15);
		if (r != &buf[500]) e(16);
		tmp = addr();
		if (a != tmp) e(17);
		level1();
		break;
	case 2:
		if (i != 10) e(21);
		if (j != 20) e(22);
		if (k != 30) e(23);
		if (p != &buf[300]) e(24);
		if (q != &buf[400]) e(25);
		if (r != &buf[500]) e(26);
		if (a != tmp) e(27);
		level2();
		break;
	case 3:
		if (i != 10) e(31);
		if (j != 20) e(32);
		if (k != 30) e(33);
		if (p != &buf[300]) e(34);
		if (q != &buf[400]) e(35);
		if (r != &buf[500]) e(36);
		if (a != tmp) e(37);
# ifndef NOSIGNAL
		hard();
	case 4:
		if (i != 10) e(41);
		if (j != 20) e(42);
		if (k != 30) e(43);
		if (p != &buf[300]) e(44);
		if (q != &buf[400]) e(45);
		if (r != &buf[500]) e(46);
		if (a != tmp) e(47);
#endif
		return;
		break;
	default:
		e(100);
	}
	e(200);
}

level1() {
	register char *p;
	register i;

	i = 1000;
	p = &buf[10];
	i = 200;
	p = &buf[20];
	longjmp(env,2);
}

level2() {
	register char *p;
	register i;

	i = 0200;
	p = &buf[2];
	*p = i;
	dolev();
}

dolev() {
	register char *p;
	register	i;

	i = 010;
	p = &buf[3];
	*p = i;
	longjmp(env,3);
}

# ifndef NOSIGNAL
catch() {
	printf(" signal caught\n");
	longjmp(env,4);
}

hard() {
	register char *p;

	signal(SIGHUP,catch);
	for(p = buf;p <= &buf[511]; p++) *p = 025;
	printf("Sending signal ...");
	kill(getpid(),SIGHUP);
}
#endif /* not NOSIGNAL */
