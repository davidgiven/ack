IMPLEMENTATION MODULE MathLib0;

  IMPORT Mathlib;

PROCEDURE cos(arg: REAL): REAL;
BEGIN
	RETURN Mathlib.cos(arg);
END cos;

PROCEDURE sin(arg: REAL): REAL;
BEGIN
	RETURN Mathlib.sin(arg);
END sin;

PROCEDURE arctan(arg: REAL): REAL;
BEGIN
	RETURN Mathlib.arctan(arg);
END arctan;

PROCEDURE sqrt(arg: REAL): REAL;
BEGIN
	RETURN Mathlib.sqrt(arg);
END sqrt;

PROCEDURE ln(arg: REAL): REAL;
BEGIN
	RETURN Mathlib.ln(arg);
END ln;

PROCEDURE exp(arg: REAL): REAL;
BEGIN
	RETURN Mathlib.exp(arg);
END exp;

PROCEDURE entier(x: REAL): INTEGER;
BEGIN
	RETURN TRUNC(x);	(* ??? *)
END entier;

PROCEDURE real(x: INTEGER): REAL;
BEGIN
	RETURN FLOAT(x);	(* ??? *)
END real;

BEGIN
END MathLib0.
