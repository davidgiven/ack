/* $Header$ */

#define BUFLEN	24	/* size of buffer in message header */

struct message_hdr {
  int	m_type;
/* Possible values of m_type: */
#define M_DB_RUN	020000	/* set for commands that cause child to run */
#define	M_SETBP	 0	/* set breakpoint at address in m_size */
#define	M_CLRBP	 1	/* clear breakpoint at address in m_size */
#define	M_SETSS	 (2|M_DB_RUN)	/* set single stepping, # of steps in m_size */
#define M_SETSSF (3|M_DB_RUN)	/* set single stepping, counting calls as one step */
#define	M_GETEMREGS 4	/* get EM registers, m_size contains level */
#define	M_GETBYTES 5	/* get data; m_size contains size, m_buf contains address */
#define M_GETSTR	 6	/* get string; m_buf contains address */
#define M_SETBYTES 7	/* set data; m_buf contains address, m_size contains size */
#define M_CALL	 8	/* call function; 
			   m_size contains size of parameter buffer,
			   m_buf contains address + size of function result
			*/
#define M_CONT	 (9|M_DB_RUN)	/* continue */
#define	M_SETEMREGS 10	/* set EM registers, m_size contains level
			   Actually, only the program counter is set.
			*/
#define M_DB_SS	040000	/* debugger wants single stepping (to be orred with
			   SETSS(F) or CONT
			*/
#define M_CLRSS	12	/* clear single stepping */
#define M_DUMP	13	/* dump command */
#define M_DGLOB	14	/* data area */
#define M_DSTACK	15	/* stack area */
#define M_SETTRACE 16	/* start tracing; range in m_mes */
#define M_CLRTRACE 17	/* end tracing */

#define	M_OK	50	/* answer of child to most messages */
#define M_FAIL	51	/* answer of child when something goes wrong */
#define M_DATA	52	/* answer of child when data requested */
#define M_END_SS	53	/* when stopped because of user single stepping */
#define M_INTR	54	/* sent on interrupt */
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
