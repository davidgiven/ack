#include "syscall.h"
DEFINE(_reboot)
SYSNORET(SYS_reboot)
	halt
