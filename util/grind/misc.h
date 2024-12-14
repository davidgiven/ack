#ifndef MISC_H
#define MISC_H

extern void fatal(char*, ...);
extern void error(char*, ...);
extern void warning(char*, ...);

extern int get_name(int);
extern void prompt(void);
extern void init_run(void);
extern void init_del(void);

#endif
