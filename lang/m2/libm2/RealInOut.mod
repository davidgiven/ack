IMPLEMENTATION MODULE RealInOut;

  FROM FIFFEF IMPORT FIF, FEF;
  IMPORT InOut;

  CONST	NDIG = 80;

  TYPE	string = ARRAY[0..NDIG+6] OF CHAR;

  PROCEDURE cvt(arg: REAL;
		ndigits: INTEGER;
		VAR decpt: INTEGER;
		VAR sign: BOOLEAN;
		eflag: BOOLEAN;
		VAR buf: string);
    VAR	r2, i: INTEGER;
	fi, fj: REAL;
	ind1, ind2 : INTEGER;
  BEGIN
	IF ndigits < 0 THEN ndigits := 0 END;
	IF ndigits >= NDIG-1 THEN ndigits := NDIG-2; END;
	r2 := 0;
	sign := arg < 0.0;
	ind1 := 0;
	IF sign THEN arg := -arg END;
	arg := FIF(arg, 1.0, fi);
	(*
	  Do integer part, which is now in "fi". "arg" now contains the
	  fraction part.
	*)
	IF fi # 0.0 THEN
		ind2 := NDIG;
		WHILE fi # 0.0 DO
			DEC(ind2);
			buf[ind2] := CHR(TRUNC((FIF(fi, 0.1, fi) +
						0.03
					       ) * 10.0
					      ) + ORD('0')
					);
			INC(r2);
		END;
		WHILE ind2 < NDIG DO
			buf[ind1] := buf[ind2];
			INC(ind1);
			INC(ind2);
		END;
	ELSIF arg > 0.0 THEN
		WHILE arg*10.0 < 1.0 DO
			arg := arg * 10.0;
			fj := arg;
			DEC(r2);
		END;
	END;
	ind2 := ndigits;
	IF NOT eflag THEN ind2 := ind2 + r2 END;
	decpt := r2;
	IF ind2 < 0 THEN
		buf[0] := 0C;
		RETURN;
	END;
	WHILE (ind1 <= ind2) AND (ind1 < NDIG) DO
		arg := FIF(arg, 10.0, fj);
		buf[ind1] := CHR(TRUNC(fj)+ORD('0'));
		INC(ind1);
	END;
	IF ind2 >= NDIG THEN
		buf[NDIG-1] := 0C;
		RETURN;
	END;
	ind1 := ind2;
	buf[ind2] := CHR(ORD(buf[ind2])+5);
	WHILE buf[ind2] > '9' DO
		buf[ind2] := '0';
		IF ind2 > 0 THEN
			DEC(ind2);
			buf[ind2] := CHR(ORD(buf[ind2])+1);
		ELSE
			buf[ind2] := '1';
			INC(decpt);
			IF NOT eflag THEN
				IF ind1 > 0 THEN buf[ind1] := '0'; END;
				INC(ind1);
			END;
		END;
	END;
	buf[ind1] := 0C;
  END cvt;

  PROCEDURE ecvt(arg: REAL;
		 ndigits: INTEGER;
		 VAR decpt: INTEGER;
		 VAR sign: BOOLEAN;
		 VAR buf: string);
  BEGIN
	cvt(arg, ndigits, decpt, sign, TRUE, buf);
  END ecvt;

  PROCEDURE fcvt(arg: REAL;
		 ndigits: INTEGER;
		 VAR decpt: INTEGER;
		 VAR sign: BOOLEAN;
		 VAR buf: string);
  BEGIN
	cvt(arg, ndigits, decpt, sign, FALSE, buf);
  END fcvt;

  PROCEDURE WriteReal(arg: REAL; ndigits: CARDINAL);
    VAR buf, cvtbuf: string;
	ind1, ind2: INTEGER;
	d,i: INTEGER;
	sign: BOOLEAN;

  BEGIN
	IF ndigits-6 < 2 THEN i := 2 ELSE i := ndigits-6; END;
	ecvt(arg,i,d,sign,cvtbuf);
	IF sign THEN buf[0] := '-' ELSE buf[0] := ' ' END;
	ind1 := 1;
	ind2 := 0;
	IF cvtbuf[ind2] = '0' THEN INC(d); END;
	buf[ind1] := cvtbuf[ind2]; INC(ind1); INC(ind2);
	buf[ind1] := '.'; INC(ind1);
	FOR i := i-1 TO 1 BY -1 DO
		buf[ind1] := cvtbuf[ind2]; INC(ind1); INC(ind2);
	END;
	buf[ind1] := 'E'; INC(ind1);
	DEC(d);
	IF d < 0 THEN
		d := -d;
		buf[ind1] := '-';
	ELSE
		buf[ind1] := '+';
	END;
	INC(ind1);
	buf[ind1] := CHR(ORD('0') + CARDINAL(d DIV 10));
	buf[ind1+1] := CHR(ORD('0') + CARDINAL(d MOD 10));
	buf[ind1+2] := 0C;
	InOut.WriteString(buf);
  END WriteReal;

  PROCEDURE ReadReal(VAR x: REAL);
    CONST	BIG = 1.0E17;
    VAR		r : REAL;
		pow10 : INTEGER;
		i : INTEGER;
		e : REAL;
		ch : CHAR;
		signed: BOOLEAN;
		signedexp: BOOLEAN;
		Buf: ARRAY[0..512] OF CHAR;
		iB: INTEGER;

    PROCEDURE dig(ch: CARDINAL);
    BEGIN
	IF r>BIG THEN INC(pow10) ELSE r:= 10.0*r + FLOAT(ch) END;
    END dig;

    PROCEDURE isdig(ch: CHAR) : BOOLEAN;
    BEGIN
	RETURN (ch >= '0') AND (ch <= '9');
    END isdig;

  BEGIN
	r := 0.0;
	pow10 := 0;
	InOut.ReadString(Buf);
	iB := 0;
	signed := FALSE;
	IF Buf[0] = '-' THEN signed := TRUE; INC(iB)
	ELSIF Buf[0] = '+' THEN INC(iB)
	END;
	ch := Buf[iB]; INC(iB);
	IF NOT isdig(ch) THEN Done := FALSE; RETURN END;
	REPEAT
		dig(ORD(ch));
		ch := Buf[iB]; INC(iB);
	UNTIL NOT isdig(ch);
	IF ch = '.' THEN
		ch := Buf[iB]; INC(iB);
		IF NOT isdig(ch) THEN Done := FALSE; RETURN END;
		REPEAT
			dig(ORD(ch));
			DEC(pow10);
			ch := Buf[iB]; INC(iB);
		UNTIL NOT isdig(ch);
	END;
	IF ch = 'E' THEN
		ch := Buf[iB]; INC(iB);
		i := 0;
		signedexp := FALSE;
		IF ch = '-' THEN signedexp := TRUE; ch:= Buf[iB]; INC(iB)
		ELSIF Buf[iB] = '+' THEN ch := Buf[iB]; INC(iB)
		END;
		IF NOT isdig(ch) THEN Done := FALSE; RETURN END;
		REPEAT
			i := i*10 + INTEGER(ORD(ch) - ORD('0'));
			ch := Buf[iB]; INC(iB);
		UNTIL NOT isdig(ch);
		IF signedexp THEN i := -i END;
		pow10 := pow10 + i;
	END;
	IF pow10 < 0 THEN i := -pow10; ELSE i := pow10; END;
	e := 1.0;
	DEC(i);
	WHILE i >= 0 DO
		e := e * 10.0;
		DEC(i)
	END;
	IF pow10<0 THEN
		r := r / e;
	ELSE
		r := r * e;
	END;
	IF signed THEN x := -r; ELSE x := r END;
  END ReadReal;

  PROCEDURE WriteRealOct(x: REAL);
  BEGIN
  END WriteRealOct;

BEGIN
	Done := FALSE;
END RealInOut.
