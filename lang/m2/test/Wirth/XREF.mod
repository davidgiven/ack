MODULE XREF;
  FROM InOut	IMPORT	Done, EOL, OpenInput, OpenOutput, Read, Write,
			WriteCard, WriteString, CloseInput, CloseOutput;
  
  FROM TableHandler IMPORT
			WordLength, Table, overflow, InitTable, Record,
			Tabulate;

  TYPE	Alfa = ARRAY [0..9] OF CHAR;

  CONST	N = 45;		(* number of keywords *)

  VAR	ch: CHAR;
	i,k,l,m,r,lno: CARDINAL;
	T: Table;
	id: ARRAY [0..WordLength-1] OF CHAR;
	key: ARRAY [1..N] OF Alfa;

  PROCEDURE copy;
  BEGIN
	Write(ch);
	Read(ch)
  END copy;

  PROCEDURE heading;
  BEGIN
	lno := lno + 1;
	WriteCard(lno, 5);
	WriteString("   ")
  END heading;

BEGIN
	InitTable(T);
	key[1] := "AND ";
	key[2] := "ARRAY ";
	key[3] := "BEGIN ";
	key[4] := "BITSET ";
	key[5] := "BOOLEAN ";
	key[6] := "BY ";
	key[7] := "CASE ";
	key[8] := "CARDINAL ";
	key[9] := "CHAR ";
	key[10] := "CONST ";
	key[11] := "DIV ";
	key[12] := "DO ";
	key[13] := "ELSE ";
	key[14] := "ELSIF ";
	key[15] := "END ";
	key[16] := "EXIT ";
	key[17] := "EXPORT ";
	key[18] := "FALSE ";
	key[19] := "FOR ";
	key[20] := "FROM ";
	key[21] := "IF ";
	key[22] := "IMPORT ";
	key[23] := "IN ";
	key[24] := "INTEGER ";
	key[25] := "LOOP ";
	key[26] := "MOD ";
	key[27] := "MODULE ";
	key[28] := "NOT ";
	key[29] := "OF ";
	key[30] := "OR ";
	key[31] := "POINTER ";
	key[32] := "PROCEDURE ";
	key[33] := "QUALIFIED ";
	key[34] := "RECORD ";
	key[35] := "REPEAT ";
	key[36] := "RETURN ";
	key[37] := "SET ";
	key[38] := "THEN ";
	key[39] := "TO ";
	key[40] := "TRUE ";
	key[41] := "TYPE ";
	key[42] := "UNTIL ";
	key[43] := "VAR ";
	key[44] := "WHILE ";
	key[45] := "WITH ";

	OpenInput("mod");
	IF NOT Done THEN HALT END;
	OpenOutput("xref");
	lno := 0;
	Read(ch);
	IF Done THEN
	    heading;
	    REPEAT
		IF (CAP(ch) >= "A") & (CAP(ch) <= "Z") THEN
		    k := 0;
		    REPEAT
			id[k] := ch;
			k := k + 1;
			copy
		    UNTIL (ch < "0") OR
			  (ch > "9") & (CAP(ch) < "A") OR
			  (CAP(ch) > "Z");
		    l := 1;
		    r := N;
		    id[k] := " ";
		    REPEAT	(* binary search *)
			m := (l + r) DIV 2;
			i := 0;
			WHILE (id[i] = key[m,i]) & (id[i] > " ") DO
				i := i+1;
			END;
			IF id[i] <= key[m,i] THEN r := m-1 END;
			IF id[i] >= key[m,i] THEN l := m+1 END;
		    UNTIL l > r;
		    IF l = r+1 THEN Record(T, id, lno) END
		ELSIF (ch >= "0") & (ch <= "9") THEN
		    REPEAT
			copy
		    UNTIL ((ch<"0") OR (ch>"9"))&((ch<"A") OR (ch>"Z"))
		ELSIF ch = "(" THEN
		    copy;
		    IF ch = "*" THEN	(* comment *)
			REPEAT
			    REPEAT
				IF ch = EOL THEN
				    copy;
				    heading
				ELSE
				    copy
				END
			    UNTIL ch = "*";
			    copy
			UNTIL ch = ")";
			copy
		    END
		ELSIF ch = "'" THEN
		    REPEAT copy UNTIL ch = "'";
		    copy
		ELSIF ch = '"' THEN
		    REPEAT copy UNTIL ch = '"';
		    copy
		ELSIF ch = EOL THEN
		    copy;
		    IF Done THEN heading END
		ELSE
		    copy
		END
	    UNTIL NOT Done OR (overflow # 0)
	END;
	IF overflow > 0 THEN
	    WriteString("Table overflow");
	    WriteCard(overflow, 6);
	    Write(EOL)
	END;
	Write(14C);
	Tabulate(T);
	CloseInput;
	CloseOutput
END XREF.
