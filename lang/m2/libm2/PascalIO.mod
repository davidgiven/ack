IMPLEMENTATION MODULE PascalIO;

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

  PROCEDURE Reset(Filename: ARRAY OF CHAR; VAR InputText: Text);
  BEGIN
	doclose(InputText);
	getstruct(InputText);
	WITH InputText^ DO
		eof := FALSE;
		fildes := Unix.open(ADR(Filename), 0);
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

  PROCEDURE Rewrite(Filename: ARRAY OF CHAR; VAR OutputText: Text);
  BEGIN
	doclose(OutputText);
	getstruct(OutputText);
	WITH OutputText^ DO
		eof := FALSE;
		fildes := Unix.creat(ADR(Filename), 666B);
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

  PROCEDURE CloseOutput();
  VAR text: Text;
  BEGIN
	text := head;
	WHILE text # NIL DO
		doclose(text);
		text := text^.next;
	END;
  END CloseOutput;

  PROCEDURE doclose(text: Text);
  VAR dummy: INTEGER;
  BEGIN
	IF text # Notext THEN
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

  PROCEDURE ReadChar(InputText: Text; VAR ch : CHAR);
  BEGIN
	ch := NextChar(InputText);
	Get(InputText);
  END ReadChar;

  PROCEDURE NextChar(InputText: Text): CHAR;
  VAR c: CHAR;
  BEGIN
	chk(InputText, reading);
	WITH InputText^ DO
		IF cnt <= maxcnt THEN
			c := buf[cnt];
		ELSE
			c := FillBuf(InputText);
		END;
	END;
	RETURN c;
  END NextChar;

  PROCEDURE Get(InputText: Text);
  VAR dummy: CHAR;
  BEGIN
	chk(InputText, reading);
	WITH InputText^ DO
		IF eof THEN
			Traps.Message("unexpected EOF");
			HALT;
		END;
		IF cnt > maxcnt THEN
			dummy := FillBuf(InputText);
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

  PROCEDURE Eoln(InputText: Text): BOOLEAN;
  BEGIN
	RETURN NextChar(InputText) = 12C;
  END Eoln;

  PROCEDURE Eof(InputText: Text): BOOLEAN;
  BEGIN
	RETURN (NextChar(InputText) = 0C) AND InputText^.eof;
  END Eof;

  PROCEDURE ReadLn(InputText: Text);
  VAR ch: CHAR;
  BEGIN
	REPEAT
		ReadChar(InputText, ch)
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

  PROCEDURE WriteChar(OutputText: Text; ch: CHAR);
  BEGIN
	chk(OutputText, writing);
	WITH OutputText^ DO
		INC(cnt);
		buf[cnt] := ch;
		IF cnt >= bufferedcount THEN
			Flush(OutputText);
		END;
	END;
  END WriteChar;

  PROCEDURE WriteLn(OutputText: Text);
  BEGIN
	WriteChar(OutputText, 12C);
  END WriteLn;

  PROCEDURE Page(OutputText: Text);
  BEGIN
	WriteChar(OutputText, 14C);
  END Page;

  PROCEDURE ReadInteger(InputText: Text; VAR int : INTEGER);
  CONST
    	SAFELIMITDIV10 = MAX(INTEGER) DIV 10;
    	SAFELIMITREM10 = MAX(INTEGER) MOD 10;
  VAR
    	neg : BOOLEAN;
    	safedigit: CARDINAL;
	ch: CHAR;
    	chvalue: CARDINAL;
  BEGIN
    	WHILE NextChar(InputText) IN spaces DO
		Get(InputText);
	END;
	ch := NextChar(InputText);
    	IF ch = '-' THEN
		Get(InputText);
		ch := NextChar(InputText);
		neg := TRUE;
    	ELSIF ch = '+' THEN
		Get(InputText);
		ch := NextChar(InputText);
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
				Get(InputText);
				ch := NextChar(InputText);
	    		END;
		END;
		IF NOT neg THEN
   			int := -int
		END;
	ELSE
		Traps.Message("integer expected");
		HALT;
	END;
  END ReadInteger;

  PROCEDURE ReadCardinal(InputText: Text; VAR card : CARDINAL);
  CONST
    	SAFELIMITDIV10 = MAX(CARDINAL) DIV 10;
    	SAFELIMITREM10 = MAX(CARDINAL) MOD 10;

  VAR
    	ch : CHAR;
    	safedigit: CARDINAL;
    	chvalue: CARDINAL;
  BEGIN
    	WHILE NextChar(InputText) IN spaces DO
		Get(InputText);
	END;
	ch := NextChar(InputText);
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
				Get(InputText);
				ch := NextChar(InputText);
		    	END;
		END;
	ELSE
		Traps.Message("cardinal expected");
		HALT;
	END;
  END ReadCardinal;

  PROCEDURE ReadReal(InputText: Text; VAR real: REAL);
  VAR
	buf: numbuf;
	ch: CHAR;
	ok: BOOLEAN;
	index: INTEGER;

    PROCEDURE inch(): CHAR;
    BEGIN
	buf[index] := ch;
	INC(index);
	Get(InputText);
	RETURN NextChar(InputText);
    END inch;

  BEGIN
	index := 0;
	ok := TRUE;
    	WHILE NextChar(InputText) IN spaces DO
		Get(InputText);
	END;
	ch := NextChar(InputText);
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
  END ReadReal;

  PROCEDURE WriteCardinal(OutputText: Text; card: CARDINAL; width: CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
	Conversions.ConvertCardinal(card, 1, buf);
	WriteString(OutputText, buf, width);
  END WriteCardinal;

  PROCEDURE WriteInteger(OutputText: Text; int: INTEGER; width: CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
    	Conversions.ConvertInteger(int, 1, buf);
	WriteString(OutputText, buf, width);
  END WriteInteger;

  PROCEDURE WriteBoolean(OutputText: Text; bool: BOOLEAN; width: CARDINAL);
  BEGIN
	IF bool THEN
		WriteString(OutputText, " TRUE", width);
	ELSE
		WriteString(OutputText, "FALSE", width);
	END;
  END WriteBoolean;

  PROCEDURE WriteReal(OutputText: Text; real: REAL; width, nfrac: CARDINAL);
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
	WriteString(OutputText, buf, 0);
  END WriteReal;

  PROCEDURE WriteString(OutputText: Text; str: ARRAY OF CHAR; width: CARDINAL);
  VAR index: CARDINAL;
  BEGIN
	index := 0;
	WHILE (index <= HIGH(str)) AND (str[index] # Eos) DO
		INC(index);
	END;
	WHILE index < width DO
		WriteChar(OutputText, " ");
		INC(index);
	END;
	index := 0;
	WHILE (index <= HIGH(str)) AND (str[index] # Eos) DO
		WriteChar(OutputText, str[index]);
		INC(index);
	END;
  END WriteString;

BEGIN	(* PascalIO initialization *)
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
	Notext := NIL;
	Input := ADR(ibuf);
	Output := ADR(obuf);
	Input^.next := Output;
	Output^.next := NIL;
	head := Input;
END PascalIO.
