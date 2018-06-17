#ifndef PC_H
#define PC_H

#include "pc_err.h"
#include "pc_file.h"
#include "pc_math.h"

extern void _trp(int trapno);
extern void _wrb(int b, struct file* f);
extern void _wrs(int len, char* s, struct file* f);
extern void _wrz(char* s, struct file* f);
extern void _wsb(int w, int b, struct file* f);
extern void _wsc(int w, char c, struct file* f);
extern void _wss(int w, int len, char* s, struct file* f);
extern void _wstrin(int width, int len, char* buf, struct file* f);
extern void _wsz(int w, char* s, struct file* f);

#endif
