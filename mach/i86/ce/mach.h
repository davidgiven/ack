#define	BSS_INIT	0

#define ONE_BYTE	int
#define TWO_BYTES	int
#define FOUR_BYTES	long


#define EM_WSIZE	2
#define EM_PSIZE	2
#define EM_BSIZE	4


#define NAME_FMT	"_%s"
#define DNAM_FMT	"_%s"
#define DLB_FMT		"I_%ld"
#define	ILB_FMT		"I%03d%ld"
#define HOL_FMT		"hol%d"

#define	GENLAB		'I'

#define TRUE		1
#define FALSE		0

#define clean_push_buf()       if(push_waiting){text1(0x50);push_waiting=FALSE;}
#define assign( l, r)          l = r
extern int push_waiting;
