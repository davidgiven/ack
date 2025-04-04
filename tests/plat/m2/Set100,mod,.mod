(*
 * Operates on sets of 100 integers.  The compiler emits, and the back
 * end must implement, the EM instructions for large sets.
 *)
MODULE Set100;
FROM Test IMPORT fail, finished;

(* Asserts condition or fails with code. *)
PROCEDURE A(cond: BOOLEAN; code: INTEGER);
BEGIN
  IF NOT cond THEN fail(code) END
END A;

TYPE
  Num = [1..100];
  NumSet = SET OF Num;
VAR
  (* VAR, not CONST, so compiler can't do constant operations. *)
  primes, teens, lowevens, eighties, nineties: NumSet;
CONST
  (* These are the expected results of some set operations. *)
  primeteen = NumSet{13, 17, 19};
  compeighties = NumSet{80..82, 84..88};
  teenxoreven = NumSet{2, 4, 6, 8, 10, 12, 13, 15, 17, 19, 20};
  eightiesnineties = NumSet{80..99};

(* Checks that some set is equal to the expected result.  Also checks
 * that the set is not equal to the other sets. *)
PROCEDURE Check(set: NumSet; what: INTEGER);
  VAR hi: INTEGER;
BEGIN
  hi := what * 100H;

  (* The compiler uses cms in EM to check set equality. *)
  A((what = 1) = (set = primeteen), hi + 1);
  A((what = 2) = (set = compeighties), hi + 2);
  A((what = 3) = (set = teenxoreven), hi + 3);
  A((what = 4) = (set = eightiesnineties), hi + 4);
END Check;

PROCEDURE Range(min: Num; max: Num): NumSet;
BEGIN
  (* The compiler calls LtoUset in lang/m2/libm2/LtoUset.e *)
  RETURN NumSet{min..max}
END Range;

BEGIN
  primes := NumSet{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
                   47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  teens := NumSet{13, 14, 15, 16, 17, 18, 19};
  lowevens := NumSet{2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
  eighties := Range(80, 89);
  nineties := Range(90, 99);

  Check(primes * teens, 1);
  Check(eighties - primes, 2);
  Check(teens / lowevens, 3);
  Check(eighties + nineties, 4);

  finished;
END Set100.
