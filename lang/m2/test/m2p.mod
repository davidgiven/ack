MODULE Modula2PrettyPrinter;

FROM InOut IMPORT
	Done, Read, Write, WriteLn, WriteString, OpenInput, OpenOutput,
	CloseInput, CloseOutput;

(*
**      Modula-2 Prettyprinter, November 1985.
**
**      by Ken Yap, U of Rochester, CS Dept.
**
**      Permission to copy, modify, and distribute, but not for profit,
**      is hereby granted, provided that this note is included.
**
**      adapted from a Pascal Program Formatter
**      by J. E. Crider, Shell Oil Company,
**      Houston, Texas 77025
**
**      This program formats Modula-2 programs according
**      to structured formatting principles
**
**      A valid Modula-2 program is read from the input and
**      a formatted program is written to the output.
**      It is basically a recursive descent parser with actions
**      intermixed with syntax scanning.
**
**      The actions of the program are as follows:
**
**      FORMATTING:  Each structured statement is formatted
**      in the following pattern (with indentation "indent"):
**
**                XXXXXX header XXXXXXXX
**                        XXXXXXXXXXXXXXXXXX
**                        XXXXX body XXXXXX
**                        XXXXXXXXXXXXXXXXXX
**                END
**
**      where the header is one of:
**
**                IF <expression> THEN
**                ELSIF <expression> THEN
**                ELSE
**                WHILE <expression> DO
**                FOR <control variable> := <FOR list> DO
**                WITH <RECORD variable> DO
**                REPEAT
**                LOOP
**                CASE <expression> OF
**                <CASE label list>:
**
**      and the last line begins with UNTIL or is END.
**      Other program parts are formatted similarly.  The headers are:
**
**                <MODULE/PROCEDURE heading>;
**                CONST
**                TYPE
**                VAR
**                BEGIN
**                (various FOR records AND RECORD variants)
**
**      COMMENTS:  Each comment that starts before or on a specified
**      column on an input line (program constant "commthresh") is
**      copied without shifting or reformatting.  Each comment that
**      starts after "commthresh" is reformatted and left-justified
**      following the aligned comment base column ("alcommbase").
**
**      SPACES AND BLANK LINES:  Spaces not at line breaks are copied from
**      the input.  Blank lines are copied from the input if they appear
**      between statements (or appropriate declaration units).  A blank
**      line is inserted above each significant part of each program/
**      procedure if one is not already there.
**
**      CONTINUATION:  Lines that are too long for an output line are
**      continued with additional indentation ("contindent").
*)

CONST
	TAB = 11C;
	NEWLINE = 12C;                  (* for Unix *)
	FF = 14C;
	maxrwlen = 15;                  (* size of reserved word strings *)
	ordminchar = 0;                 (* ord of lowest char in char set *)
	ordmaxchar = 127;               (* ord of highest char in char set *)
(* The following parameters may be adjusted for the installation: *)
	maxinlen = 255;                 (* maximum width of input line + 1 *)
	maxoutlen = 80;                 (* maximum width of output line *)
	tabinterval = 8;                (* interval between tab columns *)
	initmargin = 0;                 (* initial value of output margin *)
	commthresh = tabinterval;       (* column threshhold in input for
						comments to be aligned *)
	alcommbase = 40;                (* aligned comments in output start
						after this column *)
	indent = tabinterval;           (* RECOMMENDED indentation increment *)
	contindent = tabinterval;       (* continuation indentation, >indent *)
	commindent = tabinterval;       (* comment continuation indentation *)

TYPE
	natural = INTEGER[0..32767];
	inrange = INTEGER[0..maxinlen];
	outrange = INTEGER[0..maxoutlen];

	errortype = (longline, noendcomm, notquote, longword, notdo, notof,
	notend, notthen, notbegin, notuntil, notident, notsemicolon, notcolon,
	notperiod, notparen, noeof);

	chartype = (illegal, special, chapostrophe, chleftparen, chrightparen,
	chperiod, digit, chcolon, chsemicolon, chlessthan, chgreaterthan,
	letter, chleftbrace, chbar);

	chartypeset = SET OF chartype;  (* for reserved word recognition *)

	resword = (                     (* reserved words ordered by length *)
	rwif, rwdo, rwof, rwto, rwin, rwor,
					(* length: 2 *)
	rwend, rwfor, rwvar, rwdiv, rwmod, rwset, rwand, rwnot, rwnil,
					(* length: 3 *)
	rwthen, rwelse, rwwith, rwcase, rwtype, rwloop, rwfrom,
					(* length: 4 *)
	rwbegin, rwelsif, rwuntil, rwwhile, rwarray, rwconst,
					(* length: 5 *)
	rwrepeat, rwrecord, rwmodule, rwimport, rwexport,
					(* length: 6 *)
	rwpointer,                      (* length: 7 *)
	rwprocedure, rwqualified,       (* length: 9 *)
	rwdefinition,                   (* length: 10 *)
	rwimplementation,               (* length: 14 *)
	rwx);                           (* length: 15 for table sentinel *)
	rwstring =  ARRAY [1..maxrwlen] OF CHAR;

	firstclass = (                  (* class of word if on new line *)
	newclause,                      (* start of new clause *)
	continue,                       (* continuation of clause *)
	alcomm,                         (* start of aligned comment *)
	contalcomm,                     (* continuation of aligned comment *)
	uncomm,                         (* start of unaligned comment *)
	contuncomm);                    (* continuation of unaligned comment *)

	wordtype = RECORD               (* data record for word *)
		whenfirst : firstclass; (* class of word if on new line *)
		puncfollows : BOOLEAN;  (* to reduce dangling punctuation *)
		blanklncount : natural; (* number of preceding blank lines *)
		spaces : INTEGER;       (* number of spaces preceding word *)
		base : [-1..maxinlen];  (* inline.buf[base] precedes word *)
		size : inrange;
	END;                            (* length of word in inline.buf *)

	symboltype = (                  (* symbols for syntax analysis *)
	symodule, sydefinition, syimplementation, syfrom, syimport, syexport,
	syqual, syproc, declarator, sybegin, syend, syif, sythen, syelsif,
	syelse, syloop, sycase, syof, syuntil, syrepeat, forwhilewith, sydo,
	syrecord, ident, intconst, semicolon, leftparen, rightparen, period,
	colon, bar, othersym, otherword, comment, syeof);
	symbolset = SET OF symboltype;

VAR
	inline : RECORD                 (* input line data *)
		endoffile : BOOLEAN;    (* end of file on input? *)
		ch : CHAR;              (* current char, buf[index] *)
		index : inrange;        (* subscript of current char *)
		len : natural;          (* length of input line in buf *)
		buf : ARRAY [1..maxinlen] OF CHAR;
	END;
	outline : RECORD                (* output line data *)
		blanklns : natural;     (* number of preceding blank lines *)
		len : outrange;         (* number of chars in buf *)
		buf : ARRAY [1..maxoutlen] OF CHAR;
	END;
	curword : wordtype;             (* current word *)
	margin : outrange;              (* left margin *)
	lnpending : BOOLEAN;            (* new line before next symbol? *)
	inheader : BOOLEAN;             (* are we scanning a proc header? *)
	symbol : symboltype;            (* current symbol *)

  (* Structured Constants *)
	headersyms : symbolset;         (* headers for program parts *)
	strucsyms : symbolset;          (* symbols that begin structured
						statements *)
	stmtendsyms : symbolset;        (* symbols that follow statements *)
	stopsyms : symbolset;           (* symbols that stop expression scan *)
	recendsyms : symbolset;         (* symbols that stop record scan *)
	datawords : symbolset;          (* to reduce dangling punctuation *)
	firstrw : ARRAY [1..maxrwlen] OF resword;
	rwword : ARRAY [rwif..rwimplementation] OF rwstring;
	rwsy : ARRAY [rwif..rwimplementation] OF symboltype;
	charclass : ARRAY CHAR OF chartype;
	symbolclass : ARRAY chartype OF symboltype;

PROCEDURE StrCmp(a, b : rwstring) : BOOLEAN;
VAR
	i : INTEGER;
BEGIN
	FOR i := 1 TO maxrwlen DO
		IF a[i] # b[i] THEN
			RETURN FALSE;
		END;
	END;
	RETURN TRUE;
END StrCmp;

PROCEDURE StructConsts;
(* establish values of structured constants *)
VAR
	i : [ordminchar..ordmaxchar];   (* loop index *)
	ch : CHAR;                      (* loop index *)

PROCEDURE BuildResWord(rw : resword; symword : rwstring; symbol : symboltype);
BEGIN
	rwword[rw] := symword;          (* reserved word string *)
	rwsy[rw] := symbol;             (* map to symbol *)
END BuildResWord;

BEGIN                                   (* StructConsts *)
(* symbol sets for syntax analysis *)
	headersyms := symbolset{symodule, syproc, declarator, sybegin, syend,
	syeof};
	strucsyms := symbolset{sycase, syrepeat, syif, forwhilewith, syloop};
	stmtendsyms := symbolset{semicolon, bar, syend, syuntil, syelsif,
	syelse, syeof};
	stopsyms := headersyms + strucsyms + stmtendsyms;
	recendsyms := symbolset{rightparen, syend, syeof};
	datawords := symbolset{otherword, intconst, ident, syend};

(* constants for recognizing reserved words *)
	firstrw[1] := rwif;             (* length: 1 *)
	firstrw[2] := rwif;             (* length: 2 *)
	BuildResWord(rwif, 'IF             ', syif);
	BuildResWord(rwdo, 'DO             ', sydo);
	BuildResWord(rwof, 'OF             ', syof);
	BuildResWord(rwto, 'TO             ', othersym);
	BuildResWord(rwin, 'IN             ', othersym);
	BuildResWord(rwor, 'OR             ', othersym);
	firstrw[3] := rwend;            (* length: 3 *)
	BuildResWord(rwend, 'END            ', syend);
	BuildResWord(rwfor, 'FOR            ', forwhilewith);
	BuildResWord(rwvar, 'VAR            ', declarator);
	BuildResWord(rwdiv, 'DIV            ', othersym);
	BuildResWord(rwmod, 'MOD            ', othersym);
	BuildResWord(rwset, 'SET            ', othersym);
	BuildResWord(rwand, 'AND            ', othersym);
	BuildResWord(rwnot, 'NOT            ', othersym);
	BuildResWord(rwnil, 'NIL            ', otherword);
	firstrw[4] := rwthen;           (* length: 4 *)
	BuildResWord(rwthen, 'THEN           ', sythen);
	BuildResWord(rwelse, 'ELSE           ', syelse);
	BuildResWord(rwwith, 'WITH           ', forwhilewith);
	BuildResWord(rwloop, 'LOOP           ', syloop);
	BuildResWord(rwfrom, 'FROM           ', syfrom);
	BuildResWord(rwcase, 'CASE           ', sycase);
	BuildResWord(rwtype, 'TYPE           ', declarator);
	firstrw[5] := rwbegin;          (* length: 5 *)
	BuildResWord(rwbegin, 'BEGIN          ', sybegin);
	BuildResWord(rwelsif, 'ELSIF          ', syelsif);
	BuildResWord(rwuntil, 'UNTIL          ', syuntil);
	BuildResWord(rwwhile, 'WHILE          ', forwhilewith);
	BuildResWord(rwarray, 'ARRAY          ', othersym);
	BuildResWord(rwconst, 'CONST          ', declarator);
	firstrw[6] := rwrepeat;         (* length: 6 *)
	BuildResWord(rwrepeat, 'REPEAT         ', syrepeat);
	BuildResWord(rwrecord, 'RECORD         ', syrecord);
	BuildResWord(rwmodule, 'MODULE         ', symodule);
	BuildResWord(rwimport, 'IMPORT         ', syimport);
	BuildResWord(rwexport, 'EXPORT         ', syexport);
	firstrw[7] := rwpointer;        (* length: 7 *)
	BuildResWord(rwpointer, 'POINTER        ', othersym);
	firstrw[8] := rwprocedure;      (* length: 8 *)
	firstrw[9] := rwprocedure;      (* length: 9 *)
	BuildResWord(rwprocedure, 'PROCEDURE      ', syproc);
	BuildResWord(rwqualified, 'QUALIFIED      ', syqual);
	firstrw[10] := rwdefinition;    (* length: 10 *)
	BuildResWord(rwdefinition, 'DEFINITION     ', sydefinition);
	firstrw[11] := rwimplementation;(* length: 11 *)
	firstrw[12] := rwimplementation;(* length: 12 *)
	firstrw[13] := rwimplementation;(* length: 13 *)
	firstrw[14] := rwimplementation;(* length: 14 *)
	BuildResWord(rwimplementation, 'IMPLEMENTATION ', syimplementation);
	firstrw[15] := rwx;             (* length: 15 FOR table sentinel *)

(* constants for lexical scan *)
	FOR i := ordminchar TO ordmaxchar DO
		charclass[CHR(i)] := illegal;
	END;
	FOR ch := 'a' TO 'z' DO
		charclass[ch] := letter;
		charclass[CAP(ch)] := letter;
	END;
	FOR ch := '0' TO '9' DO
		charclass[ch] := digit;
	END;
	charclass[' '] := special;
	charclass['"'] := chapostrophe;
	charclass['#'] := special;
	charclass['&'] := special;
	charclass["'"] := chapostrophe;
	charclass['('] := chleftparen;
	charclass[')'] := chrightparen;
	charclass['*'] := special;
	charclass['+'] := special;
	charclass[','] := special;
	charclass['-'] := special;
	charclass['.'] := chperiod;
	charclass['/'] := special;
	charclass[':'] := chcolon;
	charclass[';'] := chsemicolon;
	charclass['<'] := chlessthan;
	charclass['='] := special;
	charclass['>'] := chgreaterthan;
	charclass['@'] := special;
	charclass['['] := special;
	charclass[']'] := special;
	charclass['^'] := special;
	charclass['{'] := special;
	charclass['|'] := chbar;
	charclass['}'] := special;
	symbolclass[illegal] := othersym;
	symbolclass[special] := othersym;
	symbolclass[chapostrophe] := otherword;
	symbolclass[chleftparen] := leftparen;
	symbolclass[chrightparen] := rightparen;
	symbolclass[chperiod] := period;
	symbolclass[digit] := intconst;
	symbolclass[chcolon] := colon;
	symbolclass[chsemicolon] := semicolon;
	symbolclass[chlessthan] := othersym;
	symbolclass[chgreaterthan] := othersym;
	symbolclass[chbar] := bar;
	symbolclass[letter] := ident;
END StructConsts;

(* FlushLine/WriteError/ReadLine convert between files and lines. *)

PROCEDURE FlushLine;
(* Write buffer into output file *)
VAR
	i, j, vircol : outrange;        (* loop index *)
	nonblankseen : BOOLEAN;
BEGIN
	WITH outline DO
		WHILE blanklns > 0 DO
			WriteLn;
			blanklns := blanklns - 1;
		END;
		IF len > 0 THEN
			vircol := 0;
			nonblankseen := FALSE;
					(* set this to TRUE if you don't want
						blanks to tab conversion *)
			FOR i := 0 TO len - 1 DO
				IF buf[i+1] <> ' ' THEN
					IF NOT nonblankseen THEN
						LOOP
							j := (vircol DIV
							tabinterval + 1) *
							tabinterval;
							IF j > i THEN
								EXIT;
							END;
							Write(TAB);
							vircol := j;
						END;
					END;
					nonblankseen := TRUE;
					WHILE vircol < i DO
						Write(' ');
						vircol := vircol + 1;
					END;
					Write(buf[i+1]);
					vircol := i + 1;
				END;
			END;
			WriteLn;
			len := 0;
		END;
	END;
END FlushLine;

PROCEDURE WriteError(error : errortype; nm : ARRAY OF CHAR);
(* report error to output *)
VAR
	i, ix : inrange;                (* loop index, limit *)
BEGIN
	FlushLine;
	WriteString('(* !!! error, ');
	WriteString(nm);
	CASE error OF
	longline:
		WriteString('shorter line');
	| noendcomm:
		WriteString('END OF comment');
	| notquote:
		WriteString("final ' on line");
	| longword:
		WriteString('shorter word');
	| notdo:
		WriteString('"DO"');
	| notof:
		WriteString('"OF"');
	| notend:
		WriteString('"END"');
	| notthen:
		WriteString('"THEN"');
	| notbegin:
		WriteString('"BEGIN"');
	| notuntil:
		WriteString('"UNTIL"');
	| notident:
		WriteString('"identifier"');
	| notsemicolon:
		WriteString('";"');
	| notperiod:
		WriteString('"."');
	| notcolon:
		WriteString('":"');
	| notparen:
		WriteString('")"');
	| noeof:
		WriteString('END OF file');
	END;
	WriteString(' expected');
	IF error >= longword THEN
		WriteString(', NOT "');
		WITH inline DO
			WITH curword DO
				IF size > maxrwlen THEN
					ix := maxrwlen
				ELSE
					ix := size;
				END;
				FOR i := 1 TO ix DO
					Write(buf[base + i]);
				END;
			END;
		END;
		Write('"');
	END;
	IF error = noeof THEN
		WriteString(', FORMATTING STOPS');
	END;
	WriteString(' !!! *)');
	WriteLn;
END WriteError;

PROCEDURE ReadLine;
(* Read line into input buffer *)
VAR
	c : CHAR;                       (* input character *)
BEGIN
	WITH inline DO
		len := 0;
		LOOP
			Read(c);
			IF NOT Done THEN
				endoffile := TRUE;
				EXIT;
			END;
			IF c = NEWLINE THEN
				EXIT;
			END;
			IF c < ' ' THEN (* convert ISO control chars (except
						leading form feed) to spaces *)
				IF c = TAB THEN
					(* ISO TAB char *)
					c := ' ';
					(* add last space at end *)
					WHILE len MOD 8 <> 7 DO
						len := len + 1;
						IF len < maxinlen THEN
							buf[len] := c;
						END;
					END;
					(* END tab handling *)
				ELSIF (c <> FF) OR (len > 0) THEN
					c := ' ';
				END;
			END;            (* END ISO control char conversion *)
			len := len + 1;
			IF len < maxinlen THEN
				buf[len] := c;
			END;
		END;
		IF NOT endoffile THEN
			IF len >= maxinlen THEN
					(* input line too long *)
				WriteError(longline, "(ReadLine), ");
				len := maxinlen - 1;
			END;
			WHILE (len > 0) AND (buf[len] = ' ') DO
				len := len - 1;
			END;
		END;
		len := len + 1;         (* add exactly ONE trailing blank *)
		buf[len] := ' ';
		index := 0;
	END;
END ReadLine;

PROCEDURE GetChar;
(* get next char from input buffer *)
BEGIN
	WITH inline DO
		index := index + 1;
		ch := buf[index];
	END;
END GetChar;

PROCEDURE NextChar() : CHAR;
(* look at next char in input buffer *)
BEGIN
	RETURN inline.buf[inline.index + 1];
END NextChar;

PROCEDURE StartWord(startclass : firstclass);
(* note beginning of word, and count preceding lines and spaces *)
VAR
	first : BOOLEAN;                (* is word the first on input line? *)
BEGIN
	first := FALSE;
	WITH inline DO
		WITH curword DO
			whenfirst := startclass;
			blanklncount := 0;
			WHILE (index >= len) AND NOT endoffile DO
				IF len = 1 THEN
					blanklncount := blanklncount + 1;
				END;
				IF startclass = contuncomm THEN
					FlushLine
				ELSE
					first := TRUE;
				END;
				ReadLine;
					(* with exactly ONE trailing blank *)
				GetChar;
				IF ch = FF THEN
					FlushLine;
					Write(FF);
					blanklncount := 0;
					GetChar;
				END;
			END;
			spaces := 0;    (* count leading spaces *)
			IF NOT endoffile THEN
				WHILE ch = ' ' DO
					spaces := spaces + 1;
					GetChar;
				END;
			END;
			IF first THEN
				spaces := 1;
			END;
			base := index - 1;
		END;
	END;
END StartWord;

PROCEDURE FinishWord;
(* note end of word *)
BEGIN
	WITH inline DO
		WITH curword DO
			puncfollows := (symbol IN datawords) AND (ch <> ' ');
			size := index - base - 1;
		END;
	END;
END FinishWord;

PROCEDURE CopyWord(newline : BOOLEAN; pword : wordtype);
(* copy word from input buffer into output buffer *)
VAR
	i : INTEGER;                    (* outline.len excess, loop index *)
BEGIN
	WITH pword DO
		WITH outline DO
			i := maxoutlen - len - spaces - size;
			IF newline OR (i < 0) OR ((i = 0) AND puncfollows) THEN
				FlushLine;
			END;
			IF len = 0 THEN (* first word on output line *)
				blanklns := blanklncount;
				CASE whenfirst OF
					(* update LOCAL word.spaces *)
				newclause:
					spaces := margin;
				| continue:
					spaces := margin;
				| alcomm:
					spaces := alcommbase;
				| contalcomm:
					spaces := alcommbase + commindent;
				| uncomm:
					spaces := base;
				| contuncomm:
					(* spaces := spaces *);
				END;
				IF spaces + size > maxoutlen THEN
					spaces := maxoutlen - size;
					(* reduce spaces *)
					IF spaces < 0 THEN
						WriteError(longword,
						"(CopyWord), ");
						size := maxoutlen;
						spaces := 0;
					END;
				END;
			END;
			FOR i := 1 TO spaces DO
					(* put out spaces *)
				len := len + 1;
				buf[len] := ' ';
			END;
			FOR i := 1 TO size DO
					(* copy actual word *)
				len := len + 1;
				buf[len] := inline.buf[base + i];
			END;
		END;
	END;
END CopyWord;

PROCEDURE DoComment;                    (* copy aligned or unaligned comment *)

PROCEDURE CopyComment(commclass : firstclass; commbase : inrange);
(* copy words of comment *)
VAR
	endcomment : BOOLEAN;           (* end of comment? *)
BEGIN
	WITH curword DO                 (* copy comment begin symbol *)
		whenfirst := commclass;
		spaces := commbase - outline.len;
		CopyWord((spaces < 0) OR (blanklncount > 0), curword);
	END;
	commclass := VAL(firstclass, ORD(commclass)+1);
	WITH inline DO
		REPEAT                  (* loop for successive words *)
			StartWord(commclass);
			endcomment := endoffile;
					(* premature end? *)
			IF endcomment THEN
				WriteError(noendcomm, "(CopyComment), ")
			ELSE
				REPEAT
					IF ch = '*' THEN
						GetChar;
						IF ch = ')' THEN
							endcomment := TRUE;
							GetChar;
						END;
					ELSE
						GetChar;
					END;
				UNTIL (ch = ' ') OR endcomment;
			END;
			FinishWord;
			CopyWord(FALSE, curword)
		UNTIL endcomment;
	END;
END CopyComment;

BEGIN                                   (* DoComment *)
	IF curword.base < commthresh THEN
					(* copy comment without alignment *)
		CopyComment(uncomm, curword.base)
	ELSE                            (* align AND format comment *)
		CopyComment(alcomm, alcommbase);
	END;
END DoComment;

PROCEDURE GetSymbol;
(* get next non-comment symbol *)

PROCEDURE CopySymbol(symbol : symboltype; pword : wordtype);
(* copy word(s) of symbol *)
BEGIN
	IF symbol = comment THEN
		DoComment;              (* NOTE: DoComment uses global word! *)
		lnpending := TRUE;
	ELSIF symbol = semicolon THEN
		CopyWord(FALSE, pword);
		lnpending := NOT inheader;
	ELSE
		CopyWord(lnpending, pword);
		lnpending := FALSE;
	END;
END CopySymbol;

PROCEDURE FindSymbol;
(* find next symbol in input buffer *)

VAR
	termch : CHAR;                  (* string terminator *)
	chclass : chartype;             (* classification of leading char *)

PROCEDURE CheckResWord;
(* check if current identifier is reserved word/symbol *)
VAR
	rw, rwbeyond : resword;         (* loop index, limit *)
	symword : rwstring;             (* copy of symbol word *)
	i : [-1..maxrwlen];             (* loop index *)
BEGIN
	WITH curword DO
		WITH inline DO
			size := index - base - 1;
			IF size < maxrwlen THEN
				symword := '               ';
				FOR i := 1 TO size DO
					symword[i] := CAP(buf[ base + i]);
				END;
				rw := firstrw[size];
				rwbeyond := firstrw[size + 1];
				symbol := semicolon;
				REPEAT
					IF rw >= rwbeyond THEN
						symbol := ident
					ELSIF StrCmp(symword, rwword[rw]) THEN
						symbol := rwsy[rw]
					ELSE
						rw := VAL(resword,ORD(rw)+1);
					END;
				UNTIL symbol <> semicolon;
			END;
			whenfirst := newclause;
		END;
	END;
END CheckResWord;

PROCEDURE GetName;
BEGIN
	WHILE charclass[inline.ch] IN chartypeset{letter, digit} DO
		GetChar;
	END;
	CheckResWord;
END GetName;

PROCEDURE GetNumber;
BEGIN
	WITH inline DO
		WHILE charclass[ch] = digit DO
			GetChar;
		END;
		IF ch = '.' THEN
			IF charclass[NextChar()] = digit THEN
					(* NOTE: NextChar is a function! *)
				symbol := otherword;
				GetChar;
				WHILE charclass[ch] = digit DO
					GetChar;
				END;
			END;
		END;
		IF CAP(ch) = 'E' THEN
			symbol := otherword;
			GetChar;
			IF (ch = '+') OR (ch = '-') THEN
				GetChar;
			END;
			WHILE charclass[ch] = digit DO
				GetChar;
			END;
		END;
	END;
END GetNumber;

PROCEDURE GetStringLiteral;
VAR
	endstring : BOOLEAN;            (* end of string literal? *)
BEGIN
	WITH inline DO
		endstring := FALSE;
		REPEAT
			GetChar;
			IF ch = termch THEN
				endstring := TRUE;
			ELSIF index >= len THEN
					(* error, final "'" not on line *)
				WriteError(notquote, "(GetStringLiteral), ");
				symbol := syeof;
				endstring := TRUE;
			END;
		UNTIL endstring;
		GetChar;
	END;
END GetStringLiteral;

BEGIN                                   (* FindSymbol *)
	StartWord(continue);
	WITH inline DO
		IF endoffile THEN
			symbol := syeof
		ELSE
			termch := ch;   (* save for string literal routine *)
			chclass := charclass[ch];
			symbol := symbolclass[chclass];
			GetChar;        (* second CHAR *)
			CASE chclass OF
			chsemicolon, chrightparen, chleftbrace, special,
			illegal: ;
			| letter:
				GetName;
			| digit:
				GetNumber;
			| chapostrophe:
				GetStringLiteral;
			| chcolon:
				IF ch = '=' THEN
					symbol := othersym;
					GetChar;
				END;
			| chlessthan:
				IF (ch = '=') OR (ch = '>') THEN
					GetChar;
				END;
			| chgreaterthan:
				IF ch = '=' THEN
					GetChar;
				END;
			| chleftparen:
				IF ch = '*' THEN
					symbol := comment;
					GetChar;
				END;
			| chperiod:
				IF ch = '.' THEN
					symbol := colon;
					GetChar;
				END;    (* Added by me (CJ):  *)
			ELSE
			END;
			FinishWord;
		END;
	END;                            (* FindSymbol *)
END FindSymbol;

BEGIN                                   (* GetSymbol *)
	REPEAT
		CopySymbol(symbol, curword);
					(* copy word for symbol to output *)
		FindSymbol              (* get next symbol *)
	UNTIL symbol <> comment;
END GetSymbol;

PROCEDURE StartClause;
(* (this may be a simple clause, or the start of a header) *)
BEGIN
	curword.whenfirst := newclause;
	lnpending := TRUE;
END StartClause;

PROCEDURE PassSemicolons;
(* pass consecutive semicolons *)
BEGIN
	WHILE symbol = semicolon DO
		GetSymbol;
		StartClause;
	END;
END PassSemicolons;

PROCEDURE StartBody;
(* finish header, start body of structure *)
BEGIN
	StartClause;
	margin := margin + indent;
END StartBody;

PROCEDURE FinishBody;
(* retract margin *)
BEGIN
	margin := margin - indent;
END FinishBody;

PROCEDURE PassPhrase(finalsymbol : symboltype);
(* process symbols until significant symbol encountered *)
VAR
	endsyms : symbolset;            (* complete set of stopping symbols *)
BEGIN
	IF symbol <> syeof THEN
		endsyms := stopsyms;
		INCL(endsyms, finalsymbol);
		REPEAT
			GetSymbol
		UNTIL symbol IN endsyms;
	END;
END PassPhrase;

PROCEDURE Expect(expectedsym : symboltype; error : errortype; syms : symbolset;
nm : ARRAY OF CHAR);
(* fail if current symbol is not the expected one, then recover *)
BEGIN
	IF symbol = expectedsym THEN
		GetSymbol
	ELSE
		WriteError(error, nm);
		INCL(syms, expectedsym);
		WHILE NOT (symbol IN syms) DO
			GetSymbol;
		END;
		IF symbol = expectedsym THEN
			GetSymbol;
		END;
	END;
END Expect;

PROCEDURE Heading;
(* process heading for program or procedure *)

PROCEDURE MatchParens;                  (* process parentheses in heading *)
VAR
	endsyms : symbolset;
BEGIN
	GetSymbol;
	WHILE NOT (symbol IN recendsyms) DO
		IF symbol = leftparen THEN
			MatchParens
		ELSE
			GetSymbol;
		END;
	END;
	endsyms := stopsyms + recendsyms;
	Expect(rightparen, notparen, endsyms, "(MatchParens), ");
END MatchParens;

BEGIN                                   (* heading *)
	GetSymbol;
	PassPhrase(leftparen);
	IF symbol = leftparen THEN
		inheader := TRUE;
		MatchParens;
		inheader := FALSE;
	END;
	IF symbol = colon THEN
		PassPhrase(semicolon);
	END;
	Expect(semicolon, notsemicolon, stopsyms, "(Heading), ");

END Heading;

PROCEDURE DoRecord;
(* process record declaration *)
BEGIN
	GetSymbol;
	StartBody;
	PassFields(FALSE);
	FinishBody;
	Expect(syend, notend, recendsyms, "(DoRecord), ");
END DoRecord;

PROCEDURE DoVariant;
(* process (case) variant part *)
BEGIN
	PassPhrase(syof);
	Expect(syof, notof, stopsyms, "(Dovariant), ");
	StartBody;
	PassFields(TRUE);
	FinishBody;
END DoVariant;

PROCEDURE DoParens(forvariant : BOOLEAN);
(* process parentheses in record *)
BEGIN
	GetSymbol;
	IF forvariant THEN
		StartBody;
	END;
	PassFields(FALSE);
	lnpending := FALSE;             (* for empty field list *)
	Expect(rightparen, notparen, recendsyms, "(DoParens), ");
	IF forvariant THEN
		FinishBody;
	END;
END DoParens;

PROCEDURE PassFields(forvariant : BOOLEAN);
(* process declarations *)
BEGIN
	WHILE NOT (symbol IN recendsyms) DO
		IF symbol = semicolon THEN
			PassSemicolons
		ELSIF symbol = syrecord THEN
			DoRecord
		ELSIF symbol = sycase THEN
			DoVariant
		ELSIF symbol = leftparen THEN
			DoParens(forvariant)
		ELSE
			GetSymbol;
		END;
	END;
END PassFields;

PROCEDURE Statement;
(* process statement *)
BEGIN
	CASE symbol OF
	sycase:
		CaseStatement;
		Expect(syend, notend, stmtendsyms, "(Case), ");
	| syif:
		IfStatement;
		Expect(syend, notend, stmtendsyms, "(If), ");
	| syloop:
		LoopStatement;
		Expect(syend, notend, stmtendsyms, "(Loop), ");
	| syrepeat:
		RepeatStatement;
	| forwhilewith:
		ForWhileWithStatement;
		Expect(syend, notend, stmtendsyms, "(ForWhileWith), ");
	| ident:
		AssignmentProccall;
	| semicolon: ;                  (*!!! Added by me (CJ) *)
	ELSE ;
	END;
END Statement;

PROCEDURE AssignmentProccall;
(* pass an assignment statement or procedure call *)
BEGIN
	WHILE NOT (symbol IN stmtendsyms) DO
		GetSymbol;
	END;
END AssignmentProccall;

PROCEDURE StatementSequence;
(* process sequence of statements *)
BEGIN
	Statement;
	LOOP
		IF symbol <> semicolon THEN
			EXIT;
		END;
		GetSymbol;
		Statement;
	END;
END StatementSequence;

PROCEDURE IfStatement;
(* process if statement *)
BEGIN
	PassPhrase(sythen);
	Expect(sythen, notthen, stopsyms, "(Ifstatement), ");
	StartBody;
	StatementSequence;
	FinishBody;
	WHILE symbol = syelsif DO
		StartClause;
		PassPhrase(sythen);
		Expect(sythen, notthen, stopsyms, "(Elseif), ");
		StartBody;              (* new line after 'THEN' *)
		StatementSequence;
		FinishBody;
	END;
	IF symbol = syelse THEN
		StartClause;
		GetSymbol;
		StartBody;              (* new line after 'ELSE' *)
		StatementSequence;
		FinishBody;
	END;
END IfStatement;

PROCEDURE CaseStatement;
(* process case statement *)
BEGIN
	PassPhrase(syof);
	Expect(syof, notof, stopsyms, "(caseStatement), ");
	StartClause;
	OneCase;
	WHILE symbol = bar DO
		GetSymbol;
		OneCase;
	END;
	IF symbol = syelse THEN
		GetSymbol;
		StartBody;
		StatementSequence;
		FinishBody;
	END;
END CaseStatement;

PROCEDURE OneCase;
(* process one case clause *)
BEGIN
	IF NOT (symbol IN symbolset{bar, syelse}) THEN
		PassPhrase(colon);
		Expect(colon, notcolon, stopsyms, "(OneCase), ");
		StartBody;              (* new line, indent after colon *)
		StatementSequence;
		FinishBody;             (* left-indent after case *)
	END;
END OneCase;

PROCEDURE RepeatStatement;
(* process repeat statement *)
BEGIN
	GetSymbol;
	StartBody;                      (* new line, indent after 'REPEAT' *)
	StatementSequence;
	FinishBody;                     (* left-ident after UNTIL *)
	StartClause;                    (* new line before UNTIL *)
	Expect(syuntil, notuntil, stmtendsyms, "(repeatstatement), ");
	PassPhrase(semicolon);
END RepeatStatement;

PROCEDURE LoopStatement;
(* process loop statement *)
BEGIN
	GetSymbol;
	StartBody;                      (* new line, indent after LOOP *)
	StatementSequence;
	FinishBody;                     (* left-ident before END *)
END LoopStatement;

PROCEDURE ForWhileWithStatement;
(* process for, while, or with statement *)
BEGIN
	PassPhrase(sydo);
	Expect(sydo, notdo, stopsyms, "(ForWhileWithstatement), ");
	StartBody;
	StatementSequence;
	FinishBody;
END ForWhileWithStatement;

PROCEDURE ProcedureDeclaration;
(* pass a procedure declaration *)
BEGIN
	ProcedureHeading;
	Block;
	Expect(ident, notident, stmtendsyms, "(Proceduredeclaration)1, ");
	Expect(semicolon, notsemicolon, stmtendsyms,
	"(Proceduredeclaration)2, ");
END ProcedureDeclaration;

PROCEDURE ProcedureHeading;
BEGIN
	StartClause;
	Heading;
END ProcedureHeading;

PROCEDURE Block;
BEGIN
	WHILE symbol IN symbolset{declarator, symodule, syproc} DO
		Declaration;
	END;
	IF symbol = sybegin THEN
		GetSymbol;
		StartBody;
		StatementSequence;
		FinishBody;
	END;
	Expect(syend, notend, stmtendsyms, "(Block), ");
END Block;

PROCEDURE Declaration;
BEGIN
	IF symbol = declarator THEN
		StartClause;            (* CONST, TYPE, VAR *)
		GetSymbol;
		StartBody;
		REPEAT
			PassPhrase(syrecord);
			IF symbol = syrecord THEN
				DoRecord;
			END;
			IF symbol = semicolon THEN
				PassSemicolons;
			END;
		UNTIL symbol IN headersyms;
		FinishBody;
	ELSIF symbol = symodule THEN
		ModuleDeclaration;
	ELSIF symbol = syproc THEN
		ProcedureDeclaration;
	END;
END Declaration;

PROCEDURE ModuleDeclaration;
BEGIN
	PassPhrase(semicolon);
	PassSemicolons;
	WHILE symbol IN symbolset{syimport, syexport, syfrom} DO
		ImportExport;
	END;
	Block;
	Expect(ident, notident, stmtendsyms, "(ModuleDeclaration), ");
END ModuleDeclaration;

PROCEDURE ImportExport;
BEGIN
	IF symbol = syfrom THEN
		PassPhrase(syimport);
	END;
	IF symbol = syimport THEN
		GetSymbol;
	ELSIF symbol = syexport THEN
		GetSymbol;
		IF symbol = syqual THEN
			GetSymbol;
		END;
	END;
	StartBody;
	PassPhrase(semicolon);
	FinishBody;
	GetSymbol;
END ImportExport;

PROCEDURE OneDefinition;
BEGIN
	IF symbol = declarator THEN
		Declaration;
	ELSIF symbol = syproc THEN
		ProcedureHeading;
	END;
END OneDefinition;

PROCEDURE DefinitionModule;
BEGIN
	GetSymbol;
	PassPhrase(semicolon);
	GetSymbol;
	WHILE symbol IN symbolset{syimport, syexport, syfrom} DO
		ImportExport;
	END;
	WHILE symbol IN symbolset{declarator, syproc} DO
		OneDefinition;
	END;
	Expect(syend, notend, stmtendsyms, "DefinitionModule1, " );
	GetSymbol;
	Expect(period, notperiod, stmtendsyms, 'DefintionModule2, ');
END DefinitionModule;

PROCEDURE ProgramModule;
BEGIN
	ModuleDeclaration;
	Expect(period, notperiod, stmtendsyms, "ProgramModule, ");
END ProgramModule;

PROCEDURE CompilationUnit;
BEGIN
	IF symbol = syimplementation THEN
		GetSymbol;
		ProgramModule;
	ELSIF symbol = sydefinition THEN
		DefinitionModule;
	ELSE
		ProgramModule;
	END;
END CompilationUnit;

PROCEDURE CopyRemainder;
(* copy remainder of input *)
BEGIN
	WriteError(noeof, "(Copyremainder), ");
	WITH inline DO
		REPEAT
			CopyWord(FALSE, curword);
			StartWord(contuncomm);
			IF NOT endoffile THEN
				REPEAT
					GetChar
				UNTIL ch = ' ';
			END;
			FinishWord;
		UNTIL endoffile;
	END;
END CopyRemainder;

PROCEDURE Initialize;
(* initialize global variables *)
BEGIN
	WITH inline DO
		endoffile := FALSE;
		ch := ' ';
		index := 0;
		len := 0;
	END;
	WITH outline DO
		blanklns := 0;
		len := 0;
	END;
	WITH curword DO
		whenfirst := contuncomm;
		puncfollows := FALSE;
		blanklncount := 0;
		spaces := 0;
		base := 0;
		size := 0;
	END;
	margin := initmargin;
	lnpending := FALSE;
	symbol := othersym;
	inheader := FALSE;
END Initialize;

BEGIN
	StructConsts;
	Initialize;
(* Files may be opened here. *)
	OpenInput("mod");
	OpenOutput("mod");
	GetSymbol;
	CompilationUnit;
	IF NOT inline.endoffile THEN
		CopyRemainder;
	END;
	FlushLine;
	CloseInput;
	CloseOutput;
END Modula2PrettyPrinter.
