#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fstatfs
_fstatfs: SYSTEM(SYS_fstatfs)
