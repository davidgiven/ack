extern int errno;

struct utsname {
	char sysname[9],nodename[9],release[9],version[9],machine[9];
};

static char def_node[] = "unknown";
static char rel[] = "4.2BSD";
static char ver[] = "vm";
static char mach[] = "sun";

uname(nm)
	register struct utsname *nm;
{
	register char *p = nm->nodename;

	while (p <= nm->release) *p++ = 0;
	if (gethostname(nm->nodename,9) == -1) {
		strcpy(nm->nodename, def_node);
	}
	strncpy(nm->sysname,nm->nodename,9);
	strncpy(nm->release,rel,9);
	strncpy(nm->version,ver,9);
	strncpy(nm->machine,mach,9);
	return 0;
}
