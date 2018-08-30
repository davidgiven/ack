(*
 * Calls nested procedures.  The compiler emits the EM instructions
 * _lxl_ and _lxa_ to access the variables in the statically enclosing
 * procedures.
 *
 * You can cheat this test if a = b is TRUE for any a, b.
 *)
MODULE NestProc;
FROM Test IMPORT fail, finished;

(* Asserts cond, or fails with code. *)
PROCEDURE A(cond: BOOLEAN; code: INTEGER);
BEGIN
  IF NOT cond THEN fail(code) END
END A;

TYPE
  Set8 = SET OF [0..63];
  (* Box has fields of size 8, 4, and 1. *)
  Box = RECORD
    huge: Set8;
    big: LONGINT;
    small: CHAR;
    tiny: CHAR;
  END;

PROCEDURE First(a, b: INTEGER; in: Box): Box;
  VAR c, d: INTEGER;
      out: Box;

  PROCEDURE Second(e: INTEGER);
    VAR f: INTEGER;

    PROCEDURE Third(g: INTEGER);
      VAR h: INTEGER;

      PROCEDURE CheckThird;
      BEGIN
        A(a = 1354, 31H);   (* lxa 3 *)
        A(b = 3385, 32H);
        A(c = 14349, 33H);  (* lxl 3 *)
        A(d = 30989, 34H);
        A(e = 28935, 35H);  (* lxa 2 *)
        A(f = 13366, 36H);  (* lxl 2 *)
        A(g = 7988, 37H);   (* lxa 1 *)
        A(h = 11711, 38H);  (* lxl 1 *)
      END CheckThird;

      PROCEDURE Fourth(i: INTEGER);
        VAR j: INTEGER;

        PROCEDURE Fifth(k: INTEGER);
          VAR l: INTEGER;

          PROCEDURE Sixth(): INTEGER;
          BEGIN
            A(e = 2, 61H);      (* lxa 4 *)
            A(f = 11703, 62H);  (* lxl 4 *)

            b := 3385;   (* lxa 5 *)
            d := 30989;  (* lxl 5 *)
            e := 28935;  (* lxl 4 *)
            f := 13366;  (* lxa 4 *)
            CheckThird;

            (* lxa 5 *)
            A(in.huge = Set8{11, 12, 40, 40, 43, 56}, 63H);
            A(in.big = 2130020019D, 64H);
            A(in.small = 300C, 65H);
            A(in.tiny = 175C, 66H);

            (* lxl 5 *)
            out.huge := Set8{8, 19, 36, 41, 47, 62};
            out.big := 385360915D;
            out.small := 366C;
            out.tiny := 131C;

            j := k;  (* lxl 2, lxa 1 *)
            l := i;  (* lxl 1, lxa 2 *)
            RETURN 5217;
          END Sixth;

          PROCEDURE TwiceSixth(): INTEGER;
          BEGIN
            (* lxa and lxl must follow the static chain from Sixth to
             * Fifth, not dynamic chain from Sixth to TwiceSixth. *)
            RETURN 2 * Sixth();
          END TwiceSixth;

        BEGIN (* Fifth *)
          A(TwiceSixth() = 10434, 51H);
          A(k = 11567, 51H);
          A(l = 32557, 52H);
        END Fifth;

      BEGIN (* Fourth *)
        Fifth(11567);  (* k *)
        A(i = 32557, 41H);
        A(j = 11567, 42H);
      END Fourth;

    BEGIN (* Third *)
      h := 11711;
      Fourth(32557);  (* i *)
    END Third;

  BEGIN (* Second *)
    f := 11703;
    Third(7988);  (* g *)
  END Second;

BEGIN (* First *)
  c := 14349;
  d := 17850;
  Second(2);  (* e *)
  RETURN out
END First;

VAR
  x: Box;
BEGIN
  x.huge := Set8{11, 12, 40, 40, 43, 56};
  x.big := 2130020019D;
  x.small := 300C;
  x.tiny := 175C;
  x := First(1354, 19516, x);  (* a, b, in *)
  A(x.huge = Set8{8, 19, 36, 41, 47, 62}, 71H);
  A(x.big = 385360915D, 72H);
  A(x.small = 366C, 73H);
  A(x.tiny = 131C, 74H);
  finished;
END NestProc.
