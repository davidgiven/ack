/* $Header$ */

/* a.out file reading ... */

#include "rd.h"

#if defined(__sun)
#define sun
#endif

#if defined(__i386)
#define i386
#endif

#if defined(__mc68020)
#define mc68020
#endif

#if defined(__sparc)
#define sparc
#endif

#if defined(__vax)
#define vax
#endif

#if defined(sun) || defined(vax)

struct exec {
#ifdef sun
	short a_x;
	unsigned short	a_magic;
#else
	unsigned long	a_magic;
#endif
	unsigned long	a_text;
	unsigned long	a_data;
	unsigned long	a_bss;
	unsigned long	a_syms;
	unsigned long	a_entry;
	unsigned long	a_trsize;
	unsigned long	a_drsize;
};

#define OMAGIC	0407
#define NMAGIC	0410
#define	ZMAGIC	0413

#define N_BADMAG(x) \
	((x).a_magic!=OMAGIC && (x).a_magic!=NMAGIC && (x).a_magic!=ZMAGIC)
#ifdef sun
#define N_TXTOFF(x)	((x).a_magic == ZMAGIC ? 0 : sizeof(struct exec))
#else
#define N_TXTOFF(x)	(sizeof(struct exec))
#endif
#define N_STROFF(x)	(N_TXTOFF(x)+(x).a_text+(x).a_data+(x).a_trsize+(x).a_drsize+(x).a_syms)

#ifdef sparc
#define RELOC_SIZE	12
#else
#define RELOC_SIZE	8
#endif

struct nlist {
	union {
		char	*n_name;
		long	n_strx;
	} n_un;
	unsigned char n_type;
	char	n_other;
	short	n_desc;
	unsigned long n_value;
};

#define	N_UNDF	0
#define	N_ABS	2
#define	N_TEXT	4
#define	N_DATA	6
#define	N_BSS	8
#define	N_FN	0x1e

#define	N_EXT	01

#define	N_STAB	0xe0

#include <stdio.h>

static FILE *inf;
static struct exec bh;
static long seg_strings;
static struct outhead hh;

#define readf(a, b, c)	(fread((char *)(a), (b), (int)(c), inf))

int
rd_open(f)
  char	*f;
{
  if ((inf = fopen(f, "r")) == NULL) return 0;
  return 1;
}

rd_ohead(h)
  struct outhead	*h;
{
  if (! readf(&bh, sizeof(struct exec), 1)) rd_fatal();
  if (N_BADMAG(bh)) rd_fatal();

  h->oh_magic = O_CONVERTED;
  h->oh_stamp = 0;
  h->oh_nsect = 4;
  h->oh_nname = 3 + bh.a_syms / sizeof(struct nlist);
  h->oh_nrelo = (bh.a_trsize + bh.a_drsize) / RELOC_SIZE;
  h->oh_flags = h->oh_nrelo ? HF_LINK : 0;
#if defined(sun)
  if (bh.a_magic == ZMAGIC) bh.a_text -= sizeof(struct exec);
#endif
  h->oh_nemit = bh.a_text + bh.a_data;
#if defined(sun)
  if (bh.a_magic == ZMAGIC) bh.a_text += sizeof(struct exec);
#endif
  fseek(inf, N_STROFF(bh), 0);
  h->oh_nchar = getw(inf) + 6 + 6 + 5 - 4; /* ".text", ".data", ".bss",
					      minus the size word */
  seg_strings = h->oh_nchar - 17;
  fseek(inf, N_TXTOFF(bh)+bh.a_text+bh.a_data, 0);
  hh = *h;
#if defined(sun)
  if (bh.a_magic == ZMAGIC) bh.a_text -= sizeof(struct exec);
#endif
}

/*ARGSUSED1*/
rd_name(names, count)
  register struct outname	*names;
  unsigned int		count;	/* ignored; complete namelist is read */
{
  names->on_valu = 0; names->on_foff = seg_strings + OFF_CHAR(hh);
  names->on_desc = 0; names->on_type = S_MIN | S_SCT;
  names++;
  names->on_valu = 0; names->on_foff = seg_strings + OFF_CHAR(hh) + 6;
  names->on_desc = 0; names->on_type = (S_MIN+2) | S_SCT;
  names++;
  names->on_valu = 0; names->on_foff = seg_strings + OFF_CHAR(hh) + 12;
  names->on_desc = 0; names->on_type = (S_MIN+3) | S_SCT;
  names++;
  count = bh.a_syms / sizeof(struct nlist);
  while (count > 0) {
	struct nlist n;

	if (! readf(&n, sizeof(struct nlist), 1)) rd_fatal();
	count--;
	names->on_desc = n.n_desc;
	if (n.n_un.n_strx - 4 < 0) names->on_foff = 0;
	else names->on_foff = OFF_CHAR(hh) - 4 + n.n_un.n_strx;
	names->on_valu = n.n_value;

	if (n.n_type & N_STAB) {
		names->on_type = n.n_type << 8;
		names++;
		continue;
	}
	switch(n.n_type & ~N_EXT) {
	case N_ABS:
		names->on_type = S_ABS;
		break;
	case N_TEXT:
		names->on_type = S_MIN;
		break;
	case N_DATA:
		names->on_type = S_MIN + 2;
		names->on_valu -= bh.a_text;
		break;
	case N_BSS:
		names->on_type = S_MIN + 3;
		names->on_valu -= bh.a_text + bh.a_data;
		break;
	case N_UNDF:
		if (! names->on_valu) {
			names->on_type = S_UND;
			break;
		}
		names->on_type = (S_MIN + 3) | S_COM;
		break;
	case N_FN:
		names->on_type = S_FIL;
		break;
	default:
		rd_fatal();
	}
	if (n.n_type & N_EXT) names->on_type |= S_EXT;
	names++;
  }
}

extern char	*strcpy();

rd_string(strings, count)
  register char	*strings;
  long	count;
{
#if defined(sun)
  if (bh.a_magic == ZMAGIC) bh.a_text += sizeof(struct exec);
#endif
  fseek(inf, N_STROFF(bh)+4, 0);
  if (! readf(strings, (int)count-17, 1)) rd_fatal();
  strings += count-17;
  strcpy(strings, ".text"); strings += 6;
  strcpy(strings, ".data"); strings += 6;
  strcpy(strings, ".bss");
}

rd_close()
{
  fclose(inf);
}

#endif

#if defined(i386)
#include <stdio.h>
#include <alloc.h>

struct xexec {
	unsigned short  x_magic;
#define XMAGIC  01006
	unsigned short  x_ext;
	long            x_text;
	long            x_data;
	long            x_bss;
	long            x_syms;
	long            x_reloc;
	long            x_entry;
	char		x_cpu;
	char		x_relsym;
	unsigned short	x_renv;
};

struct xseg {
	unsigned short	xs_type;
	unsigned short	xs_attr;
	unsigned short	xs_seg;
	unsigned short	xs_sres;
	long		xs_filpos;
	long		xs_psize;
	long		xs_vsize;
	long		xs_rbase;
	long		xs_lres;
	long		xs_lres2;
};

static FILE *inf;
static struct outname *names;
static char *strings;

#define readf(a, b, c)	(fread((char *)(a), (b), (int)(c), inf))

#define getshort(val, p)	(val = (*p++ & 0377), val |= (*p++ & 0377) << 8)
#define getlong(val, p)		(val = (*p++ & 0377), \
				 val |= (*p++ & 0377) << 8, \
				 val |= (*p++ & 0377L) << 16, \
				 val |= (*p++ & 0377L) << 24)
static 
get_names(h, sz)
  struct outhead	*h;
  long sz;
{
  register char	*xnms = malloc((unsigned) sz);
  register char *p;
  register struct outname *onm = (struct outname *) malloc((((unsigned)sz+8)/9)*sizeof(struct outname));
  struct  xnm {
	unsigned short s_type, s_seg;
	long	s_value;
  } xnm;

  if (xnms == 0 || onm == 0) No_Mem();
  if (!readf(xnms, (unsigned) sz, 1)) rd_fatal();

  names = onm;
  strings = p = xnms;
  while (sz > 0) {
	getshort(xnm.s_type, xnms);
	getshort(xnm.s_seg, xnms);
	getlong(xnm.s_value, xnms);
	onm->on_desc = 0;
	if (xnm.s_type & S_STB) {
		onm->on_type = xnm.s_type;
		onm->on_desc = xnm.s_seg;
	}
	else {
		switch(xnm.s_type & 0x1f) {
		case 0x1f:
			onm->on_type = S_FIL;
			break;
		case 0x8:
			onm->on_type = S_SCT;
			break;
		case 0:
			onm->on_type = S_UND;
			break;
		case 1:
			onm->on_type = S_ABS;
			break;
		default:
			onm->on_type = xnm.s_type & 0x1f;
			break;
		}
	}
	if (xnm.s_type & 0x20) onm->on_type |= S_EXT;
	onm->on_valu = xnm.s_value;
	sz -= 9;
	if (*xnms == '\0') {
		onm->on_foff = -1;
		xnms++;
	}
	else {
		onm->on_foff = p - strings;
		while (*p++ = *xnms++)  sz--;
	}
	onm++;
  }
  h->oh_nname = onm - names;
  h->oh_nchar = p - strings;
  while (--onm >= names) {
	if (onm->on_foff == -1) onm->on_foff = 0;
	else onm->on_foff += OFF_CHAR(*h);
  }
  names = (struct outname *) realloc((char *) names, h->oh_nname * sizeof(struct outname));
  strings = realloc(strings, (unsigned) h->oh_nchar);
}

int
rd_open(f)
  char	*f;
{
  if ((inf = fopen(f, "r")) == NULL) return 0;
  return 1;
}

rd_ohead(h)
  struct outhead	*h;
{
  int sepid;
  struct xexec xhdr;
  struct xseg xseg[3];

  if (! readf(&xhdr, sizeof(xhdr), 1)) rd_fatal();
  if (xhdr.x_magic != XMAGIC) rd_fatal();
  h->oh_magic = O_CONVERTED;
  h->oh_stamp = 0;
  h->oh_nsect = 4;
  h->oh_nrelo = 0;
  h->oh_flags = 0;
  h->oh_nemit = xhdr.x_text+xhdr.x_data;
  sepid = (xhdr.x_renv & 02) ? 1 : 0;
  fseek(inf, 0140L, 0);
  if (! readf(&xseg[0], sizeof(xseg[0]), sepid + 2)) rd_fatal();
  fseek(inf, xseg[sepid+1].xs_filpos, 0);
  get_names(h, xhdr.x_syms);
  fclose(inf);
}

rd_name(nm, count)
  struct outname	*nm;
  unsigned int		count;
{
  memcpy(nm, names, (int) count * sizeof(struct outname));
  free((char *) names);
}

rd_string(nm, count)
  char		*nm;
  long		count;
{
  memcpy(nm, strings, (int) count);
  free((char *) strings);
}

rd_close()
{
}
#endif
