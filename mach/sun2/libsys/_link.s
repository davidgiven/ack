#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __link
__link: SYSTEM(SYS_link)
