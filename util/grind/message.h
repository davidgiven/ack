/* $Header$ */

#define BUFLEN	24	/* size of buffer in message header */

struct message_hdr {
  int	m_type;
/* Possible values of m_type: */
#define DB_RUN	020000	/* set for commands that cause child to run */
#define	SETBP	 0	/* set breakpoint at address in m_size */
#define	CLRBP	 1	/* clear breakpoint at address in m_size */
#define	SETSS	 (2|DB_RUN)	/* set single stepping, # of steps in m_size */
#define SETSSF	 (3|DB_RUN)	/* set single stepping, counting calls as one step */
#define	GETEMREGS 4	/* get EM registers, m_size contains level */
#define	GETBYTES 5	/* get data; m_size contains size, m_buf contains address */
#define GETSTR	 6	/* get string; m_buf contains address */
#define SETBYTES 7	/* set data; m_buf contains address, m_size contains size */
#define CALL	 8	/* call function; 
			   m_size contains size of parameter buffer,
			   m_buf contains address + size of function result
			*/
#define CONT	 (9|DB_RUN)	/* continue */
#define	SETEMREGS 10	/* set EM registers, m_size contains level
			   Actually, only the program counter is set.
			*/
#define DB_SS	040000	/* debugger wants single stepping (to be orred with
			   SETSS(F) or CONT
			*/
#define CLRSS	12	/* clear single stepping */
#define DUMP	13	/* dump command */
#define DGLOB	14	/* data area */
#define DSTACK	15	/* stack area */
#define SETTRACE 16	/* start tracing; range in m_mes */
#define CLRTRACE 17	/* end tracing */

#define	OK	50	/* answer of child to most messages */
#define FAIL	51	/* answer of child when something goes wrong */
#define DATA	52	/* answer of child when data requested */
#define END_SS	53	/* when stopped because of user single stepping */
#define INTR	54	/* sent on interrupt */
  long	m_size;		/* size */
  char	m_buf[BUFLEN];	/* some of the data required included in message */
};

#define	LB_OFF	0
#define AB_OFF	1
#define PC_OFF	2
#define HP_OFF	3
#define SP_OFF	4

#define IN_FD	3
#define OUT_FD	6
