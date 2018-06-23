The functions here all use Posix system calls to do the actual work, and so
require `unistd.h` (at the minimum). Typically each group of functions will
be protected by an `ACKCONF` variable so the plat can turn them on and off as
necessary.
