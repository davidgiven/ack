MODULE ConvTest;
FROM Conversions IMPORT
  ConvertOctal, ConvertHex, ConvertCardinal, ConvertInteger;
FROM Strings IMPORT CompareStr;
FROM Test IMPORT fail, finished;

(* Asserts a = b, or fails with code. *)
PROCEDURE A(a, b: ARRAY OF CHAR; code: INTEGER);
BEGIN
  IF (CompareStr(a, b) # 0) OR (CompareStr(a, "wrong string") = 0) THEN
    fail(code)
  END
END A;

VAR
  str: ARRAY [0..15] OF CHAR;
BEGIN
  ConvertOctal(  9, 6, str); A("    11", str, 1);
  ConvertOctal( 59, 6, str); A("    73", str, 2);
  ConvertOctal(278, 6, str); A("   426", str, 3);

  ConvertHex(  9, 6, str); A("     9", str, 11H);
  ConvertHex( 59, 6, str); A("    3B", str, 12H);
  ConvertHex(278, 6, str); A("   116", str, 13H);

  ConvertCardinal(  9, 6, str); A("     9", str, 21H);
  ConvertCardinal( 59, 6, str); A("    59", str, 22H);
  ConvertCardinal(278, 6, str); A("   278", str, 23H);

  ConvertInteger(   9, 6, str); A("     9", str, 31H);
  ConvertInteger(  59, 6, str); A("    59", str, 32H);
  ConvertInteger( 278, 6, str); A("   278", str, 33H);
  ConvertInteger(-424, 6, str); A("  -424", str, 34H);

  finished;
END ConvTest.
