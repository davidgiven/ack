#include "f2c.h"
#include "fio.h"
integer f_clos(a) cllist *a;
{	unit *b;
	if(a->cunit >= MXUNIT) return(0);
	b= &units[a->cunit];
	if(b->ufd==NULL)
		goto done;
	if (!a->csta)
		if (b->uscrtch == 1)
			goto delete;
		else
			goto keep;
	switch(*a->csta) {
		default:
	 	keep:
		case 'k':
		case 'K':
			if(b->uwrt == 1)
				(void) t_runc((alist *)a);
			if(b->ufnm) {
				(void) fclose(b->ufd);
				free(b->ufnm);
				}
			break;
		case 'd':
		case 'D':
		delete:
			if(b->ufnm) {
				(void) fclose(b->ufd);
				(void) unlink(b->ufnm); /*SYSDEP*/
				free(b->ufnm);
				}
		}
	b->ufd=NULL;
 done:
	b->uend=0;
	b->ufnm=NULL;
	return(0);
	}
 void
f_exit()
{	int i;
	static cllist xx;
	if (!xx.cerr) {
		xx.cerr=1;
		xx.csta=NULL;
		for(i=0;i<MXUNIT;i++)
		{
			xx.cunit=i;
			(void) f_clos(&xx);
		}
	}
}
flush_()
{	int i;
	for(i=0;i<MXUNIT;i++)
		if(units[i].ufd != NULL) (void) fflush(units[i].ufd);
}
