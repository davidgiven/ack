(*$R-*)
IMPLEMENTATION MODULE Traps;
  IMPORT EM;
  IMPORT Unix;
  FROM SYSTEM IMPORT ADDRESS, ADR;
  FROM Arguments IMPORT Argv;

  PROCEDURE InstallTrapHandler(t: TrapHandler): TrapHandler;
  (* Install a new trap handler, and return the previous one.
     Parameter of trap handler is the trap number.
  *)
  BEGIN
	RETURN EM.SIG(t);
  END InstallTrapHandler;

  PROCEDURE Message(str: ARRAY OF CHAR);
  (* Write message "str" on standard error, preceeded by filename and
     linenumber if possible
  *)
  VAR 	p, q: POINTER TO CHAR;
	l: CARDINAL;
	dummy, lino: INTEGER;
	buf, buf2: ARRAY [0..255] OF CHAR;
	i, j: CARDINAL;
  BEGIN
	p := EM.FILN();
	IF p # NIL THEN
		q := p;
		WHILE p^ # 0C DO
			p := ADDRESS(p) + 1;
		END;
		dummy := Unix.write(2, q, ADDRESS(p) - ADDRESS(q));
	ELSE
		l := Argv(0, buf);
		dummy := Unix.write(2, ADR(buf), l);
	END;
	lino := EM.LINO();
	i := 0;
	IF lino # 0 THEN
		i := 2;
		buf[0] := ',';
		buf[1] := ' ';
		IF lino < 0 THEN
			buf[2] := '-';
			i := 3;
			lino := - lino;
		END;
		j := 0;
		REPEAT
			buf2[j] := CHR(CARDINAL(lino) MOD 10 + ORD('0'));
			lino := lino DIV 10;
			INC(j);
		UNTIL lino = 0;
		WHILE j > 0 DO
			DEC(j);
			buf[i] := buf2[j];
			INC(i);
		END;
	END;
	buf[i] := ':';
	buf[i+1] := ' ';
	dummy := Unix.write(2, ADR(buf), i+2);
	i := 0;
	WHILE (i <= HIGH(str)) AND (str[i] # 0C) DO
		INC(i);
	END;
	dummy := Unix.write(2, ADR(str), i);
	buf[0] := 12C;
	dummy := Unix.write(2, ADR(buf), 1);
  END Message;

  PROCEDURE Trap(n: INTEGER);
  (* cause trap number "n" to occur *)
  BEGIN
	EM.TRP(n);
  END Trap;

END Traps.
