#include "f2c.h"
#include "fio.h"
#include "fmt.h"
#include "lio.h"
int L_len;

t_putc(c)
{
	recpos++;
	putc(c,cf);
	return(0);
}
lwrt_I(n) ftnint n;
{
	char buf[LINTW],*p;
#ifdef USE_STRLEN
	(void) sprintf(buf," %ld",(long)n);
	if(recpos+strlen(buf)>=L_len)
#else
	if(recpos + sprintf(buf," %ld",(long)n) >= L_len)
#endif
		(*donewrec)();
	for(p=buf;*p;PUT(*p++));
}
lwrt_L(n, len) ftnint n; ftnlen len;
{
	if(recpos+LLOGW>=L_len)
		(*donewrec)();
	(void) wrt_L((uint *)&n,LLOGW, len);
}
lwrt_A(p,len) char *p; ftnlen len;
{
	int i;
	if(recpos+len>=L_len)
		(*donewrec)();
	if (!recpos)
		{ PUT(' '); ++recpos; }
	for(i=0;i<len;i++) PUT(*p++);
}

 static int
l_g(buf, absn) char *buf; double absn;
{
	doublereal n;
	char *fmt;

	n = absn;
	if (absn < 0)
		absn = -absn;
	fmt = LLOW <= absn && absn < LHIGH ? LFFMT : LEFMT;
#ifdef USE_STRLEN
	sprintf(buf, fmt, n);
	return strlen(buf);
#else
	return sprintf(buf, fmt, n);
#endif
	}

 static void
l_put(s) register char *s;
{
	register int c, (*pn)() = putn;
	while(c = *s++)
		(*pn)(c);
	}

lwrt_F(n) double n;
{
	char buf[LEFBL];

	if(recpos + l_g(buf,n) >= L_len)
		(*donewrec)();
	l_put(buf);
}
lwrt_C(a,b) double a,b;
{
	char *ba, *bb, bufa[LEFBL], bufb[LEFBL];
	int al, bl;

	al = l_g(bufa, a);
	for(ba = bufa; *ba == ' '; ba++)
		--al;
	bl = l_g(bufb, b) + 1;	/* intentionally high by 1 */
	for(bb = bufb; *bb == ' '; bb++)
		--bl;
	if(recpos + al + bl + 3 >= L_len && recpos)
		(*donewrec)();
	PUT(' ');
	PUT('(');
	l_put(ba);
	PUT(',');
	if (recpos + bl >= L_len) {
		(*donewrec)();
		PUT(' ');
		}
	l_put(bb);
	PUT(')');
}
l_write(number,ptr,len,type) ftnint *number,type; char *ptr; ftnlen len;
{
#define Ptr ((flex *)ptr)
	int i;
	ftnint x;
	double y,z;
	real *xx;
	doublereal *yy;
	for(i=0;i< *number; i++)
	{
		switch((int)type)
		{
		default: fatal(204,"unknown type in lio");
		case TYSHORT:
			x=Ptr->flshort;
			goto xint;
		case TYLONG:
			x=Ptr->flint;
		xint:	lwrt_I(x);
			break;
		case TYREAL:
			y=Ptr->flreal;
			goto xfloat;
		case TYDREAL:
			y=Ptr->fldouble;
		xfloat: lwrt_F(y);
			break;
		case TYCOMPLEX:
			xx= &Ptr->flreal;
			y = *xx++;
			z = *xx;
			goto xcomplex;
		case TYDCOMPLEX:
			yy = &Ptr->fldouble;
			y= *yy++;
			z = *yy;
		xcomplex:
			lwrt_C(y,z);
			break;
		case TYLOGICAL:
			lwrt_L(Ptr->flint, len);
			break;
		case TYCHAR:
			lwrt_A(ptr,len);
			break;
		}
		ptr += len;
	}
	return(0);
}
