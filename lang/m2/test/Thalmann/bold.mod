MODULE BoldFormatter;

(* From: MODULA-2, An Introduction, by Daniel Thalmann, Springer-Verlag,
	 New York, 1985
   Figure 18.2
*)

  FROM InOut IMPORT Done, EOL, Read, Write, OpenInput, OpenOutput, CloseInput, CloseOutput;

  CONST
    N = 40;
    WordLength = 32;

  TYPE
    alpha = ARRAY [0..14] OF CHAR;

  VAR
    ch : CHAR;
    i, j, k, l, m, r : CARDINAL;
    id : ARRAY [0..WordLength] OF CHAR;
    key : ARRAY [1..N] OF alpha;

  PROCEDURE copy;
  BEGIN
    Write(ch); Read(ch);
  END copy;

  PROCEDURE InitTable;
  BEGIN
    key[ 1] := "AND            ";
    key[ 2] := "ARRAY          ";
    key[ 3] := "BEGIN          ";
    key[ 4] := "BY             ";
    key[ 5] := "CASE           ";
    key[ 6] := "CONST          ";
    key[ 7] := "DEFINITION     ";
    key[ 8] := "DIV            ";
    key[ 9] := "DO             ";
    key[10] := "ELSE           ";
    key[11] := "ELSIF          ";
    key[12] := "END            ";
    key[13] := "EXIT           ";
    key[14] := "EXPORT         ";
    key[15] := "FOR            ";
    key[16] := "FROM           ";
    key[17] := "IF             ";
    key[18] := "IMPLEMENTATION ";
    key[19] := "IMPORT         ";
    key[20] := "IN             ";
    key[21] := "LOOP           ";
    key[22] := "MOD            ";
    key[23] := "MODULE         ";
    key[24] := "NOT            ";
    key[25] := "OF             ";
    key[26] := "OR             ";
    key[27] := "POINTER        ";
    key[28] := "PROCEDURE      ";
    key[29] := "QUALIFIED      ";
    key[30] := "RECORD         ";
    key[31] := "REPEAT         ";
    key[32] := "RETURN         ";
    key[33] := "SET            ";
    key[34] := "THEN           ";
    key[35] := "TO             ";
    key[36] := "TYPE           ";
    key[37] := "UNTIL          ";
    key[38] := "VAR            ";
    key[39] := "WHILE          ";
    key[40] := "WITH           ";
  END InitTable;

  PROCEDURE Identifier() : BOOLEAN;

  BEGIN
    l := 1; r := N; id[k] := " ";
    REPEAT
      m := (l + r) DIV 2;
      i := 0;
      WHILE (id[i]=key[m,i]) AND (id[i]#" ") DO i := i+1; END;

      IF id[i] <= key[m,i] THEN r := m-1; END;
      IF id[i] >= key[m,i] THEN l := m+1; END;
    UNTIL l > r;
    RETURN l = r+1;
  END Identifier;

BEGIN
    InitTable;
    OpenInput("mod");
    OpenOutput("text");
    IF NOT Done THEN HALT; END;
    Read(ch);
    REPEAT
      IF (CAP(ch) >= "A") AND (CAP(ch) <= "Z") THEN
	k := 0;
	REPEAT
	  id[k] := ch; k := k+1;
	  Read(ch);
	UNTIL (ch<"0") OR (ch>"9") AND (CAP(ch)<"A") OR (CAP(ch)>"Z");
	IF Identifier() THEN
	  FOR i:= 0 TO k-1 DO
	    Write(id[i]);
	  END;
	ELSE
	  FOR i := 0 TO k-1 DO
	    Write(id[i]); Write(10C); Write(id[i]); Write(10C); Write(id[i]);
	  END;
	END
      ELSIF (ch >= "0") AND (ch <= "9") THEN
	REPEAT copy;
	UNTIL ((ch<"0") OR (ch>"9")) AND ((ch < "A") OR (ch > "Z"))
      ELSIF ch="(" THEN
	copy;
	IF ch = "*" THEN
	  REPEAT
	    REPEAT
	      copy;
	    UNTIL ch = "*";
	    copy;
	  UNTIL ch = ")";
	END
      ELSIF ch = "'" THEN
	REPEAT copy; UNTIL ch = "'";
	copy;
      ELSIF ch='"' THEN
	REPEAT copy; UNTIL ch = '"';
	copy
      ELSE copy;
      END;
    UNTIL NOT Done;
    CloseInput;
    CloseOutput;
END BoldFormatter.
