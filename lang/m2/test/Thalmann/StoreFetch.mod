MODULE StoreFetch;

(* From: MODULA-2, An Introduction, by Daniel Thalmann, Springer-Verlag,
	 New York, 1985
   Figure 20.3
*)

  FROM InOut IMPORT	ReadString, WriteString, WriteLn;
  FROM Processes IMPORT	SIGNAL, StartProcess, SEND, WAIT, Awaited, Init;

  MODULE SharedBuffer;

    IMPORT	SIGNAL, SEND, WAIT, Awaited, Init;

    EXPORT	Deposit, Remove;

    CONST	N = 16;

    VAR		n, in, out: CARDINAL;
		NonFull, NonEmpty: SIGNAL;
		Buffer: ARRAY [0..N-1] OF INTEGER;

    PROCEDURE Deposit(integer: INTEGER);
    BEGIN
	IF n=N THEN WAIT(NonFull) END;
	INC(n);
	Buffer[in] := integer;
	in := (in+1) MOD N;
	IF Awaited(NonEmpty) THEN SEND(NonEmpty) END;
    END Deposit;

    PROCEDURE Remove(VAR integer: INTEGER);
    BEGIN
	IF n=0 THEN WAIT(NonEmpty) END;
	DEC(n);
	integer := Buffer[out];
	out := (out+1) MOD N;
	IF Awaited(NonFull) THEN SEND(NonFull) END;
    END Remove;

  BEGIN
	n := 0;
	in := 0;
	out := 0;
	Init(NonFull);
	Init(NonEmpty);
  END SharedBuffer;

  CONST	Max = 256;
	eos = 0C;

  TYPE	StringType = ARRAY[0..Max-1] OF CHAR;

  VAR	EndOfTransfer: SIGNAL;

  PROCEDURE Store;
    VAR i: INTEGER;
    	String: StringType;
  BEGIN
	WriteString("Enter a string: ");
	i := -1;
	ReadString(String);
	REPEAT
		INC(i);
		Deposit(ORD(String[i]));
	UNTIL String[i] = eos;
	WAIT(EndOfTransfer);
  END Store;

  PROCEDURE Fetch;
    VAR	i, OrdOfChar: INTEGER;
	String: StringType;
  BEGIN
	i := -1;
	REPEAT
		INC(i);
		Remove(OrdOfChar);
		String[i] := CHR(CARDINAL(OrdOfChar));
	UNTIL String[i] = eos;
	WriteLn;
	WriteString("After transfer: ");
	WriteString(String);
	WriteLn;
  END Fetch;

BEGIN
	Init(EndOfTransfer);
	StartProcess(Store, 2000);
	StartProcess(Fetch, 2000);
	WAIT(EndOfTransfer);
END StoreFetch.
