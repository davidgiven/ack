unsigned
alarm(n)
	unsigned n;
{
	struct { long l1,l2,l3,l4; } t1,t2;
	t1.l1 = 0;
	t1.l2 = 0;
	t1.l4 = 0;
	t1.l3 = n;
	if (setitimer(0,&t1,&t2) < 0) return -1;
	if (t2.l4) t2.l3++;
	return t2.l3;
}
