/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ X F O R M . H
 */

void init_replacements(short psize, short wsize);
				/*
				 * This routine must be called once, before
				 * any call to xform_proc. It initializes
				 * a machine dependent table.
				 */
void xform_proc(proc_p p, alloc_p alloclist, short nrinstrs,
		line_p instrmap[]);
				/*
				 * Transform a procedure. Alloclist must
				 * contain the packed allocations (i.e. those
				 * allocations that are assigned a register).
				 */
bool always_in_reg(offset off, alloc_p allocs, short *size_out);
				/*
				 * See if the local variable with the given 
				 * offset is stored in a register during its 
				 * entire lifetime. As a side effect,
				 * return the size of the local.
				 */
void rem_locals(proc_p p, alloc_p allocs);
				/*
				 * Try to decrease the number of locals of
				 * procedure p, by looking at which locals
				 * are always stored in a register.
				 */
void rem_formals(proc_p p, alloc_p allocs);
				/*
				 * Try to decrease the number of formals of
				 * procedure p, by looking at which formals
				 * are always stored in a register.
				 */
