#define	ONE_BYTE	int
#define TWO_BYTES	int
#define FOUR_BYTES	long

#define EM_WSIZE	4
#define EM_PSIZE	4
#define EM_BSIZE	0

#define	BSS_INIT	0

#define NAME_FMT	"_%s"
#define DNAM_FMT	"_%s"
#define DLB_FMT		"I_%ld"
#define	ILB_FMT		"I%03d%ld"
#define HOL_FMT		"hol%d"

#define GENLAB		'I'	/* compiler generated labels start with I */

#define ALIGN_FMT       ""

#define	BYTE_FMT		".byte %ld\n"
#define	WORD_FMT		".word %ld\n"
#define	LONG_FMT  		".long %ld\n"
#define	BSS_FMT			".space %ld\n"

#define	SEGTXT_FMT  		".text\n"
#define	SEGDAT_FMT		".data\n"
#define	SEGBSS_FMT		".data\n"

#define	SYMBOL_DEF_FMT		"%s :"
#define	GLOBAL_FMT		".globl %s\n"
#define	LOCAL_FMT		""

#define	RELOC1_FMT		".byte %s + %ld\n"
#define	RELOC2_FMT		".word %s + %ld\n"
#define	RELOC4_FMT		".long %s + %ld\n"

#define fit_6bits(val)		((unsigned long)(val) < 64)
#define fit_byte(val)		((unsigned long)((val)+128) < 256)
#define fit_word(val)		((unsigned long)((val)+32768L) < 65536L)
 
