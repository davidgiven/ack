static	qsort1();
static	int (*qcompar)();
static	qexchange();
static	q3exchange();

qsort(base, nel, width, compar)
	char *base;
	int (*compar)();
{
	qcompar = compar;
	qsort1(base, base + (nel - 1) * width, width);
}

static
qsort1(a1, a2, width)
	char *a1, *a2;
	register int width;
{
	register char *left, *right;
	register char *lefteq, *righteq;
	int cmp;

	for (;;) {
		if (a2 <= a1) return;
		left = a1;
		right = a2;
		lefteq = righteq = a1 + width * (((a2-a1)+width)/(2*width));
		/*
		   Pick an element in the middle of the array.
		   We will collect the equals around it.
		   "lefteq" and "righteq" indicate the left and right
		   bounds of the equals respectively.
		   Smaller elements end up left of it, larger elements end
		   up right of it.
		*/
again:
		while (left < lefteq && (cmp = (*qcompar)(left, lefteq)) <= 0) {
			if (cmp < 0) {
				/* leave it where it is */
				left += width;
			}
			else {
				/* equal, so exchange with the element to
				   the left of the "equal"-interval.
				*/
				lefteq -= width;
				qexchange(left, lefteq, width);
			}
		}
		while (right > righteq) {
			if ((cmp = (*qcompar)(right, righteq)) < 0) {
				/* smaller, should go to left part
				*/
				if (left < lefteq) {
					/* yes, we had a larger one at the
					   left, so we can just exchange
					*/
					qexchange(left, right, width);
					left += width;
					right -= width;
					goto again;
				}
				/* no more room at the left part, so we
				   move the "equal-interval" one place to the
				   right, and the smaller element to the
				   left of it.
				   This is best expressed as a three-way
				   exchange.
				*/
				righteq += width;
				q3exchange(left, righteq, right, width);
				lefteq += width;
				left = lefteq;
			}
			else if (cmp == 0) {
				/* equal, so exchange with the element to
				   the right of the "equal-interval"
				*/
				righteq += width;
				qexchange(right, righteq, width);
			}
			else	/* just leave it */ right -= width;
		}
		if (left < lefteq) {
			/* larger element to the left, but no more room,
			   so move the "equal-interval" one place to the
			   left, and the larger element to the right
			   of it.
			*/
			lefteq -= width;
			q3exchange(right, lefteq, left, width);
			righteq -= width;
			right = righteq;
			goto again;
		}
		/* now sort the "smaller" part */
		qsort1(a1, lefteq - width, width);
		/* and now the larger, saving a subroutine call
		   because of the for(;;)
		*/
		a1 = righteq + width;
	}
	/*NOTREACHED*/
}

static
qexchange(p, q, n)
	register char *p, *q;
	register int n;
{
	register int c;

	while (n-- > 0) {
		c = *p;
		*p++ = *q;
		*q++ = c;
	}
}

static
q3exchange(p, q, r, n)
	register char *p, *q, *r;
	register int n;
{
	register int c;

	while (n-- > 0) {
		c = *p;
		*p++ = *r;
		*r++ = *q;
		*q++ = c;
	}
}
