#
IMPLEMENTATION MODULE TTY;
FROM Unix IMPORT ioctl;
FROM SYSTEM IMPORT ADR;
PROCEDURE isatty(fd: INTEGER): BOOLEAN;
VAR buf: ARRAY[1..100] OF CHAR;
BEGIN
#ifdef __USG
	RETURN ioctl(fd, INTEGER(ORD('T') * 256 + 1), ADR(buf)) >= 0;
#else
#ifdef __BSD4_2
	RETURN ioctl(fd, INTEGER(ORD('t') * 256 + 8 + 6*65536 + 40000000H), ADR(buf)) >= 0;
#else
	RETURN ioctl(fd, INTEGER(ORD('t') * 256 + 8), ADR(buf)) >= 0;
#endif
#endif
END isatty;
END TTY.
