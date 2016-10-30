/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* M A I N   P R O G R A M */

/* stripped down version from the one in the Modula-2 compiler */

/* $Id$ */

#include	<alloc.h>

#include	"input.h"
#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"tokenname.h"

int		state;			/* either IMPLEMENTATION or PROGRAM */
char		*ProgName;
char		**DEFPATH;
int		nDEF, mDEF;
struct file_list *CurrentArg;
extern int 	err_occurred;
extern int 	Roption;

char *
basename(s)
	char *s;
{
	static char buf[256];
	char *p = strrchr(s, '.');

	if (p != 0) *p = 0;
	strcpy(buf, s);
	if (p != 0) *p = '.';
	return buf;
}

char *
getwdir(fn)
        register char *fn;
{
        register char *p;

        p = strrchr(fn, '/');
        while (p && *(p + 1) == '\0') { /* remove trailing /'s */
                *p = '\0';
                p = strrchr(fn, '/');
        }

        if (p) {
		register char **d = DEFPATH;

                *p = '\0';
		while (*d && strcmp(*d, fn) != 0) d++;
                if (*d) {
                	*p = '/';
			return *d;
		}
                fn = Salloc(fn, (unsigned) (p - &fn[0] + 1));
		*p = '/';
		return fn;
        }
        return ".";
}

static struct file_list *arglist;

#ifndef M2COMPILER
#define M2COMPILER	"m2"
#endif

char *mflags = "";
char *compiler = M2COMPILER;
char *suff = "o";
char *llibs = 0;

main(argc, argv)
	register char **argv;
{
	extern struct tokenname tkidf[];
	extern char *getwdir();
	int i;

	ProgName = *argv++;
	DEFPATH = (char **) Malloc(10 * sizeof(char *));
	DEFPATH[1] = 0;
	mDEF = 10;
	nDEF = 2;

	while (--argc > 0) {
		if (**argv == '-')
			DoOption((*argv++) + 1);
		else {
			Add(&arglist, *argv, getwdir(*argv), 1);
			argv++;
		}
	}

	init_idf();
	reserve(tkidf);
	print("IFLAGS =");
	for (i = 1; i < nDEF; i++) {
		if (DEFPATH[i]) print(" -I%s", DEFPATH[i]);
	}
	print("\nM2FLAGS = %s\nMOD = %s\nSUFFIX = %s\nLIBS = %s\n", mflags, compiler, suff, llibs ? llibs : "");
	init_lib();
	ProcessArgs();
	find_dependencies();
	print_dep();
	programs();
	exit(err_occurred);
}

struct file_list *
new_file_list()
{
	static struct file_list *p;
	register struct file_list *f;
	static int cnt;

	if (--cnt < 0) {
		p = (struct file_list *)Malloc(50*sizeof(struct file_list));
		cnt = 49;
	}
	f = p++;
	f->a_filename = 0;
	f->a_dir = 0;
	f->a_next = 0;
	f->a_idf = 0;
	f->a_notfound = 0;
	return f;
}

Add(parglist, f, d, copy)
	char *f, *d;
	struct file_list **parglist;
{
	register struct file_list *a, *b = 0;

	if (f == 0) return;

	f_walk(*parglist, a) {
		if (strcmp(f_filename(a), f) == 0) break;
		b = a;
	}
	if (a) return 0;
	a = new_file_list();
	if (copy) {
		a->a_filename = Salloc(f, (unsigned) (strlen(f)+1));
	}
	else {
		a->a_filename = f;
	}
	a->a_dir = d;
	if (! b) *parglist = a;
	else b->a_next = a;
	return 1;
}

int
openfile(a)
	register struct file_list *a;
{
	char *fn;

	if (!f_notfound(a) && ! InsertFile(f_filename(a), DEFPATH, &fn)) {
		a->a_notfound = 1;
		Gerror("Could not find %s", f_filename(a));
		return 0;
	}
	FileName = fn;
	LineNumber = 1;
	a->a_dir = WorkingDir = getwdir(FileName);
	return 1;
}

ProcessArgs()
{
	register struct file_list *a;

	f_walk(arglist, a) {
		register char *p = strrchr(f_filename(a), '.');

		CurrentArg = a;
		DEFPATH[0] = f_dir(a);
		if ( p && strcmp(p, ".def") == 0) {
			ForeignFlag = 0;
			if (! openfile(a)) {
				continue;
			}
			DefModule();
		}
		else if (p && strcmp(p, ".mod") == 0) {
			if (! openfile(a)) {
				*p = 0;		/* ??? */
				continue;
			}
			CompUnit();
		}
		else fatal("No Modula-2 file: %s", f_filename(a));
	}
}

void
No_Mem()
{
	fatal("out of memory");
}

AddToList(name, ext)
	char *name, *ext;
{
	/*	Try to find a file with basename "name" and extension ".def",
		in the directories mentioned in "DEFPATH".
	*/
	char buf[15];
	char *strncpy();

	if (strcmp(name, "SYSTEM") != 0 && ! is_library_dir(WorkingDir)) {
		strncpy(buf, name, 10);
		buf[10] = '\0';			/* maximum length */
		strcat(buf, ext);
		Add(&arglist, buf, WorkingDir, 1);
		return 1;
	}
	return 0;
}

find_dependencies()
{
	register struct file_list *arg;

	print("\nall:\t");
	f_walk(arglist, arg) {
		char *fn = f_filename(arg);
		char *dotspot = strrchr(fn, '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = f_idf(arg);

			if (! f_notfound(arg) && id) {
				if (id->id_type == PROGRAM) {
					*dotspot = 0;
					print("%s ", fn);
					*dotspot = '.';
				}
				file_dep(id);
			}
		}
	}
	print("\n\n");
	print("objects:\t");
	f_walk(arglist, arg) {
		char *fn = f_filename(arg);
		char *dotspot = strrchr(fn, '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = f_idf(arg);

			if (! f_notfound(arg) && id) {
				if (id->id_type == PROGRAM) {
					*dotspot = 0;
					print("%s_o_files ", fn);
					*dotspot = '.';
				}
			}
		}
	}
	print("\n\n\n");
}

file_dep(id)
	register struct idf *id;
{
	register struct lnk *m;

	if (id->id_ddependson || id->id_mdependson) return;
	if (id->id_def) Add(&(id->id_mdependson), id->id_def, id->id_dir, 0);
	for (m = id->id_defimports; m; m = m->lnk_next) {
		register struct idf *iid = m->lnk_imp;

		Add(&(id->id_mdependson), iid->id_def, iid->id_dir, 0);
		if (Add(&(id->id_ddependson), iid->id_def, iid->id_dir, 0)) {
			register struct file_list *p;

			file_dep(iid);
			f_walk(iid->id_ddependson, p) {
				Add(&(id->id_ddependson), f_filename(p),
				    f_dir(p), 0);
				Add(&(id->id_mdependson), f_filename(p),
				    f_dir(p), 0);
			}
		}
	}
	for (m = id->id_modimports; m; m = m->lnk_next) {
		register struct idf *iid = m->lnk_imp;

		if (Add(&(id->id_mdependson), iid->id_def, iid->id_dir, 0)) {
			register struct file_list *p;

			file_dep(iid);
			f_walk(iid->id_ddependson, p) {
				Add(&(id->id_mdependson), f_filename(p),
				    f_dir(p), 0);
			}
		}
	}
}

char *
object(arg)
	register struct file_list *arg;
{
	static char buf[512];
	char *dotp = strrchr(f_filename(arg), '.');

	buf[0] = 0;
/*
	if (strcmp(f_dir(arg), ".") != 0) {
		strcpy(buf, f_dir(arg));
		strcat(buf, "/");
	}
*/
	if (dotp) *dotp = 0;
	strcat(buf, f_filename(arg));
	if (dotp) *dotp = '.';
	strcat(buf, ".$(SUFFIX)");
	return buf;
}

pr_arg(a)
	register struct file_list *a;
{
	char *f = f_filename(a);
	char *d = f_dir(a);

	if (strcmp(d, ".") == 0 || *f == '/' || *f == '.') {
		print(f);
	}
	else	print("%s/%s", d, f);
}

print_dep()
{
	register struct file_list *arg;

	f_walk(arglist, arg) {
		char *dotspot = strrchr(f_filename(arg), '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = f_idf(arg);

			if (! f_notfound(arg) && id) {
				char *obj = object(arg);
				register struct file_list *a;

				print("%s: \\\n\t", obj);
				pr_arg(arg);
				f_walk(id->id_mdependson, a) {
					if (*(f_filename(a))) /* ??? */ {
						print(" \\\n\t");
						pr_arg(a);
					}
				}
				print("\n\t$(MOD) -c $(M2FLAGS) $(IFLAGS) ");
				pr_arg(arg);
				print("\n");
			}
		}
	}
}

prog_dep(id, a)
	register struct idf *id;
	struct file_list *a;
{
	register struct lnk *m;
	register struct file_list *p;

	id->id_mdependson = 0;
	id->id_def = 0;
	if (id->id_type == PROGRAM) {
		Add(&(id->id_mdependson), f_filename(a), f_dir(a), 0);
	}
	else {
		if (strlen(id->id_text) >= 10) id->id_text[10] = 0;
		Add(&(id->id_mdependson), id->id_text, id->id_dir, 0);
	}
	for (m = id->id_modimports; m; m = m->lnk_next) {
		register struct idf *iid = m->lnk_imp;

		if (Add(&(id->id_mdependson), iid->id_text, iid->id_dir, 0)) {
			if (iid->id_def) prog_dep(iid);
			f_walk(iid->id_mdependson, p) {
				Add(&(id->id_mdependson), f_filename(p),
				    f_dir(p), 0);
			}
		}
	}
}

module_in_arglist(n)
	char *n;
{
	register struct file_list *a;

	f_walk(arglist, a) {
		char *dotp = strrchr(f_filename(a), '.');

		if (dotp && strcmp(dotp, ".mod") == 0 && ! f_notfound(a)) {
			*dotp = 0;
			if (strcmp(f_filename(a), n) == 0) {
				*dotp = '.';
				return 1;
			}
			*dotp = '.';
		}
	}
	return 0;
}

pr_prog_dep(id, a)
	register struct idf *id;
	struct file_list *a;
{
	register struct file_list *p;

	print("\nOBS_%s =", id->id_text);
	f_walk(id->id_mdependson, p) {
		if (module_in_arglist(f_filename(p)) || ! f_dir(p)) {
			print(" \\\n\t%s", object(p));
		}
	}
	print("\n\nOBS2_%s =", id->id_text);
	f_walk(id->id_mdependson, p) {
		if (module_in_arglist(f_filename(p)) || ! f_dir(p)) {
			/* nothing */
		}
		else if (! is_library_dir(f_dir(p))) {
			print(" \\\n\t%s/%s", f_dir(p), object(p));
		}
	}
	print("\n\n");
	print("%s_o_files:\t$(OBS_%s)\n\n", basename(f_filename(a)), id->id_text);
	print("%s:\t$(OBS_%s) $(OBS2_%s)\n", basename(f_filename(a)), id->id_text, id->id_text);
	print("\t$(MOD) -o %s $(M2FLAGS) $(OBS_%s) $(OBS2_%s) $(LIBS)\n", basename(f_filename(a)), id->id_text, id->id_text);
}

programs()
{
	register struct file_list *a;

	f_walk(arglist, a) {
		char *dotspot = strrchr(f_filename(a), '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = f_idf(a);

			if (! f_notfound(a) && id && id->id_type == PROGRAM) {
				prog_dep(id, a);
				/* *dotspot = 0; */
				pr_prog_dep(id, a);
				/* *dotspot = '.'; */
			}
		}
	}
}
