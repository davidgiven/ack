#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getpgrp
_getpgrp: SYSTEM(SYS_getpgrp)
