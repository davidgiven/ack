#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setdomainname
_setdomainname: SYSTEM(SYS_setdomainname)
