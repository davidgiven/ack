/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* M A I N   P R O G R A M */

/* stripped down version from the one in the Modula-2 compiler */

/* $Header$ */

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
extern char	*strrindex();
extern char	*strcpy(), *strcat();

char *
getwdir(fn)
        register char *fn;
{
        register char *p;

        p = strrindex(fn, '/');
        while (p && *(p + 1) == '\0') { /* remove trailing /'s */
                *p = '\0';
                p = strrindex(fn, '/');
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

char *mflags = "";
char *compiler = "ack";
char *suff = "o";

main(argc, argv)
	register char **argv;
{
	extern struct tokenname tkidf[];
	extern char *getwdir();
	int i;

	ProgName = *argv++;
	DEFPATH = (char **) Malloc(10 * sizeof(char *));
	mDEF = 10;
	nDEF = 1;

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
		print(" -I%s", DEFPATH[i]);
	}
	print("\nM2FLAGS = %s\nMOD = %s\nSUFFIX = %s\n", mflags, compiler, suff);
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
	static int cnt;
	extern char *calloc();

	if (cnt--) return p++;
	p = (struct file_list *)calloc(50, sizeof(struct file_list));
	cnt = 49;
	return p++;
}

Add(parglist, f, d, copy)
	char *f, *d;
	struct file_list **parglist;
{
	register struct file_list *a = *parglist, *b = 0;

	if (f == 0) return;

	while (a && strcmp(a->a_filename, f) != 0) {
		b = a;
		a = a->a_next;
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

ProcessArgs()
{
	register struct file_list *a = arglist;
	char *fn;

	while (a) {
		register char *p = strrindex(a->a_filename, '.');

		CurrentArg = a;
		DEFPATH[0] = a->a_dir;
		if ( p && strcmp(p, ".def") == 0) {
			ForeignFlag = 0;
			if (! InsertFile(a->a_filename, DEFPATH, &fn)) {
				Gerror("Could not find %s", a->a_filename);
				a->a_filename = "";
				a = a->a_next;
				continue;
			}
			FileName = fn;
			a->a_dir = WorkingDir = getwdir(FileName);
			DefModule();
		}
		else if (p && strcmp(p, ".mod") == 0) {
			if (! InsertFile(a->a_filename, DEFPATH, &fn)) {
				Gerror("Could not find %s", a->a_filename);
				*p = 0;
				a->a_filename = Salloc(a->a_filename,
							strlen(a->a_filename) + 
							(unsigned)11);
				strcat(a->a_filename, ".$(SUFFIX)");
				a = a->a_next;
				continue;
			}
			FileName = fn;
			a->a_dir = WorkingDir = getwdir(FileName);
			CompUnit();
		}
		else fatal("No Modula-2 file: %s", a->a_filename);
		a = a->a_next;
	}
}

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
	register struct file_list *arg = arglist;

	print("\nall:\t");
	while (arg) {
		char *dotspot = strrindex(arg->a_filename, '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = arg->a_idf;

			if (id) {
				if (id->id_type == PROGRAM) {
					print("%s ", id->id_text);
				}
				file_dep(id);
			}
		}
		arg = arg->a_next;
	}
	print("\n\n");
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
			for (p = iid->id_ddependson; p; p = p->a_next) {
				Add(&(id->id_ddependson), p->a_filename,
				    p->a_dir, 0);
				Add(&(id->id_mdependson), p->a_filename,
				    p->a_dir, 0);
			}
		}
	}
	for (m = id->id_modimports; m; m = m->lnk_next) {
		register struct idf *iid = m->lnk_imp;

		if (Add(&(id->id_mdependson), iid->id_def, iid->id_dir, 0)) {
			register struct file_list *p;

			file_dep(iid);
			for (p = iid->id_ddependson; p; p = p->a_next) {
				Add(&(id->id_mdependson), p->a_filename,
				    p->a_dir, 0);
			}
		}
	}
}

char *
object(arg)
	register struct file_list *arg;
{
	static char buf[512];
	char *dotp = strrindex(arg->a_filename, '.');

	buf[0] = 0;
/*
	if (strcmp(arg->a_dir, ".") != 0) {
		strcpy(buf, arg->a_dir);
		strcat(buf, "/");
	}
*/
	*dotp = 0;
	strcat(buf, arg->a_filename);
	*dotp = '.';
	strcat(buf, ".$(SUFFIX)");
	return buf;
}

pr_arg(a)
	register struct file_list *a;
{
	if (strcmp(a->a_dir, ".") == 0) {
		print(a->a_filename);
	}
	else	print("%s/%s", a->a_dir, a->a_filename);
}

print_dep()
{
	register struct file_list *arg = arglist;

	while (arg) {
		char *dotspot = strrindex(arg->a_filename, '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = arg->a_idf;

			if (id) {
				char *obj = object(arg);
				register struct file_list *a;

				print("%s: \\\n\t", obj);
				pr_arg(arg);
				for (a = id->id_mdependson; a; a = a->a_next) {
					if (*(a->a_filename)) {
						print(" \\\n\t");
						pr_arg(a);
					}
				}
				print("\n\t$(MOD) -c $(M2FLAGS) $(IFLAGS) ");
				pr_arg(arg);
				print("\n");
			}
		}
	    	arg = arg->a_next;
	}
}

prog_dep(id)
	register struct idf *id;
{
	register struct lnk *m;
	register struct file_list *p;

	id->id_mdependson = 0;
	id->id_def = 0;
	if (strlen(id->id_text) >= 10) id->id_text[10] = 0;
	Add(&(id->id_mdependson), id->id_text, id->id_dir, 0);
	for (m = id->id_modimports; m; m = m->lnk_next) {
		register struct idf *iid = m->lnk_imp;

		if (Add(&(id->id_mdependson), iid->id_text, iid->id_dir, 0)) {
			if (iid->id_def) prog_dep(iid);
			for (p = iid->id_mdependson; p; p = p->a_next) {
				Add(&(id->id_mdependson), p->a_filename,
				    p->a_dir, 0);
			}
		}
	}
}

module_in_arglist(n)
	char *n;
{
	register struct file_list *a;

	for (a = arglist; a; a = a->a_next) {
		char *dotp = strrindex(a->a_filename, '.');

		if (dotp && strcmp(dotp, ".mod") == 0) {
			*dotp = 0;
			if (strcmp(a->a_filename, n) == 0) {
				*dotp = '.';
				return 1;
			}
			*dotp = '.';
		}
	}
	return 0;
}

pr_prog_dep(id)
	register struct idf *id;
{
	register struct file_list *p;

	print("\nOBS_%s =", id->id_text);
	for (p = id->id_mdependson; p; p = p->a_next) {
		if (module_in_arglist(p->a_filename) || ! p->a_dir) {
			print(" \\\n\t%s.$(SUFFIX)", p->a_filename);
		}
		else if (! is_library_dir(p->a_dir))  {
			print(" \\\n\t%s/%s.$(SUFFIX)", p->a_dir, p->a_filename);
		}
	}
	print("\n\n");
	print("%s:\t$(OBS_%s)\n", id->id_text, id->id_text);
	print("\t$(MOD) -.mod -o %s $(M2FLAGS) $(OBS_%s)\n", id->id_text, id->id_text);
}

programs()
{
	register struct file_list *a;

	for (a = arglist; a; a = a->a_next) {
		char *dotspot = strrindex(a->a_filename, '.');

		if (dotspot && strcmp(dotspot, ".mod") == 0) {
			register struct idf *id = a->a_idf;

			if (id && id->id_type == PROGRAM) {
				prog_dep(id);
				pr_prog_dep(id);
			}
		}
	}
}
