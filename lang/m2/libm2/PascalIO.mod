IMPLEMENTATION MODULE PascalIo;

  IMPORT Unix;
  IMPORT Conversions;
  IMPORT Traps;
  IMPORT RealConversions;
  FROM TTY IMPORT isatty;
  FROM Storage IMPORT ALLOCATE;
  FROM SYSTEM IMPORT ADR;

  TYPE	charset = SET OF CHAR;
	btype = (reading, writing, free);

  CONST	BUFSIZ = 1024;		(* Tunable *)
	spaces = charset{11C, 12C, 13C, 14C, 15C, ' '};

  TYPE	IOBuf = RECORD
			type: btype;
			eof: BOOLEAN;
			next: Text;
			fildes: INTEGER;
			cnt: INTEGER;
			maxcnt: INTEGER;
			bufferedcount: INTEGER;
			buf: ARRAY [1..BUFSIZ] OF CHAR;
		END;
	Text =	POINTER TO IOBuf;
	numbuf = ARRAY[0..255] OF CHAR;

  VAR	ibuf, obuf: IOBuf;
	head: Text;

  PROCEDURE Reset(filename: ARRAY OF CHAR; VAR inputtext: Text);
  BEGIN
	doclose(inputtext);
	getstruct(inputtext);
	WITH inputtext^ DO
		eof := FALSE;
		fildes := Unix.open(ADR(filename), 0);
		IF fildes < 0 THEN
			Traps.Message("could not open input file");
			HALT;
		END;
		type := reading;
		cnt := 1;
		maxcnt := 0;
		bufferedcount := BUFSIZ;
	END;
  END Reset;

  PROCEDURE Rewrite(filename: ARRAY OF CHAR; VAR outputtext: Text);
  BEGIN
	doclose(outputtext);
	getstruct(outputtext);
	WITH outputtext^ DO
		eof := FALSE;
		fildes := Unix.creat(ADR(filename), 666B);
		IF fildes < 0 THEN
			Traps.Message("could not open output file");
			HALT;
		END;
		type := writing;
		cnt := 0;
		maxcnt := 0;
		bufferedcount := BUFSIZ;
	END;
  END Rewrite;

  PROCEDURE PascalIoCleanup();
  VAR text: Text;
  BEGIN
	text := head;
	WHILE text # NIL DO
		doclose(text);
		text := text^.next;
	END;
  END PascalIoCleanup;

  PROCEDURE doclose(text: Text);
  VAR dummy: INTEGER;
  BEGIN
	IF text # notext THEN
		WITH text^ DO
			IF type = writing THEN
				Flush(text);
			END;
			IF type # free THEN
				type := free;
				dummy := Unix.close(fildes);
			END;
		END;
	END;
  END doclose;

  PROCEDURE getstruct(VAR text: Text);
  BEGIN
	text := head;
	WHILE (text # NIL) AND (text^.type # free) DO
		text := text^.next;
	END;
	IF text = NIL THEN
		NEW(text);
		text^.next := head;
		head := text;
	END;
  END getstruct;

  PROCEDURE chk(text: Text; tp: btype);
  BEGIN
	IF text^.type # tp THEN
		IF tp = reading THEN
			Traps.Message("input text expected");
		ELSE
			Traps.Message("output text expected");
		END;
		HALT;
	END;
  END chk;

  PROCEDURE ReadCHAR(inputtext: Text; VAR ch : CHAR);
  BEGIN
	ch := NextCHAR(inputtext);
	Get(inputtext);
  END ReadCHAR;

  PROCEDURE NextCHAR(inputtext: Text): CHAR;
  VAR c: CHAR;
  BEGIN
	chk(inputtext, reading);
	WITH inputtext^ DO
		IF cnt <= maxcnt THEN
			c := buf[cnt];
		ELSE
			c := FillBuf(inputtext);
		END;
	END;
	RETURN c;
  END NextCHAR;

  PROCEDURE Get(inputtext: Text);
  VAR dummy: CHAR;
  BEGIN
	chk(inputtext, reading);
	WITH inputtext^ DO
		IF eof THEN
			Traps.Message("unexpected EOF");
			HALT;
		END;
		IF cnt > maxcnt THEN
			dummy := FillBuf(inputtext);
		END;
		INC(cnt);
	END;
  END Get;

  PROCEDURE FillBuf(ib: Text) : CHAR;
  VAR c : CHAR;
  BEGIN
	WITH ib^ DO
		IF eof THEN RETURN 0C; END;
		maxcnt := Unix.read(fildes, ADR(buf), bufferedcount);
		cnt := 1;
		IF maxcnt <= 0 THEN
			c :=  0C;
			eof := TRUE;
		ELSE
			c :=  buf[1];
		END;
	END;
	RETURN c;
  END FillBuf;

  PROCEDURE Eoln(inputtext: Text): BOOLEAN;
  BEGIN
	RETURN NextCHAR(inputtext) = 12C;
  END Eoln;

  PROCEDURE Eof(inputtext: Text): BOOLEAN;
  BEGIN
	RETURN (NextCHAR(inputtext) = 0C) AND inputtext^.eof;
  END Eof;

  PROCEDURE ReadLn(inputtext: Text);
  VAR ch: CHAR;
  BEGIN
	REPEAT
		ReadCHAR(inputtext, ch)
	UNTIL ch = 12C;
  END ReadLn;

  PROCEDURE Flush(ob: Text);
  VAR dummy: INTEGER;
  BEGIN
	WITH ob^ DO
		dummy := Unix.write(fildes, ADR(buf), cnt);
		cnt := 0;
	END;
  END Flush;

  PROCEDURE WriteCHAR(outputtext: Text; ch: CHAR);
  BEGIN
	chk(outputtext, writing);
	WITH outputtext^ DO
		INC(cnt);
		buf[cnt] := ch;
		IF cnt >= bufferedcount THEN
			Flush(outputtext);
		END;
	END;
  END WriteCHAR;

  PROCEDURE WriteLn(outputtext: Text);
  BEGIN
	WriteCHAR(outputtext, 12C);
  END WriteLn;

  PROCEDURE Page(outputtext: Text);
  BEGIN
	WriteCHAR(outputtext, 14C);
  END Page;

  PROCEDURE ReadINTEGER(inputtext: Text; VAR int : INTEGER);
  CONST
    	SAFELIMITDIV10 = MAX(INTEGER) DIV 10;
    	SAFELIMITREM10 = MAX(INTEGER) MOD 10;
  VAR
    	neg : BOOLEAN;
    	safedigit: CARDINAL;
	ch: CHAR;
    	chvalue: CARDINAL;
  BEGIN
    	WHILE NextCHAR(inputtext) IN spaces DO
		Get(inputtext);
	END;
	ch := NextCHAR(inputtext);
    	IF ch = '-' THEN
		Get(inputtext);
		ch := NextCHAR(inputtext);
		neg := TRUE;
    	ELSIF ch = '+' THEN
		Get(inputtext);
		ch := NextCHAR(inputtext);
		neg := FALSE;
    	ELSE
		neg := FALSE
    	END;

    	safedigit := SAFELIMITREM10;
    	IF neg THEN safedigit := safedigit + 1 END;
    	int := 0;
	IF (ch >= '0') AND (ch <= '9') THEN
		WHILE (ch >= '0') & (ch <= '9') DO
  			chvalue := ORD(ch) - ORD('0');
	   		IF (int < -SAFELIMITDIV10) OR 
		   	   ( (int = -SAFELIMITDIV10) AND
		     	     (chvalue > safedigit)) THEN
				Traps.Message("integer too large");
				HALT;
	    		ELSE
				int := 10*int - VAL(INTEGER, chvalue);
				Get(inputtext);
				ch := NextCHAR(inputtext);
	    		END;
		END;
		IF NOT neg THEN
   			int := -int
		END;
	ELSE
		Traps.Message("integer expected");
		HALT;
	END;
  END ReadINTEGER;

  PROCEDURE ReadCARDINAL(inputtext: Text; VAR card : CARDINAL);
  CONST
    	SAFELIMITDIV10 = MAX(CARDINAL) DIV 10;
    	SAFELIMITREM10 = MAX(CARDINAL) MOD 10;

  VAR
    	ch : CHAR;
    	safedigit: CARDINAL;
    	chvalue: CARDINAL;
  BEGIN
    	WHILE NextCHAR(inputtext) IN spaces DO
		Get(inputtext);
	END;
	ch := NextCHAR(inputtext);
    	safedigit := SAFELIMITREM10;
    	card := 0;
	IF (ch >= '0') AND (ch <= '9') THEN
		WHILE (ch >= '0') & (ch <= '9') DO
  			chvalue := ORD(ch) - ORD('0');
	    		IF (card > SAFELIMITDIV10) OR 
			   ( (card = SAFELIMITDIV10) AND
			     (chvalue > safedigit)) THEN
				Traps.Message("cardinal too large");
				HALT;
		    	ELSE
				card := 10*card + chvalue;
				Get(inputtext);
				ch := NextCHAR(inputtext);
		    	END;
		END;
	ELSE
		Traps.Message("cardinal expected");
		HALT;
	END;
  END ReadCARDINAL;

  PROCEDURE ReadREAL(inputtext: Text; VAR real: REAL);
  VAR
	buf: numbuf;
	ch: CHAR;
	ok: BOOLEAN;
	index: INTEGER;

    PROCEDURE inch(): CHAR;
    BEGIN
	buf[index] := ch;
	INC(index);
	Get(inputtext);
	RETURN NextCHAR(inputtext);
    END inch;

  BEGIN
	index := 0;
	ok := TRUE;
    	WHILE NextCHAR(inputtext) IN spaces DO
		Get(inputtext);
	END;
	ch := NextCHAR(inputtext);
	IF (ch ='+') OR (ch = '-') THEN
		ch := inch();
	END;
	IF (ch >= '0') AND (ch <= '9') THEN
		WHILE (ch >= '0') AND (ch <= '9') DO
			ch := inch();
		END;
		IF (ch = '.') THEN
			ch := inch();
			IF (ch >= '0') AND (ch <= '9') THEN
				WHILE (ch >= '0') AND (ch <= '9') DO
					ch := inch();
				END;
			ELSE
				ok := FALSE;
			END;
		END;
		IF ok AND (ch = 'E') THEN
			ch := inch();
			IF (ch ='+') OR (ch = '-') THEN
				ch := inch();
			END;
			IF (ch >= '0') AND (ch <= '9') THEN
				WHILE (ch >= '0') AND (ch <= '9') DO
					ch := inch();
				END;
			ELSE
				ok := FALSE;
			END;
		END;
	ELSE
		ok := FALSE;
	END;
	IF ok THEN
		buf[index] := 0C;
		RealConversions.StringToReal(buf, real, ok);
	END;
	IF NOT ok THEN
		Traps.Message("Illegal real");
		HALT;
	END;
  END ReadREAL;

  PROCEDURE WriteCARDINAL(outputtext: Text; card: CARDINAL; width: CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
	Conversions.ConvertCardinal(card, 1, buf);
	WriteSTRING(outputtext, buf, width);
  END WriteCARDINAL;

  PROCEDURE WriteINTEGER(outputtext: Text; int: INTEGER; width: CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
    	Conversions.ConvertInteger(int, 1, buf);
	WriteSTRING(outputtext, buf, width);
  END WriteINTEGER;

  PROCEDURE WriteBOOLEAN(outputtext: Text; bool: BOOLEAN; width: CARDINAL);
  BEGIN
	IF bool THEN
		WriteSTRING(outputtext, " TRUE", width);
	ELSE
		WriteSTRING(outputtext, "FALSE", width);
	END;
  END WriteBOOLEAN;

  PROCEDURE WriteREAL(outputtext: Text; real: REAL; width, nfrac: CARDINAL);
  VAR
	buf: numbuf;
	ok: BOOLEAN;
	digits: INTEGER;
  BEGIN
	IF width > SIZE(buf) THEN
		width := SIZE(buf);
	END;
	IF nfrac > 0 THEN
		RealConversions.RealToString(real, width, nfrac, buf, ok);
	ELSE
		IF width < 9 THEN width := 9; END;
		IF real < 0.0 THEN
			digits := 7 - INTEGER(width);
		ELSE
			digits := 6 - INTEGER(width);
		END;
		RealConversions.RealToString(real, width, digits, buf, ok);
	END;
	WriteSTRING(outputtext, buf, 0);
  END WriteREAL;

  PROCEDURE WriteSTRING(outputtext: Text; str: ARRAY OF CHAR; width: CARDINAL);
  VAR index: CARDINAL;
  BEGIN
	index := 0;
	WHILE (index <= HIGH(str)) AND (str[index] # EOS) DO
		INC(index);
	END;
	WHILE index < width DO
		WriteCHAR(outputtext, " ");
		INC(index);
	END;
	index := 0;
	WHILE (index <= HIGH(str)) AND (str[index] # EOS) DO
		WriteCHAR(outputtext, str[index]);
		INC(index);
	END;
  END WriteSTRING;

BEGIN	(* PascalIo initialization *)
	WITH ibuf DO
		eof := FALSE;
		type := reading;
		fildes := 0;
		bufferedcount := BUFSIZ;
		maxcnt := 0;
		cnt := 1;
	END;
	WITH obuf DO
		eof := FALSE;
		type := writing;
		fildes := 1;
		IF isatty(1) THEN
			bufferedcount := 1;
		ELSE
			bufferedcount := BUFSIZ;
		END;
		cnt := 0;
	END;
	notext := NIL;
	input := ADR(ibuf);
	output := ADR(obuf);
	input^.next := output;
	output^.next := NIL;
	head := input;
END PascalIo.
