#define BYTES_REVERSED
#define WORDS_REVERSED

#define ONE_BYTE        int
#define TWO_BYTES	int
#define FOUR_BYTES	long

#define EM_WSIZE	4
#define EM_PSIZE	4
#define EM_BSIZE	8

#define	BSS_INIT	0

#define NAME_FMT	"_%s"
#define DNAM_FMT	"_%s"
#define DLB_FMT		"I_%ld"
#define	ILB_FMT		"I%x_%lx"
#define HOL_FMT		"hol%d"

#define GENLAB		'I'

#define	ALIGN_FMT		".align\n"

#define	BYTE_FMT		".data1 %ld\n"
#define	WORD_FMT		".data2 %ld\n"
#define	LONG_FMT  		".data4 %ld\n"
#define	BSS_FMT			".space %ld\n"

#define	SEGTXT_FMT  		".sect .text\n"
#define	SEGDAT_FMT		".sect .data\n"
#define	SEGBSS_FMT		".sect .bss\n"

#define	SYMBOL_DEF_FMT		"%s :\n"
#define	GLOBAL_FMT		".extern %s\n"
#define	LOCAL_FMT		""

#define	RELOC1_FMT		".data1 %s + %ld\n"
#define	RELOC2_FMT		".data2 %s + %ld\n"
#define	RELOC4_FMT		".data4 %s + %ld\n"




#define small( x)	( 1 <= (x) && (x) <= 8)
