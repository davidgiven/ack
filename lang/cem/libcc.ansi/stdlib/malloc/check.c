/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include	<stdio.h>

#ifdef	CHECK			/* otherwise this whole file is skipped */

/* ??? check these later */
private acquire_malout(void), check_ml_last(const char *s);
private dump_all_mallinks(void), dump_free_list(int i);
private dump_mallink(const char *s, mallink *ml), print_loop(mallink *ml);
private working_on(mallink *ml);
private size_type checksum(mallink *ml);
static FILE *malout;

private mallink *free_list_entry(int i);

#define	for_free_list(i,p) \
	for (p = free_list_entry(i); p; p = log_next_of(p))

#define	for_all_mallinks(ml)	/* backwards! */ \
	for (ml = ml_last; ml; \
		ml = first_mallink(ml) ? MAL_NULL : phys_prev_of(ml))

/* Maldump */

static int pr_cnt = 0;

maldump(int n)	{
	/*	Dump pertinent info in pseudo-readable format;
		abort afterwards if n != 0.
	*/
	static int dumping = 0;
	int i;
	
	if (dumping)
		return;
	dumping++;
	acquire_malout();
	fprintf(malout,
		">>>>>>>>>>>>>>>> DUMP OF ALL MALLINKS <<<<<<<<<<<<<<<<");
	fprintf(malout, "    ml_last = %p\n", ml_last);
	if (++pr_cnt == 100) pr_cnt = 0;
	dump_all_mallinks();
	fprintf(malout,
		">>>>>>>>>>>>>>>> DUMP OF FREE_LISTS <<<<<<<<<<<<<<<<\n");
	if (++pr_cnt == 100) pr_cnt = 0;
	for (i = 0; i < MAX_FLIST; i++)
		dump_free_list(i);
	fprintf(malout,
		">>>>>>>>>>>>>>>> END OF DUMP <<<<<<<<<<<<<<<<\n");
	fclose(malout);
	dumping--;
	if (n)
		abort();
}

private
acquire_malout(void)	{
	static char buf[BUFSIZ];
	
	if (!malout)	{
		malout = freopen("mal.out", "w", stderr);	
		setbuf(malout, buf);
	}
}

private
dump_all_mallinks(void)	{
	mallink *ml;
	
	for_all_mallinks (ml)	{
		if (print_loop(ml))
			return;
		dump_mallink((char *)0, ml);
	}
}

private
dump_free_list(int i)	{
	mallink *ml = free_list_entry(i);
	
	if (!ml)
		return;
	fprintf(malout, "%2d: ", i);
	for_free_list(i, ml)	{
		if (print_loop(ml))
			return;
		fprintf(malout, "%p ", ml);
	}
	fprintf(malout, "<\n");
}

private int
print_loop(mallink *ml)	{
	if (print_of(ml) == pr_cnt)	{
		fprintf(malout, "... PRINT LOOP\n");
		return 1;
	}
	set_print(ml, pr_cnt);
	return 0;
}

private
dump_mallink(const char *s, mallink *ml)	{
	acquire_malout();
	if (s)
		fprintf(malout, "%s: ", s);
	fprintf(malout, "@: %p;", ml);
	if (ml && checksum_of(ml) != checksum(ml))
		fprintf(malout, ">>>> CORRUPTED <<<<");
	if (!ml)	{
		fprintf(malout, "\n");
		return;
	}	
	if (free_of(ml))	{
		fprintf(malout, " l_p: %p;", _log_prev_of(ml));
		fprintf(malout, " l_n: %p;", _log_next_of(ml));
	}
	fprintf(malout, " p_s: %p;", prev_size_of(ml));
	fprintf(malout, " t_s: %p;", _this_size_of(ml));
	fprintf(malout, " sz: %lu;", (unsigned long) size_of(ml));
	fprintf(malout, " fr: %d;", free_of(ml));
	fprintf(malout, "\n");
}

/*	Check_mallinks() checks the total data structure as accessible
	through free_list[] and ml_last.  All check_sums should be OK,
	except those held in the small array off_colour.  This is a
	trick to allow to continue checking even when a few mallinks
	are temporarily out of order.
	Check_mallinks() tests for a lot of internal consistency.
*/

/* Some arbitrary constants */
#define	IN_ML_LAST	93
#define	IN_FREE_LIST	57		/* and in ml_last */
#define	CLEAR		21

#define	VRIJ		1
#define	BEZET		2

private
check_mallinks(const char *s)	{
	mallink *ml;
	size_type size;
	int i;
	char stat;
	
	check_ml_last(s);
	stat = BEZET;
	for_all_mallinks(ml)	{
		if (checksum_of(ml) != checksum(ml))
			Error("mallink info at %p corrupted", s, ml);
		if (working_on(ml))	{
			stat = BEZET;
			continue;
		}
		if (	!last_mallink(ml) &&
			phys_prev_of(phys_next_of(ml)) != ml
		)
			Error("upward chain bad at %p", s, ml);
		if (	!first_mallink(ml) &&
			phys_next_of(phys_prev_of(ml)) != ml
		)
			Error("downward chain bad at %p", s, ml);
		if (free_of(ml))	{
			if (stat == VRIJ)
				Error("free mallink at %p follows free mallink",
								s, ml);
			stat = VRIJ;
		}
		else
			stat = BEZET;
		set_mark(ml, IN_ML_LAST);
	}
	
	for (i = 0, size = MIN_SIZE; i < MAX_FLIST; i++, size *= 2)	{
		for_free_list(i, ml)	{
			if (working_on(ml))
				continue;
			if (!free_of(ml))
				Error("occupied mallink %p occurs in free_list", s, ml);
			switch (mark_of(ml))	{
			case IN_ML_LAST:
				set_mark(ml, IN_FREE_LIST);
				break;
			case IN_FREE_LIST:
				Error("mallink %p occurs in 2 free_lists",
								s, ml);
			default:
				Error("unknown mallink %p in free_list",
								s, ml);
			}
			if (size_of(ml) < size)
				Error("size of mallink %p too small", s, ml);
			if (size_of(ml) >= 2*size)
				Error("size of mallink %p too large", s, ml);
		}
	}
	for_all_mallinks (ml)	{
		if (working_on(ml))
			continue;
		if (free_of(ml) && mark_of(ml) != IN_FREE_LIST)
			Error("free mallink %p is in no free_list", s, ml);
		set_mark(ml, CLEAR);
	}
}

private
check_ml_last(const char *s)	{
	if (ml_last && _this_size_of(ml_last) == 0)
		Error("size of ml_last == 0, at %p", s, ml_last);
}

private size_type
checksum(mallink *ml)	{
	size_type sum = 0;
	
	if (free_of(ml))	{
		sum += (size_type)_log_prev_of(ml);
		sum += (size_type)_log_next_of(ml);
	}
	sum += (size_type)prev_size_of(ml);
	sum += (size_type)_this_size_of(ml);
	return sum;
}

private
calc_checksum(mallink *ml)	{
	set_checksum(ml, checksum(ml));
}

#define	N_COLOUR	10
static mallink *off_colour[N_COLOUR];

private
started_working_on(mallink *ml)	{
	int i;
	
	for (i = 0; i < N_COLOUR; i++)
		if (off_colour[i] == MAL_NULL)	{
			off_colour[i] = ml;
			return;
		}
	Error("out of off_colour array at %p", "started_working_on", ml);
}

private
stopped_working_on(mallink *ml)	{
	int i;
	
	for (i = 0; i < N_COLOUR; i++)
		if (off_colour[i] == ml)	{
			off_colour[i] = MAL_NULL;
			return;
		}
	Error("stopped working on mallink %p", "stopped_working_on", ml);
}

private int
working_on(mallink *ml)	{
	int i;
	
	for (i = 0; i < N_COLOUR; i++)
		if (off_colour[i] == ml)
			return 1;
	return 0;
}

private
check_work_empty(const char *s)	{
	int i;
	int cnt = 0;
	
	for (i = 0; i < N_COLOUR; i++)
		if (off_colour[i] != MAL_NULL)
			cnt++;
	if (cnt != 0)
		Error("off_colour not empty", s, MAL_NULL);
}

private int
Error(const char *fmt, const char *s, mallink *ml)	{
	static int already_called = 0;

	if (already_called++) return 0;
	setbuf(stdout, (char *) 0);
	printf("%s: ", s);
	printf(fmt, (long)ml);
	printf("\n");
	acquire_malout();
	fprintf(malout, "%s: ", s);
	fprintf(malout, fmt, (long)ml);
	fprintf(malout, "\n");
	fflush(stdout);
	maldump(1);
	return 0;			/* to satisfy lint */
}

#endif	/* CHECK */
