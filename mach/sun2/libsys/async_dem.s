#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _async_daemon
_async_daemon: SYSTEM(SYS_async_daemon)
