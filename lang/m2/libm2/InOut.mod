IMPLEMENTATION MODULE InOut ;

  IMPORT Unix;
  IMPORT Conversions;
  FROM TTY IMPORT isatty;
  FROM SYSTEM IMPORT ADR;

  CONST	BUFSIZ = 1024;		(* Tunable *)
	TAB = 11C;

  TYPE	IOBuf = RECORD
			fildes: INTEGER;
			cnt: INTEGER;
			maxcnt: INTEGER;
			bufferedcount: INTEGER;
			buf: ARRAY [1..BUFSIZ] OF CHAR;
		END;
	numbuf = ARRAY[0..255] OF CHAR;

  VAR	ibuf, obuf: IOBuf;
	unread: BOOLEAN;
	unreadch: CHAR;

  PROCEDURE Read(VAR c : CHAR);
  BEGIN
	IF unread THEN
		unread := FALSE;
		c := unreadch;
		Done := TRUE;
	ELSE
		WITH ibuf DO
			IF cnt <= maxcnt THEN
				c := buf[cnt];
				INC(cnt);
				Done := TRUE;
			ELSE
				c := FillBuf(ibuf);
			END;
		END;
	END;
  END Read;

  PROCEDURE UnRead(ch: CHAR);
  BEGIN
	unread := TRUE;
	unreadch := ch;
  END UnRead;

  PROCEDURE FillBuf(VAR ib: IOBuf) : CHAR;
  VAR c : CHAR;
  BEGIN
	WITH ib DO
		maxcnt := Unix.read(fildes, ADR(buf), bufferedcount);
		cnt := 2;
		Done := maxcnt > 0;
		IF NOT Done THEN
			c :=  0C;
		ELSE
			c :=  buf[1];
		END;
	END;
	RETURN c;
  END FillBuf;

  PROCEDURE Flush(VAR ob: IOBuf);
  VAR dummy: INTEGER;
  BEGIN
	WITH ob DO
		dummy := Unix.write(fildes, ADR(buf), cnt);
		cnt := 0;
	END;
  END Flush;

  PROCEDURE Write(c: CHAR);
  BEGIN
	WITH obuf DO
		INC(cnt);
		buf[cnt] := c;
		IF cnt >= bufferedcount THEN
			Flush(obuf);
		END;
	END;
  END Write;

  PROCEDURE OpenInput(defext: ARRAY OF CHAR);
  VAR namebuf : ARRAY [1..256] OF CHAR;
  BEGIN
	IF ibuf.fildes # 0 THEN
		CloseInput;
	END;
	MakeFileName("Name of input file: ", defext, namebuf);
	IF (namebuf[1] = '-') AND (namebuf[2] = 0C) THEN
	ELSE
		WITH ibuf DO
			fildes := Unix.open(ADR(namebuf), 0);
			Done := fildes >= 0;
			maxcnt := 0;
			cnt := 1;
		END;
	END;
  END OpenInput;

  PROCEDURE OpenInputFile(filename: ARRAY OF CHAR);
  BEGIN
	IF ibuf.fildes # 0 THEN
		CloseInput;
	END;
	IF (filename[0] = '-') AND (filename[1] = 0C) THEN
	ELSE
		WITH ibuf DO
			fildes := Unix.open(ADR(filename), 0);
			Done := fildes >= 0;
			maxcnt := 0;
			cnt := 1;
		END;
	END;
  END OpenInputFile;

  PROCEDURE CloseInput;
  BEGIN
	WITH ibuf DO
		IF (fildes > 0) AND (Unix.close(fildes) < 0) THEN
			;
		END;
		fildes := 0;
		maxcnt := 0;
		cnt := 1;
	END;
  END CloseInput;

  PROCEDURE OpenOutput(defext: ARRAY OF CHAR);
  VAR namebuf : ARRAY [1..256] OF CHAR;
  BEGIN
	IF obuf.fildes # 1 THEN
		CloseOutput;
	END;
	MakeFileName("Name of output file: ", defext, namebuf);
	IF (namebuf[1] = '-') AND (namebuf[2] = 0C) THEN
	ELSE	
		WITH obuf DO
			fildes := Unix.creat(ADR(namebuf), 666B);
			Done := fildes >= 0;
			bufferedcount := BUFSIZ;
			cnt := 0;
		END;
	END;
  END OpenOutput;

  PROCEDURE OpenOutputFile(filename: ARRAY OF CHAR);
  BEGIN
	IF obuf.fildes # 1 THEN
		CloseOutput;
	END;
	IF (filename[0] = '-') AND (filename[1] = 0C) THEN
	ELSE
		WITH obuf DO
			fildes := Unix.creat(ADR(filename), 666B);
			Done := fildes >= 0;
			bufferedcount := BUFSIZ;
			cnt := 0;
		END;
	END;
  END OpenOutputFile;

  PROCEDURE CloseOutput;
  BEGIN
	Flush(obuf);
	WITH obuf DO
		IF (fildes # 1) AND (Unix.close(fildes) < 0) THEN
			;
		END;
		fildes := 1;
		bufferedcount := 1;
		cnt := 0;
	END;
  END CloseOutput;

  PROCEDURE MakeFileName(prompt, defext : ARRAY OF CHAR;
		       VAR buf : ARRAY OF CHAR);
  VAR	i, k : INTEGER;
	j : CARDINAL;
	ch: CHAR;
  BEGIN
	FOR k := 1 TO 3 DO
		IF isatty(0) THEN
			XWriteString(prompt);
		END;
		XReadString(buf);
		i := 0;
		WHILE buf[i] # 0C DO i := i + 1 END;
		IF i # 0 THEN
			i := i - 1;
			IF buf[i] = '.' THEN
	    			FOR j := 0 TO HIGH(defext) DO
					i := i + 1;
					buf[i] := defext[j];
	    			END;
	    			buf[i+1] := 0C;
			END;
			RETURN;
		END;
	END;
	Error("no proper file name in three attempts. Giving up.");
  END MakeFileName;

  PROCEDURE Error(s: ARRAY OF CHAR);
  VAR Xch: ARRAY[1..1] OF CHAR;
  BEGIN
	XWriteString("Error: ");
	XWriteString(s);
	Xch[1] := 12C;
	XWriteString(Xch);
	Unix.exit(1);
  END Error;

  PROCEDURE ReadInt(VAR integ : INTEGER);
  CONST
    	SAFELIMITDIV10 = MAX(INTEGER) DIV 10;
    	SAFELIMITREM10 = MAX(INTEGER) MOD 10;
  VAR
    	int : INTEGER;
    	ch  : CHAR;
    	neg : BOOLEAN;
    	safedigit: [0 .. 9];
    	chvalue: CARDINAL;
  BEGIN
    	Read(ch);
    	WHILE (ch = ' ') OR (ch = TAB) OR (ch = 12C) DO
		Read(ch)
    	END;
    	IF ch = '-' THEN
		neg := TRUE;
		Read(ch)
    	ELSIF ch = '+' THEN
		neg := FALSE;
		Read(ch)
    	ELSE
		neg := FALSE
    	END;

    	safedigit := SAFELIMITREM10;
    	IF neg THEN safedigit := safedigit + 1 END;
    	int := 0;
    	IF (ch >= '0') & (ch <= '9') THEN
		WHILE (ch >= '0') & (ch <= '9') DO
  	    		chvalue := ORD(ch) - ORD('0');
	    		IF (int > SAFELIMITDIV10) OR 
			   ( (int = SAFELIMITDIV10) AND
			     (chvalue > safedigit)) THEN
				Error("integer overflow");
	    		ELSE
				int := 10*int + VAL(INTEGER, chvalue);
				Read(ch)
	    		END;
		END;
		IF neg THEN
   	    		integ := -int
		ELSE
	    		integ := int
		END;
		Done := TRUE;
    	ELSE
		Done := FALSE
    	END;
    	UnRead(ch)
  END ReadInt;

  PROCEDURE ReadCard(VAR card : CARDINAL);
  CONST
    	SAFELIMITDIV10 = MAX(CARDINAL) DIV 10;
    	SAFELIMITREM10 = MAX(CARDINAL) MOD 10;
    
  VAR
    	int : CARDINAL;
    	ch  : CHAR;
    	safedigit: [0 .. 9];
    	chvalue: CARDINAL;
  BEGIN
    	Read(ch);
    	WHILE (ch = ' ') OR (ch = TAB) OR (ch = 12C) DO
		Read(ch)
    	END;

    	safedigit := SAFELIMITREM10;
    	int := 0;
    	IF (ch >= '0') & (ch <= '9') THEN
		WHILE (ch >= '0') & (ch <= '9') DO
  	    		chvalue := ORD(ch) - ORD('0');
	    		IF (int > SAFELIMITDIV10) OR 
			   ( (int = SAFELIMITDIV10) AND
			     (chvalue > safedigit)) THEN
				Error("cardinal overflow");
	    		ELSE
				int := 10*int + chvalue;
				Read(ch)
	    		END;
		END;
		card := int;
		Done := TRUE;
    	ELSE
		Done := FALSE
    	END;
    	UnRead(ch)
  END ReadCard;

  PROCEDURE ReadString(VAR s : ARRAY OF CHAR);
  VAR	i : CARDINAL;
    	ch : CHAR;

  BEGIN
    	i := 0;
	REPEAT
		Read(ch);
	UNTIL (ch # ' ') AND (ch # TAB);
	UnRead(ch);
    	LOOP
		Read(ch);
		termCH := ch;
		IF (NOT Done) OR (ch <= " ") THEN s[i] := 0C; RETURN END;
		s[i] := ch;
		INC(i);
		IF i > HIGH(s) THEN DEC(i); END;
    	END;
  END ReadString;

  PROCEDURE XReadString(VAR s : ARRAY OF CHAR);
  VAR	i : INTEGER;
  	j : CARDINAL;
    	ch : CHAR;

  BEGIN
	j := 0;
	LOOP
		i := Unix.read(0, ADR(ch), 1);
		IF i < 0 THEN
			Error("failed read");
		END;
		IF ch <= " " THEN
			s[j] := 0C;
			EXIT;
		END;
		IF j < HIGH(s) THEN
			s[j] := ch;
			INC(j);
		END;
	END;
  END XReadString;

  PROCEDURE XWriteString(s: ARRAY OF CHAR);
  VAR i: CARDINAL;
  BEGIN
	i := 0;
	LOOP
		IF (i <= HIGH(s)) AND (s[i] # 0C) THEN
			INC(i);
		ELSE
			EXIT;
		END;
	END;
	IF Unix.write(1, ADR(s), i) < 0 THEN
		;
	END;
  END XWriteString;

  PROCEDURE WriteCard(card, width : CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
	Conversions.ConvertCardinal(card, width, buf);
	WriteString(buf);
  END WriteCard;

  PROCEDURE WriteInt(int : INTEGER; width : CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
    	Conversions.ConvertInteger(int, width, buf);
	WriteString(buf);
  END WriteInt;

  PROCEDURE WriteHex(card, width : CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
	Conversions.ConvertHex(card, width, buf);
	WriteString(buf);
  END WriteHex;

  PROCEDURE WriteLn;
  BEGIN
    	Write(EOL)
  END WriteLn;

  PROCEDURE WriteOct(card, width : CARDINAL);
  VAR
    	buf : numbuf;
  BEGIN
    	Conversions.ConvertOctal(card, width, buf);
	WriteString(buf);
  END WriteOct;

  PROCEDURE WriteString(str : ARRAY OF CHAR);
  VAR
    	nbytes : CARDINAL;
  BEGIN
    	nbytes := 0;
    	WHILE (nbytes <= HIGH(str)) AND (str[nbytes] # 0C) DO
		Write(str[nbytes]);
		INC(nbytes)
    	END;
  END WriteString;

BEGIN	(* InOut initialization *)
	WITH ibuf DO
		fildes := 0;
		bufferedcount := BUFSIZ;
		maxcnt := 0;
		cnt := 1;
	END;
	WITH obuf DO
		fildes := 1;
		bufferedcount := 1;
		cnt := 0;
	END;
END InOut.
