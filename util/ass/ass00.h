/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <em_spec.h>
#include <as_spec.h>
#include <em_flag.h>
#include <arch.h>
#include <local.h>

#define	RCS_ASS	"$Id$"

/*
 * compile time options
 */

#define DUMP            1       /* dump between passes */
/*  #define TIMING          1       /* some timing measurements */
#define JOHAN           1       /* dump the loaded instructions */
#define MEMUSE          1       /* print memory usage statistics */

#ifndef DUMP
#define dump(x)                  /* nothing */
#endif

#ifndef TIMING
#define timing()                /* nothing */
#endif

#ifndef MEMUSE
#define memuse()                /* nothing */
#endif

/* Used to clear the upper byte(s) of characters.
   Not nessecary if your C-compiler does not sign-extend char's
*/

#ifdef CPM
# define        LC(ch)   ( ((ch)<'A' | (ch)>'Z' ) ? (ch) : ((ch)-('A'-'a')))
#else
# define        LC(ch)          (ch)
#endif

#define ctrunc(val)             ( (val)&0377 )

#define odd(n)                  ((n)&1)         /* Boolean odd function */

#define lnp_cast                (line_t *)
#define gbp_cast                (glob_t *)
#define lbp_cast                (locl_t *)
#define prp_cast                (proc_t *)
#define ptp_cast                (ptab_t *)
#define rlp_cast                (relc_t *)
#define pst_cast                (stat_t *)
#define chp_cast                (char   *)
#define ipp_cast                (int   **)
#define iip_cast                (int    *)
#define int_cast                (int     )

typedef struct lines            line_t;
typedef struct loc_label        locl_t;
typedef struct glob_label       glob_t;
typedef struct rel              relc_t;
typedef struct procstat         stat_t;
typedef struct sizes            siz_t;
typedef struct ar_hdr           arch_t;
typedef struct procs            proc_t;
typedef struct proctab          ptab_t;
typedef char *                  area_t;
typedef long                    cons_t;

typedef union {
	cons_t  ad_i;
	locl_t  *ad_lp;
	glob_t  *ad_gp;
	proc_t  *ad_pp;
	struct sad_ln {
		short   ln_extra;
		short   ln_first;
	}       ad_ln ;
	struct sad_df {
		cons_t  df_i;
		glob_t  *df_gp;
	}       ad_df;
}       addr_u;

typedef union {
	cons_t  rel_i;
	locl_t  *rel_lp;
	glob_t  *rel_gp;
} rel_u;

#define FOFFSET         long             /* offset into file */

/*
 * Global variables and definitions for EM1-assembler/loader
 */

#define DEFINING        0       /* parameters for glolookup */
#define OCCURRING       1
#define INTERNING       2
#define EXTERNING       3
#define SEARCHING       4
#define ENTERING        5

#define PRO_OCC         0       /* parameters for prolookup */
#define PRO_DEF         1
#define PRO_INT         2
#define PRO_EXT         3

#define TRUE            1
#define FALSE           0

#define MAXBYTE 255

#define MAXSTRING       200     /* Maximum string length accepted */
#define LOCLABSIZE      128     /* size of local label hash table */
				/* may not be smaller */
#define ABSSIZE         8

struct  lines {
	char    instr_num;      /* index into mnemon[] */
	char    type1;          /* see below */
	line_t  *l_next;        /* next in chain */
	char    *opoff;         /* pointer into opchoice[] */
	addr_u  ad;             /* depending on type, various pointers */
};

/* contents of type1 */
#define MISSING         0       /* no operand */
#define CONST           1       /* ad contains operand */
#define PROCNAME        2       /* ad contains struct procs pointer */
#define GLOSYM          3       /* ad contains pointer into mproc[] */
#define LOCSYM          4       /* ad contains pointer into locs[] */
#define GLOOFF          5       /* ad contains CONST and GLOSYM in ad_df */
#define LINES           6       /* Line number setting, only param of pseudo*/
#define VALLOW          7       /* value's between LOW and HIGH are x-MID */
#define VALMID         50
#define VALHIGH       127       /* to avoid sign extension problems */

#define VAL1(x)         ((x)-VALMID)

/* Used to indicate a invalid contents of opoff */
#define NO_OFF          ((char *)-1)

struct  loc_label {
	locl_t  *l_chain;       /* The next label with same low order bits */
	char    l_hinum;        /* high bits of number of label */
	char    l_defined;      /* see below */
	int     l_min,l_max;    /* boundaries of value */
};

/* The structure containing procedure pertinent data */
/* Used for environment stacking for nested PRO's */

struct  procstat  {
	line_t  *s_fline;       /* points to first line of procedure */
	locl_t  (*s_locl)[1];    /* pointer to local labels */
	proc_t  *s_curpro;      /* identifies current procedure */
	relc_t  *s_fdata;       /* last datareloc before procedure */
	stat_t  *s_prevstat;    /* backward chain of nested procedures */
} ;

/* contents of l_defined */
#define EMPTY           0       /* Empty slot */
#define NO              1       /* not defined yet */
#define YES             2       /* defined */
#define SEEN            3       /* intermediate state */
#define NOTPRESENT      4       /* Undefined and error message given */

struct  glob_label {
	char    *g_name;
	char    g_status;               /* see below */
	union {
		cons_t  g_addr;         /* value if status&DEF */
		struct glob_label *g_gp; /* ref. to xglobs */
	} g_val ;
};

#define glostring(gl)   ((gl)->g_name)

/* contents of g_status */
#define DEF             01      /* defined */
#define OCC             02      /* used */
#define EXT             04      /* external */

struct  rel {                   /* for relocation tables */
	relc_t  *r_next;        /* chain */
	FOFFSET r_off;          /* offset in text/data of word to relocate */
	rel_u   r_val;          /* constant or pointer to global symbol */
	int     r_typ;          /* different use in text or data */
};

/*
 * When used with textrelocation r_typ contains the flag bits as defined
 * in ip_spec.h together with the RELMNS bit if r_val contains an integer
 */

#define RELMNS          020000          /* indicates integer i.s.o. glob */

/* Contents of r_typ when used with data relocation */
#define RELNULL         0
#define RELGLO          1
#define RELHEAD         2
#define RELLOC          3
#define RELADR          4

/* modes of data output */
#define DATA_NUL        0
#define DATA_REP        1
#define DATA_CONST      2
#define DATA_BSS        3
#define DATA_DPTR       4
#define DATA_IPTR       5
#define DATA_ICON       6
#define DATA_UCON       7
#define DATA_FCON       8
#define DATA_BYTES      9

/* name of procedure to be called first */
#define MAIN            "_m_a_i_n"

/* headers of datablocks written */
#define HEADREP         0
#define HEADBSS         1
#define HEADBYTE        2
#define HEADCONST       3
#define HEADDPTR        4
#define HEADIPTR        5
#define HEADICON        6
#define HEADUCON        7
#define HEADFCON        8

#define NDEFAULT        4       /* number of different sizes available */
struct  sizes {
	int     n_mlab;         /* # of global labels per module */
	int     n_glab;         /* # of extern global labels */
	int     n_mproc;        /* # of local procs per module */
	int     n_xproc;        /* # of external procs */
	int     n_proc;         /* total # of procedures */
};

struct  procs {                 /* format of mprocs[] and xprocs[] */
	char    *p_name;
	char    p_status;       /* same bits as g_status except REL */
	int     p_num;          /* unique procedure descriptor */
};

struct  proctab {
	cons_t  pr_off;         /* distance from pb */
	cons_t  pr_loc;         /* number of bytes locals */
};
