IMPLEMENTATION MODULE TableHandler;

  FROM InOut	IMPORT	Write, WriteLn, WriteInt;
  FROM Storage	IMPORT	Allocate;

  CONST	TableLength = 3000;

  TYPE
	TreePtr = POINTER TO Word;
	ListPtr = POINTER TO Item;
	Item = RECORD
		num: INTEGER;
		next: ListPtr
	       END;
	Word = RECORD
		key: CARDINAL;	(* table index *)
		first: ListPtr;	(* list head *)
		left, right: TreePtr
	       END;
	Table = TreePtr;

  VAR
	id: ARRAY [0..WordLength] OF CHAR;
	ascinx: CARDINAL;
	asc: ARRAY [0..TableLength-1] OF CHAR;

  PROCEDURE InitTable(VAR t: Table);
  BEGIN
	Allocate(t, SIZE(Word));
	t^.right := NIL
  END InitTable;

  PROCEDURE Search(p: TreePtr): TreePtr;
  (* search node with name equal to id
  *)
    TYPE	Relation = (less, equal, greater);
    VAR		q: TreePtr;
		r: Relation;
		i: CARDINAL;
    
    PROCEDURE rel(k: CARDINAL): Relation;
    (* compare id with asc[k]
    *)
      VAR	i: CARDINAL;
		R: Relation;
		x,y: CHAR;
    BEGIN
	i := 0;
	R := equal;
	LOOP
	    x := id[i];
	    y := asc[k];
	    IF CAP(x) # CAP(y) THEN EXIT END;
	    IF x <= " " THEN RETURN R END;
	    IF x < y THEN R := less ELSIF x > y THEN R := greater END;
	    i := i+1;
	    k := k+1;
	END;
	IF CAP(x) > CAP(y) THEN RETURN greater ELSE RETURN less END
    END rel;

  BEGIN	(* Search *)
	q := p^.right;
	r := greater;
	WHILE q # NIL DO
	    p := q;
	    r := rel(p^.key);
	    IF r = equal THEN RETURN p
	    ELSIF r = less THEN q := p^.left
	    ELSE q := p^.right
	    END
	END;
	Allocate(q, SIZE(Word));	(* not found, hence insert *)
	IF q # NIL THEN
	    WITH q^ DO
		key := ascinx;
		first := NIL;
		left := NIL;
		right := NIL
	    END;
	    IF r = less THEN p^.left := q ELSE p^.right := q END;
	    i := 0;	(* copy identifier into asc table *)
	    WHILE id[i] > " " DO
		IF ascinx = TableLength THEN
		    asc[ascinx] := " ";
		    id[i] := " ";
		    overflow := 1
		ELSE
		    asc[ascinx] := id[i];
		    ascinx := ascinx + 1;
		    i := i + 1
		END
	    END;
	    asc[ascinx] := " ";
	    ascinx := ascinx + 1;
	END;
	RETURN q;
  END Search;

  PROCEDURE Record(t: Table; VAR x: ARRAY OF CHAR; n: INTEGER);
    VAR	p: TreePtr;
	q: ListPtr;
	i: CARDINAL;
  BEGIN
	i := 0;
	REPEAT
	    id[i] := x[i];
	    i := i + 1
	UNTIL (id[i-1] = " ") OR (i = WordLength);
	p := Search(t);
	IF p = NIL THEN
	    overflow := 2
	ELSE
	    Allocate(q, SIZE(Item));
	    IF q = NIL THEN
		overflow := 3;
	    ELSE
		q^.num := n;
		q^.next := p^.first;
		p^.first := q
	    END
	END
  END Record;

  PROCEDURE Tabulate(t: Table);

    PROCEDURE PrintItem(p: TreePtr);
      CONST	L = 6;
		N = (LineWidth - WordLength) DIV L;
      VAR	ch: CHAR;
		i, k: CARDINAL;
		q: ListPtr;
    BEGIN
	i := WordLength + 1;
	k := p^.key;
	REPEAT
	    ch := asc[k];
	    i := i - 1;
	    k := k + 1;
	    Write(ch)
	UNTIL ch <= " ";
	WHILE i > 0 DO
	    Write(" ");
	    i := i-1
	END;
	q := p^.first;
	i := N;
	WHILE q # NIL DO
	    IF i = 0 THEN
		WriteLn;
		i := WordLength+1;
		REPEAT
		    Write(" ");
		    i := i-1
		UNTIL i = 0;
		i := N
	    END;
	    WriteInt(q^.num, L);
	    q := q^.next;
	    i := i - 1
	END;
	WriteLn
    END PrintItem;

    PROCEDURE TraverseTree(p: TreePtr);
    BEGIN
	IF p # NIL THEN
	    TraverseTree(p^.left);
	    PrintItem(p);
	    TraverseTree(p^.right)
	END
    END TraverseTree;

  BEGIN (* Tabulate *)
	WriteLn;
	TraverseTree(t^.right)
  END Tabulate;

BEGIN
	ascinx := 0;
	id[WordLength] := " ";
	overflow := 0
END TableHandler.
