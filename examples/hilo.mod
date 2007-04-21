(* $Source$
 * $State$
 * $Revision$
 *)

MODULE HiLo;
FROM InOut IMPORT WriteInt, WriteLn, WriteString, ReadString, ReadInt;
FROM random IMPORT Uniform;

VAR
	buffer : ARRAY [0..32] OF CHAR;
	
PROCEDURE reads;
BEGIN
	WriteString("> ");
	ReadString(buffer);
END reads;

PROCEDURE game;
VAR
	Number : INTEGER;
	Attempts : INTEGER;
	guess : INTEGER;
	finished : BOOLEAN;
BEGIN
	WriteString("See if you can guess my number.");
	WriteLn;
	
	Number := Uniform(0, 99);
	Attempts := 1;
	finished := FALSE;
	
	WHILE NOT finished DO
		WriteLn;
		WriteString("> ");
		ReadInt(guess);
		
		IF guess = Number THEN
			WriteLn;
			WriteString("You got it right in only ");
			WriteInt(Attempts, 0);
			WriteString(" ");
			IF Attempts = 1 THEN
				WriteString("go");
			ELSE
				WriteString("goes");
			END;
			WriteString("!");
			WriteLn;
			finished := TRUE;
		ELSIF guess < Number THEN
			WriteLn;
			WriteString("Try a bit higher.");
			WriteLn;
		ELSIF guess > Number THEN
			WriteLn;
			WriteString("Try a bit lower.");
			WriteLn;
		END;
		
		Attempts := Attempts + 1;
	END;
END game;

VAR
	finished : BOOLEAN;
BEGIN
	WriteLn;
	WriteString("Hi there! I'm written in Modula-2. Before we start, what is your name?");
	WriteLn;
	reads;
	WriteLn;
	WriteString("Hello, ");
	WriteString(buffer);
	WriteString("! ");
	
	finished := FALSE;
	WHILE NOT finished DO
		game;
		WriteLn;
		WriteString("Would you like another go?");
		WriteLn;
		reads;
		
		IF (buffer[0] = 'n') OR (buffer[0] = 'N') THEN
			finished := TRUE;
			WriteLn;
			WriteString("Thanks for playing --- goodbye!");
			WriteLn;
		ELSE
			WriteLn;
			WriteString("Excellent! ");
		END;
	END;
END HiLo.
