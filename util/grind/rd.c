/* $Header$ */

/* a.out file reading ... */

#include "rd.h"

#if defined(__sun)
#define sun
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
#if defined(sun) && defined(mc68020)
#define relocation_info	reloc_info_68k
#endif
#if defined(sun) && defined(sparc)
#define relocation_info	reloc_info_sparc
#endif

#include <a.out.h>
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
  h->oh_nrelo = (bh.a_trsize + bh.a_drsize) / sizeof(struct relocation_info);
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
