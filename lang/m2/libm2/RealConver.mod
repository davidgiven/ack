IMPLEMENTATION MODULE RealConversions;

  FROM EM IMPORT FIF, FEF;

  PROCEDURE RealToString(arg: REAL;
		width, digits: INTEGER;
		VAR str: ARRAY OF CHAR;
		VAR ok: BOOLEAN);
  BEGIN
	LongRealToString(LONG(arg), width, digits, str, ok);
  END RealToString;

  PROCEDURE LongRealToString(arg: LONGREAL;
		width, digits: INTEGER;
		VAR str: ARRAY OF CHAR;
		VAR ok: BOOLEAN);
    VAR	pointpos: INTEGER;
	i: CARDINAL;
	ecvtflag: BOOLEAN;
	r, intpart, fractpart: LONGREAL;
	ind1, ind2 : CARDINAL;
	sign: BOOLEAN;
	tmp : CHAR;
	ndigits: CARDINAL;
	dummy, dig: LONGREAL;

  BEGIN
	r := arg;
	DEC(width);
	IF digits < 0 THEN
		ecvtflag := TRUE;
		ndigits := -digits;
	ELSE
		ecvtflag := FALSE;
		ndigits := digits;
	END;
	IF HIGH(str) < ndigits + 3 THEN str[0] := 0C; ok := FALSE; RETURN END;
	pointpos := 0;
	sign := r < 0.0D;
	IF sign THEN r := -r END;
	r := FIF(r, 1.0D, intpart);
	fractpart := r;
	pointpos := 0;
	ind1 := 0;
	ok := TRUE;
	(*
	  Do integer part, which is now in "intpart". "r" now contains the
	  fraction part.
	*)
	IF intpart # 0.0D THEN
		ind2 := 0;
		WHILE intpart # 0.0D DO
			IF ind2 > HIGH(str) THEN
				IF NOT ecvtflag THEN
					str[0] := 0C;
					ok := FALSE;
					RETURN;
				END;
				FOR ind1 := 1 TO HIGH(str) DO
					str[ind1-1] := str[ind1];
				END;
				DEC(ind2);
			END;
			dummy := FIF(FIF(intpart, 0.1D, intpart),10.0D, dig);
			IF (dummy > 0.5D) AND (dig < 9.0D) THEN
				dig := dig + 1.0D;
			END;
			str[ind2] := CHR(TRUNC(dig+0.5D) + ORD('0'));
			INC(ind2);
			INC(pointpos);
		END;
		i := 0;
		ind1 := ind2;
		WHILE ind2 > i DO
			DEC(ind2);
			tmp := str[i];
			str[i] := str[ind2];
			str[ind2] := tmp;
			INC(i);
		END;
	ELSE
		INC(pointpos);
		IF r > 0.0D THEN
			WHILE r < 1.0D DO
				fractpart := r;
				r := r * 10.0D;
				DEC(pointpos);
			END;
		END;
	END;
	ind2 := ndigits;
	IF NOT ecvtflag THEN 
		IF INTEGER(ind2) + pointpos < 0 THEN
			ind2 := ndigits;
			FOR i := 0 TO ndigits DO str[i] := '0'; END;
			ind1 := ndigits+1;
		ELSE
			ind2 := INTEGER(ind2) + pointpos
		END;
	END;
	IF ind2 > HIGH(str) THEN
		ok := FALSE;
		str[0] := 0C;
		RETURN;
	END;
	WHILE ind1 <= ind2 DO
		fractpart := FIF(fractpart, 10.0D, r);
		str[ind1] := CHR(TRUNC(r)+ORD('0'));
		INC(ind1);
	END;
	ind1 := ind2;
	str[ind2] := CHR(ORD(str[ind2])+5);
	WHILE str[ind2] > '9' DO
		str[ind2] := '0';
		IF ind2 > 0 THEN
			DEC(ind2);
			str[ind2] := CHR(ORD(str[ind2])+1);
		ELSE
			str[ind2] := '1';
			INC(pointpos);
			IF NOT ecvtflag THEN
				IF ind1 > 0 THEN str[ind1] := '0'; END;
				INC(ind1);
			END;
		END;
	END;
	str[ind1] := 0C;
	IF ecvtflag THEN
		FOR i := ind1 TO 2 BY -1 DO
			str[i] := str[i-1];
		END;
		str[1] := '.';
		INC(ind1);
		IF sign THEN
			FOR i := ind1 TO 1 BY -1 DO
				str[i] := str[i-1];
			END;
			INC(ind1);
			str[0] := '-';
		END;
		IF (ind1 + 4) > HIGH(str) THEN
			str[0] := 0C;
			ok := FALSE;
			RETURN;
		END;
		str[ind1] := 'E'; INC(ind1);
		DEC(pointpos);
		IF pointpos < 0 THEN
			pointpos := -pointpos;
			str[ind1] := '-';
		ELSE
			str[ind1] := '+';
		END;
		INC(ind1);
		str[ind1] := CHR(ORD('0') + CARDINAL(pointpos DIV 100));
		pointpos := pointpos MOD 100;
		INC(ind1);
		str[ind1] := CHR(ORD('0') + CARDINAL(pointpos DIV 10));
		INC(ind1);
		str[ind1] := CHR(ORD('0') + CARDINAL(pointpos MOD 10));
	ELSE
		IF pointpos <= 0 THEN
			FOR i := ind1 TO 1 BY -1 DO
				str[i+CARDINAL(-pointpos)] := str[i-1];
			END;
			FOR i := 0 TO CARDINAL(-pointpos) DO
				str[i] := '0';
			END;
			ind1 := ind1 + CARDINAL(1 - pointpos);
			pointpos := 1;
		END;
		FOR i := ind1 TO CARDINAL(pointpos+1) BY -1 DO
			str[i] := str[i-1];
		END;
		IF ndigits = 0 THEN
			str[pointpos] := 0C;
		ELSE
			str[pointpos] := '.';
		END;
		IF sign THEN
			FOR i := ind1 TO 0 BY -1 DO
				str[i+1] := str[i];
			END;
			str[0] := '-';
			INC(ind1);
		END;
	END;
	IF ind1 > CARDINAL(width) THEN
		ok := FALSE;
		RETURN;
	END;
	IF ind1 < CARDINAL(width) THEN
		FOR i := ind1 TO 0 BY -1 DO
			str[i + CARDINAL(width) - ind1] := str[i];
		END;
		FOR i := 0 TO CARDINAL(width)-(ind1+1) DO
			str[i] := ' ';
		END;
		ind1 := CARDINAL(width);
	END;
	IF (ind1+1) <= HIGH(str) THEN str[ind1+1] := 0C; END;

  END LongRealToString;

	
  PROCEDURE StringToReal(str: ARRAY OF CHAR;
			 VAR r: REAL; VAR ok: BOOLEAN);
    VAR x: LONGREAL;
  BEGIN
	StringToLongReal(str, x, ok);
	IF ok THEN
		r := x;
	END;
  END StringToReal;

  PROCEDURE StringToLongReal(str: ARRAY OF CHAR;
			 VAR r: LONGREAL; VAR ok: BOOLEAN);
    CONST	BIG = 1.0D17;
    TYPE	SETOFCHAR = SET OF CHAR;
    VAR		pow10 : INTEGER;
		i : INTEGER;
		e : LONGREAL;
		ch : CHAR;
		signed: BOOLEAN;
		signedexp: BOOLEAN;
		iB: CARDINAL;

  BEGIN
	r := 0.0D;
	pow10 := 0;
	iB := 0;
	ok := TRUE;
	signed := FALSE;
	WHILE (str[iB] = ' ') OR (str[iB] = CHR(9)) DO
		INC(iB);
		IF iB > HIGH(str) THEN
			ok := FALSE;
			RETURN;
		END;
	END;
	IF str[iB] = '-' THEN signed := TRUE; INC(iB)
	ELSIF str[iB] = '+' THEN INC(iB)
	END;
	ch := str[iB]; INC(iB);
	IF NOT (ch IN SETOFCHAR{'0'..'9'}) THEN ok := FALSE; RETURN END;
	REPEAT
		IF r>BIG THEN INC(pow10) ELSE r:= 10.0D*r+FLOATD(ORD(ch)-ORD('0')) END;
		IF iB <= HIGH(str) THEN
			ch := str[iB]; INC(iB);
		END;
	UNTIL (iB > HIGH(str)) OR NOT (ch IN SETOFCHAR{'0'..'9'});
	IF (ch = '.') AND (iB <= HIGH(str)) THEN
		ch := str[iB]; INC(iB);
		IF NOT (ch IN SETOFCHAR{'0'..'9'}) THEN ok := FALSE; RETURN END;
		REPEAT
			IF r < BIG THEN
				r := 10.0D * r + FLOATD(ORD(ch)-ORD('0'));
				DEC(pow10);
			END;
			IF iB <= HIGH(str) THEN
				ch := str[iB]; INC(iB);
			END;
		UNTIL (iB > HIGH(str)) OR NOT (ch IN SETOFCHAR{'0'..'9'});
	END;
	IF (ch = 'E') THEN
		IF iB > HIGH(str) THEN
			ok := FALSE;
			RETURN;
		ELSE
			ch := str[iB]; INC(iB);
		END;
		i := 0;
		signedexp := FALSE;
		IF (ch = '-') OR (ch = '+') THEN
			signedexp := ch = '-';
			IF iB > HIGH(str) THEN
				ok := FALSE;
				RETURN;
			ELSE
				ch := str[iB]; INC(iB);
			END;
		END;
		IF NOT (ch IN SETOFCHAR{'0'..'9'}) THEN ok := FALSE; RETURN END;
		REPEAT
			i := i*10 + INTEGER(ORD(ch) - ORD('0'));
			IF iB <= HIGH(str) THEN
				ch := str[iB]; INC(iB);
			END;
		UNTIL (iB > HIGH(str)) OR NOT (ch IN SETOFCHAR{'0'..'9'});
		IF signedexp THEN i := -i END;
		pow10 := pow10 + i;
	END;
	IF pow10 < 0 THEN i := -pow10; ELSE i := pow10; END;
	e := 1.0D;
	DEC(i);
	WHILE i >= 10 DO
		e := e * 10000000000.0D;
		DEC(i,10);
	END;
	WHILE i >= 0 DO
		e := e * 10.0D;
		DEC(i)
	END;
	IF pow10<0 THEN
		r := r / e;
	ELSE
		r := r * e;
	END;
	IF signed THEN r := -r; END;
	IF (iB <= HIGH(str)) AND (ORD(ch) > ORD(' ')) THEN ok := FALSE; END
  END StringToLongReal;

END RealConversions.
