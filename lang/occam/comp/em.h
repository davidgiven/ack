/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef nil
#define nil 0
#endif

#define word_constant(c)	(-32768L<=(c) && (c)<=32767L)
void Label(), dot_label(), branch();
int new_label(), new_dot_label();
char *proc_label();
extern char *curr_file;

void cwv(), cvw();
void bxx(), Loc(), Lol(), Lolp(), Lil(), Lof(), Lofp(), Lif();
void Txx(), xxi(), Stl(), Inl(), Del(), Loe();
void cmi(), ngi(), and(), ior(), xor(), sli(), sri(), com(), lar(), lxl();
void lxa(), lfr(), ste(), lae(), aar(), lal(), adp(), ldc0(), zeq(), zne();
void zlt(), zgt(), blm(), sti(), cal(), asp(), loc(), lor0(), loi(), pro();
void ret(), x_end(), stl(), laedot(), del(), lol(), ldl(), meswp(), meserr();
void init_rt(), exp(), rom(), blt(), magic(), lin(), tst(), fil(), trp();
void main_fil(), init(), openfile(), closefile(), maxdes();

void par_begin(), par_fork(), par_end(), resumenext(), no_deadlock();
extern int Lflag;
