/*
	For dumping the stack, GDA, heap and text segment.
*/

/* $Header$ */

#include	<ctype.h>

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"memdirect.h"
#include	"mem.h"
#include	"fra.h"
#include	"text.h"
#include	"proctab.h"
#include	"shadow.h"
#include	"linfil.h"
#include	"rsb.h"

extern long inr;			/* from log.c */

/****************************************************************
 *	Dumping routines for debugging, in human-readable form.	*
 ****************************************************************/

#ifdef	LOGGING

/*	The file is repetitive and should probably be partly generated,
	although it is not directly evident how.
*/

PRIVATE char *displ_undefs(), *displ_fil(), *displ_sh(), *displ_code();
PRIVATE ptr std_raw(), std_rsb();
PRIVATE int std_bytes(), dtd_bytes(), FRAd_bytes();
PRIVATE std_item(), std_left_undefs();
PRIVATE gdad_item(), gdad_left_undefs();
PRIVATE hpd_item(), hpd_left_undefs();
PRIVATE FRA_dump(), FRA_item();

/******** Stack Dump ********/

std_all(sz, rawfl)
	long sz;
	int rawfl;
{
	register ptr addr;
	
	if (!check_log(" d1 "))
		return;
	
	LOG((" d2 "));
	LOG((" d2 . . STACK_DUMP[%ld/%ld%s] . . INR = %lu . . STACK_DUMP . .",
				wsize, psize, rawfl ? ", raw" : "", inr));
	LOG((" d2 ----------------------------------------------------------------"));

	/* find a good start address */
	addr = (sz && sz < ML - SP ? SP + sz : ML);
	/* find RSB backwards */
	while (in_stack(addr) && !is_st_prot(addr)) {
		addr++;
	}
	/* find end of RSB backwards */
	while (in_stack(addr) && is_st_prot(addr)) {
		addr++;
	}
	addr--;

	/* dump the stack */
	while (in_stack(addr)) {
		addr = std_raw(addr, rawfl);
		addr = std_rsb(addr);
	}
	FRA_dump();
	LOG((" d1 >> AB = %lu, LB = %lu, SP = %lu, HP = %lu, LIN = %lu, FIL = %s",
		AB, LB, SP, HP, getLIN(), displ_fil(getFIL())));
	LOG((" d2 ----------------------------------------------------------------"));
	LOG((" d2 "));
}

PRIVATE ptr
std_raw(addr, rawfl)
	ptr addr;
	int rawfl;
{	/*	Produces a formatted dump of the stack segment starting
		at  addr, up to the Return Status Block (identified
		by protection bits)
	*/
	register int nundef = 0;
	
	LOG((" d2       ADDRESS     BYTE     ITEM VALUE   SHADOW"));
	
	while (	in_stack(addr)
	&&	(!is_st_prot(addr) || rawfl)
	) {
		if (st_sh(addr) == UNDEFINED) {
			if (nundef++ == 0)
				LOG((" d2    %10lu    undef", addr));
		}
		else {
			if (nundef) {
				std_left_undefs(nundef, addr + 1);
				nundef = 0;
			}
			std_item(addr);
		}
		addr--;
	}
	if (nundef)
		std_left_undefs(nundef, addr + 1);
	return addr;
}

PRIVATE std_item(addr)
	ptr addr;
{
	if (	is_wordaligned(addr)
	&&	is_in_stack(addr, psize)
	&&	std_bytes(addr, addr + psize, SH_DATAP|SH_INSP)
	) {
		/* print a pointer value */
		LOG((" d2    %10lu      %3lu    [%10lu]  (%-s)",
			addr,
			btol(stack_loc(addr)),
			p_in_stack(addr),
			displ_sh(st_sh(addr), stack_loc(addr))));
	}
	else
	if (	is_wordaligned(addr)
	&&	w_is_in_stack(addr)
	&&	std_bytes(addr, addr + wsize, SH_INT)
	) {
		/* print a word value */
		LOG((" d2    %10lu      %3lu    [%10ld]  (%-s)",
			addr,
			btol(stack_loc(addr)),
			w_in_stack(addr),
			displ_sh(st_sh(addr), stack_loc(addr))));
	}
	else {
		/* just print the byte */
		LOG((" d2    %10lu      %3lu                  (%-s)",
			addr,
			btol(stack_loc(addr)),
			displ_sh(st_sh(addr), stack_loc(addr))));
	}
}

PRIVATE ptr
std_rsb(addr)
	ptr addr;
{	/*	Dumps the Return Status Block */
	ptr dmp_lb;
	int code;
	long pi;
	ptr pc;
	ptr lb;
	long lin;
	ptr fil;
	char pr_descr[300];
	
	if (!in_stack(addr))
		return addr;

	dmp_lb = addr - (rsbsize-1);	/* pseudo local base */
	if (!in_stack(dmp_lb)) {
		LOG((" d1 >>RSB: >>>> INCOMPLETE <<<<"));
		return dmp_lb;
	}

	code = (int)w_in_stack(dmp_lb + rsb_rsbcode);
	pi = (long)p_in_stack(dmp_lb + rsb_PI);
	pc = p_in_stack(dmp_lb + rsb_PC);
	lb = p_in_stack(dmp_lb + rsb_LB);
	lin = LIN_in_stack(dmp_lb + rsb_LIN);
	fil = p_in_stack(dmp_lb + rsb_FIL);

	if (pi == -1) {
		sprintf(pr_descr, "uninit");
	}
	else
	if (pi < NProc) {
		sprintf(pr_descr, "(%ld,%ld)",
				pi, (long)proctab[pi].pr_nloc);
	}
	else {
		sprintf(pr_descr, "%ld >>>> ILLEGAL <<<<", pi);
	}
	LOG((" d1 >> RSB: code = %s, PI = %s, PC = %lu, LB = %lu, LIN = %lu, FIL = %s",
		displ_code(code), pr_descr, pc, lb, lin, displ_fil(fil)));
	
	LOG((" d2 "));
	return addr - rsbsize;
}

PRIVATE char *displ_code(rsbcode)
	int rsbcode;
{
	switch (rsbcode) {
	case RSB_STP:	return "STP";
	case RSB_CAL:	return "CAL";
	case RSB_RTT:	return "RTT";
	case RSB_NRT:	return "NRT";
	default:	return ">>Bad RSB code<<";
	}
	/*NOTREACHED*/
}

PRIVATE std_left_undefs(nundef, addr)
	int nundef;
	ptr addr;
{
	/* handle pending undefineds */
	switch (nundef) {
	case 1:
		break;
	case 2:
		LOG((" d2    %10lu    undef", addr));
		break;
	default:
		LOG((" d2         | | |    | | |"));
		LOG((" d2    %10lu    undef (%s)",
				addr, displ_undefs(nundef, addr)));
		break;
	}
}

PRIVATE FRA_dump()
{
	register int addr;

	LOG((" d2        FRA: size = %d, %s",
			FRASize, FRA_def ? "defined" : "undefined"));

	for (addr = 0; addr < FRASize; addr++) {
		FRA_item(addr);
	}
}

PRIVATE FRA_item(addr)
	int addr;
{
	if (	is_wordaligned(addr)
	&&	is_in_FRA(addr, psize)
	&&	FRAd_bytes(addr, (int)(addr + psize), SH_DATAP|SH_INSP)
	) {
		/* print a pointer value */
		LOG((" d2        FRA[%1d]      %3lu    [%10lu]  (%-s)",
			addr,
			btol(FRA[addr]),
			p_in_FRA(addr),
			displ_sh(FRA_sh[addr], FRA[addr])));
	}
	else
	if (	is_wordaligned(addr)
	&&	is_in_FRA(addr, wsize)
	&&	FRAd_bytes(addr, (int)(addr + wsize), SH_INT)
	) {
		/* print a word value */
		LOG((" d2        FRA[%1d]      %3lu    [%10ld]  (%-s)",
			addr,
			btol(FRA[addr]),
			w_in_FRA(addr),
			displ_sh(FRA_sh[addr], FRA[addr])));
	}
	else {
		/* just print the byte */
		LOG((" d2        FRA[%1d]      %3lu                  (%-s)",
			addr,
			btol(FRA[addr]),
			displ_sh(FRA_sh[addr], FRA[addr])));
	}
}


/******** Global Data Area Dump ********/

gdad_all(low, high)
	ptr low, high;
{
	register ptr addr;
	register int nundef = 0;
	
	if (!check_log(" +1 "))
		return;
	
	if (low == 0 && high == 0)
		high = HB;
	
	LOG((" +1 "));
	LOG((" +1 . . GDA_DUMP[%ld/%ld] . . INR = %lu . . GDA_DUMP . .",
				wsize, psize, inr));
	LOG((" +1 ----------------------------------------------------------------"));
	LOG((" +1       ADDRESS     BYTE     WORD VALUE   SHADOW"));
	
	/* dump global data area contents */
	addr = low;
	while (addr < min(HB, high)) {
		if (dt_sh(addr) == UNDEFINED) {
			if (nundef++ == 0)
				LOG((" +1    %10lu    undef", addr));
		}
		else {
			if (nundef) {
				gdad_left_undefs(nundef, addr-1);
				nundef = 0;
			}
			gdad_item(addr);
		}
		addr++;
	}
	if (nundef)
		    gdad_left_undefs(nundef, addr-1);
	LOG((" +1 ----------------------------------------------------------------"));
	LOG((" +1 "));
}

PRIVATE gdad_item(addr)
	ptr addr;
{
	if (	is_wordaligned(addr)
	&&	is_in_data(addr, psize)
	&&	dtd_bytes(addr, addr + psize, SH_DATAP|SH_INSP)
	) {
		/* print a pointer value */
		LOG((" +1    %10lu      %3lu    [%10lu]  (%-s)",
			addr,
			btol(data_loc(addr)),
			p_in_data(addr),
			displ_sh(dt_sh(addr), data_loc(addr))));
	}
	else
	if (	is_wordaligned(addr)
	&&	is_in_data(addr, wsize)
	&&	dtd_bytes(addr, addr + wsize, SH_INT)
	) {
		/* print a word value */
		LOG((" +1    %10lu      %3lu    [%10ld]  (%-s)",
			addr,
			btol(data_loc(addr)),
			w_in_data(addr),
			displ_sh(dt_sh(addr), data_loc(addr))));
	}
	else {
		/* just print the byte */
		LOG((" +1    %10lu      %3lu                  (%-s)",
			addr,
			btol(data_loc(addr)),
			displ_sh(dt_sh(addr), data_loc(addr))));
	}
}

PRIVATE gdad_left_undefs(nundef, addr)
	int nundef;
	ptr addr;
{
	/* handle pending undefineds */
	switch (nundef) {
	case 1:
		break;
	case 2:
		LOG((" +1    %10lu    undef", addr));
		break;
	default:
		LOG((" +1         | | |    | | |"));
		LOG((" +1    %10lu    undef (%s)",
				addr, displ_undefs(nundef, addr)));
		break;
	}
}

/******** Heap Area Dump ********/

hpd_all()
{
	register ptr addr;
	register int nundef = 0;
	
	if (!check_log(" *1 "))
		return;

	LOG((" *1 "));
	LOG((" *1 . . HEAP_DUMP[%ld/%ld] . . INR = %lu . . HEAP_DUMP . .",
				wsize, psize, inr));
	LOG((" *1 ----------------------------------------------------------------"));
	LOG((" *1       ADDRESS     BYTE     WORD VALUE   SHADOW"));
	
	/* dump heap contents */
	for (addr = HB; addr < HP; addr++) {
		if (dt_sh(addr) == UNDEFINED) {
			if (nundef++ == 0)
				LOG((" *1    %10lu    undef", addr));
		}
		else {
			if (nundef) {
				hpd_left_undefs(nundef, addr-1);
				nundef = 0;
			}
			hpd_item(addr);
		}
	}
	if (nundef)
		hpd_left_undefs(nundef, addr-1);
	LOG((" *1 ----------------------------------------------------------------"));
	LOG((" *1 "));
}

PRIVATE hpd_item(addr)
	ptr addr;
{
	if (	is_wordaligned(addr)
	&&	is_in_data(addr, psize)
	&&	dtd_bytes(addr, addr + psize, SH_DATAP|SH_INSP)
	) {
		/* print a pointer value */
		LOG((" *1    %10lu      %3lu    [%10lu]  (%-s)",
			addr,
			btol(data_loc(addr)),
			p_in_data(addr),
			displ_sh(dt_sh(addr), data_loc(addr))));
	}
	else
	if (	is_wordaligned(addr)
	&&	is_in_data(addr, wsize)
	&&	dtd_bytes(addr, addr + wsize, SH_INT)
	) {
		/* print a word value */
		LOG((" *1    %10lu      %3lu    [%10ld]  (%-s)",
			addr,
			btol(data_loc(addr)),
			w_in_data(addr),
			displ_sh(dt_sh(addr), data_loc(addr))));
	}
	else {
		/* just print the byte */
		LOG((" *1    %10lu      %3lu                  (%-s)",
			addr,
			btol(data_loc(addr)),
			displ_sh(dt_sh(addr), data_loc(addr))));
	}
}

PRIVATE hpd_left_undefs(nundef, addr)
	int nundef;
	ptr addr;
{
	/* handle pending undefineds */
	switch (nundef) {
	case 1:
		break;
	case 2:
		LOG((" *1    %10lu    undef", addr));
		break;
	default:
		LOG((" *1         | | |    | | |"));
		LOG((" *1    %10lu    undef (%s)",
				addr, displ_undefs(nundef, addr)));
		break;
	}
}


/* Service routines */

PRIVATE int std_bytes(low, high, bits)
	ptr low, high;
	int bits;
{
	/*	True if all stack bytes from low to high-1 have one of the
		bits in bits on.
	*/
	int byte = bits;

	while (low < high) {
		byte &= st_sh(low);
		low++;
	}

	return byte & bits;
}

PRIVATE int dtd_bytes(low, high, bits)
	ptr low, high;
	int bits;
{
	/*	True if all data bytes from low to high-1 have one of the
		bits in bits on.
	*/
	int byte = bits;

	while (low < high) {
		byte &= dt_sh(low);
		low++;
	}

	return byte & bits;
}

PRIVATE int FRAd_bytes(low, high, bits)
	int low, high;
	int bits;
{
	/*	True if all data bytes from low to high-1 have one of the
		bits in bits on.
	*/
	int byte = bits;

	while (low < high) {
		byte &= FRA_sh[low];
		low++;
	}

	return byte & bits;
}

PRIVATE char *				/* transient */
displ_undefs(nundef, addr)
	int nundef;
	ptr addr;
{
	/*	Given the number of undefineds, we want to report the number
		of words with the left-over numbers of bytes on both sides:
			|             nundef               |
			|left|          wrds            |right
			.....|........|........|........|...
			a
			d
			d
			r
		This takes some arithmetic.
	*/
	static char buf[30];
	register int left = wsize - 1 - p2i(addr-1) % wsize;
	register int wrds = (nundef-left) / wsize;
	register int right = nundef - left - wrds*wsize;

	if (wrds == 0) {
		sprintf(buf, "%d byte%s",
			nundef, nundef == 1 ? "" : "s");
	}
	else if (left == 0 && right == 0) {
		sprintf(buf, "%d word%s",
			wrds, wrds == 1 ? "" : "s");
	}
	else if (left == 0) {
		sprintf(buf, "%d word%s + %d byte%s",
			wrds, wrds == 1 ? "" : "s",
			right, right == 1 ? "" : "s");
	}
	else if (right == 0) {
		sprintf(buf, "%d byte%s + %d word%s",
			left, left == 1 ? "" : "s",
			wrds, wrds == 1 ? "" : "s");
	}
	else {
		sprintf(buf, "%d byte%s + %d word%s + %d byte%s",
			left, left == 1 ? "" : "s",
			wrds, wrds == 1 ? "" : "s",
			right, right == 1 ? "" : "s");
	}
	return buf;
}

PRIVATE char *
displ_fil(fil)				/* transient */
	ptr fil;
{	/*	Returns a buffer containing a representation of the
		filename derived from FIL-value fil.
	*/
	static char buf[40];
	char *bp = &buf[0];
	int ch;
	
	if (!fil)
		return "NULL";
	if (fil >= HB)
		return "***NOT IN GDA***";
	
	*bp++ = '"';
	while (in_gda(fil) && (ch = data_loc(fil))) {
		if (bp < &buf[sizeof buf-1]) {
			*bp++ = (ch < 040 || ch > 126 ? '?' : ch);
		}
		fil++;
	}
	if (bp < &buf[sizeof buf-1])
		*bp++ = '"';
	*bp++ = '\0';
	return &buf[0];
}

PRIVATE char *
displ_sh(shadow, byte)				/* transient */
	char shadow;
	int byte;
{	/*	Returns a buffer containing a description of the
		shadow byte.
	*/
	static char buf[32];
	register char *bufp;
	int check = 0;

	bufp = buf;
	if (shadow & SH_INT) {
		*bufp++ = 'I';
		*bufp++ = 'n';
		check++;
	}
	if (shadow & SH_FLOAT) {
		*bufp++ = 'F';
		*bufp++ = 'l';
	}
	if (shadow & SH_DATAP) {
		*bufp++ = 'D';
		*bufp++ = 'p';
	}
	if (shadow & SH_INSP) {
		*bufp++ = 'I';
		*bufp++ = 'p';
	}

	if (shadow & SH_PROT) {
		*bufp++ = ',';
		*bufp++ = ' ';
		*bufp++ = 'P';
		*bufp++ = 'r';
		*bufp++ = 'o';
		*bufp++ = 't';
	}

	if (check && isascii(byte) && isprint(byte)) {
		*bufp++ = ',';
		*bufp++ = ' ';
		*bufp++ = byte;
		*bufp++ = ' ';
	}
	*bufp = 0;
	return (buf);
}

#endif	LOGGING

