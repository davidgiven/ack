/* $Id$ */
/* perror(s) print the current error message. */

extern int errno;
char *sys_errlist[] = {
        "Error 0",
        "Not owner",
        "No such file or directory",
        "No such process",
        "Interrupted system call",
        "I/O error",
        "No such device or address",
        "Arg list too long",
        "Exec format error",
        "Bad file number",
        "No children",
        "No more processes",
        "Not enough core",
        "Permission denied",
        "Bad address",
        "Block device required",
        "Mount device busy",
        "File exists",
        "Cross-device link",
        "No such device",
        "Not a directory",
        "Is a directory",
        "Invalid argument",
        "File table overflow",
        "Too many open files",
        "Not a typewriter",
        "Text file busy",
        "File too large",
        "No space left on device",
        "Illegal seek",
        "Read-only file system",
        "Too many links",
        "Broken pipe",
        "Math argument",
        "Result too large"
};

int	sys_nerr = sizeof(sys_errlist) / sizeof(sys_errlist[0]);

perror(s)
	char *s;
{
  	char *c;
  	if (errno < 0 || errno >= sizeof(sys_errlist) / sizeof(char *)) {
		c = "unknown error";
  	} else {
		c = sys_errlist[errno];
  	}
	write(2, s, strlen(s));
	write(2, ": ", 2);
	write(2, c, strlen(c));
	write(2, "\n", 1);
}
