#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _exportfs
_exportfs: SYSTEM(SYS_exportfs)
