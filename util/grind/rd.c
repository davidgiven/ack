/* $Header$ */

/* a.out file reading ... */

#include "rd.h"
#include "misc.h"
#include <assert.h>
#include <alloc.h>

#if defined(__sun)
#if ! defined(sun)
#define sun
#endif
#endif

#if defined(__i386)
#define i386
#endif

#if defined(__mc68020)
#define mc68020
#endif

#if defined(__sparc)
#if ! defined(sparc)
#define sparc
#endif
#endif

#if defined(__vax)
#define vax
#endif

#if defined(__solaris) || defined(__solaris__)
#define solaris
#endif

#if ! defined(solaris)
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

#if defined(solaris)
#include <libelf.h>
#include <sys/elf_M32.h>
#include <stb.h>

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

static int fildes;
static Elf *elf;
static Elf32_Ehdr *ehdr;
static struct nlist *dbtab;
static char *dbstringtab;
static Elf32_Sym *tab;
static char *stringtab;
static struct outhead hh;
static struct nlist *maxdn;

#define	N_STAB	0xe0

int
rd_open(f)
  char	*f;
{
  if ((fildes = open(f, 0)) < 0) return 0;
  elf_version(EV_CURRENT);
  if ((elf = elf_begin(fildes, ELF_C_READ, (Elf *) 0)) == 0) {
	close(fildes);
	return 0;
  }
  if ((ehdr = elf32_getehdr(elf)) == NULL) {
	elf_end(elf);
	close(fildes);
	return 0;
  }
  return 1;
}

rd_ohead(h)
  struct outhead	*h;
{
  Elf_Scn *scn = 0;
  Elf32_Shdr *shdr;
  Elf_Data *sectnames;
  Elf_Data *dt;
  register struct nlist *dn;
  register Elf32_Sym *n;
  long text_offset, data_offset, bss_offset, fun_offset;
  int fixnamoff = 0, newfixnamoff = 0;

  h->oh_magic = O_CONVERTED;
  h->oh_stamp = 0;
  h->oh_nsect = 4;
  h->oh_nrelo = 0;
  h->oh_flags = 0;
  h->oh_nemit = 0;
  h->oh_nname = 0;

  scn = elf_getscn(elf, (size_t) ehdr->e_shstrndx);
  sectnames = elf_getdata(scn, (Elf_Data *) 0);

  scn = 0;
  while ((scn = elf_nextscn(elf, scn)) != 0) {
	shdr = elf32_getshdr(scn);
	switch(shdr->sh_type) {
	case SHT_PROGBITS:
		/* Get stab symbol table. Elf does not know about it,
		   and, unfortunately, no relocation is done on it.
		*/
		h->oh_nemit += shdr->sh_size;
		if (! strcmp(".stab", (char *)(sectnames->d_buf)+shdr->sh_name)) {
  			dt = elf_getdata(scn, (Elf_Data *) 0);
			if (dt->d_size == 0) {
				fatal("(part of) symbol table is missing");
			}
			dbtab = (struct nlist *) Malloc(dt->d_size);
			memcpy((char *) dbtab, (char *) dt->d_buf, dt->d_size);
			maxdn = (struct nlist *)((char *)dbtab+dt->d_size);
			break;
		}
		break;

	case SHT_STRTAB:
		/* Get the stab string table, as well as the usual string
		   table.
		*/
		if (! strcmp(".stabstr", (char *)(sectnames->d_buf)+shdr->sh_name)) {
  			dt = elf_getdata(scn, (Elf_Data *) 0);
			if (dt->d_size == 0) {
				fatal("(part of) symbol table is missing");
			}
			dbstringtab = dt->d_buf;
			h->oh_nchar = dt->d_size;
			break;
		}
		if (! strcmp(".strtab", (char *)(sectnames->d_buf)+shdr->sh_name)) {
  			dt = elf_getdata(scn, (Elf_Data *) 0);
			if (dt->d_size == 0) {
				fatal("(part of) symbol table is missing");
			}
			stringtab = dt->d_buf;
		}
		break;

	case SHT_SYMTAB:
		/* Get the symbol table. */
		if (! strcmp(".symtab", (char *)(sectnames->d_buf)+shdr->sh_name)) {
  			dt = elf_getdata(scn, (Elf_Data *) 0);
			if (dt->d_size == 0) {
				fatal("(part of) symbol table is missing");
			}
			tab = dt->d_buf;
		}
		break;
	}
  }

  /* Convert offsets in stab symbol table. */
  n = tab;
  dn = dbtab;
  while (dn < maxdn) {
	int i;

	if (dn->n_un.n_strx) {
		dn->n_un.n_strx += fixnamoff;
	}
	switch(dn->n_type) {
	case 0:
		fixnamoff = newfixnamoff;
		newfixnamoff += dn->n_value;
		break;

	case N_SO:
		h->oh_nname++;
		i = 0;
		while (i < 3) {
			while (stringtab[n->st_name] != 'B') n++;
			if (! strcmp("Btext.text", &(stringtab[n->st_name]))) {
				text_offset = n->st_value; i++;
			}
			else if (! strcmp("Bdata.data", &(stringtab[n->st_name]))) {
				data_offset = n->st_value; i++;
			}
			else if (! strcmp("Bbss.bss", &(stringtab[n->st_name]))) {
				bss_offset = n->st_value; i++;
			}
			n++;
		}
		break;

	case N_GSYM:
		h->oh_nname++;
		/* Fortunately, we don't use this in ACK, so we don't
		   have to handle it here. The problem is that we don't know
		   which segment it comes from.
		*/
		break;

	case N_STSYM:
		h->oh_nname++;
		dn->n_value += data_offset;
		break;

	case N_LCSYM:
		h->oh_nname++;
		dn->n_value += bss_offset;
		break;

	case N_FUN:
		h->oh_nname++;
		dn->n_value += text_offset;
		fun_offset = dn->n_value;
		break;

	case N_MAIN:
		dn->n_value += text_offset;
		break;

	case N_LBRAC:
	case N_RBRAC:
	case N_SLINE:
		h->oh_nname++;
		dn->n_value += fun_offset;
		break;

	case N_SOL:
	case N_EINCL:
	case N_BINCL:
	case N_PSYM:
	case N_SSYM:
	case N_SCOPE:
	case N_RSYM:
	case N_LSYM:
		h->oh_nname++;
		/* Nothing to be done. */
		break;
	}
	dn++;
  }
  hh = *h;
}

rd_name(nm, count)
  struct outname	*nm;
  unsigned int		count;
{
  register struct nlist *dn = dbtab;
  register struct outname *n = nm;
  while (dn < maxdn) {
	if (dn->n_type & N_STAB) {
		n->on_type = dn->n_type << 8;
		n->on_valu = dn->n_value;
		n->on_desc = dn->n_desc;
		if (dn->n_un.n_strx == 0) n->on_foff = 0;
		else n->on_foff = OFF_CHAR(hh) + dn->n_un.n_strx;
		n++;
	}
	dn++;
  }
  free(dbtab);
}

rd_string(nm, count)
  char		*nm;
  long		count;
{
  memcpy(nm, dbstringtab, count);
}

rd_close()
{
  elf_end(elf);
  close(fildes);
}

#endif
