/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */

char rcs_id[] = "$Id$" ;

/* This programs is a collection of derived from small tests develloped
   for specific bugs/features in the C->EM compiler
*/

char * err_name ;

set_err(s) char *s ; {
	printf("%s\n",s) ;
	err_name= s ;
}
e(i) {
	printf("%s: error %d\n",err_name,i) ;
}

main() {
	cmp_rev() ;
	loc_dif() ;
	con_fold() ;
	ass_res() ;
	c_to_l() ;
	acc_struct() ;
	char_param() ;
	addr_lb() ;
	compl_ind() ;
	printf("END\n") ;
	return 0;
}

cmp_rev() {
	/* Some compilers magically transform the second < into a > */
	int i,j ;
	int result ;

	set_err("cmp_rev") ;
	i=2 ; j=1 ;
	result= ( (j-i<0) == (j-i<0) ) ? 1 : 0 ;
	if ( !result ) e(1) ;
}

loc_dif() {
	set_err("loc_dif") ;
	loc_fa(1,2) ;
}

loc_fa(p1,p2) {
	int i ;
	if ( &p1-&p2 != -1 ) e(1) ;
	if ( &i-&p1 >=0 ) e(2) ;
	if ( &p1-&i <=0 ) e(3) ;
}

con_fold() {
	set_err("con_fold") ;
#ifndef NOFLOAT
	con_flo( (1 ? 3 : 4.5), 200, 200, 200 ) ;
#endif
	con_lo( 4L + 3, 1 ) ;
}
#ifndef NOFLOAT
con_flo(d) double d ; {
	if ( d>3.00001 || d<2.99999 ) e(1) ;
}
#endif
con_lo(l) long l ; {
	if ( l!=7 ) e(2) ;
}

ass_res() {
	char c, *pc ;
	int i ;
	int s_extend ;

	set_err("ass_res") ;
	c = -1 ;  i=c ;
	s_extend= i== -1 ;

	pc = &c ;
	i = ( *pc++ = 01777 ) ;
	switch ( i ) {
	case 01777 :
		e(1) ; break ;
	case -1 :
		if ( !s_extend ) e(2) ;
		break ;
	case 0377 :
		if  ( s_extend ) e(3) ;
		break ;
	default :
		e(4) ;
	}
}

c_to_l() {
	char c = -1 ;
	long l ;

	set_err("c_to_l") ;
	l= c ;
	if ( c==255 ) {
		if ( l!=255 ) e(1) ;
	} else {
		if ( l!= -1 ) e(2) ;
	}
}

acc_struct() {
	struct s1 { char s1_a[3] ; } ss1, is1 ;
	struct s2 {
		int             s2_i ;
		struct s1       s2_s1 ;
	} ;
	struct s3 {
		int             s3_i ;
		struct s2       s3_s2 ;
	} ss3, *ps3 ;

	set_err("acc_struct") ;
	ps3 = &ss3 ;
	is1.s1_a[0]=1 ; is1.s1_a[1]=100 ; is1.s1_a[2]=127 ;
	ss3.s3_s2.s2_s1= is1 ;
	ss1 = ps3->s3_s2.s2_s1 ;
	if ( ss1.s1_a[0]!=1 )   e(1) ;
	if ( ss1.s1_a[1]!=100 ) e(2) ;
	if ( ss1.s1_a[2]!=127 ) e(3) ;
}

char_param() {
	set_err("char_param") ;
	fcall(1,01002,-1) ;
}

fcall(c1,c2,c3) char c1,c2,c3 ; {
	if ( c1!=1 ) e(1) ;
	if ( c2!=2 ) e(2) ;
	c_alter(&c1,127) ;
	if ( c1!=127 ) e(3) ;
	c_alter(&c3,0) ;
	if ( c3 ) e(4) ;
}

c_alter(ptr,val) char *ptr ; int val ; {
	*ptr= val ;
}

addr_lb() {
	char a[6] ;
	int i ;

	set_err("addr_lb");
	i=6 ;
	if ( &a[6] != a+i ) e(1) ;
}
compl_ind() {
	char arr[20] ;
	int i ;
	set_err("compl_ind") ;
	arr[10]=111 ;
	i=0 ; if ( arr[i+10] != 111 ) e(1) ;
}
