MODULE LifeGame;

(* From: MODULA-2, An Introduction, by Daniel Thalmann, Springer-Verlag,
	 New York, 1985
   Figure 10.18
*)

(* John Horton Conway's game "life" *)

  FROM InOut IMPORT	Write, WriteString, WriteLn, WriteCard,
			ReadCard, Done;

  CONST
	MaxInd = 20;
	MaxInd1 = MaxInd+1;

  TYPE
	IndRange = [1..MaxInd];
	IndRange1 = [0..MaxInd1];
	State = [0..1];
	Cells = ARRAY IndRange1, IndRange1 OF State;
	IndStat = [0..17];

  VAR
	Generation, NbOfGen: CARDINAL;
	PreviousNext: BOOLEAN;
	CellsState: ARRAY BOOLEAN OF Cells;
	Status: ARRAY IndStat OF State;

  PROCEDURE InitGame;

    PROCEDURE InitAndReadPos;
      VAR
	Line, Column: CARDINAL;

    BEGIN
	FOR Line := 0 TO MaxInd1 DO
	  FOR Column := 0 TO MaxInd1 DO
		CellsState[FALSE][Line, Column] := 0;
	  END;
	END;
	CellsState[TRUE] := CellsState[FALSE];

	(* Read positions *)
	ReadCard(Line);
	WHILE Done DO
		ReadCard(Column);
		CellsState[FALSE][Line, Column] := 1;
		ReadCard(Line);
	END;

	PreviousNext := FALSE;
	Generation := 0;
    END InitAndReadPos;

    PROCEDURE InitStatus;
    (* Ezra Gottheil method *)
      VAR
	Ind: IndStat;
    BEGIN
	FOR Ind := 0 TO 17 DO
		Status[Ind] := 0;
	END;
	Status[3] := 1;
	Status[11] := 1;
	Status[12] := 1;
    END InitStatus;

  BEGIN	(* InitGame *)
	WriteString("Please, enter the number of generations: ");
	ReadCard(NbOfGen);
	WriteLn;
	WriteString("              line and column positions: ");
	InitAndReadPos;
	InitStatus;
  END InitGame;

  PROCEDURE NextGeneration;
    VAR
	Line, Column: IndRange;
	nbN: CARDINAL;

    PROCEDURE Neighbourhood(L, C: IndRange1; VAR nbn: CARDINAL);
      VAR
	Line1, Column1: IndRange1;
    BEGIN
	nbn := 0;
	FOR Line1 := L - 1 TO L + 1 DO
	  FOR Column1 := C - 1 TO C + 1 DO
		INC(nbn, CellsState[PreviousNext][Line1, Column1]);
	  END;
	END;
	DEC(nbn, CellsState[PreviousNext][L, C]);
    END Neighbourhood;

  BEGIN	(* NextGeneration *)
	FOR Line := 1 TO MaxInd DO
	  FOR Column := 1 TO MaxInd DO
		Neighbourhood(Line, Column, nbN);
		CellsState[NOT PreviousNext][Line, Column] := 
		  Status[CellsState[PreviousNext][Line, Column]*9 + nbN];
	  END;
	END;
	PreviousNext := NOT PreviousNext;
  END NextGeneration;

  PROCEDURE Impression;
  VAR
	N: CARDINAL;
	Line, Column: IndRange;
  BEGIN
	WriteLn ;
	WriteString("           GENERATION : ");
	WriteCard(Generation, 3);
	WriteLn;
	WriteLn;
	WriteString("          ");
	FOR N := 1 TO 2 * MaxInd + 3 DO
		Write("-");
	END;
	WriteLn;
	FOR Line := 1 TO MaxInd DO
		WriteString("          |");
		FOR Column := 1 TO MaxInd DO
		  IF CellsState[PreviousNext][Line, Column] = 1 THEN
			WriteString(" @");
		  ELSE
			WriteString(" .");
		  END;
		END;
		WriteString(" |");
		WriteLn;
	END;
	WriteString("          ");
	FOR N := 1 TO 2*MaxInd + 3 DO
		Write("-");
	END;
	WriteLn;
	WriteLn;
  END Impression;

BEGIN
	InitGame;
	Impression;
	LOOP
		INC(Generation);
		NextGeneration;
		Impression;
		IF Generation = NbOfGen THEN EXIT; END;
	END;
END LifeGame.
