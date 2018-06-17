#ifndef PC_H
#define PC_H

#include "pc_err.h"
#include "pc_file.h"
#include "pc_math.h"

struct descr
{
	int low;
	int diff;
	int size;
};

struct array_descr
{
	int lbound;
	unsigned n_elts_min_one;
	unsigned size; /* doesn't really matter */
};


extern int _extflc;
extern struct file** _extfl;

extern struct file* _curfil;
extern int _pargc;
extern char** _pargv;
extern char** _penvp;

extern char* _m_lb; /* LB of _m_a_i_n */
extern struct file* _curfil; /* points to file struct in case of errors */

extern char *_ecvt(double value, int ndigit, int *decpt, int *sign);
extern char *_fcvt(double value, int ndigit, int *decpt, int *sign);
extern char *_wdw(struct file *f);
extern char *argv(int i);
extern char *strbuf(char *s);
extern double _abr(double r);
extern double _atn(double x);
extern double _cos(double x);
extern double _exp(double x);
extern double _log(double x);
extern double _rdr(struct file *f);
extern double _rnd(double r);
extern void _sig(void (*)(unsigned int));
extern double _sin(double x);
extern double _sqt(double x);
extern void _xcls(struct file *f);
extern int argc(void);
extern void argshift(void);
extern void buff(struct file *f);
extern void diag(struct file *f);
extern void nobuff(struct file *f);
extern void notext(struct file *f);
extern void pclose(struct file *f);
extern void pcreat(struct file *f, char *s);
extern int perrno(void);
extern void popen(struct file *f, char *s);
extern void procentry(char *name);
extern void procexit(char *name);
extern int strfetch(char *s, int i);
extern int strlen(char *s);
extern void strstore(char *s, int i, int c);
extern int strtobuf(char *s, char *b, int l);
extern int uread(int fd, char *b, int n);
extern int uwrite(int fd, char *b, int n);
extern long _abl(long i);
extern long _dvil(unsigned long j, unsigned long i);
extern long _mdil(long j, long i);
extern long _mdl(long j, long i);
extern long _rdl(struct file *f);
extern int _abi(int i);
extern void _ass(int line, int bool);
extern int _asz(struct descr *dp);
extern int _bcp(int sz, unsigned char *y, unsigned char *x);
extern void _catch(unsigned erno);
extern void _cls(struct file *f);
extern void _cre(int sz, struct file *f);
extern void _dis(int n, void **ptr);
extern int _dvi(unsigned int j, unsigned int i);
extern int _efl(struct file *f);
extern int _eln(struct file *f);
extern double _fef(double fl, int* res);
extern double _fif(double f1, double f2, double* f3);
extern void _flush(struct file *f);
extern int _fstdig(struct file *f);
extern void _get(struct file *f);
extern int _getint(struct file *f);
extern int _getsig(struct file *f);
extern void _hlt(int ecode);
extern void _incpt(struct file *f);
extern void _ini(char *args, int c, struct file **p, char *mainlb);
extern int _mdi(int j, int i);
extern void _new(int n, void **ptr);
extern void _nfa(int bool);
extern int _nxtdig(struct file *f);
extern void _opn(int sz, struct file *f);
extern void _outcpt(struct file *f);
extern void _pac(struct descr *ad, struct descr *zd, char *zp, int i, char *ap);
extern void _pag(struct file *f);
extern void _put(struct file *f);
extern void _rcka(struct array_descr *descr, int index);
extern int _rdc(struct file *f);
extern int _rdi(struct file *f);
extern void _rf(struct file *f);
extern void _rln(struct file *f);
extern void _skipsp(struct file *f);
extern void _trp(int trapno);
extern void _unp(struct descr *ad, struct descr *zd, int i, char *ap, char *zp, int noext);
extern void _wf(struct file *f);
extern void _wln(struct file *f);
extern void _wrb(int b, struct file *f);
extern void _wrc(int c, struct file *f);
extern void _wrf(int n, int w, double r, struct file *f);
extern void _wri(int i, struct file *f);
extern void _wrl(long l, struct file *f);
extern void _wrr(double r, struct file *f);
extern void _wrs(int len, char *s, struct file *f);
extern void _wrz(char *s, struct file *f);
extern void _wsb(int w, int b, struct file* f);
extern void _wsc(int w, char c, struct file* f);
extern void _wsi(int w, int i, struct file* f);
extern void _wsl(int w, long l, struct file *f);
extern void _wsr(int w, double r, struct file *f);
extern void _wss(int w, int len, char* s, struct file* f);
extern void _wstrin(int width, int len, char* buf, struct file* f);
extern void _wsz(int w, char* s, struct file* f);
extern char* _hol0(void);

#endif 
