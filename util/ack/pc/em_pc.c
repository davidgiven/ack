/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 * put all the pieces of the pascal part of the EM project together
 *      original author: Johan Stevenson, Vrije Universiteit, Amsterdam
 *      heavily modified by: Ed Keizer, Vrije Universiteit, Amsterdam
 */

#include        <stdio.h>
#include        <signal.h>
#include        <sys/types.h>
#include        <sys/dir.h>
#include        <em_path.h>
#include        <pc_size.h>
#include        <local.h>

#define	MAX_FLAG		40	/* The Max. no of '{' flags allowed */

#define void                    int

char	def_pc_path[200] ;
char	def_err_path[200] ;

char	*pc_path	=	def_pc_path ;
char    *err_path       =	def_err_path ;

int     toterr;
int     parent;

char    *eeflag;
char    *vvflag = "-V";
int	no_pemflag = 0 ;
char    *pemflag[MAX_FLAG];
char    *eflag;
char    *wflag;

int     sizes[sz_last+1] = {
	2,      /* sz_addr */
	8,      /* sz_real */
	0,      /* sz_head */
	512,    /* sz_buff */
	4096,   /* sz_mset */
	2,      /* sz_iset */
};

#define CALLSIZE                60
char    *callvector[CALLSIZE];
char    **av;
int     ac;
int     fileargs;               /* number of recognized, processed args */
int     flagargs;
char    *progname;
char    *source;

#define CHARSIZE                2500
#define CHARMARG                50
char    charbuf[CHARSIZE];
char    *charp                  = charbuf;

char    *tmp_dir                = TMP_DIR;
char    *unique                 = "pcXXXXXX";

char    sigs[] = {
	SIGHUP,
	SIGINT,
	SIGTERM,
	0
};

/*
 * forward function declarations
 */
void    finish();
void    pem();
int     list();
char    *flag();
char    *tempfile();
char    **initvector();
char    *basename();

/*
 * used library routines and data
 */

extern  char    *sys_errlist[];
extern  int     errno;

int     atoi();
void    exit();
void    sleep();
void    execv();
char    *sbrk();
int     chdir();
int     fork();
int     wait();
int     getpid();
int     open();
int     close();
int     read();

main(argc,argv) char **argv; {
	register char *p;
	char *files[3] ;

	for (p = sigs; *p; p++)
		if (signal(*p,finish) == SIG_IGN)
			signal(*p,SIG_IGN);
	ac = argc;
	av = argv;
	progname = *av++;
	init();
	while ( --ac>0 ) {
		p = *av++;
		if (*p == '-') {
			flagargs++;
			p = flag(p);
		} else {
			if ( fileargs>=3 ) fatal("Too many file arguments") ;
			files[fileargs++]= p;
		}
	}
	if ( fileargs!=3 ) fatal("Not enough arguments") ;
	source=files[2] ;
	pem(files[0],files[1]) ;
	finish();
}

char *flag(f) char *f; {
	register char *p;

	p = f+1;
	switch (*p++) {
	case 'e':
		eflag = f;
		break;
	case 'E':
		eeflag = f;
		break;
	case 'w':
		wflag = f;
		break;
	case 'V':
		vvflag = f;
		return(0);
	case '{':
		if ( no_pemflag>=MAX_FLAG ) {
			ermess("too many flags, ignored %s",f) ;
		} else {
			pemflag[no_pemflag++] = p;
		}
		return(0);
	case 'R':
		pc_path= p ;
		return 0 ;
	case 'r' :
		err_path= p ;
		return 0 ;
	default:
		return(f);
	}
	if (*p)
		fatal("bad flag %s",f);
	return(0);
}

initsizes(f) FILE *f; {
	register c, i;
	register char *p;

	p = vvflag + 2;
	while (c = *p++) {
		i = atoi(p);
		while (*p >= '0' && *p <= '9')
			p++;
		switch (c) {
		case 'p': sz_addr = i; continue;
		case 'f': sz_real = i; continue;
		case 'h': sz_head = i; continue;
		case 'b': sz_buff = i; continue;
		case 'm': sz_mset = i; continue;
		case 'j': sz_iset = i; continue;
		case 'w':
		case 'i': if (i == 2) continue; break;
		case 'l': if (i == 4) continue; break;
		}
		fatal("bad V-flag %s",vvflag);
	}
	if (sz_head == 0)
		sz_head = 6*sz_word + 2*sz_addr;
	for (i = 0; i <= sz_last; i++)
		fprintf(f, "%d\n",sizes[i]);
}

/* ------------------ calling sequences -------------------- */

pem(p,q) char *p,*q; {
	register char **v,*d;
	int i;
	FILE *erfil;

	v = initvector(pc_path);
	d = tempfile('d');
	if ((erfil = fopen(d,"w")) == NULL)
		syserr(d);
	initsizes(erfil);
	fprintf(erfil,"%s\n",basename(source));
	for ( i=0 ; i<no_pemflag ; i++ ) fprintf(erfil,"%s\n",pemflag[i]);
	fclose(erfil);
	*v++ = q;
	*v++ = d;
	call(v,p,(char *)0);
	if (toterr == 0)
		if (list(p,d) < 0)
			toterr++;
	donewith(d);
}

/* ------------------- miscellaneous routines --------------- */

char *basename(p) char *p; {
	register char *q;

	q = p;
	while (*q)
		if (*q++ == '/')
			p = q;
	return(p);
}

char *tempfile(suf) {
	register char *p,*q;
	register i;

	p = charp; q = tmp_dir;
	while (*p = *q++)
		p++;
	*p++ = '/';
	q = unique;
	while (*p = *q++)
		p++;
	i = fileargs;
	do
		*p++ = i % 10 + '0';
	while (i /= 10);
	*p++ = '.'; *p++ = suf; *p++ = '\0';
	q = charp; charp = p;
	return(q);
}

call(v,in,out) char **v,*in,*out; {
	register pid;
	int     status;

	while ((parent = fork()) < 0)
		sleep(1);
	if (parent == 0) {
		if (in) {
			close(0);
			if (open(in,0) != 0)
				syserr(in);
		}
		if (out) {
			close(1);
			if (creat(out,0666) != 1)
				syserr(out);
		}
		*v = 0;
		execv(callvector[0],callvector+1);
		syserr(callvector[0]);
	}
	while ((pid = wait(&status)) != parent) {
		if (pid == -1)
			fatal("process %d disappeared",parent);
		fatal("unknown child %d died",pid);
	}
	if ((status & 0177) > 3) {
/*
		if ((status & 0200) && tflag==0)
			unlink("core");
*/
		fatal("signal %d in %s. Ask an expert for help",
				status&0177,callvector[0]);
	}
	if (status & 0177400)
		toterr++;
}

char **initvector(path) char *path; {
	register char *p,**v;

	v = callvector;
	p = path;
	*v++ = p;
	*v++ = basename(p);
	return(v);
}

finish() {
	register char *p,*q;
	register fd;
	struct direct dir;

	signal(SIGINT,SIG_IGN);
	if (parent != 0) {
		chdir(tmp_dir);
		fd = open(".",0);
		while (read(fd,(char *) &dir,sizeof dir) == sizeof dir) {
			if (dir.d_ino == 0)
				continue;
			p = unique;
			q = dir.d_name;
			while (*p++ == *q++)
				if (*p == '\0') {
					unlink(dir.d_name);
					break;
				}
		}
		close(fd);
	}
	exit(toterr ? -1 : 0);
}


donewith(p) char *p; {

	if (p >= charbuf && p < &charbuf[CHARSIZE])
		unlink(p);
}

init() {
	register char *p,*s ;
	register i,fd;

	if ((fd = open(tmp_dir,0)) < 0)
		tmp_dir = ".";
	close(fd);
	p= def_pc_path ;
	s= EM_DIR ; while ( *p++ = *s++ ) ; *p='/' ;
	s= PEM_PATH ; while ( *p++ = *s++ ) ;
	p= def_err_path ;
	s= EM_DIR ; while ( *p++ = *s++ ) ; *p='/' ;
	s= ERR_PATH ; while ( *p++ = *s++ ) ;
	p = unique+2;
	parent = i = getpid();
	do
		*p++ = i % 10 + '0';
	while (i /= 10);
	*p++ = '.'; *p = '\0';
}

/* ------------------- pascal listing ----------------------- */

#define MAXERNO         300
#define MAXERRLIST      10
#define IDMAX           8

struct errec {
	int     erno;
	char    mess[IDMAX+1];
	int     mesi;
	int     chno;
	int     lino;
};

struct  errec   curr;
struct  errec   next;

int     *index          = 0;
int     maxerno;

int     errerr;
int     errfat;

int     listlino;
int     listorig;
int     listrela;
char    *listfnam;

FILE    *inpfil;
FILE    *mesfil;
FILE    *errfil;

int     errorline();
int     geterrec();
int     nexterror();

int list(p,q) char *p,*q; {

	if ((errfil = fopen(q,"r")) == NULL)
		syserr(q);
	if (geterrec() == 0)
		if (eeflag==0) {
			fclose(errfil);
			return(0);
		}
	if (index == 0) {
		index = (int *) sbrk(MAXERNO * sizeof index[0]);
		fillindex();
	}
	if ((inpfil = fopen(p,"r")) == NULL)
		syserr(p);
	errerr = 0;
	errfat = 0;
	listlino = 0;
	listorig = 0;
	listrela = 0;
	listfnam = source;
	if (eeflag)
		listfull();
	else if (eflag)
		listpartial();
	else
		listshort();
	fclose(errfil);
	fclose(inpfil);
	fflush(stdout);
	return(errfat ? -1 : 1);
}

listshort() {

	while (nexterror()) {
		while (listlino < curr.lino)
			nextline(0);
		printf("%s, line %d: ",listfnam,listrela);
		string(&curr);
	}
}

listfull() {

	if (nexterror())
		do {
			do {
				nextline(1);
			} while (listlino < curr.lino);
		} while (errorline());
	while (nextline(1))
		;
}

listpartial() {

	if (nexterror())
		do {
			do {
				nextline(listlino >= curr.lino-2);
			} while (listlino < curr.lino);
		} while (errorline());
}

int nextline(printing) {
	register ch;

	listlino++;
	ch = getc(inpfil);
	if (ch == '#') {
		if (lineline(printing) == 0)
			fatal("bad line directive");
		return(1);
	}
	listrela++;
	if (listfnam == source)
		listorig++;
	if (ch != EOF) {
		if (printing)
			printf("%5d\t",listorig);
		do {
			if (printing)
				putchar(ch);
			if (ch == '\n')
				return(1);
		} while ((ch = getc(inpfil)) != EOF);
	}
	return(0);
}

lineline(printing) {
	register ch;
	register char *p,*q;
	static char line[100];

	p = line;
	while ((ch = getc(inpfil)) != '\n') {
		if (ch == EOF || p == &line[100-1])
			return(0);
		*p++ = ch;
	}
	*p = '\0'; p = line;
	if (printing)
		printf("\t#%s\n",p);
	if ((listrela = atoi(p)-1) < 0)
		return(0);
	while ((ch = *p++) != '"')
		if (ch == '\0')
			return(0);
	q = p;
	while (ch = *p++) {
		if (ch == '"') {
			*--p = '\0';
			if ( source ) {
				listfnam = strcmp(q,source)==0 ? source : q;
				return(1);
			}
			source=q ; listfnam=q ;
			return 1 ;
		}
		if (ch == '/')
			q = p;
	}
	return(0);
}

int errorline() {
	register c;
	register struct errec *p,*q;
	struct  errec   lerr[MAXERRLIST];
	int     goon;

	printf("*** ***");
	p = lerr;
	c = 0;
	do {
		if (c < curr.chno) {
			printf("%*c",curr.chno-c,'^');
			c = curr.chno;
		}
		if (p < &lerr[MAXERRLIST])
			*p++ = curr;
		goon = nexterror();
	} while (goon && curr.lino==listlino);
	putchar('\n');
	for (q = lerr; q < p; q++)
		string(q);
	putchar('\n');
	return(goon);
}

int geterrec() {
	register ch;
	register char *p;

	ch = getc(errfil);
	next.erno = 0;
	next.mesi = -1;
	next.mess[0] = '\0';
	if (ch == EOF)
		return(0);
	if (ch >= '0' && ch <= '9') {
		ch = getnum(ch,&next.mesi);
	} else if (ch == '\'') {
		p = next.mess;
		while ((ch = getc(errfil)) != ' ' && ch != EOF)
			if (p < &next.mess[IDMAX])
				*p++ = ch;
		*p = '\0';
	}
	ch = getnum(ch, &next.erno);
	ch = getnum(ch, &next.lino);
	ch = getnum(ch, &next.chno);
	if (ch != '\n')
		fatal("bad error line");
	return(1);
}

int getnum(ch, ip) register ch; register *ip; {
	register neg;

	*ip = 0;
	while (ch == ' ')
		ch = getc(errfil);
	if (neg = ch=='-')
		ch = getc(errfil);
	while (ch >= '0' && ch <= '9') {
		*ip = *ip * 10 - '0' + ch;
		ch = getc(errfil);
	}
	if (neg)
		*ip = -(*ip);
	return(ch);
}

int nexterror() {

	do {    /* skip warnings if wflag */
		curr = next;
		if (curr.erno == 0)
			return(0);
		for (;;) {
			if (geterrec() == 0)
				break;
			if (next.lino != curr.lino || next.chno != curr.chno)
				break;
			if (curr.erno < 0 && next.erno > 0)
				/* promote warnings if they cause fatals */
				curr.erno = -curr.erno;
			if (next.mess[0] != '\0' || next.mesi != -1)
				/* give all parameterized errors */
				break;
			if (curr.mess[0] != '\0' || curr.mesi != -1)
				/* and at least a non-parameterized one */
				break;
		}
	} while (curr.erno < 0 && wflag != 0);
	return(1);
}

fillindex() {
	register *ip,n,c;

	if ((mesfil = fopen(err_path,"r")) == NULL)
		syserr(err_path);
	ip = index;
	*ip++ = 0;
	n = 0;
	while ((c = getc(mesfil)) != EOF) {
		n++;
		if (c == '\n') {
			*ip++ = n;
			if (ip > &index[MAXERNO])
				fatal("too many errors on %s",err_path);
		}
	}
	maxerno = ip - index;
}

string(ep) register struct errec *ep; {
	register i,n;

	errerr++;
	if ((i = ep->erno) < 0) {
		i = -i;
		printf("Warning: ");
	} else
		errfat++;
	if (i == 0 || i >= maxerno)
		fatal("bad error number %d",i);
	n = index[i] - index[i-1];
	fseek(mesfil,(long)index[i-1],0);
	while (--n >= 0) {
		i = getc(mesfil);
		if (i == '%' && --n>=0) {
			i = getc(mesfil);
			if (i == 'i')
				printf("%d", ep->mesi);
			else if (i == 's')
				printf("%s", ep->mess);
			else
				putchar(i);
		} else
			putchar(i);
	}
}

/* ------------------- error routines -------------------------- */

/* VARARGS1 */
void ermess(s,a1,a2,a3,a4) char *s; {

	fprintf(stderr,"%s: ",progname);
	fprintf(stderr,s,a1,a2,a3,a4);
	fprintf(stderr,"\n");
}

syserr(s) char *s; {
	fatal("%s: %s",s,sys_errlist[errno]);
}

/* VARARGS1 */
void fatal(s,a1,a2,a3,a4) char *s; {

	ermess(s,a1,a2,a3,a4);
	toterr++;
	finish();
}
