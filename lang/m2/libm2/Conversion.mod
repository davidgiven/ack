IMPLEMENTATION MODULE Conversions;

  PROCEDURE ConvertNum(num, len, base: CARDINAL;
		       neg: BOOLEAN;
		       VAR str: ARRAY OF CHAR);
    VAR i: CARDINAL;
	r: CARDINAL;
	tmp: ARRAY [0..20] OF CHAR;
    BEGIN
	i := 0;
	REPEAT
		r := num MOD base;
		num := num DIV base;
		IF r <= 9 THEN
			tmp[i] := CHR(r + ORD('0'));
		ELSE
			tmp[i] := CHR(r - 10 + ORD('A'));
		END;
		INC(i);
	UNTIL num = 0;
	IF neg THEN
		tmp[i] := '-';
		INC(i)
	END;
	IF len > HIGH(str) + 1 THEN len := HIGH(str) + 1; END;
	IF i > HIGH(str) + 1 THEN i := HIGH(str) + 1; END;
	r := 0;
	WHILE len > i DO str[r] := ' '; INC(r); DEC(len); END;
	WHILE i > 0 DO str[r] := tmp[i-1]; DEC(i); INC(r); END;
	IF r <= HIGH(str) THEN str[r] := 0C; END;
    END ConvertNum;

  PROCEDURE ConvertOctal(num, len: CARDINAL; VAR str: ARRAY OF CHAR);
  BEGIN   
	ConvertNum(num, len, 8, FALSE, str);
  END ConvertOctal;   

  PROCEDURE ConvertHex(num, len: CARDINAL; VAR str: ARRAY OF CHAR);
  BEGIN   
	ConvertNum(num, len, 16, FALSE, str);
  END ConvertHex;   

  PROCEDURE ConvertCardinal(num, len: CARDINAL; VAR str: ARRAY OF CHAR);   
  BEGIN   
	ConvertNum(num, len, 10, FALSE, str);
  END ConvertCardinal;   

  PROCEDURE ConvertInteger(num: INTEGER;
			   len: CARDINAL;   
                           VAR str: ARRAY OF CHAR); 
  BEGIN 
	IF num < 0 THEN
		ConvertNum(-num, len, 10, TRUE, str);
	ELSE
		ConvertNum(num, len, 10, FALSE, str);
	END;
  END ConvertInteger; 

END Conversions.
