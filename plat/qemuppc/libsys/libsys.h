#ifndef LIBSYS_H
#define LIBSYS_H

extern uint32_t openfirmware_call(void* array);

extern void _sys_rawwrite(unsigned char b);
extern unsigned char _sys_rawread(void);

extern void _sys_write_tty(char c);

/* extern int _sys_ttyflags; */

#endif
