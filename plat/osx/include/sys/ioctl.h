#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#define TIOCGETD        0x4004741a

extern int ioctl(int fd, unsigned long request, ...);

#endif
