/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* ar - archiver        Author: Michiel Huisjes */
/* Made into arch/aal by Ceriel Jacobs
 */



/*
 * Usage: [arch|aal] [qdprtx][vlcu] archive [file] ...
 * possible key
 *    d: delete
 *    p: print named files
 *    q: append
 *    r: replace (append when not in archive)
 *    t: print contents of archive
 *    x: extract
 * possible args
 *    c: don't give "create" message
 *    u: replace only if dated later than member in archive
 *    v: verbose
 #ifdef DISTRIBUTION
 *    D: make distribution: use distr_time, uid=2, gid=2, mode=0644
 #endif
 */

#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "print.h"
#include "system.h"
#include "object.h"
#include "arch.h"
#include "ranlib.h"

/* UNIX specific */
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#ifndef unix
#define unix
#endif
#endif

#ifdef unix
#include <unistd.h>
#else
#define getuid()    0
#define getgid()    0
#endif

#ifdef AAL
#include "out.h"
#define MAGIC_NUMBER    AALMAG
long offset;
struct ranlib *tab;
unsigned int tnum = 0;
char *tstrtab;
unsigned int tssiz = 0;
unsigned int tabsz, strtabsz;
#else
#define MAGIC_NUMBER    ARMAG
#endif

#define odd(nr)     (nr & 01)
#define even(nr)    (odd(nr) ? nr + 1 : nr)

typedef char BOOL;
#define FALSE       0
#define TRUE        1

#define READ        0
#define APPEND      2
#define CREATE      1

#define MEMBER      struct ar_hdr

#define NIL_PTR     ((char *) 0)
#define NIL_MEM     ((MEMBER *) 0)
#define NIL_LONG    ((long *) 0)

#define IO_SIZE     (10 * 1024)

#define equal(str1, str2)   (!strncmp((str1), (str2), AR_NAME_MAX))

BOOL verbose;
BOOL app_fl;
BOOL ex_fl;
BOOL show_fl;
/* print files found in archive. */
BOOL pr_fl;
BOOL u_fl;
BOOL rep_fl;
BOOL del_fl;
BOOL nocr_fl;
BOOL local_fl;
#ifdef DISTRIBUTION
BOOL distr_fl;
time_t distr_time;
#endif

#ifndef S_ISUID
#define S_ISUID 0
#endif
#ifndef S_ISGID
#define S_ISGID 0
#endif

#define MODE_COUNT 11

char io_buffer[IO_SIZE];

char *progname;

char temp_buf[L_tmpnam];
char *temp_arch = &temp_buf[0];

void do_object(FILE* f, long size);
void do_names(struct outhead *headp);
void enter_name(struct outname *namep);
void write_symdef(FILE *ar);

void error(BOOL quit, char *str1, char *str2);
FILE* open_archive(char *name, int mode);
void
catch(int param);
MEMBER *get_member(FILE*);
void get(int argc, char *argv[]);
void add(char *name, FILE* ar, FILE* dst, char *mess);
void extract(FILE* ar, MEMBER *member);
void copy_member(MEMBER *member, FILE* from, FILE* to, BOOL extracting);
char *get_mode(int mode);
void wr_fatal(void);
void rd_fatal(void);
void mwrite(FILE* f, void* address, size_t bytes);
void show(char *s, char *name);

/* Conversion utilities. */
static mode_t ar2mode(short mode);
static short mode2ar(mode_t mode);

/** Maps an AR mode to the current system mode. */
struct modemap
{
	short ar_mode;
	mode_t mode;
};

/** Mapping table to map an AR mode to a system mode. */
static const struct modemap armodes[MODE_COUNT] =
{
{ AR_IRUSR, S_IRUSR },
{ AR_IWUSR, S_IWUSR },
{ AR_IXUSR, S_IXUSR },

{ AR_IRGRP, S_IRGRP },
{ AR_IWGRP, S_IWGRP },
{ AR_IXGRP, S_IXGRP },

{ AR_IROTH, S_IROTH },
{ AR_IWOTH, S_IWOTH },
{ AR_IXOTH, S_IXOTH },

{ AR_ISUID, S_ISUID },
{ AR_ISGID, S_ISGID } };

/** Convert an "ar" mode to a system specific
 *  mode.
 */
static mode_t ar2mode(short mode)
{
	int i;
	mode_t result = 0;
	for (i = 0; i < MODE_COUNT; i++)
	{
		if (mode & armodes[i].ar_mode)
		{
			result = result | armodes[i].mode;
		}
	}
	return result;
}

/** Convert a system specified mode to
 *  an ar compatible mode.
 */
static short mode2ar(mode_t mode)
{
	int i;
	short result = 0;
	for (i = 0; i < MODE_COUNT; i++)
	{
		if (mode & armodes[i].mode)
		{
			result = result | armodes[i].ar_mode;
		}
	}
	return result;
}

static void usage(void)
{
	error(TRUE, "usage: %s [qdprtxl][vc] archive [file] ...\n", progname);
}

/*VARARGS2*/
void error(BOOL quit, char *str1, char *str2)
{
	char errbuf[256];

	sprint(errbuf, str1, str2);
	fwrite(errbuf, strlen(errbuf), 1, stderr);
	if (quit)
	{
		remove(temp_arch);
		exit(1);
	}
}

/** Opens the specified archive. */
FILE* open_archive(char *name, int mode)
{
	unsigned short magic = 0;
	FILE* file = NULL;

	if (mode == CREATE)
	{
		file = fopen(name, "wb+");
		if (file == NULL)
			error(TRUE, "cannot create %s\n", name);
		magic = MAGIC_NUMBER;
		wr_int2(file, magic);
		return file;
	}

	file = fopen(name, "rb");
	if (file == NULL)
	{
		/* mode APPEND and files does not exist. */
		if (mode == APPEND)
		{
			fclose(open_archive(name, CREATE));
			if (!nocr_fl)
				error(FALSE, "creating %s\n", name);
			return open_archive(name, APPEND);
		}
		error(TRUE, "cannot open %s\n", name);
	}
	else
	/* file already exists, simply open it for appending */
	{
		if (mode == APPEND)
		{
			fclose(file);
			file = fopen(name, "a+b");
		}
	}
	fseek(file, 0, SEEK_SET);
	magic = rd_unsigned2(file);
	if (magic != AALMAG && magic != ARMAG)
		error(TRUE, "%s is not in ar format\n", name);

	return file;
}

void
catch(int param)
{
	remove(temp_arch);
	exit(2);
}

int main(int argc, char *argv[])
{
	register char *ptr;
	int needs_arg = 0;

	progname = argv[0];

	if (argc < 3)
		usage();

	for (ptr = argv[1]; *ptr; ptr++)
	{
		switch (*ptr)
		{
		case 't':
			show_fl = TRUE;
			break;
		case 'v':
			verbose = TRUE;
			break;
		case 'x':
			ex_fl = TRUE;
			break;
		case 'q':
			needs_arg = 1;
			app_fl = TRUE;
			break;
		case 'c':
			nocr_fl = TRUE;
			break;
		case 'u':
			u_fl = TRUE;
			break;
		case 'p':
			needs_arg = 1;
			pr_fl = TRUE;
			break;
		case 'd':
			needs_arg = 1;
			del_fl = TRUE;
			break;
		case 'r':
			needs_arg = 1;
			rep_fl = TRUE;
			break;
#ifdef DISTRIBUTION
			case 'D' :
			distr_fl = TRUE;
			break;
#endif
		default:
			usage();
		}
	}

	if (needs_arg && argc <= 3)
		usage();
#ifdef DISTRIBUTION
	if (distr_fl)
	{
		static struct stat statbuf;

		stat(progname, &statbuf);
		distr_time = statbuf.st_mtime;
	}
#endif
	if (sys_tmpnam(temp_arch) == NULL)
	{
		error(TRUE, "Cannot create a temporary filename\n", NULL);
	}

	if (app_fl + ex_fl + del_fl + rep_fl + show_fl + pr_fl != 1)
		usage();

	if (u_fl && !rep_fl)
		usage();

	if (rep_fl || del_fl
#ifdef AAL
			|| app_fl
#endif
			)
	{
		/*fclose(mkstemp(temp_arch));*/
	}
#ifdef AAL
	tab = (struct ranlib *) malloc(512 * sizeof(struct ranlib));
	tstrtab = malloc(4096);
	if (!tab || !tstrtab)
		error(TRUE, "Out of core\n", NULL);
	tabsz = 512;
	strtabsz = 4096;
#endif

	signal(SIGINT,
	catch);
	get(argc, argv);

	return 0;
}

/* Read next member of in the archive file "f". */
MEMBER *get_member(FILE *f)
{
	static MEMBER member;

	again:
	if (rd_arhdr(f, &member) == 0)
	return NIL_MEM;
	if (member.ar_size < 0)
	{
		error(TRUE, "archive has member with negative size\n",NULL);
	}
	if (equal(SYMDEF, member.ar_name))
	{
		fseek(f, member.ar_size, SEEK_CUR);
		goto again;
	}
	return &member;
}

void get(int argc, char *argv[])
{
	register MEMBER *member;
	FILE *ar_f;
	int i = 0;
	char buffer[FILENAME_MAX];
	size_t read_chars;
	FILE* temp_fd;

	ar_f = open_archive(argv[2], (show_fl || pr_fl || ex_fl) ? READ : APPEND);
	if (rep_fl || del_fl
#ifdef AAL
			|| app_fl
#endif
	)
	temp_fd = open_archive(temp_arch, CREATE);
	while ((member = get_member(ar_f)) != NIL_MEM)
	{
		if (argc > 3)
		{
			for (i = 3; i < argc; i++)
			{
				sys_basename(argv[i],buffer);
				if (equal(buffer, member->ar_name))
				break;
			}
			if (i == argc || app_fl)
			{
				if (rep_fl || del_fl
#ifdef AAL
						|| app_fl
#endif
				)
				{
#ifdef AAL
					if (i != argc)
					{
						print("%s: already in archive\n", argv[i]);
						argv[i] = "";
					}
#endif
					wr_arhdr(temp_fd, member);
					copy_member(member, ar_f, temp_fd, FALSE);
				}
				else
				{
#ifndef AAL
					if (app_fl && i != argc)
					{
						print("%s: already in archive\n", argv[i]);
						argv[i] = "";
					}
#endif
					fseek(ar_f, even(member->ar_size),SEEK_CUR);
				}
				continue;
			}
		}
		if (ex_fl || pr_fl)
		extract(ar_f,member);
		else
		{
			if (rep_fl)
			add(argv[i], ar_f, temp_fd, "r - %s\n");
			else if (show_fl)
			{
				char buf[sizeof(member->ar_name) + 2];
				register char *p = buf, *q = member->ar_name;

				while (q <= &member->ar_name[sizeof(member->ar_name)-1] && *q)
				{
					*p++ = *q++;
				}
				*p++ = '\n';
				*p = '\0';
				if (verbose)
				{
					char *mode = get_mode(member->ar_mode);
					char *date = ctime(&(member->ar_date));

					*(date + 16) = '\0';
					*(date + 24) = '\0';

					print("%s%3u/%u%7ld %s %s %s",
							mode,
							(unsigned) (member->ar_uid & 0377),
							(unsigned) (member->ar_gid & 0377),
							member->ar_size,
							date+4,
							date+20,
							buf);
				}
				else print(buf);
			}
			else if (del_fl)
			{
				show("d - %s\n", member->ar_name);
			}
			fseek(ar_f, even(member->ar_size), SEEK_CUR);
		}
		argv[i] = "";
	}

	if (argc > 3)
	{
		for (i = 3; i < argc; i++)
		if (argv[i][0] != '\0')
		{
#ifndef AAL
			if (app_fl)
			add(argv[i], ar_f, "a - %s\n");
			else
#endif
			if (rep_fl
#ifdef AAL
					|| app_fl
#endif
			)
			add(argv[i], ar_f, temp_fd, "a - %s\n");
			else
			{
				print("%s: not found\n", argv[i]);
			}
		}
	}

	if (rep_fl || del_fl
#ifdef AAL
			|| app_fl
#endif
	)
	{
		signal(SIGINT, SIG_IGN);
		fclose(ar_f);
		fclose(temp_fd);
		ar_f = open_archive(argv[2], CREATE);
		temp_fd = open_archive(temp_arch, APPEND);
#ifdef AAL
		write_symdef(ar_f);
#endif
		while ((read_chars = fread(io_buffer, 1, IO_SIZE, temp_fd)) > 0)
		mwrite(ar_f, io_buffer, read_chars);
		fclose(temp_fd);
		remove(temp_arch);
	}
	fclose(ar_f);
}

/** Add an entry into the archive.
 *
 *  @param[in] name path specification of the file to add.
 *  @param[in] ar Original ar file for update.
 *  @param[in] dst Archive name that will have its file added.
 *
 */
void add(char *name, FILE* ar, FILE* dst, char *mess)
{
<<<<<<< HEAD
	static MEMBER member;
	size_t read_chars;
	struct stat status;
	char buffer[FILENAME_MAX];
	FILE* src_fd = NULL;

	if (stat(name, &status) < 0)
	{
		error(FALSE, "cannot find %s\n", name);
		return;
	}
	else if (S_ISDIR(status.st_mode))
	{
		error(FALSE, "%s is a directory (ignored)\n", name);
		return;
	}
	else if (u_fl && status.st_mtime <= member.ar_date)
	{
		wr_arhdr(dst, &member);
		copy_member(&member, ar, dst, FALSE);
		return;
	}
	else if ((src_fd = fopen(name, "rb")) == NULL)
	{
		error(FALSE, "cannot open %s\n", name);
		return;
	}

	sys_basename(name, buffer);
	strncpy (member.ar_name, buffer, sizeof(member.ar_name));
	member.ar_uid = status.st_uid;
	member.ar_gid = status.st_gid;
	member.ar_mode = mode2ar(status.st_mode);
	member.ar_date = status.st_mtime;
	member.ar_size = status.st_size;
#ifdef DISTRIBUTION
	if (distr_fl)
	{
		member.ar_uid = 2;
		member.ar_gid = 2;
		member.ar_mode = AR_IUSR | AR_IWUSR | AR_IRGRP | AR_IROTH;
		member.ar_date = distr_time;
	}
#endif
	wr_arhdr(dst, &member);
#ifdef AAL
	do_object(src_fd, member.ar_size);
	fseek(src_fd, 0L, SEEK_SET);
	offset += AR_TOTAL + even(member.ar_size);
#endif
	while (status.st_size > 0)
	{
		size_t x = IO_SIZE;

		read_chars = x;
		if (status.st_size < x)
		{
			x = status.st_size;
			read_chars = x;
			status.st_size = 0;
			x = even(x);
		}
		else status.st_size -= x;
		if (fread(io_buffer, 1, read_chars, src_fd) != read_chars)
		{
			error(FALSE,"%s seems to shrink\n", name);
			break;
		}
		mwrite(dst, io_buffer, x);
	}

	if (verbose)
		show(mess, member.ar_name);
	fclose(src_fd);
}

/** Extract an archive entry pointed to by member to
 *  either standard output or to a file.
 *
 */
void extract(FILE* ar, MEMBER *member)
{
	FILE* file = stdout;
	char buf[sizeof(member->ar_name) + 1];

	strncpy(buf, member->ar_name, sizeof(member->ar_name));
	buf[sizeof(member->ar_name)] = 0;
	if (pr_fl == FALSE)
	{
		file = fopen(buf, "wb");
		if (file == NULL)
		{
			error(FALSE, "cannot create %s\n", buf);
			file = NULL;
		}
	};

	if (verbose)
	{
		if (pr_fl == FALSE)
			show("x - %s\n", buf);
		else
			show("\n<%s>\n\n", buf);
	}

	copy_member(member, ar, file, TRUE);

	if (file != NULL)
		fclose(file);
	if (pr_fl == FALSE)
		chmod(buf, ar2mode(member->ar_mode));
}

void copy_member(MEMBER *member, FILE* from, FILE* to, BOOL extracting)
{
	size_t rest;
	long mem_size = member->ar_size;
	BOOL is_odd = odd(mem_size) ? TRUE : FALSE;

#ifdef AAL
	if (!extracting)
	{
		long pos = ftell(from);

		do_object(from, mem_size);
		offset += AR_TOTAL + even(mem_size);
		fseek(from, pos, SEEK_SET);
	}
#endif
	do
	{
		rest = mem_size > (size_t) IO_SIZE ? IO_SIZE : (size_t) mem_size;
		if (fread(io_buffer, 1, rest, from) != rest)
		{
			char buf[sizeof(member->ar_name) + 1];

			strncpy(buf, member->ar_name, sizeof(member->ar_name));
			buf[sizeof(member->ar_name)] = 0;
			error(TRUE, "read error on %s\n", buf);
		}
		if (to != NULL)
			mwrite(to, io_buffer, rest);
		mem_size -= (long) rest;
	} while (mem_size > 0L);

	if (is_odd)
	{
		fseek(from, 1L, SEEK_CUR);
		if ((to != NULL) && (extracting == FALSE))
			fseek(to, 1L, SEEK_CUR);
	}
}

char *get_mode(int mode)
{
	static char mode_buf[11];
	register int tmp = mode;
	int i;

	mode_buf[9] = ' ';
	for (i = 0; i < 3; i++)
	{
		mode_buf[i * 3] = (tmp & AR_IRUSR) ? 'r' : '-';
		mode_buf[i * 3 + 1] = (tmp & AR_IWUSR) ? 'w' : '-';
		mode_buf[i * 3 + 2] = (tmp & AR_IXUSR) ? 'x' : '-';
		tmp <<= 3;
	}
	if (mode & AR_ISUID)
		mode_buf[2] = 's';
	if (mode & AR_ISGID)
		mode_buf[5] = 's';
	return mode_buf;
}

void wr_fatal(void)
{
	error(TRUE, "write error\n", NULL);
}

void rd_fatal(void)
{
	error(TRUE, "read error\n", NULL);
}

void mwrite(FILE* f, void* address, size_t bytes)
{
	if (fwrite(address, 1, bytes, f) != bytes)
		error(TRUE, "write error\n", NULL);
}

void show(char *s, char *name)
{
	MEMBER x;
	char buf[sizeof(x.ar_name) + 1];
	register char *p = buf, *q = name;

	while (q <= &name[sizeof(x.ar_name) - 1] && *q)
		*p++ = *q++;
	*p++ = '\0';
	print(s, buf);
}

#ifdef AAL
/*
 * Write out the ranlib table: first 4 bytes telling how many ranlib structs
 * there are, followed by the ranlib structs,
 * then 4 bytes giving the size of the string table, followed by the string
 * table itself.
 */
void write_symdef(FILE *ar)
{
	register struct ranlib *ran;
	register int i;
	register long delta;
	time_t time_value;
	MEMBER arbuf;

	if (!tnum)
		return;
	if (odd(tssiz))
		tstrtab[tssiz++] = '\0';
	for (i = 0; i < sizeof(arbuf.ar_name); i++)
		arbuf.ar_name[i] = '\0';
	strcpy(arbuf.ar_name, SYMDEF);
	arbuf.ar_size = 4 + 2 * 4 * (long) tnum + 4 + (long) tssiz;
	time(&time_value);
	arbuf.ar_date = (long) time_value;
	arbuf.ar_uid = getuid();
	arbuf.ar_gid = getgid();
	arbuf.ar_mode = AR_IRUSR | AR_IRGRP | AR_IROTH;
#ifdef DISTRIBUTION
	if (distr_fl)
	{
		arbuf.ar_uid = 2;
		arbuf.ar_gid = 2;
		arbuf.ar_date = distr_time;
	}
#endif
	wr_arhdr(ar, &arbuf);
	wr_long(ar, (long) tnum);
	/*
	 * Account for the space occupied by the magic number
	 * and the ranlib table.
	 */
	delta = 2 + AR_TOTAL + arbuf.ar_size;
	for (ran = tab; ran < &tab[tnum]; ran++)
	{
		ran->ran_pos += delta;
	}

	wr_ranlib(ar, tab, (long) tnum);
	wr_long(ar, (long) tssiz);
	wr_bytes(ar, tstrtab, (long) tssiz);
}

/*
 * Return whether the bytes in `buf' form a good object header.
 * The header is put in `headp'.
 */
int is_outhead(struct outhead *headp)
{
	return !BADMAGIC(*headp) && headp->oh_nname != 0;
}

void do_object(FILE* f, long size)
{
	struct outhead headbuf;

	if (size < SZ_HEAD)
	{
		/* It can't be an object file. */
		return;
	}
	/*
	 * Read a header to see if it is an object file.
	 */
	if (!rd_fdopen(f))
	{
		rd_fatal();
	}
	rd_ohead(&headbuf);
	if (!is_outhead(&headbuf))
	{
		return;
	}
	do_names(&headbuf);
}

/*
 * First skip the names and read in the string table, then seek back to the
 * name table and read and write the names one by one. Update the ranlib table
 * accordingly.
 */
void do_names(struct outhead *headp)
{
	register char *strings = NULL;
	register int nnames = headp->oh_nname;
#define NNAMES 100
	struct outname namebuf[NNAMES];
	long xxx = OFF_CHAR(*headp);

	if ( (headp->oh_nchar != (unsigned int) headp->oh_nchar)
			|| ((strings = malloc((unsigned int) headp->oh_nchar))) == NULL)
	{
		error(TRUE, "string table too big\n", NULL);
	}
	rd_string(strings, headp->oh_nchar);
	while (nnames)
	{
		int i = nnames >= NNAMES ? NNAMES : nnames;
		register struct outname *p = namebuf;

		nnames -= i;
		rd_name(namebuf, i);
		while (i--)
		{
			long off = p->on_foff - xxx;
			if (p->on_foff == (long) 0)
			{
				p++;
				continue; /* An unrecognizable name. */
			}
			p->on_mptr = strings + off;
			/*
			 * Only enter names that are exported and are really
			 * defined. Also enter common names. Note, that
			 * this might cause problems when the name is really
			 * defined in a later file, with a value != 0.
			 * However, this problem also exists on the Unix
			 * ranlib archives.
			 */
			if ((p->on_type & S_EXT) && (p->on_type & S_TYP) != S_UND)
				enter_name(p);
			p++;
		}
	}
	free(strings);
}

void enter_name(struct outname *namep)
{
	register char *cp;

	if (tnum >= tabsz)
	{
		tab = (struct ranlib *) realloc((char *) tab,
				(tabsz += 512) * sizeof(struct ranlib));
		if (!tab)
			error(TRUE, "Out of core\n", NULL);
	}
	tab[tnum].ran_off = tssiz;
	tab[tnum].ran_pos = offset;

	for (cp = namep->on_mptr;; cp++)
	{
		if (tssiz >= strtabsz)
		{
			tstrtab = realloc(tstrtab, (strtabsz += 4096));
			if (!tstrtab)
				error(TRUE, "string table overflow\n", NULL);
		}
		tstrtab[tssiz++] = *cp;
		if (!*cp)
			break;
	}
	tnum++;
}
#endif /* AAL */
