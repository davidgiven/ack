/* $Header$ */

#define BUFLEN	32	/* size of buffer in message header */

#define M_DB_RUN	0100	/* set for commands that cause child to run */
#define M_DB_SS		0200	/* debugger wants single stepping (to be orred
				   with SETSS(F) or CONT
				*/

#define m_type		m_buf[0]

#define PS		4
#define LS		4

struct message_hdr {
  char	m_buf[BUFLEN];
/* Possible values of m_buf[0]: */
#define	M_SETBP		0	/* set breakpoint at address in next PS bytes */
#define	M_CLRBP		1	/* clear breakpoint at address in next PS bytes */
#define	M_SETSS		(2|M_DB_RUN)
				/* set single stepping, # of steps in next LS bytes */
#define M_SETSSF	(3|M_DB_RUN)
				/* set single stepping,
				   counting calls as one step
				*/
#define	M_GETEMREGS	4	/* get EM registers, level in next LS bytes */
#define	M_GETBYTES	5	/* get data; size in next LS bytes,
				   address in next PS bytes
				*/
#define M_GETSTR	6	/* get string; max size in next LS bytes,
				   address in next PS bytes
				*/
#define M_SETBYTES	7	/* get data; size in next LS bytes,
				   address in next PS bytes
				*/
/* #define M_CALL	 	8	/* call function; not implemented */
#define M_CONT		(9|M_DB_RUN)
				/* continue */
#define	M_SETEMREGS	10	/* set EM registers, level in next LS bytes;
				   Actually, only the program counter is set.
				*/
#define M_CLRSS		12	/* clear single stepping */
#define M_DUMP		13	/* dump command */
#define M_DGLOB		14	/* data area */
#define M_DSTACK	15	/* stack area */
#define M_SETTRACE	16	/* start tracing; range in next two PS bytes */
#define M_CLRTRACE	17	/* end tracing */

#define	M_OK		50	/* answer of child to most messages */
#define M_FAIL		51	/* answer of child when something goes wrong */
#define M_DATA		52	/* answer of child when data requested */
#define M_END_SS	53	/* stopped because of user single stepping */
#define M_INTR		54	/* sent on interrupt */
};

#define	LB_OFF	(0*PS+LS+1)
#define AB_OFF	(1*PS+LS+1)
#define PC_OFF	(2*PS+LS+1)
#define HP_OFF	(3*PS+LS+1)
#define SP_OFF	(4*PS+LS+1)
