(*  COPYRIGHT 1979 YAVUZ ONDER, UNIVERSITY OF MANCHESTER  *)
(*$G-*) 

PROGRAM TAILOR ( INPUT, INFILE, error, output );
(*         HOW TO USE 'TAILOR'
 *  -ANY VERSION IN TEXT IS OPENED BY (*SNN() AND 
                            CLOSED BY ()SNN*) (*
 *        WHERE S IS '+' OR '-' (NO DEFAULT), 
 *             NN IS AN UNSIGNED TWO DIGIT INTEGER (NO ZERO SUPRESSION) 
 *        IN SOME CASES output WILL CONTAIN '+)' INSTEAD OF
 *             'ASTERISK)' AS COMMENT CLOSER. 
 *  -THE NAME OF THE FILE TO BE TAILORED IS THE FIRST ARGUMENT. 
 *  -THERE ARE THREE BASIC OPERATIONS :  
 *        'INCLUDE'  :  (I) REMOVES VERSION ENTRY AND CLOSING
 *                          SYMBOLS AND CHANGES '+)'S TO 'ASTERISK )'S
 *                          WITHIN THE VERSION ;
 *                     (II) CHANGES ALL 'ASTERISK )'S TO '+)'S
 *                          WITHIN THE COMPLEMENTED VERSION, EXCEPT IN 
 *                          VERSION CLOSER. 
 *        'SKIP'     :  (I) REMOVES ALL VERSION INCLUDING ENTRY AND
 *                          CLOSING SYMBOLS ; 
 *                     (II) PERFORMS 'INCLUDE' (I) ON COMPLEMENTED VERSION
 *       'LEAVE ALONE':      IF NO COMMAND EXISTS FOR ANY ONE OF THE VERSIONS 
 *                          IN THE TEXT 'INCLUDE' IS PERFORMED ON 
 *                          -(ABS(VERSION-NOT-IN-TEXT)).
 *  -COMMANDS ARE INPUT WHEN REQUIRED BY 'TAILOR'.
 *  -TO 'INCLUDE' ANY VERSION GIVE ITS NUMBER ('+'S NEED NOT BE GIVEN.).
 *  -TO 'SKIP' ANY VERSION ENTER ABS(ITS-NUMBER)+100 SIGNED AS IN TEXT... 
 *
 *        ... E.G.  COMMAND SEQUENCE ' 1 -102 103 -20 200 ' MEANS
 *              (PERFORM 'INCLUDE' ON 1,2,-3 AND -20 ) AND
 *              (PERFORM 'SKIP'    ON -2 AND 3 )     AND
 *              (PERFORM 'LEAVE ALONE' ON ALL OTHER VERSIONS IN TEXT.). 
 *  -TO TERMINATE COMMAND SEQUENCE BEFORE THIRTY-SECOND ENTER ANY COMMAND>=300
 *         THIRTYTWO OR MORE COMMANDS START THE EXECUTION OF THE TAILOR 
 *         AND ONLY FIRST THIRTYTWO (NOW APPROX 50) ARE ACCEPTED.
 *  -IF ANY VERSION OR ITS COMPLEMENT TAKES PLACE IN MORE THAN ONE
 *         COMMAND THE LAST ONE IS OBEYED.
 *  -ZERO CANNOT BE USED AS VERSION NUMBER OR IN COMMANDS.
 *  -TO REMOVE ALL TAILORING BRACKETS (USEFUL PRIOR TO XREF) INPUT 1000 ONLY.
 *  -LINE NUMBER ARE REMOVED FROM FILES, EXCEPT WITH THE 1000 COMMAND.
 *  -THE TAILORED PROGRAM APPEARS ON THE STANDARD OPUTPUT.
 *  -ERROR MESSAGES APPEAR ON THE FILE GIVEN BY THE SECOND ARGUMENT.
 ************* END OF HOW TO USE ************************************)

CONST verslimit=50;
VAR   VERLIST : ARRAY[1..verslimit]OF INTEGER;
      INFILE : TEXT;
      error : TEXT;
      (* INPUT AND OUTPUT FILES *)
      NOOFVER         : INTEGER; 
      (* NUMBER OF COMMANDS (MAX. verslimit) *)
      INLFLAG, INIFLAG : INTEGER;
      (* FLAGS SHOWING WHETHER IN A 'LEAVE ALONE' OR
                 'INCLUDE' RESPECTIVELY *)
      LINBUF           : ARRAY[1..200]OF CHAR; 
      (* TEMPORARY STORAGE FOR MANIPULATION OF
                 THE CURRENT LINE *)
      FIRSTNONBLANK    : INTEGER; (* KEEPS THE POSITION OF FIRST 
         NONBLANK 
                                 CHAR IN LINBUF *)
      INCLUDEALL       : BOOLEAN;
      (*******************************************************) 

PROCEDURE INITIALISE ( VAR NOOFVER : INTEGER );
  (* READS COMMANDS AND INITIALISES THE GLOBALS *)

  LABEL 9;

  VAR   VERNO, I : INTEGER;
BEGIN 
INCLUDEALL := FALSE; 
I := 0;
REPEAT
  IF I < verslimit THEN
    BEGIN 
    I := I+1;
    READ ( VERNO ); 
    IF VERNO < 300 THEN 
      VERLIST[I]:= VERNO 
    ELSE
      BEGIN 
      I := I-1;
      IF ( VERNO=1000 ) AND ( I=0 ) THEN
        INCLUDEALL := TRUE;
      GOTO 9
      END;
    END 
  ELSE
    GOTO 9
UNTIL 1=0;
9: NOOFVER := I;
INLFLAG := 0;
INIFLAG := 0;
FOR I := 1 TO 120 DO 
  LINBUF[I]:= ' '; 
FIRSTNONBLANK := 1(*0*);
RESET ( INFILE ); 
REWRITE ( output );
REWRITE ( error );
END;
(*******************************************************) 

PROCEDURE SEARCHVER;
  (* SEARCHES FOLLOWING VERSION IN THE TEXT 
    WHEN FOUND CALLS PROC SCANLIST          *)

  LABEL 99; 

  VAR   CH    : CHAR;
        I, II : INTEGER;

  PROCEDURE SCANLIST; 
    FORWARD;
  (****************************)

  PROCEDURE FINDEND ( VER : INTEGER ); 
    (*  SEARCHES END OF THE VERSION GIVEN IN PARAMETER
      IF ENCOUNTERS ANOTHER VERSION ENTRY IN THE MEANTIME 
      CALLS PROC SCANLIST ( AND ITSELF INDIRECTLY )       *)

    LABEL 999, 888, 9999;

    VAR   II, FIXI, ABVER, CLSVER : INTEGER; 
          OP                      : CHAR;
  BEGIN 
  FIXI := I; 
  ABVER := ABS ( VER );
  IF ABVER < 100 THEN 
    OP := 'I'
  ELSE
    BEGIN 
    IF ABVER < 200 THEN 
      OP := 'S'
    ELSE
      OP := 'L'; 
    VER := ( ABVER MOD 100 )*VER DIV ABVER;
    END;
  REPEAT
    while EOLN ( INFILE ) (* END-OF-LINE ACTION *) 
    do
      BEGIN 
      IF NOT ( OP='S' ) THEN
        IF FIRSTNONBLANK <> 0 THEN
          BEGIN 
          FOR II := 1 TO I DO
            WRITE ( output, LINBUF[II]); 
          WRITELN ( output ) 
          END 
        ELSE (*NOTHING*)
      ELSE
        IF  (FIXI>=FIRSTNONBLANK) AND (FIRSTNONBLANK <> 0) THEN 
          BEGIN 
          FOR II := 1 TO FIXI-1 DO 
            WRITE ( output, LINBUF[II]); 
          WRITELN ( output ) 
          END
      ELSE writeln(output) (*to keep line nos in step*);
      READLN ( INFILE );
      IF EOF ( INFILE ) THEN
        GOTO 888;
      if (infile^ in ['0'..'9']) AND NOT INCLUDEALL then
        for ii := 1 to 6 do get(infile); (*ignore line numbers*) 
      I := 0;
      fixi := 0;
      FIRSTNONBLANK := 1(*0*) 
      END;
    READ ( INFILE, CH );
    (* ACTION FOR EVERY CHARACTER *)
    I := I+1;
    LINBUF[I]:= CH;
    IF ( FIRSTNONBLANK=0 ) THEN 
      IF CH<>' ' THEN 
        BEGIN 
        FIRSTNONBLANK := I;
        FIXI := I-1
        END;
    IF ( CH=')' ) AND ( I > 6 ) (* A VERSIN CLOSER ? *) 
    THEN
      IF LINBUF[I-6]='(' THEN 
        IF LINBUF[I-5]=')' THEN 
          IF ( LINBUF[I-1]='*' ) OR ( LINBUF[I-1]='+' ) THEN
            IF ( LINBUF[I-2]IN['0'..'9']) AND ( LINBUF[I-3]IN['0'..'9']) AND ( LINBUF[I-4]IN['+', '-']) THEN
              BEGIN 
              CLSVER := ORD ( LINBUF[I-2])-ORD ( '0' )+( ORD ( LINBUF[I-3])-ORD ( '0' ) )*10;
              IF LINBUF[I-4]='-' THEN 
                CLSVER :=-CLSVER;
              IF ( VER=CLSVER ) OR INCLUDEALL THEN
                BEGIN 
                IF ( OP='I' ) OR INCLUDEALL THEN
                  IF FIRSTNONBLANK=I-6 THEN 
                    BEGIN 
                    FOR II := I DOWNTO I-6 DO
                      LINBUF[II]:= ' ';
                    FIRSTNONBLANK := 1(*0*) 
                    END 
                  ELSE
                    I := I-7;
                IF OP='S' THEN
                  BEGIN 
                  I := FIXI; 
                  IF FIRSTNONBLANK >= FIXI THEN 
                    FIRSTNONBLANK := 1(*0*) 
                  END;
                GOTO 9999;
                END;
              END;
    IF OP='S' THEN
      GOTO 999; 
    IF ( CH=')' ) AND ( I > 6 ) (* A NEW VERSION ENTRY ? *) 
    THEN
      IF LINBUF[I-6]='(' THEN 
        IF LINBUF[I-1]='(' THEN 
          IF LINBUF[I-5]='*' THEN 
            IF ( LINBUF[I-2]IN['0'..'9']) AND ( LINBUF[I-3]IN['0'..'9']) AND ( LINBUF[I-4]IN['+', '-']) THEN
              SCANLIST; 
    IF I>1 THEN 
      IF LINBUF[I]=')' (* CORRECTIONS ON COMMENT CLOSERS
         *) 
      THEN
      BEGIN 
      IF ( INLFLAG > 0 ) AND ( LINBUF[I-1]='*' ) THEN 
        LINBUF[I-1]:= '+'; 
      IF ( INIFLAG > 0 ) AND ( INLFLAG=0 ) AND ( LINBUF[I-1]='+' ) THEN 
        LINBUF[I-1]:= '*'; 
      END;
999:
  UNTIL EOF ( INFILE ); 
888:
  WRITELN ( error, 'VERSION ', VER : 2, ' NOT CLOSED AT EOF.' );
9999:  
  END;
  (******************************)

  PROCEDURE SCANLIST; 

    VAR   II, III, VERSN, COMMAND, ABSVER : INTEGER; 
  BEGIN 
  (* COMPUTES VERSION NUMBER FROM TEXT *) 
  VERSN := ORD ( LINBUF[I-2])-ORD ( '0' )+( ORD ( LINBUF[I-3])-ORD ( '0' ) )*10; 
  IF LINBUF[I-4]='-' THEN 
    VERSN :=-VERSN;
  ABSVER := ABS ( VERSN ); 
  COMMAND :=-ABS ( VERSN );
  (* FINDS COMMAND RELATED TO CURRENT VERSION, IF ANY *)
  FOR II := 1 TO NOOFVER DO
    IF ( ABSVER=ABS ( VERLIST[II]) ) OR ( ABSVER=ABS ( VERLIST[II])-100 ) or (absver=abs(verlist[ii])-200) THEN
      COMMAND := VERLIST[II];
  IF ( COMMAND=VERSN ) OR ( ABS ( COMMAND+VERSN )=100 ) OR INCLUDEALL (*
         CHECK & ACTION FOR 'INCLUDE' CONDITION 
         *) 
  THEN
    BEGIN 
    FOR III := I DOWNTO I-6 DO 
      LINBUF[III]:= ' '; 
    IF FIRSTNONBLANK=I-6 THEN 
      FIRSTNONBLANK := 1(*0*) 
    ELSE
      I := I-7;
    INIFLAG := INIFLAG+1;
    FINDEND ( VERSN );
    INIFLAG := INIFLAG-1;
    END 
  ELSE
    IF COMMAND+VERSN=0 (* CHECK & ACTION FOR 'LEAVE ALONE 
            ' CONDITION *)
    THEN
      BEGIN 
      INLFLAG := INLFLAG+1;
      FINDEND ( ( ABSVER+200 )*VERSN DIV ABSVER );
      INLFLAG := INLFLAG-1;
      END 
    ELSE
      BEGIN 
      (* ACTION FOR 'SKIP' CONDITION *) 
      I := I-7;
      IF FIRSTNONBLANK=I-6 THEN 
        FIRSTNONBLANK := 1(*0*);
      FINDEND ( ( ABSVER+100 )*VERSN DIV ABSVER );
      END;
  END;
  (*******************************) 
BEGIN (* BODY OF SEARCHVER *) 
if (infile^ in ['0'..'9']) AND NOT INCLUDEALL then
  for ii := 1 to 6 do get(infile); (*ignore line numbers*)
I := 0;
REPEAT
  while EOLN ( INFILE ) do
    begin 
    (* ACTION FOR EOLN S OUT OF ANY VERSION *)
    IF FIRSTNONBLANK <> 0 THEN
      BEGIN 
      FOR II := 1 TO I DO
        WRITE ( output, LINBUF[II]); 
      WRITELN ( output );
      READLN ( INFILE );
      IF EOF ( INFILE ) THEN
        GOTO 99;
      FIRSTNONBLANK := 1(*0*);
      END 
    ELSE
      BEGIN 
      if eof(infile) then goto 99;
      READLN ( INFILE );
      IF EOF ( INFILE ) THEN
        GOTO 99 
      END;
    if (infile^ in ['0'..'9']) AND NOT INCLUDEALL then
      for ii := 1 to 6 do get(infile); (*ignore line numbers*)
    I := 0;
    end;
  READ ( INFILE, CH );
  I := I+1;
  LINBUF[I]:= CH;
  IF FIRSTNONBLANK=0 THEN 
    IF CH<>' ' THEN 
      FIRSTNONBLANK := I;
  IF ( CH=')' ) AND ( I > 5 ) (* A VERSION ENTRY ? *) 
  THEN
    IF LINBUF[I-1]='(' THEN 
      IF LINBUF[I-5]='*' THEN 
        IF LINBUF[I-6]='(' THEN 
          IF ( LINBUF[I-2]IN['0'..'9']) AND ( LINBUF[I-3]IN['0'..'9']) AND ( LINBUF[I-4]IN['+', '-']) THEN
            SCANLIST; 
UNTIL EOF ( INFILE );
99:
END;
(***************************************************************) 

BEGIN 
INITIALISE ( NOOFVER ); 
SEARCHVER;
END.
