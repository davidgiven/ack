(*$R-*)
IMPLEMENTATION MODULE Mathlib;

  FROM EM IMPORT FIF, FEF;
  FROM Traps IMPORT Message;

	(* From:	Handbook of Mathematical Functions
			Edited by M. Abramowitz and I.A. Stegun
			National Bureau of Standards
			Applied Mathematics Series 55
	*)

  CONST
	OneRadianInDegrees	= 57.295779513082320876798155D;
	OneDegreeInRadians	=  0.017453292519943295769237D;

  (* basic functions *)

  PROCEDURE pow(x: REAL; i: INTEGER): REAL;
  BEGIN
	RETURN SHORT(longpow(LONG(x), i));
  END pow;

  PROCEDURE longpow(x: LONGREAL; i: INTEGER): LONGREAL;
    VAR
	val: LONGREAL;
	ri: LONGREAL;
  BEGIN
	ri := FLOATD(i);
	IF x < 0.0D THEN
		val := longexp(longln(-x) * ri);
		IF ODD(i) THEN RETURN -val;
		ELSE RETURN val;
		END;
	ELSIF x = 0.0D THEN
		RETURN 0.0D;
	ELSE
		RETURN longexp(longln(x) * ri);
	END;
  END longpow;

  PROCEDURE sqrt(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longsqrt(LONG(x)));
  END sqrt;

  PROCEDURE longsqrt(x: LONGREAL): LONGREAL;
    VAR
	temp: LONGREAL;
	exp, i: INTEGER;
  BEGIN
	IF x <= 0.0D THEN
		IF x < 0.0D THEN
			Message("sqrt: negative argument");
			HALT
		END;
		RETURN 0.0D;
	END;
	temp := FEF(x,exp);
	(*
	 * NOTE
	 * this wont work on 1's comp
	 *)
	IF ODD(exp) THEN
		temp := 2.0D * temp;
		DEC(exp);
	END;
	temp := 0.5D*(1.0D + temp);

	WHILE exp > 28 DO
		temp := temp * 16384.0D;
		exp := exp - 28;
	END;
	WHILE exp < -28 DO
		temp := temp / 16384.0D;
		exp := exp + 28;
	END;
	WHILE exp >= 2 DO
		temp := temp * 2.0D;
		exp := exp - 2;
	END;
	WHILE exp <= -2 DO
		temp := temp / 2.0D;
		exp := exp + 2;
	END;
	FOR i := 0 TO 4 DO
		temp := 0.5D*(temp + x/temp);
	END;
	RETURN temp;
  END longsqrt;

  PROCEDURE exp(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longexp(LONG(x)));
  END exp;

  PROCEDURE longexp(x: LONGREAL): LONGREAL;
  (*
   * n = floor(x / ln2), d = x / ln2 - n
   * exp(x) = exp((x / ln2) * ln2) = exp((n + d) * ln2) =
   * exp(n * ln2) * exp(d * ln2) = 2 ** n * exp(d * ln2)
   *)
    CONST
	a1 = -0.9999999995D;
	a2 =  0.4999999206D;
	a3 = -0.1666653019D;
	a4 =  0.0416573475D;
	a5 = -0.0083013598D;
	a6 =  0.0013298820D;
	a7 = -0.0001413161D;
    VAR
	neg: BOOLEAN;
	polval: LONGREAL;
	n: LONGREAL;
	n1 : INTEGER;
  BEGIN
	neg := x < 0.0D;
	IF neg THEN
		x := -x;
	END;
	x := FIF(x, 1.0D/LONG(ln2), n) * LONG(ln2);
	polval := 1.0D /(1.0D + x*(a1+x*(a2+x*(a3+x*(a4+x*(a5+x*(a6+x*a7)))))));
	n1 := TRUNCD(n + 0.5D);
	WHILE n1 >= 16 DO
		polval := polval * 65536.0D;
		n1 := n1 - 16;
	END;
	WHILE n1 > 0 DO
		polval := polval * 2.0D;
		DEC(n1);
	END;
	IF neg THEN RETURN 1.0D/polval; END;
	RETURN polval;
  END longexp;

  PROCEDURE ln(x: REAL): REAL;	(* natural log *)
  BEGIN
	RETURN SHORT(longln(LONG(x)));
  END ln;

  PROCEDURE longln(x: LONGREAL): LONGREAL;	(* natural log *)
    CONST
	a1 =  0.9999964239D;
	a2 = -0.4998741238D;
	a3 =  0.3317990258D;
	a4 = -0.2407338084D;
	a5 =  0.1676540711D;
	a6 = -0.0953293897D;
	a7 =  0.0360884937D;
	a8 = -0.0064535442D;
    VAR
	exp: INTEGER;
	polval: LONGREAL;

  BEGIN
	IF x <= 0.0D THEN
		Message("ln: argument <= 0");
		HALT
	END;
	x := FEF(x, exp);
	WHILE x < 1.0D DO
		x := x + x;
		DEC(exp);
	END;
	x := x - 1.0D;
	polval := x*(a1+x*(a2+x*(a3+x*(a4+x*(a5+x*(a6+x*(a7+a8*x)))))));
	RETURN polval + FLOATD(exp) * LONG(ln2);
  END longln;

  PROCEDURE log(x: REAL): REAL;	(* log with base 10 *)
  BEGIN
	RETURN SHORT(longlog(LONG(x)));
  END log;

  PROCEDURE longlog(x: LONGREAL): LONGREAL;	(* log with base 10 *)
  BEGIN
	RETURN longln(x)/LONG(ln10);
  END longlog;

  (* trigonometric functions; arguments in radians *)

  PROCEDURE sin(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longsin(LONG(x)));
  END sin;

  PROCEDURE longsin(x: LONGREAL): LONGREAL;
    CONST
	a2  = -0.1666666664D;
	a4  =  0.0083333315D;
	a6  = -0.0001984090D;
	a8  =  0.0000027526D;
	a10 = -0.0000000239D;
    VAR
	xsqr: LONGREAL;
	neg: BOOLEAN;
  BEGIN
	neg := FALSE;
	IF x < 0.0D THEN
		neg := TRUE;
		x := -x;
	END;
	x := FIF(x, 1.0D / LONG(twicepi), (* dummy *) xsqr) * LONG(twicepi);
	IF x >= LONG(pi) THEN
		neg := NOT neg;
		x := x - LONG(pi);
	END;
	IF x > LONG(halfpi) THEN
		x := LONG(pi) - x;
	END;
	xsqr := x * x;
	x := x * (1.0D + xsqr*(a2+xsqr*(a4+xsqr*(a6+xsqr*(a8+xsqr*a10)))));
	IF neg THEN RETURN -x; END;
	RETURN x;
  END longsin;

  PROCEDURE cos(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longcos(LONG(x)));
  END cos;

  PROCEDURE longcos(x: LONGREAL): LONGREAL;
    CONST
	a2  = -0.4999999963D;
	a4  =  0.0416666418D;
	a6  = -0.0013888397D;
	a8  =  0.0000247609D;
	a10 = -0.0000002605D;
    VAR
	xsqr: LONGREAL;
	neg: BOOLEAN;
  BEGIN
	neg := FALSE;
	IF x < 0.0D THEN x := -x; END;
	x := FIF(x, 1.0D / LONG(twicepi), (* dummy *) xsqr) * LONG(twicepi);
	IF x >= LONG(pi) THEN
		x := LONG(twicepi) - x;
	END;
	IF x > LONG(halfpi) THEN
		neg := NOT neg;
		x := LONG(pi) - x;
	END;
	xsqr := x * x;
	x := 1.0D + xsqr*(a2+xsqr*(a4+xsqr*(a6+xsqr*(a8+xsqr*a10))));
	IF neg THEN RETURN -x; END;
	RETURN x;
  END longcos;

  PROCEDURE tan(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longtan(LONG(x)));
  END tan;

  PROCEDURE longtan(x: LONGREAL): LONGREAL;
    VAR cosinus: LONGREAL;
  BEGIN
	cosinus := longcos(x);
	IF cosinus = 0.0D THEN
		Message("tan: result does not exist");
		HALT
	END;
	RETURN longsin(x)/cosinus;
  END longtan;

  PROCEDURE arcsin(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarcsin(LONG(x)));
  END arcsin;

  PROCEDURE longarcsin(x: LONGREAL): LONGREAL;
    CONST
	a0 =  1.5707963050D;
	a1 = -0.2145988016D;
	a2 =  0.0889789874D;
	a3 = -0.0501743046D;
	a4 =  0.0308918810D;
	a5 = -0.0170881256D;
	a6 =  0.0066700901D;
	a7 = -0.0012624911D;
  BEGIN
	IF x < 0.0D THEN x := -x; END;
	IF x > 1.0D THEN
		Message("arcsin: argument > 1");
		HALT
	END;
	RETURN LONG(halfpi) -
		longsqrt(1.0D - x)*(a0+x*(a1+x*(a2+x*(a3+x*(a4+x*(a5+x*(a6+x*a7)))))));
  END longarcsin;

  PROCEDURE arccos(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarccos(LONG(x)));
  END arccos;

  PROCEDURE longarccos(x: LONGREAL): LONGREAL;
  BEGIN
	RETURN LONG(halfpi) - longarcsin(x);
  END longarccos;

  PROCEDURE arctan(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarctan(LONG(x)));
  END arctan;

  PROCEDURE longarctan(x: LONGREAL): LONGREAL;
    CONST
	a2  = -0.3333314528D;
	a4  =  0.1999355085D;
	a6  = -0.1420889944D;
	a8  =  0.1065626393D;
	a10 = -0.0752896400D;
	a12 =  0.0429096318D;
	a14 = -0.0161657367D;
	a16 =  0.0028662257D;
    VAR
	xsqr: LONGREAL;
	rev: BOOLEAN;
	neg: BOOLEAN;
  BEGIN
	rev := FALSE;
	neg := FALSE;
	IF x < 0.0D THEN
		neg := TRUE;
		x := -x;
	END;
	IF x > 1.0D THEN
		rev := TRUE;
		x := 1.0D / x;
	END;
	xsqr := x * x;
	x := x * (1.0D + xsqr*(a2+xsqr*(a4+xsqr*(a6+xsqr*(a8+xsqr*(a10+xsqr*(a12+xsqr*(a14+xsqr*a16))))))));
	IF rev THEN
		x := LONG(quartpi) - x;
	END;
	IF neg THEN RETURN -x; END;
	RETURN x;
  END longarctan;

  (* hyperbolic functions *)

  PROCEDURE sinh(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longsinh(LONG(x)));
  END sinh;

  PROCEDURE longsinh(x: LONGREAL): LONGREAL;
    VAR expx: LONGREAL;
  BEGIN
	expx := longexp(x);
	RETURN (expx - 1.0D/expx)/2.0D;
  END longsinh;

  PROCEDURE cosh(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longcosh(LONG(x)));
  END cosh;

  PROCEDURE longcosh(x: LONGREAL): LONGREAL;
    VAR expx: LONGREAL;
  BEGIN
	expx := longexp(x);
	RETURN (expx + 1.0D/expx)/2.0D;
  END longcosh;

  PROCEDURE tanh(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longtanh(LONG(x)));
  END tanh;

  PROCEDURE longtanh(x: LONGREAL): LONGREAL;
    VAR expx: LONGREAL;
  BEGIN
	expx := longexp(x);
	RETURN (expx - 1.0D/expx) / (expx + 1.0D/expx);
  END longtanh;

  PROCEDURE arcsinh(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarcsinh(LONG(x)));
  END arcsinh;

  PROCEDURE longarcsinh(x: LONGREAL): LONGREAL;
    VAR neg: BOOLEAN;
  BEGIN
	neg := FALSE;
	IF x < 0.0D THEN
		neg := TRUE;
		x := -x;
	END;
	x := longln(x + longsqrt(x*x+1.0D));
	IF neg THEN RETURN -x; END;
	RETURN x;
  END longarcsinh;

  PROCEDURE arccosh(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarccosh(LONG(x)));
  END arccosh;

  PROCEDURE longarccosh(x: LONGREAL): LONGREAL;
  BEGIN
	IF x < 1.0D THEN
		Message("arccosh: argument < 1");
		HALT
	END;
	RETURN longln(x + longsqrt(x*x - 1.0D));
  END longarccosh;

  PROCEDURE arctanh(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarctanh(LONG(x)));
  END arctanh;

  PROCEDURE longarctanh(x: LONGREAL): LONGREAL;
  BEGIN
	IF (x <= -1.0D) OR (x >= 1.0D) THEN
		Message("arctanh: ABS(argument) >= 1");
		HALT
	END;
	RETURN longln((1.0D + x)/(1.0D - x)) / 2.0D;
  END longarctanh;

  (* conversions *)

  PROCEDURE RadianToDegree(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longRadianToDegree(LONG(x)));
  END RadianToDegree;

  PROCEDURE longRadianToDegree(x: LONGREAL): LONGREAL;
  BEGIN
	RETURN x * OneRadianInDegrees;
  END longRadianToDegree;

  PROCEDURE DegreeToRadian(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longDegreeToRadian(LONG(x)));
  END DegreeToRadian;

  PROCEDURE longDegreeToRadian(x: LONGREAL): LONGREAL;
  BEGIN
	RETURN x * OneDegreeInRadians;
  END longDegreeToRadian;

END Mathlib.
