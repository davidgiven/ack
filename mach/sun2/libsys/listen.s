#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _listen
_listen: SYSTEM(SYS_listen)
