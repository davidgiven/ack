(*
 * Generates some integer sequences.  Each generator is a process that
 * yields integers to the main process.  ACK switches processes by
 * saving and restoring the stack.  It uses _lor_ and _str_ to save
 * and restore the local base and frame pointer.
 *)
MODULE SemaTest;
FROM Semaphores IMPORT Sema, NewSema, Down, Up, StartProcess;
FROM Storage IMPORT ALLOCATE;
FROM Test IMPORT fail, finished;

TYPE
  Generator = POINTER TO GeneratorRecord;
  GeneratorRecord = RECORD
    resume: Sema;       (* up when resuming generator *)
    yield: Sema;        (* up when yielding value *)
    value: INTEGER;
  END;
VAR
  curgen: Generator;    (* current generator *)
  startLock: Sema;      (* down when booting generator *)
  startProc: PROC;
  startSelf: Generator;

PROCEDURE BootGenerator;
  VAR pr: PROC; self: Generator;
BEGIN
  pr := startProc;
  self := startSelf;
  Up(startLock);
  Down(self^.resume);   (* wait for first Resume *)
  pr();
END BootGenerator;

PROCEDURE StartGenerator(gen: Generator; pr: PROC);
BEGIN
  gen^.resume := NewSema(0);
  gen^.yield := NewSema(0);
  Down(startLock);
  startProc := pr;
  startSelf := gen;
  StartProcess(BootGenerator, 8192);
END StartGenerator;

PROCEDURE Resume(gen: Generator): INTEGER;
  VAR self: Generator;
BEGIN
  self := curgen;
  curgen := gen;
  Up(gen^.resume);
  Down(gen^.yield);     (* wait for Yield *)
  curgen := self;
  RETURN gen^.value
END Resume;

PROCEDURE Yield(i: INTEGER);
  VAR self: Generator;
BEGIN
  self := curgen;
  self^.value := i;
  Up(self^.yield);      (* curgen becomes invalid *)
  Down(self^.resume);   (* wait for Resume *)
END Yield;

PROCEDURE YieldHalfOf(i: INTEGER);
BEGIN
  Yield(i DIV 2);
END YieldHalfOf;

PROCEDURE Triangular;
  (* Yields the triangular numbers, http://oeis.org/A000217 *)
  VAR n: INTEGER;
BEGIN
  n := 0;
  LOOP
    YieldHalfOf(n * (n + 1));
    INC(n);
  END;
END Triangular;

PROCEDURE Pentagonal;
  (* Yields the pentagonal numbers, http://oeis.org/A000326 *)
  VAR n: INTEGER;
BEGIN
  n := 0;
  LOOP
    YieldHalfOf(n * (3 * n - 1));
    INC(n);
  END;
END Pentagonal;

PROCEDURE Odious;
  (* Yields the odius numbers, http://oeis.org/A000069 *)
  VAR b, i, n: INTEGER;
BEGIN
  n := 1;
  LOOP
    (* b := count bits in n *)
    b := 0;
    i := n;
    WHILE i # 0 DO
      INC(b, i MOD 2);
      i := i DIV 2;
    END;

    IF (b MOD 2) = 1 THEN
      Yield(n);
    END;
    INC(n);
  END;
END Odious;

TYPE
  Triple = ARRAY[1..3] OF INTEGER;
PROCEDURE T(i1, i2, i3: INTEGER): Triple;
  VAR t: Triple;
BEGIN
  t[1] := i1; t[2] := i2; t[3] := i3; RETURN t
END T;

CONST
  two28 = 268435456D;   (* 0x1000_0000 *)
VAR
  a: ARRAY [0..9] OF Triple;
  tri, pen, odi: Generator;
  i, g1, g2, g3: INTEGER;
BEGIN
  startLock := NewSema(1);

  ALLOCATE(tri, SIZE(GeneratorRecord));
  ALLOCATE(pen, SIZE(GeneratorRecord));
  ALLOCATE(odi, SIZE(GeneratorRecord));
  StartGenerator(tri, Triangular);
  StartGenerator(pen, Pentagonal);
  StartGenerator(odi, Odious);

  a[0] := T( 0,   0,  1);
  a[1] := T( 1,   1,  2);
  a[2] := T( 3,   5,  4);
  a[3] := T( 6,  12,  7);
  a[4] := T(10,  22,  8);
  a[5] := T(15,  35, 11);
  a[6] := T(21,  51, 13);
  a[7] := T(28,  70, 14);
  a[8] := T(36,  92, 16);
  a[9] := T(45, 117, 19);

  FOR i := 0 TO INTEGER(9) DO
    g1 := Resume(tri);
    g2 := Resume(pen);
    g3 := Resume(odi);
    IF g1 # a[i][1] THEN fail(1D * two28 + LONG(a[i][1])) END;
    IF g2 # a[i][2] THEN fail(2D * two28 + LONG(a[i][2])) END;
    IF g3 # a[i][3] THEN fail(3D * two28 + LONG(a[i][3])) END;
  END;
  finished;
END SemaTest.
