PROCEDURE dbug (number : INTEGER);
BEGIN
	writeln('dbug value',number)
END;

PROCEDURE dump (VAR start,finish : INTEGER);  EXTERN;

BEGIN
	reset(A68INIT);
	reset(LGO);
	reset(SOURCDECS);
	rewrite(LSTFILE);
	rewrite(DUMPF);
	dump(firststack, laststack);
END.
