PROCEDURE dbug (number : INTEGER);
BEGIN
	writeln('dbug value ',number)
END;

PROCEDURE dbugl (number : LONG);
BEGIN
	writeln('long dbug value ',number);
END;

PROCEDURE algol68; EXTERNAL;
PROCEDURE encaps(PROCEDURE p;PROCEDURE q(n:integer));EXTERNAL;
PROCEDURE trap(err: integer); EXTERNAL;
PROCEDURE abort; EXTERNAL;

PROCEDURE traphandler (n:INTEGER);
BEGIN
	writeln('***Trap ',n:0,' has occured');
	trap(n);
	abort;
END;

PROCEDURE mainprog;
BEGIN
	algol68
END;

BEGIN
	encaps(mainprog,traphandler)
END.

