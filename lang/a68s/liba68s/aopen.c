#include <pc_file.h>
#include <pc_err.h>

#define BUFFLENGTH 512          /* number of items in buffer */

extern struct file    *_curfil; /* for error mesages from trap */
extern                _trp();   /* pc runtime trap routine */
extern                creat();  /* unix open for write */
extern                open();   /* unix open for read */

static int initfile (desc,f) int desc; struct file *f; {

	_curfil=f;
	if ( (desc & WRBIT) == 0) {
		if ( (f->ufd = open(f->fname,0)) < 0 )
			_trp(ERESET);
	} else {
		if ( (f->ufd = creat(f->fname,0644)) < 0 )
			_trp(EREWR);
	}
	f->buflen = BUFFLENGTH;
	f->size = 1;
	f->ptr = f->bufadr;
	f->flags = desc;
	return(1);

}

/* as both the following routines are called from a pascal subroutine */
/* and the pascal compiler has been fixed to alwayd supply static links */
/* to non-global externals the parameter 'statlink' is a dummy to fill */
/* the space occupied by the static link. The parameter is first instead */
/* of last because of the C method of passing its parameters backwards */

AOPN(statlink,f) int *statlink; struct file *f; {

	if ( initfile ((int)(MAGIC|TXTBIT),f) )
		f->count=0;
}

ACRE(statlink,f) int *statlink; struct file *f; {

	if ( initfile ((int)(WRBIT|EOFBIT|ELNBIT|MAGIC|TXTBIT),f) )
		f->count=f->buflen;
}

