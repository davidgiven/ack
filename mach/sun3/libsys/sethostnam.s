#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sethostname
_sethostname: SYSTEM(SYS_sethostname)
