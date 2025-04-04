(*
 * Passes an open array to a procedure.  The back end must implement
 * some EM instructions for accessing arrays.
 *)
MODULE OpenArray;
FROM Test IMPORT fail, finished;

(* Asserts condition or fails with code. *)
PROCEDURE A(cond: BOOLEAN; code: INTEGER);
BEGIN
  IF NOT cond THEN fail(code) END
END A;

(* Called as Modify(ary1, 1) or Modify(ary2, 2). *)
PROCEDURE Modify(VAR ary: ARRAY OF INTEGER; what: INTEGER);
  VAR hi: INTEGER;
BEGIN
  hi := what * 100H;

  (* Indices must be from 0 to HIGH(ary). *)
  A((what = 1) = (HIGH(ary) = 3), hi + 1);
  A((what = 2) = (HIGH(ary) = 9), hi + 2);

  (* ary[2] must equal ary1[3] or ary2[3]. *)
  A((what = 1) = (ary[2] = 13), hi + 3);
  A((what = 2) = (ary[2] = 37), hi + 4);

  (* Modify some values. *)
  IF HIGH(ary) >= 3 THEN ary[3] := 20 END;
  IF HIGH(ary) >= 6 THEN ary[6] := 40 END;
  IF HIGH(ary) >= 9 THEN ary[9] := 12 END;
END Modify;

VAR
  ary1: ARRAY [1..4] OF INTEGER;
  ary2: ARRAY [1..10] OF INTEGER;
BEGIN
  (* Initialize the arrays. *)
  ary1[1] :=  6; ary1[2] :=  9; ary1[3] := 13; ary1[4] := 49;

  ary2[1] := 56; ary2[2] := 79; ary2[3] := 37; ary2[4] :=  0;
  ary2[5] := 70; ary2[6] := 62; ary2[7] := 64; ary2[8] := 92;
  ary2[9] := 29; ary2[10] := 90;

  (* Pass them as open arrays. *)
  Modify(ary1, 1);
  Modify(ary2, 2);

  (* Check that ary1[4], ary2[4, 7, 10] have been modified. *)
  A(ary1[1] =  6, 301H); A(ary1[2] =  9, 301H); A(ary1[3] = 13, 303H);
  A(ary1[4] = 20, 304H);

  A(ary2[1] = 56, 401H); A(ary2[2] = 79, 402H); A(ary2[3] = 37, 403H);
  A(ary2[4] = 20, 404H); A(ary2[5] = 70, 406H); A(ary2[6] = 62, 406H);
  A(ary2[7] = 40, 407H); A(ary2[8] = 92, 408H); A(ary2[9] = 29, 409H);
  A(ary2[10] = 12, 40AH);

  finished;
END OpenArray.
