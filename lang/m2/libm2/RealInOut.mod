IMPLEMENTATION MODULE RealInOut;

  IMPORT InOut;
  IMPORT RealConversions;
  IMPORT Traps;
  FROM SYSTEM IMPORT WORD;

  CONST	MAXNDIG = 32;
	MAXWIDTH = MAXNDIG+7;
  TYPE	RBUF = ARRAY [0..MAXWIDTH+1] OF CHAR;

  PROCEDURE WriteReal(arg: REAL; ndigits: CARDINAL);
    VAR buf : RBUF;
	ok : BOOLEAN;

  BEGIN
	IF ndigits > MAXWIDTH THEN ndigits := MAXWIDTH; END;
	IF ndigits < 10 THEN ndigits := 10; END;
	RealConversions.RealToString(arg, ndigits, -(ndigits - 7), buf, ok);
	InOut.WriteString(buf);
  END WriteReal;

  PROCEDURE ReadReal(VAR x: REAL);
    VAR	Buf: ARRAY[0..512] OF CHAR;
	ok: BOOLEAN;

  BEGIN
	InOut.ReadString(Buf);
	RealConversions.StringToReal(Buf, x, ok);
	IF NOT ok THEN
		Traps.Message("real expected");
		HALT;
	END;
	Done := TRUE;
  END ReadReal;

  PROCEDURE wroct(x: ARRAY OF WORD);
  VAR	i: CARDINAL;
  BEGIN
	FOR i := 0 TO HIGH(x) DO
		InOut.WriteOct(CARDINAL(x[i]), 0);
		InOut.WriteString("  ");
	END;
  END wroct;

  PROCEDURE WriteRealOct(x: REAL);
  BEGIN
	wroct(x);
  END WriteRealOct;

BEGIN
	Done := FALSE;
END RealInOut.
