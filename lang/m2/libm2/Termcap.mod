(*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*)
 
(* 
  Module:       Interface to termcap database
  From:         Unix manual chapter 3 
  Version:      $Header$ 
*)

(*$R-*)
IMPLEMENTATION MODULE Termcap;

  IMPORT XXTermcap;
  FROM SYSTEM IMPORT ADR, ADDRESS;

  VAR	Buf, Buf1 : ARRAY [1..1024] OF CHAR;
      	Initialized: BOOLEAN;
	BufCnt : INTEGER;

  PROCEDURE Tgetent(name: ARRAY OF CHAR) : INTEGER;
  VAR i: INTEGER;
  BEGIN
	i := XXTermcap.tgetent(ADR(Buf), ADR(name));
	IF i > 0 THEN
		Initialized := TRUE
	END;
	BufCnt := 1;
	RETURN i;
  END Tgetent;

  PROCEDURE Tgetnum(id: ARRAY OF CHAR): INTEGER;
  BEGIN
	IF NOT Initialized THEN
		RETURN -1;
	END;
	RETURN XXTermcap.tgetnum(ADR(id));
  END Tgetnum;

  PROCEDURE Tgetflag(id: ARRAY OF CHAR): BOOLEAN;
  BEGIN
	IF NOT Initialized THEN
		RETURN FALSE;
	END;
	RETURN XXTermcap.tgetflag(ADR(id)) = 1;
  END Tgetflag;

  PROCEDURE Tgoto(cm: PTSTR; col, line: INTEGER): PTSTR;
  BEGIN
	XXTermcap.UP := UP;
	XXTermcap.BC := BC;
	RETURN XXTermcap.tgoto(cm, col, line);
  END Tgoto;

  PROCEDURE Tgetstr(id: ARRAY OF CHAR): PTSTR;
  VAR a, a2: ADDRESS;
      b: CARDINAL;
  BEGIN
	IF NOT Initialized THEN
		RETURN NIL;
	END;
	a := ADR(Buf1[BufCnt]);
	a2 := XXTermcap.tgetstr(ADR(id), ADR(a));
	IF a2 = NIL THEN
		RETURN NIL;
	END;
	b := a - a2;
	INC(BufCnt, b);
	RETURN a2;
  END Tgetstr;

  PROCEDURE Tputs(cp: PTSTR; affcnt: INTEGER; p: PUTPROC);
  BEGIN
	XXTermcap.PC := PC;
	XXTermcap.ospeed := ospeed;
	XXTermcap.tputs(cp, affcnt, XXTermcap.PUTPROC(p));
  END Tputs;

BEGIN
	Initialized := FALSE;
	PC := 0C;
END Termcap.
