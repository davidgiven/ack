#
IMPLEMENTATION MODULE Terminal;
(* This implementation is Unix-dependant
*)
  IMPORT Unix;
  FROM SYSTEM IMPORT ADR;

  VAR fildes: INTEGER;
      unreadch: CHAR;
      unread: BOOLEAN;
      tty: ARRAY[0..8] OF CHAR;

  PROCEDURE Read(VAR ch: CHAR);
  BEGIN
	IF unread THEN
		ch := unreadch;
		unread := FALSE
	ELSE
		IF Unix.read(fildes, ADR(ch), 1) < 0 THEN
			;
		END;
	END;
	unreadch := ch;
  END Read;

  PROCEDURE BusyRead(VAR ch: CHAR);
    VAR l: INTEGER;
  BEGIN
	IF unread THEN
		ch := unreadch;
		unread := FALSE
	ELSE
#ifdef __USG
		l := Unix.fcntl(fildes, (*FGETFL*) 3, 0);
		IF Unix.fcntl(fildes,
			      (* FSETFL *) 4,
			      l + (*ONDELAY*) 2) < 0 THEN
			;
		END;
		IF Unix.read(fildes, ADR(ch), 1) = 0 THEN
			ch := 0C;
		ELSE
			unreadch := ch;
		END;
		IF Unix.fcntl(fildes, (*FSETFL*)4, l) < 0 THEN
			;
		END;
#else
#ifdef __BSD4_2
		IF Unix.ioctl(fildes, INTEGER(ORD('f')*256+127+4*65536+40000000H), ADR(l)) < 0 THEN
#else
		IF Unix.ioctl(fildes, INTEGER(ORD('f')*256+127), ADR(l)) < 0 THEN
#endif
			;
		END;

		IF l = 0 THEN
			ch := 0C;
		ELSE
			IF Unix.read(fildes, ADR(ch), 1) < 0 THEN
				;
			END;
			unreadch := ch;
		END;
#endif
  	END;
  END BusyRead;	

  PROCEDURE ReadAgain;
  BEGIN
	unread := TRUE;
  END ReadAgain;

  PROCEDURE Write(ch: CHAR);
  BEGIN
	IF Unix.write(fildes, ADR(ch), 1) < 0 THEN
		;
	END;
  END Write;

  PROCEDURE WriteLn;
  BEGIN
	Write(12C);
  END WriteLn;

  PROCEDURE WriteString(s: ARRAY OF CHAR);
    VAR i: CARDINAL;
  BEGIN
	i := 0;
	WHILE (i <= HIGH(s)) & (s[i] # 0C) DO
		Write(s[i]);
		INC(i)
	END
  END WriteString;

BEGIN
	tty := "/dev/tty";
	fildes := Unix.open(ADR(tty), 2);
	unread := FALSE;
END Terminal.
