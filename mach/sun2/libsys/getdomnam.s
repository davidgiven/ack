#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getdomainname
_getdomainname: SYSTEM(SYS_getdomainname)
