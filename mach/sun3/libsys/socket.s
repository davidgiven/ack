#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _socket
_socket: SYSTEM(SYS_socket)
