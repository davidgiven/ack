/* chstack - set stack size */

#include <stdio.h>

#define OFF         48L		/* offset of stacksize in file */
#define MAX	    32768L	/* maximum accepted stacksize */

main(argc, argv)
int argc;
char *argv[];
{
  char *p;
  long n;
  int fd;
  long current;
  long old;
  long atol(), lseek();

  p = argv[1];
  if (argc != 3 || (*p != '=' && *p != '+' && *p != '-')) {
  	fprintf(stderr, "Usage: %s {=+-}amount file\n", argv[0]);
	exit(1);
  }
  n = atol(p+1);

  fd = open(argv[2], 2);
  if (fd < 0) {
	fprintf(stderr, "%s: cannot open %s\n", argv[0], argv[2]);
	exit(1);
  }

  if (lseek(fd, OFF, 0) < 0 || getlong(fd, &current) < 0) {
	fprintf(stderr, "%s: cannot read %s\n", argv[0], argv[2]);
	exit(1);
  }
  old = current;
  if (*p == '=') current = n;
  else if (*p == '-') current -= n;
  else current += n;

  if (current > MAX || current <= 0) {
	fprintf(stderr, "%s: resulting stack size %ld too large or too small\n", argv[0], current);
	exit(1);
  }
  if (lseek(fd, OFF, 0) < 0 || putlong(fd, &current) < 0) {
	fprintf(stderr, "%s: can't modify %s\n", argv[0], argv[2]);
	exit(1);
  }
  printf("%s: Stack area changed from %ld to %ld bytes.\n",
			 argv[2],  old, current);
  exit(0);
}

getlong(fd, l)
	long *l;
{
	char buf[4];

	if (read(fd, buf, 4) < 4) return -1;
	*l = ((((long)(buf[3]&0377)<<8)|(long)(buf[2]&0377)<<8)|(long)(buf[1]&0377)<<8)|(long)(buf[0]&0377);
	return 0;
}

putlong(fd, l)
	long *l;
{
	char buf[4];

	buf[3] = *l >> 24;
	buf[2] = *l >> 16;
	buf[1] = *l >> 8;
	buf[0] = *l;
	if (write(fd, buf, 4) < 4) return -1;
	return 0;
}
