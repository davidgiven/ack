/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */

char rcs_id[] = "$Id$" ;

/* Test initialisation of a V7 C-compiler */
/* 1 sept 1980 */
#include "../local.h"

/* Integers and constant expressions */

int in1 = 4 ;
int in2 = MAXINT ;
int in3 = MININT ;
int in4 ;
int inzero ;

int ice1 = (1-2+3*4/2)%3 ;
int ice2 = ((((1&3)|4)^014) >> 1) <<1 ;
int ice3 = ( (1==2) & (3<4) ) | (4>3) | (0<=0) | -2>=17 ;
int ice4 = (~-1) ;
int ice5 = (1==1 ? 3+4 : 5+6 ) ;
int ice6 = (1!=1 ? 7+8 : 9+10 ) ;
int ina[] = { 1, 3, 5 } ;

pint() {
	static int sint = -1 ;
	int lint = in1+in3+sint ;

	printf("Integers:\n\n") ;
	printf("in1\t%d\nin2\t%d\nin3\t%d\nin4\t%d\ninzero\t%d\n\n",
		in1,in2,in3,in4,inzero ) ;
	printf("ice1\t%d\nice2\t%d\nice3\t%d\nice4\t%d\nice5\t%d\nice6\t%d\n\n",
		ice1,ice2,ice3,ice4,ice5,ice6 ) ;
	printf("ina\t%d,%d,%d\n\n",ina[0],ina[1],ina[2]) ;
	printf("sint\t%d\nlint\t%d\n\n",sint,lint) ;
}

/* Characters */

char ch1 = 'a' ;
char ch2 ;
char cha1[] = "Mesg" ;
char cha2[] = "" ;
char cha3[] = "1" ;
char cha4[] = "12" ;
char cha5[] = "0123456789112345678921234567893123456789412345678951234567896123456789712345678981234567899123456789" ;

char cha6[2][3] = {
	{ 1, 2, 3 },
	{ 4, 5, 6 }
};
char *pch1 = cha2 ;
char *pch2 = "pch2" ;
char *pch3 = "ppch3" ;
char *pch4 = 0 ;

pch() {
	static char stc[] = "123" ;
	static char stc1[] = "1234" ;
	static char *mult[] = { "ab" , "bc" , "de" } ;

	printf("Characters:\n\n") ;

	printf("ch1\t%c(%d)\n",ch1,ch1) ;
	printf("ch2\t%d\n",ch2) ;
	printf("cha1\t%s\ncha2\t%s\ncha3\t%s\ncha4\t%s\n",
		cha1,cha2,cha3,cha4 ) ;
	printf("cha5\t%s\n\n",cha5) ;
	printf("cha6\t%d, %d, %d\n\t%d, %d, %d\n",
       cha6[0][0],cha6[0][1],cha6[0][2],cha6[1][0],cha6[1][1],cha6[1][2]);
	printf("pch1==cha2\t%s\n",(pch1 == cha2 ? "yes" : "no" ) ) ;
	printf("pch2\t%s\npch3\t%s\n",pch2,pch3+1) ;
	printf("pch4==0\t%s\n\n",(pch4 != 0 ? "no" : "yes" ) ) ;
	printf("stc\t%s\nstc1\t%s\n",stc,stc1) ;
	printf("mult[0],mult[1],mult[2] %s, %s, %s\n",mult[0],mult[1],mult[2]);
}

#ifndef NOFLOAT
/* floats */

float fl1 = 0 ;
float fl2 = 2 ;
float fl3 = 2e-10 ;
float fl4 = 4.0 ;
float fl5 = EPSFLOAT ;
float fl6 = MAXFLOAT ;
float fl7 ;

float fla1[4][3] = {
	{ 1, 3, 5 },
	{ 2, 4, 6 },
	{ 3, 5, 7 }
} ;
float fla2[4][3] = {
	-1,-3,-5,-2,-4,-6,-3,-5,-7
} ;
float fla3[4][3] = {
	{ 11 } , { 12 } , { 13 } , { 14 }
} ;

pflt() {
	register i,j ;

	printf("Floats:\n\n") ;

printf("fl1\t%.20e\nfl2\t%.20e\nfl2\t%.20e\nfl4\t%.20e\nfl5\t%.20e\nfl6\t%.20e\nfl7\t%.20e\n",
	fl1,fl2,fl2,fl4,fl5,fl6,fl7 ) ;

	printf("    fla1                    fla2                    fla3\n") ;
	for ( i=0 ; i<4 ; i++ ) {
		for ( j=0 ; j<3 ; j++ ) {
			printf("    %20e    %20e    %20e\n",
				fla1[i][j],fla2[i][j],fla3[i][j]) ;
		}
	}

	printf("\n") ;
}

/* doubles */

double dbl1 = 0 ;
double dbl2 = 2 ;
double dbl3 = 2e-10 ;
double dbl4 = 4.0 ;
double dbl5 = EPSDOUBLE ;
double dbl6 = MAXDOUBLE ;
double dbl7 ;

double dbla1[4][3] = {
	{ 1, 3, 5 },
	{ 2, 4, 6 },
	{ 3, 5, 7 }
} ;
double dbla2[4][3] = {
	-1,-3,-5,-2,-4,-6,-3,-5,-7
} ;
double dbla3[4][3] = {
	{ 11 } , { 12 } , { 13 } , { 14 }
} ;

pdbl() {
	register i,j ;

	printf("Doubles:\n\n") ;

printf("dbl1\t%.20e\ndbl2\t%.20e\ndbl2\t%.20e\ndbl4\t%.20e\ndbl5\t%.20e\ndbl6\t%.20e\ndbl7\t%.20e\n",
	dbl1,dbl2,dbl2,dbl4,dbl5,dbl6,dbl7 ) ;

	printf("    dbla1                    dbla2                    dbla3\n") ;
	for ( i=0 ; i<4 ; i++ ) {
		for ( j=0 ; j<3 ; j++ ) {
			printf("    %20e    %20e    %20e\n",
				dbla1[i][j],dbla2[i][j],dbla3[i][j]) ;
		}
	}

	printf("\n") ;
}
#endif

/* long */
long lo1 = 14L ;
long lo2 = -17 ;
long lo3 = MAXLONG ;
long lo4 = MINLONG ;
long lo5 ;
long lo6 = ( 0==1 ? -1L : 1L ) ;

plong() {
	printf("long\n\n") ;

	printf("lo1\t%ld\nlo2\t%ld\nlo3\t%ld\nlo4\t%ld\nlo5\t%ld\nlo6\t%ld\n\n",
		lo1,lo2,lo3,lo4,lo5,lo6 ) ;
}

/* structures and bit fields */

struct s1 {
	int	s_i ;
	char	s_ca[3] ;
	long	s_l ;
#ifndef NOFLOAT
	double	s_f ;
#endif
	struct	s1 *s_s1 ;
} ;
struct s1 st1 ;
struct s1 sta[3] = {
#ifndef NOFLOAT
	1 , { 'a' , 'b' , 'c' } , 10 , -10 , &sta[0] ,
#else
	1 , { 'a' , 'b' , 'c' } , 10 , &sta[0] ,
#endif
	{ 2 } ,
	3
} ;
struct s2 {
	int s2_1 :1 ;
	int s2_2 :2 ;
	int s2_3 :4 ;
	int s2_4 :7 ;
	int s2_5 :2 ;
	int s2_6 :11 ;
	int s2_7 :6 ;
} stb = {
	1,2,3,4,3,6,7
} ;

pstruct() {
	printf("structures\n\n") ;

	printf("\t st1          sta[0..2]\n") ;

	printf("s_i\t%15d%15d%15d%15d\n",
		st1.s_i,sta[0].s_i,sta[1].s_i,sta[2].s_i) ;
	printf("s_ca[0]\t%15d%15d%15d%15d\n",
		st1.s_ca[0],sta[0].s_ca[0],sta[1].s_ca[0],sta[2].s_ca[0]) ;
	printf("s_ca[1]\t%15d%15d%15d%15d\n",
		st1.s_ca[1],sta[0].s_ca[1],sta[1].s_ca[1],sta[2].s_ca[1]) ;
	printf("s_ca[2]\t%15d%15d%15d%15d\n",
		st1.s_ca[2],sta[0].s_ca[2],sta[1].s_ca[2],sta[2].s_ca[2]) ;
	printf("s_l\t%15ld%15ld%15ld%15ld\n",
		st1.s_l,sta[0].s_l,sta[1].s_l,sta[2].s_l) ;
#ifndef NOFLOAT
	printf("s_f\t  %13e  %13e  %13e  %13e\n\n",
		st1.s_f,sta[0].s_f,sta[1].s_f,sta[2].s_f) ;
#endif
	printf("(sta[0].s_s1)->s_i = %d\n",(sta[0].s_s1)->s_i) ;

	printf("\nbit fields:\n\n") ;
	printf("sizeof stb %d\n",sizeof stb) ;
	printf("stb\t%d %d %d %d %d %d %d\n\n",
		stb.s2_1,stb.s2_2,stb.s2_3,stb.s2_4,stb.s2_5,stb.s2_6,stb.s2_7);
}

main() {
	pint() ;
	pch() ;
#ifndef NOFLOAT
	pflt() ;
	pdbl() ;
#endif
	plong() ;
	pstruct() ;
	return(0) ;
}
