MODULE Characters;

(* From: MODULA-2, An Introduction, by Daniel Thalmann, Springer-Verlag,
	 New York, 1985
   Figure 8.8
   Changed a little, to have an ELSE part in the CASE statement
*)

	FROM InOut IMPORT WriteLn, WriteString, Write;

	CONST
		StrByLine = 4;

	VAR
		c : CHAR;
	
	BEGIN
		FOR c := 0C TO 177C DO
			IF ORD(c) MOD StrByLine = 0 THEN WriteLn; END;
			CASE c OF
			0C..37C, 177C :	WriteString("Control character   ") |
			"0".."9":	WriteString("Digit               ") |
			"a".."z":	WriteString("Lower case letter   ") |
			"A"..'Z':	WriteString("Upper case LETTER   ")
			ELSE		WriteString("Special character   ")
			END;
		END;
	END Characters.

