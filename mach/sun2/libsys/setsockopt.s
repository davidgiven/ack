#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setsockopt
_setsockopt: SYSTEM(SYS_setsockopt)
