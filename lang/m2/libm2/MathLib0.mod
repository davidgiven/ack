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
	IF x < 0.0 THEN
		RETURN - TRUNC(-x);
	END;
	RETURN TRUNC(x);
END entier;

PROCEDURE real(x: INTEGER): REAL;
BEGIN
	IF x < 0 THEN
		RETURN - FLOAT(-x);
	END;
	RETURN FLOAT(x);
END real;

BEGIN
END MathLib0.
