#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _writev
_writev: SYSTEM(SYS_writev)
