MODULE PrEnv;
FROM InOut IMPORT WriteString, WriteLn, ReadString, Done;
FROM Arguments IMPORT GetEnv, Argv, Argc;
VAR  Buf: ARRAY[1..256] OF CHAR;
     i: INTEGER;
BEGIN
	FOR i := 0 TO INTEGER(Argc) - 1 DO
		IF Argv(i, Buf) > SIZE(Buf) THEN
			WriteString("Argument too long");
			WriteLn;
			HALT;
		END;
		WriteString(Buf);
		WriteString(" ");
	END;
	WriteLn;
	LOOP
		WriteString("Environment name: ");
		ReadString(Buf);
		IF NOT Done THEN EXIT; END;
		IF GetEnv(Buf, Buf) = 0 THEN
			WriteString("No environment variable");
		ELSE
			WriteString(Buf);
		END;
		WriteLn;
	END;
	WriteLn;
END PrEnv.
