#define	ONE_BYTE	char
#define TWO_BYTES	short
#define FOUR_BYTES	long

#define EM_WSIZE	4
#define EM_PSIZE	4
#define EM_BSIZE	0

#define	BSS_INIT	0

#define NAME_FMT	"_%s"
#define DNAM_FMT	"_%s"
#define DLB_FMT		"_%ld"
#define	ILB_FMT		"I%03d%ld"
#define HOL_FMT		"hol%d"


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
 
