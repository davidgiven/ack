#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setpgrp
_setpgrp: SYSTEM(SYS_setpgrp)
