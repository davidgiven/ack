MODULE queen;
FROM InOut IMPORT WriteString, WriteLn;
	TYPE row = ARRAY[1..8] OF INTEGER;
	VAR  maxpos: INTEGER;
	     d: row;
	PROCEDURE free(i,j: INTEGER): BOOLEAN;
		VAR k: INTEGER;
	BEGIN
		FOR k := 1 TO i-1 DO
			IF (d[k]=j) OR (j-d[k]=i-k) OR (d[k]-j=i-k) THEN
				RETURN FALSE;
			END;
		END;
		RETURN TRUE;
	END free;

	PROCEDURE print;
		VAR i,j: INTEGER;
	BEGIN
		FOR j := maxpos TO 1 BY -1 DO
			FOR i := 1 TO maxpos DO
				IF d[i] = j THEN
					WriteString("D ");
				ELSE
					WriteString(". ");
				END;
			END;
			WriteLn;
		END;
		WriteLn;
	END print;

	PROCEDURE queen(k: INTEGER);
		VAR i: INTEGER;
	BEGIN
		IF k = maxpos THEN
			FOR i := 1 TO maxpos DO
				IF free(k,i) THEN
					d[k] := i;
					print();
				END;
			END;
		ELSE
			FOR i := 1 TO maxpos DO
				IF free(k,i) THEN
					d[k] := i;
					queen(k+1);
				END;
			END;
		END;
	END queen;
BEGIN
	maxpos := 8;
	queen(1);
END queen.
