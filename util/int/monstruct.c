/*
	Moving system structs between UNIX and EM
*/

/* $Header$ */

#include	"sysidf.h"
#include	"v7ioctl.h"
#include	"global.h"
#include	"mem.h"
#include	"monstruct.h"

#include	<sys/types.h>
#include	<sys/stat.h>
#if __STDC__
#include	<time.h>
#endif
#include	<sys/times.h>
#include	<sgtty.h>

#ifdef	BSD_X				/* from system.h */
#include	<sys/timeb.h>
#endif	BSD_X
#ifdef	SYS_V				/* from system.h */
struct timeb {	/* non-existing; we use an ad-hoc definition */
	long time;
	unsigned short millitm;
	short timezone, dstflag;
}
#endif	SYS_V

/******** System to EM memory ********/

PRIVATE mem_stfld(addr, offset, length, val)
	ptr addr;
	size offset, length;
	long val;
{
	mem_stn(addr + offset, val, length);
}

int stat2mem(addr, statb)
	ptr addr;
	struct stat *statb;
{
	if (memfault(addr, V7st_sz))
		return 0;
	mem_stfld(addr, V7st_dev, (long) statb->st_dev);
	mem_stfld(addr, V7st_ino, (long) statb->st_ino);
	mem_stfld(addr, V7st_mode, (long) statb->st_mode);
	mem_stfld(addr, V7st_nlink, (long) statb->st_nlink);
	mem_stfld(addr, V7st_uid, (long) statb->st_uid);
	mem_stfld(addr, V7st_gid, (long) statb->st_gid);
	mem_stfld(addr, V7st_rdev, (long) statb->st_rdev);
	mem_stfld(addr, V7st_size, (long) statb->st_size);
	mem_stfld(addr, V7st_atime, (long) statb->st_atime);
	mem_stfld(addr, V7st_mtime, (long) statb->st_mtime);
	mem_stfld(addr, V7st_ctime, (long) statb->st_ctime);
	return 1;
}

int timeb2mem(addr, timebb)
	ptr addr;
	struct timeb *timebb;
{
	if (memfault(addr, V7tb_sz))
		return 0;
	mem_stfld(addr, V7tb_time, (long) timebb->time);
	mem_stfld(addr, V7tb_millitm, (long) timebb->millitm);
	mem_stfld(addr, V7tb_timezone, (long) timebb->timezone);
	mem_stfld(addr, V7tb_dstflag, (long) timebb->dstflag);
	return 1;
}

int tms2mem(addr, tmsb)
	ptr addr;
	struct tms *tmsb;
{
	if (memfault(addr, V7tms_sz))
		return 0;
	mem_stfld(addr, V7tms_utime, (long) tmsb->tms_utime);
	mem_stfld(addr, V7tms_stime, (long) tmsb->tms_stime);
	mem_stfld(addr, V7tms_cutime, (long) tmsb->tms_cutime);
	mem_stfld(addr, V7tms_cstime, (long) tmsb->tms_cstime);
	return 1;
}

int sgttyb2mem(addr, sgttybb)
	ptr addr;
	struct sgttyb *sgttybb;
{
	if (memfault(addr, V7sg_sz))
		return 0;
	mem_stfld(addr, V7sg_ispeed, (long) sgttybb->sg_ispeed);
	mem_stfld(addr, V7sg_ospeed, (long) sgttybb->sg_ospeed);
	mem_stfld(addr, V7sg_erase, (long) sgttybb->sg_erase);
	mem_stfld(addr, V7sg_kill, (long) sgttybb->sg_kill);
	mem_stfld(addr, V7sg_flags, (long) sgttybb->sg_flags);
	return 1;
}

#ifdef	BSD_X				/* from system.h */
int tchars2mem(addr, tcharsb)
	ptr addr;
	struct tchars *tcharsb;
{
	if (memfault(addr, V7t_sz_tch))
		return 0;
	mem_stfld(addr, V7t_intrc, (long) tcharsb->t_intrc);
	mem_stfld(addr, V7t_quitc, (long) tcharsb->t_quitc);
	mem_stfld(addr, V7t_startc, (long) tcharsb->t_startc);
	mem_stfld(addr, V7t_stopc, (long) tcharsb->t_stopc);
	mem_stfld(addr, V7t_eofc, (long) tcharsb->t_eofc);
	mem_stfld(addr, V7t_brkc, (long) tcharsb->t_brkc);
	return 1;
}

#ifndef	V7IOCTL
int ltchars2mem(addr, ltcharsb)
	ptr addr;
	struct ltchars *ltcharsb;
{
	if (memfault(addr, V7t_sz_ltch))
		return 0;
	mem_stfld(addr, V7t_suspc, (long) ltcharsb->t_suspc);
	mem_stfld(addr, V7t_dsuspc, (long) ltcharsb->t_dsuspc);
	mem_stfld(addr, V7t_rprntc, (long) ltcharsb->t_rprntc);
	mem_stfld(addr, V7t_flushc, (long) ltcharsb->t_flushc);
	mem_stfld(addr, V7t_werasc, (long) ltcharsb->t_werasc);
	mem_stfld(addr, V7t_lnextc, (long) ltcharsb->t_lnextc);
	return 1;
}
#endif	V7IOCTL
#endif	BSD_X


/******** EM memory to system ********/

PRIVATE unsigned long mem_ldfld(addr, offset, length)
	ptr addr;
	size offset, length;
{
	return mem_ldu(addr + offset, length);
}

int mem2sgtty(addr, sgttybb)
	ptr addr;
	struct sgttyb *sgttybb;
{
	if (memfault(addr, V7sg_sz))
		return 0;
	sgttybb->sg_ispeed = (char) mem_ldfld(addr, V7sg_ispeed);
	sgttybb->sg_ospeed = (char) mem_ldfld(addr, V7sg_ospeed);
	sgttybb->sg_erase = (char) mem_ldfld(addr, V7sg_erase);
	sgttybb->sg_kill = (char) mem_ldfld(addr, V7sg_kill);
	sgttybb->sg_flags = (short) mem_ldfld(addr, V7sg_flags);
	return 1;
}

#ifdef	BSD_X				/* from system.h */
int mem2tchars(addr, tcharsb)
	ptr addr;
	struct tchars *tcharsb;
{
	if (memfault(addr, V7t_sz_tch))
		return 0;
	tcharsb->t_intrc = (char) mem_ldfld(addr, V7t_intrc);
	tcharsb->t_quitc = (char) mem_ldfld(addr, V7t_quitc);
	tcharsb->t_startc = (char) mem_ldfld(addr, V7t_startc);
	tcharsb->t_stopc = (char) mem_ldfld(addr, V7t_stopc);
	tcharsb->t_eofc = (char) mem_ldfld(addr, V7t_eofc);
	tcharsb->t_brkc = (char) mem_ldfld(addr, V7t_brkc);
	return 1;
}

#ifndef	V7IOCTL
int mem2ltchars(addr, ltcharsb)
	ptr addr;
	struct ltchars *ltcharsb;
{
	if (memfault(addr, V7t_sz_ltch))
		return 0;
	ltcharsb->t_suspc = (char) mem_ldfld(addr, V7t_suspc);
	ltcharsb->t_dsuspc = (char) mem_ldfld(addr, V7t_dsuspc);
	ltcharsb->t_rprntc = (char) mem_ldfld(addr, V7t_rprntc);
	ltcharsb->t_flushc = (char) mem_ldfld(addr, V7t_flushc);
	ltcharsb->t_werasc = (char) mem_ldfld(addr, V7t_werasc);
	ltcharsb->t_lnextc = (char) mem_ldfld(addr, V7t_lnextc);
	return 1;
}
#endif	V7IOCTL
#endif	BSD_X

