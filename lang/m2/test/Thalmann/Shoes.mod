MODULE Shoes;

(* From: MODULA-2, An Introduction, by Daniel Thalmann, Springer-Verlag,
	 New York, 1985
   Figure 21.3
*)

  FROM SYSTEM IMPORT WORD, ADR, ADDRESS, NEWPROCESS, TRANSFER;
  FROM InOut IMPORT Write, WriteLn;

  CONST
	WorkLength = 200;
	MaxShoes = 50;
	MaxDif = 6;

  TYPE
	WorkSpace = ARRAY [0..WorkLength-1] OF WORD;

  VAR
	NbLeft, NbRight : INTEGER;
	WSLeft, WSRight : WorkSpace;
	Left, Right, Main : ADDRESS;

  PROCEDURE Leftp;
  BEGIN
	WHILE NbLeft < MaxShoes DO
		INC(NbLeft);
		Write("L");
		IF (NbLeft-NbRight>=MaxDif) OR (NbLeft>=MaxShoes) THEN
			TRANSFER(Left,Right);
		END;
	END;
	WriteLn;
  END Leftp;

  PROCEDURE Rightp;
  BEGIN
	WHILE NbRight < MaxShoes DO
		INC(NbRight);
		Write("R");
		IF (NbRight-NbLeft>=MaxDif) OR (NbRight>=MaxShoes) THEN
			TRANSFER(Right,Left);
		END;
	END;
	WriteLn;
  END Rightp;

BEGIN
	NbLeft := 0;
	NbRight := 0;
	NEWPROCESS(Leftp,ADR(WSLeft),SIZE(WSLeft),Left);
	NEWPROCESS(Rightp,ADR(WSRight),SIZE(WSRight),Right);
	TRANSFER(Main,Left);
END Shoes.
