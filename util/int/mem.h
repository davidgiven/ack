/*
	Memory access facilities
*/

/* $Header$ */


/******** Memory address & location defines ********/

#define	data_loc(a)	(*(data + (p2i(a))))
#define	stack_loc(a)	(*(stackML - (a)))
#define	mem_loc(a)	(in_stack(a) ? stack_loc(a) : data_loc(a))

#define	loc_addr(o)	(((o) < 0) ? (LB + (o)) : (AB + (o)))


/******** Checks on adresses and ranges ********/

#define	is_aligned(a,n)	((p2i(a)) % (n) == 0)
#define	is_wordaligned(a)	(((p2i(a)) & wsizem1) == 0)

#define	ch_aligned(a,n)	{ if (!is_aligned(a, min(n, wsize))) \
						{ trap(EBADPTR); } }
#define	ch_wordaligned(a)	{ if (!is_wordaligned(a)) \
						{ trap(EBADPTR); } }

#define	in_gda(p)	((p) < HB)
#define	in_stack(p)	(SP <= (p) && (p) <= ML)

#define	is_in_data(a,n)	((a) + (n) <= HP)
#define	ch_in_data(a,n)	{ if (!is_in_data(a, n)) { trap(EMEMFLT); } }

#define	is_in_stack(a,n) (SP <= (a) && (a) + (n) - 1 <= ML)
#define	ch_in_stack(a,n) { if (!is_in_stack(a, n)) { trap(EMEMFLT); } }

#define	is_in_FRA(a,n)	((a) + (n) <= FRASize)


/******* Address-depending memory defines *******/

#define	is_in_mem(a,n)	(is_in_data(a, n) || is_in_stack(a, n))

#define	mem_stn(a,l,n)	{ if (in_stack(a)) st_stn(a, l, n); else dt_stn(a, l, n); }

#define	mem_lddp(a)	(in_stack(a) ? st_lddp(a) : dt_lddp(a))
#define	mem_ldip(a)	(in_stack(a) ? st_ldip(a) : dt_ldip(a))
#define	mem_ldu(a,n)	(in_stack(a) ? st_ldu(a, n) : dt_ldu(a, n))
#define	mem_lds(a,n)	(in_stack(a) ? st_lds(a, n) : dt_lds(a, n))

#define	push_m(a,n)	{ if (in_stack(a)) push_st(a, n); else push_dt(a, n); }
#define	pop_m(a,n)	{ if (in_stack(a)) pop_st(a, n); else pop_dt(a, n); }


/******** Simple stack manipulation ********/

#define	st_inc(n)	newSP(SP - (n))	/* stack grows */
#define	st_dec(n)	newSP(SP + (n))	/* stack shrinks */


/******** Function return types ********/

extern ptr st_ldip(), dt_ldip();
extern ptr st_lddp(), dt_lddp(), dppop();
extern long st_lds(), dt_lds(), spop(), wpop();
extern unsigned long st_ldu(), dt_ldu(), upop();

