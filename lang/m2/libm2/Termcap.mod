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

  TYPE	STR = ARRAY[1..32] OF CHAR;
	STRCAP = POINTER TO STR;

  VAR	Buf, Buf1 : ARRAY [1..1024] OF CHAR;
	UP, BC: STRCAP;
	PC: CHAR;
      	Initialized: BOOLEAN;
	BufCnt : INTEGER;

  PROCEDURE Tgetent(name: ARRAY OF CHAR) : INTEGER;
  VAR i: INTEGER;
  BEGIN
	i := XXTermcap.tgetent(ADR(Buf), ADR(name));
	BufCnt := 1;
	IF i > 0 THEN
		Initialized := TRUE;
		UP := Tgetstr("pc");
		IF UP # NIL THEN
			PC := UP^[1];
		ELSE	PC := 0C;
		END;
		UP := Tgetstr("up");
		BC := Tgetstr("bc");
	END;
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

  PROCEDURE Tgoto(cm: STRCAP; col, line: INTEGER): STRCAP;
  BEGIN
	XXTermcap.UP := UP;
	XXTermcap.BC := BC;
	RETURN XXTermcap.tgoto(cm, col, line);
  END Tgoto;

  PROCEDURE Tgetstr(id: ARRAY OF CHAR): STRCAP;
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

  PROCEDURE Tputs(cp: STRCAP; affcnt: INTEGER; p: PUTPROC);
  BEGIN
	XXTermcap.PC := PC;
	XXTermcap.ospeed := ospeed;
	XXTermcap.tputs(cp, affcnt, XXTermcap.PUTPROC(p));
  END Tputs;

BEGIN
	Initialized := FALSE;
END Termcap.
