/*
	These are descriptions of the fields of the structs as returned
	by the MON instruction.  Each field is described by its offset and
	its length.  The offset may be dependent on the word size, which
	is supposed to be given by  wsize  . (This  wsize  should actually
	be a parameter to all #defines, but this is not done to avoid
	excessive clutter.)
	
	The description is intended as one parameter for a routine that
	expects two parameters, the offset and the length, both ints.
*/

/* $Id$ */

/* struct stat */
#define	V7st_dev	0L, 2L			/* short */
#define	V7st_ino	2L, 2L			/* unsigned short */
#define V7st_mode	4L, 2L			/* unsigned short */
#define	V7st_nlink	6L, 2L			/* short */
#define V7st_uid	8L, 2L			/* short */
#define V7st_gid	10L, 2L			/* short */
#define	V7st_rdev	12L, 2L			/* short */
#define	V7st_align1	((14 + wsize - 1) / wsize * wsize)
#define	V7st_size	V7st_align1 + 0L, 4L	/* long */
#define	V7st_atime	V7st_align1 + 4L, 4L	/* long */
#define	V7st_mtime	V7st_align1 + 8L, 4L	/* long */
#define	V7st_ctime	V7st_align1 + 12L, 4L	/* long */
#define	V7st_sz		V7st_align1 + 16L

/* struct timeb */
#define	V7tb_time	0L, 4L			/* long */
#define	V7tb_millitm	4L, 2L			/* unsigned short */
#define	V7tb_timezone	6L, 2L			/* short */
#define	V7tb_dstflag	8L, 2L			/* short */
#define	V7tb_sz		10L

/* struct tms */
#define	V7tms_utime	0L, 4L			/* long */
#define	V7tms_stime	4L, 4L			/* long */
#define	V7tms_cutime	8L, 4L			/* long */
#define	V7tms_cstime	12L, 4L			/* long */
#define	V7tms_sz	16L

/* struct sgttyb */
#define	V7sg_ispeed	0L, 1L			/* char */
#define	V7sg_ospeed	1L, 1L			/* char */
#define	V7sg_erase	2L, 1L			/* char */
#define	V7sg_kill	3L, 1L			/* char */
#define	V7sg_flags	4L, 2L			/* short */
#define	V7sg_sz		6L

/* struct tchars */
#define	V7t_intrc	0L, 1L			/* char */
#define	V7t_quitc	1L, 1L			/* char */
#define	V7t_startc	2L, 1L			/* char */
#define	V7t_stopc	3L, 1L			/* char */
#define	V7t_eofc	4L, 1L			/* char */
#define	V7t_brkc	5L, 1L			/* char */
#define	V7t_sz_tch	6L

/* struct ltchars */
#define	V7t_suspc	0L, 1L			/* char */
#define	V7t_dsuspc	1L, 1L			/* char */
#define	V7t_rprntc	2L, 1L			/* char */
#define	V7t_flushc	3L, 1L			/* char */
#define	V7t_werasc	4L, 1L			/* char */
#define	V7t_lnextc	5L, 1L			/* char */
#define	V7t_sz_ltch	6L

