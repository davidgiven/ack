/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* F I L E   I N F O R M A T I O N   S T R U C T U R E */

struct file_info {
	unsigned int	fil_lino;
	int		fil_nestlow;
	char		*fil_name;
	char		*fil_wdir;
};

#define nestlow		finfo.fil_nestlow
#define LineNumber	finfo.fil_lino
#define FileName	finfo.fil_name
#define WorkingDir	finfo.fil_wdir

extern struct file_info finfo;	/* input.c */
