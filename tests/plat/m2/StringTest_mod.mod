MODULE StringTest;
FROM Strings IMPORT
  Assign, Insert, Delete, Pos, Copy, Concat, Length, CompareStr;
FROM Test IMPORT fail, finished;

(* Asserts condition or fails with code. *)
PROCEDURE A(cond: BOOLEAN; code: INTEGER);
BEGIN
  IF NOT cond THEN fail(code) END
END A;

VAR
  small: ARRAY [0..3] OF CHAR;
  big: ARRAY [0..99] OF CHAR;
BEGIN
  (* CompareStr *)
  A(CompareStr("ablaze", "ablaze") = 0, 1);
  A(CompareStr("ablaze", "abloom") < 0, 2);
  A(CompareStr("abloom", "ablaze") > 0, 3);
  A(CompareStr("abloom", "abloom") = 0, 4);

  (* Assign, Insert, Delete *)
  Assign("obsequiosity", small);
  A(CompareStr("obsequiosity", small) > 0, 11H);
  Assign("obsequiosity", big);
  A(CompareStr("obsequiosity", big) = 0, 12H);
  A(big[11] = 'y', 13H);
  A(big[11] # 0C, 14H);
  A(big[12] # 'y', 15H);
  A(big[12] = 0C, 16H);
  Insert(" omnihuman", big, 9);
  A(CompareStr("obsequios omnihumanity", big) = 0, 17H);
  Delete(big, 6, 15);
  A(CompareStr("obsequy", big) = 0, 18H);

  (* Pos, Concat *)
  Assign("Now is the time for all good men to come...", big);
  A(Pos("w", big) = 2, 21H);
  A(Pos("t", big) = 7, 22H);
  A(Pos("ti", big) = 11, 23H);
  A(Pos("men", big) = 29, 24H);
  A(Pos("women", big) > 42, 25H);
  Copy(big, 29, 2, small);
  A(CompareStr("me", small) = 0, 26H);

  (* Concat, Length *)
  Concat("pictorial", "ist", big);
  A(CompareStr("pictorialist", big) = 0, 31H);
  A(Length(big) = 12, 32H);
  Concat("zit", "her", small);
  A(CompareStr("zither", small) > 0, 33H);
  A(Length(small) < 5, 34H);

  finished;
END StringTest.
