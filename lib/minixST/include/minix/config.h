/* This file sets configuration parameters for the MINIX kernel, FS, and MM.
 * It is divided up into two main sections.  The first section contains
 * user-settable parameters.  In the second section, various internal system
 * parameters are set based on the user-settable parameters.
 */

/*===========================================================================*
 *		This section contains user-settable parameters		     *
 *===========================================================================*/

/* MACHINE must be set to one of the machine types list below. */
#define MACHINE        ATARI	/* Must be one of the names listed below */
#define IBM_PC             1	/* any  8088 or 80x86-based system */
#define SUN_4             40	/* any SUN SPARC-based system */
#define ATARI             60	/* ATARI ST (68000) */
#define AMIGA             61	/* Commodore Amiga (68000) */
#define MACINTOSH         62	/* Apple Macintosh (68000) */


/* If ROBUST is set to 1, writes of i-node, directory, and indirect blocks
 * from the cache happen as soon as the blocks are modified.  This gives a more
 * robust, but slower, file system.  If it is set to 0, these blocks are not
 * given any special treatment, which may cause problems if the system crashes.
 */
#define ROBUST             0	/* 0 for speed, 1 for robustness */

/* If HAVE_SCATTERED_IO is set to 1, scattered I/O is enabled. */
/* DEBUG - 1.4b will always use this. */
#define HAVE_SCATTERED_IO  1


/* The buffer cache should be made as large as you can afford. */
#if INTEL_32BITS
#define NR_BUFS          320	/* # blocks in the buffer cache */
#define NR_BUF_HASH      512	/* size of buf hash table; MUST BE POWER OF 2*/
#else
#define NR_BUFS           30	/* # blocks in the buffer cache */
#define NR_BUF_HASH       32	/* size of buf hash table; MUST BE POWER OF 2*/
#endif


/* Defines for kernel configuration. */
#define AUTO_BIOS          0	/* xt_wini.c - use Western's autoconfig BIOS */
#define C_RS232_INT_HANDLERS 0	/* rs232.c - use slower C int handlers */
#define DEFAULT_CLASS      0	/* floppy.c - 3 or 5 to get only that size */
#define LINEWRAP           0	/* console.c - wrap lines at column 80 */
#define NO_HANDSHAKE       1	/* rs232.c - don't use CTS/RTS handshaking */

/* These configuration defines control debugging and unfinished code. */
#define FLOPPY_TIMING      0	/* floppy.c - for fine tuning floppy driver */
#define MONITOR		   0	/* xt_wini.c - monitor loop in w_wait_int */
#define RECORD_FLOPPY_SKEW 0	/* floppy.c - for deciding nr_sectors */

/* These configuration defines control worthless code. */
#define SPARE_VIDEO_MEMORY 0	/* misc.c - use memory from any 2nd vid card */
#define SPLIMITS           0	/* mpx*.x - set stack limits (never checked) */

#if (MACHINE == ATARI)
/* this define says whether the keyboard generates VT100 or IBM_PC escapes */
#define KEYBOARD	   VT100 /* either VT100 or IBM_PC */
#define VT100		   100
/* IBM_PC is already defined above. Use IBM_PC to be Minix ST 1.1 compatible */

/* the next define says whether you have SUPRA or ATARI extended partitioning */
/* hook for future extensions; not yet implemented; not yet used */
#define PARTITIONING	   SUPRA /* either SUPRA or ATARI */
#define SUPRA		   1
/* ATARI is already defined */

/* define the number of hard disk drives on your system */
/* (assuming 2 drives/controller) */
#define NR_DRIVES	   1	/* typically 0 or 1 */
#endif


/*===========================================================================*
 *	There are no user-settable parameters after this line		     *
 *===========================================================================*/
/* Set the CHIP type based on the machine selected. The symbol CHIP is actually
 * indicative of more than just the CPU.  For example, machines for which
 * CHIP == INTEL are expected to have 8259A interrrupt controllers and the
 * other properties of IBM PC/XT/AT/386 types machines in general. */
#define INTEL             1	/* CHIP type for PC, XT, AT, 386 and clones */
#define M68000            2	/* CHIP type for Atari, Amiga, Macintosh    */
#define SPARC             3	/* CHIP type for SUN-4 (e.g. SPARCstation)  */

#if MACHINE == IBM_PC
#define CHIP           INTEL
#endif

#if (MACHINE == ATARI) | (MACHINE == AMIGA) | (MACHINE == MACINTOSH)
#define CHIP          M68000
#endif

#if (MACHINE == SUN_4)
#define CHIP           SPARC
#endif

#if MACHINE == ATARI
#define ASKDEV             1	/* ask for boot device */
#define FASTLOAD           1	/* use multiple block transfers to init ram */
#endif

/* The file buf.h uses MAYBE_WRITE_IMMED. */
#if ROBUST
#define MAYBE_WRITE_IMMED  WRITE_IMMED	/* slower but perhaps safer */
#else
#define MAYBE_WRITE_IMMED  0		/* faster */
#endif

#ifndef MACHINE
#error "In <minix/config.h> please define MACHINE"
#endif

#ifndef CHIP
#error "In <minix/config.h> please define MACHINE to have a legal value"
#endif

#if (MACHINE == 0)
#error "MACHINE has incorrect value (0)"
#endif
