/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*
**	print symbol tables for
**	ACK object files
**
**	anm [-gopruns] [name ...]
*/

#include	"out.h"

#include	<stdio.h>
#include	<ctype.h>

int	numsort_flg;
int	sectsort_flg;
int	undef_flg;
int	revsort_flg = 1;
int	globl_flg;
int	nosort_flg;
int	arch_flg;
int	prep_flg;
int	read_error;
struct	outhead	hbuf;
struct	outsect	sbuf;
long	off;
char	*malloc();
char	*realloc();
long	s_base[S_MAX];	/* for specially encoded bases */
char	*filename;

main(argc, argv)
char **argv;
{
	int	narg;
	int	compare();

	if (--argc>0 && argv[1][0]=='-' && argv[1][1]!=0) {
		argv++;
		while (*++*argv) switch (**argv) {
		case 'n':		/* sort numerically */
			numsort_flg++;
			continue;

		case 's':		/* sort in section order */
			sectsort_flg++;
			continue;

		case 'g':		/* globl symbols only */
			globl_flg++;
			continue;

		case 'u':		/* undefined symbols only */
			undef_flg++;
			continue;

		case 'r':		/* sort in reverse order */
			revsort_flg = -1;
			continue;

		case 'p':		/* don't sort -- symbol table order */
			nosort_flg++;
			continue;

		case 'o':		/* prepend a name to each line */
			prep_flg++;
			continue;

		default:		/* oops */
			fprintf(stderr, "anm: invalid argument -%c\n", *argv[0]);
			exit(1);
		}
		argc--;
	}
	if (argc == 0) {
		argc = 1;
		argv[1] = "a.out";
	}
	narg = argc;

	while(argc--) {
		struct	outname	*nbufp = NULL;
		struct	outname	nbuf;
		char		*cbufp;
		long		fi_to_co;
		long		n;
		unsigned	readcount;
		int		i,j;

		read_error = 0;
		if (! rd_open(*++argv)) {
			fprintf(stderr, "anm: cannot open %s\n", *argv);
			continue;
		}

		filename = *argv;
		rd_ohead(&hbuf);
		if (read_error) {
			rd_close();
			continue;
		}
		if (BADMAGIC(hbuf)) {
			fprintf(stderr, "anm: %s -- bad format\n", *argv);
			rd_close();
			continue;
		}
		if (narg > 1)
			printf("\n%s:\n", *argv);

		n = hbuf.oh_nname;
		if (n == 0) {
			fprintf(stderr, "anm: %s -- no name list\n", *argv);
			rd_close();
			continue;
		}

		if (hbuf.oh_nchar == 0) {
			fprintf(stderr, "anm: %s -- no names\n", *argv);
			rd_close();
			continue;
		}
		if ((readcount = hbuf.oh_nchar) != hbuf.oh_nchar) {
			fprintf(stderr, "anm: string area too big in %s\n", *argv);
			exit(2);
		}

		/* store special section bases ??? */
		if (hbuf.oh_flags & HF_8086) {
			rd_sect(&sbuf, hbuf.oh_nsect);
			if (read_error) {
				rd_close();
				continue;
			}
			for (i=0; i<hbuf.oh_nsect; i++) {
				s_base[i+S_MIN] =
					(sbuf.os_base>>12) & 03777760;
			}
		}
		 
		if ((cbufp = (char *)malloc(readcount)) == NULL) {
			fprintf(stderr, "anm: out of memory on %s\n", *argv);
			exit(2);
		}
		rd_string(cbufp, hbuf.oh_nchar);
		if (read_error) {
			free(cbufp);
			rd_close();
			continue;
		}

		fi_to_co = (long) (cbufp - OFF_CHAR(hbuf));
		i = 0;
		while (--n >= 0) {
			rd_name(&nbuf, 1);
			if (read_error) {
				break;
			}

			if (globl_flg && (nbuf.on_type&S_EXT)==0)
				continue;

			if (undef_flg
			    &&
			    ((nbuf.on_type&S_TYP)!=S_UND || (nbuf.on_type&S_ETC)!=0))
				continue;

			if (nbuf.on_foff == 0) nbuf.on_mptr = 0;
			else nbuf.on_mptr = (char *) (nbuf.on_foff + fi_to_co);

			/* adjust value for specially encoded bases */
			if (hbuf.oh_flags & HF_8086) {
			    if (((nbuf.on_type&S_ETC) == 0) ||
				((nbuf.on_type&S_ETC) == S_SCT)) {
				j = nbuf.on_type&S_TYP;
				if ((j>=S_MIN) && (j<=S_MAX))
				    nbuf.on_valu += s_base[j];
			    }
			}

			if (nbufp == NULL)
				nbufp = (struct outname *)malloc(sizeof(struct outname));
			else
				nbufp = (struct outname *)realloc(nbufp, (i+1)*sizeof(struct outname));
			if (nbufp == NULL) {
				fprintf(stderr, "anm: out of memory on %s\n", *argv);
				exit(2);
			}
			nbufp[i++] = nbuf;
		}

		if (nbufp && nosort_flg==0)
			qsort(nbufp, i, sizeof(struct outname), compare);

		for (n=0; n<i; n++) {
			char	cs1[4];
			char	cs2[4];

			if (prep_flg)
				printf("%s:", *argv);

			switch(nbufp[n].on_type&S_ETC) {
			case S_SCT:
				sprintf(cs1, "%2d", (nbufp[n].on_type&S_TYP) - S_MIN);
				sprintf(cs2, " S");
				break;
			case S_FIL:
				sprintf(cs1, " -");
				sprintf(cs2, " F");
				break;
			case S_MOD:
				sprintf(cs1, " -");
				sprintf(cs2, " M");
				break;
			case S_COM:
				sprintf(cs1, " C");
				if (nbufp[n].on_type&S_EXT)
					sprintf(cs2, " E");
				else
					sprintf(cs2, " -");
				break;
			case 0:
				if (nbufp[n].on_type&S_EXT)
					sprintf(cs2, " E");
				else
					sprintf(cs2, " -");

				switch(nbufp[n].on_type&S_TYP) {
				case S_UND:
					sprintf(cs1, " U");
					break;
				case S_ABS:
					sprintf(cs1, " A");
					break;
				default:
					sprintf(cs1, "%2d", (nbufp[n].on_type&S_TYP) - S_MIN);
				}
				break;
			default:
				sprintf(cs1, "??");
				sprintf(cs2, " ?");
			}

			printf("%8lx %s %s %s\n",nbufp[n].on_valu,cs1,cs2,nbufp[n].on_mptr ? nbufp[n].on_mptr : "(NULL)");
		}

		if (nbufp)
			free((char *)nbufp);
		if (cbufp)
			free((char *)cbufp);
		rd_close();
	}
	exit(0);
}

compare(p1, p2)
struct outname	*p1, *p2;
{
	int	i;

	if (sectsort_flg) {
		if ((p1->on_type&S_TYP) > (p2->on_type&S_TYP))
			return(revsort_flg);
		if ((p1->on_type&S_TYP) < (p2->on_type&S_TYP))
			return(-revsort_flg);
	}

	if (numsort_flg) {
		if (p1->on_valu > p2->on_valu)
			return(revsort_flg);
		if (p1->on_valu < p2->on_valu)
			return(-revsort_flg);
	}

	if (! p1->on_mptr) {
		if (! p2->on_mptr) return 0;
		return -revsort_flg;
	}
	if (! p2->on_mptr) return revsort_flg;

	i = strcmp(p1->on_mptr, p2->on_mptr);

	if (i > 0)
		return(revsort_flg);
	if (i < 0)
		return(-revsort_flg);

	return(0);
}

rd_fatal()
{
	fprintf(stderr,"read error on %s\n", filename);
	read_error = 1;
}
