(*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*)

(*$R-*)
IMPLEMENTATION MODULE Mathlib;
(*
  Module:	Mathematical functions
  Author:	Ceriel J.H. Jacobs
  Version:	$Header$
*)

  FROM EM IMPORT FIF, FEF;
  FROM Traps IMPORT Message;

  CONST
	OneRadianInDegrees	= 57.295779513082320876798155D;
	OneDegreeInRadians	=  0.017453292519943295769237D;
	Sqrt2			= 1.41421356237309504880168872420969808D;
	OneOverSqrt2		= 0.70710678118654752440084436210484904D;

  (* basic functions *)

  PROCEDURE pow(x: REAL; i: INTEGER): REAL;
  BEGIN
	RETURN SHORT(longpow(LONG(x), i));
  END pow;

  PROCEDURE longpow(x: LONGREAL; i: INTEGER): LONGREAL;
    VAR	val: LONGREAL;
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
	FOR i := 0 TO 5 DO
		temp := 0.5D*(temp + x/temp);
	END;
	RETURN temp;
  END longsqrt;

  PROCEDURE exp(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longexp(LONG(x)));
  END exp;

  PROCEDURE longexp(x: LONGREAL): LONGREAL;
  (*      2**x = (Q(x*x)+x*P(x*x))/(Q(x*x)-x*P(x*x)) for x in [0,0.5] *)
  (*      Hart & Cheney #1069 *)
    CONST
	p0 = 0.2080384346694663001443843411D+07;
	p1 = 0.3028697169744036299076048876D+05;
	p2 = 0.6061485330061080841615584556D+02;
	q0 = 0.6002720360238832528230907598D+07;
	q1 = 0.3277251518082914423057964422D+06;
	q2 = 0.1749287689093076403844945335D+04;
	q3 = 0.1000000000000000000000000000D+01;

    VAR
	neg: BOOLEAN;
	xPxx, Qxx: LONGREAL;
	n: LONGREAL;
	n1 : INTEGER;
	xsq : LONGREAL;
	large: BOOLEAN;
  BEGIN
	neg := x < 0.0D;
	IF neg THEN
		x := -x;
	END;
	x := FIF(x/longln2, 1.0D, n);
	large := x > 0.5D;
	IF large THEN x := x - 0.5D; END;
	xsq := x*x;
	xPxx := x*((p2*xsq+p1)*xsq+p0);
	Qxx := ((q3*xsq+q2)*xsq+q1)*xsq+q0;
	x := (Qxx + xPxx)/(Qxx - xPxx);
	IF large THEN
		x := x * Sqrt2;
	END;
	n1 := TRUNCD(n + 0.5D);
	WHILE n1 >= 16 DO
		x := x * 65536.0D;
		n1 := n1 - 16;
	END;
	WHILE n1 > 0 DO
		x := x * 2.0D;
		DEC(n1);
	END;
	IF neg THEN RETURN 1.0D/x; END;
	RETURN x;
  END longexp;

  PROCEDURE ln(x: REAL): REAL;	(* natural log *)
  BEGIN
	RETURN SHORT(longln(LONG(x)));
  END ln;

  PROCEDURE longln(x: LONGREAL): LONGREAL;	(* natural log *)
  (* log(x) = z*P(z*z)/Q(z*z), z = (x-1)/(x+1), x in [1/sqrt(2), sqrt(2)]
     Hart & Cheney #2707
  *)
    CONST
	p0 =  0.7504094990777122217455611007D+02;
	p1 = -0.1345669115050430235318253537D+03;
	p2 =  0.7413719213248602512779336470D+02;
	p3 = -0.1277249755012330819984385000D+02;
	p4 =  0.3327108381087686938144000000D+00;
	q0 =  0.3752047495388561108727775374D+02;
	q1 = -0.7979028073715004879439951583D+02;
	q2 =  0.5616126132118257292058560360D+02;
	q3 = -0.1450868091858082685362325000D+02;
	q4 =  0.1000000000000000000000000000D+01;
    VAR
	exp: INTEGER;
	z, zsq: LONGREAL;

  BEGIN
	IF x <= 0.0D THEN
		Message("ln: argument <= 0");
		HALT
	END;
	x := FEF(x, exp);
	WHILE x < OneOverSqrt2 DO
		x := x + x;
		DEC(exp);
	END;
	z := (x - 1.0D) / (x + 1.0D);
	zsq := z*z;
	RETURN z * ((((p4*zsq+p3)*zsq+p2)*zsq+p1)*zsq+p0) /
		   ((((q4*zsq+q3)*zsq+q2)*zsq+q1)*zsq+q0) +
		FLOATD(exp) * longln2;
  END longln;

  PROCEDURE log(x: REAL): REAL;	(* log with base 10 *)
  BEGIN
	RETURN SHORT(longlog(LONG(x)));
  END log;

  PROCEDURE longlog(x: LONGREAL): LONGREAL;	(* log with base 10 *)
  BEGIN
	RETURN longln(x)/longln10;
  END longlog;

  (* trigonometric functions; arguments in radians *)

  PROCEDURE sin(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longsin(LONG(x)));
  END sin;

  PROCEDURE sinus(x: LONGREAL; quadrant: INTEGER) : LONGREAL;
  (* sin(0.5*pi*x) = x * P(x*x)/Q(x*x) for x in [0,1]
     Hart & Cheney # 3374
  *)
    CONST
	p0 =  0.4857791909822798473837058825D+10;
	p1 = -0.1808816670894030772075877725D+10;
	p2 =  0.1724314784722489597789244188D+09;
	p3 = -0.6351331748520454245913645971D+07;
	p4 =  0.1002087631419532326179108883D+06;
	p5 = -0.5830988897678192576148973679D+03;
	q0 =  0.3092566379840468199410228418D+10;
	q1 =  0.1202384907680254190870913060D+09;
	q2 =  0.2321427631602460953669856368D+07;
	q3 =  0.2848331644063908832127222835D+05;
	q4 =  0.2287602116741682420054505174D+03;
	q5 =  0.1000000000000000000000000000D+01;
	A1 =  6.2822265625D;
	A2 =  0.00095874467958647692528676655900576D;
    VAR
	xsq, x1, x2, n : LONGREAL;
	t : INTEGER;
  BEGIN
	IF x < 0.0D THEN
		INC(quadrant, 2);
		x := -x;
	END;
	IF longhalfpi - x = longhalfpi THEN
		CASE quadrant OF
		| 0,2:
			RETURN 0.0D;
		| 1:
			RETURN 1.0D;
		| 3:
			RETURN -1.0D;
		END;
	END;
	IF x >= longtwicepi THEN
		IF x <= FLOATD(MAX(LONGINT)) THEN
		(*      Use extended precision to calculate reduced argument.
                        Split 2pi in 2 parts a1 and a2, of which the first only
                        uses some bits of the mantissa, so that n * a1 is
                        exactly representable, where n is the integer part of
                        x/pi.
                        Here we used 12 bits of the mantissa for a1.
	                Also split x in integer part x1 and fraction part x2.
	                We then compute x-n*2pi as ((x1 - n*a1) + x2) - n*a2.
		*)
			n := FLOATD(TRUNCD(x/longtwicepi));
			x1 := FLOATD(TRUNCD(x));
			x2 := x - x1;
			x := ((x1 - n * A1) + x2) - n * A2;
		ELSE
			x := FIF(x/longtwicepi, 1.0D, x1) * longtwicepi;
		END
	END;
	x := x / longhalfpi;
	t := TRUNC(x);
	x := x - FLOATD(t);
	quadrant := (quadrant + t MOD 4) MOD 4;
	IF ODD(quadrant) THEN
		x := 1.0D - x;
	END;
	IF quadrant > 1 THEN
		x := -x;
	END;
	xsq := x * x;
	RETURN x * (((((p5*xsq+p4)*xsq+p3)*xsq+p2)*xsq+p1)*xsq+p0) /
		   (((((q5*xsq+q4)*xsq+q3)*xsq+q2)*xsq+q1)*xsq+q0);
  END sinus;
		
  PROCEDURE longsin(x: LONGREAL): LONGREAL;
  BEGIN
	RETURN sinus(x, 0);
  END longsin;

  PROCEDURE cos(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longcos(LONG(x)));
  END cos;

  PROCEDURE longcos(x: LONGREAL): LONGREAL;
  BEGIN
	IF x < 0.0D THEN x := -x; END;
	RETURN sinus(x, 1);	
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

  PROCEDURE arcsincos(x: LONGREAL; cosfl: BOOLEAN): LONGREAL;
  VAR
	negative : BOOLEAN;
  BEGIN
	negative := x <= 0.0D;
	IF negative THEN x := -x; END;
	IF x > 1.0D THEN
		Message("arcsin or arccos: argument > 1");
		HALT
	END;
	IF x = 1.0D THEN
		x := longhalfpi;
	ELSE
		x := longarctan(x/longsqrt(1.0D - x*x));
	END;
	IF negative THEN x := -x; END;
	IF cosfl THEN
		RETURN longhalfpi - x;
	END;
	RETURN x;
  END arcsincos;	

  PROCEDURE longarcsin(x: LONGREAL): LONGREAL;
  BEGIN
	RETURN arcsincos(x, FALSE);
  END longarcsin;

  PROCEDURE arccos(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarccos(LONG(x)));
  END arccos;

  PROCEDURE longarccos(x: LONGREAL): LONGREAL;
  BEGIN
	RETURN arcsincos(x, TRUE);
  END longarccos;

  PROCEDURE arctan(x: REAL): REAL;
  BEGIN
	RETURN SHORT(longarctan(LONG(x)));
  END arctan;

  TYPE
	precomputed = RECORD
			X:	LONGREAL;	(* partition point *)
			arctan:	LONGREAL;	(* arctan of evaluation node *)
			OneOverXn: LONGREAL;	(* 1/xn *)
			OneOverXnSquarePlusone: LONGREAL;	(* ... *)
		      END;

  VAR	arctaninit: BOOLEAN;
	precomp : ARRAY[0..4] OF precomputed;

  PROCEDURE longarctan(x: LONGREAL): LONGREAL;
  (*      The interval [0, infinity) is treated as follows:
	  Define partition points Xi
	          X0 = 0
	          X1 = tan(pi/16)
		  X2 = tan(3pi/16)
		  X3 = tan(5pi/16)
		  X4 = tan(7pi/16)
		  X5 = infinity
          and evaluation nodes xi
	          x2 = tan(2pi/16)
	          x3 = tan(4pi/16)
		  x4 = tan(6pi/16)
		  x5 = infinity
          An argument x in [Xn-1, Xn] is now reduced to an argument
          t in [-X1, X1] by the following formulas:

                   t = 1/xn - (1/(xn*xn) + 1)/((1/xn) + x)

                   arctan(x) = arctan(xi) + arctan(t)

          For the interval [0, tan(p/16)] an approximation is used:
                   arctan(x) = x * P(x*x)/Q(x*x)
  *)
  (* Hart & Cheney # 5037 *)
    CONST
	p0 = 0.7698297257888171026986294745D+03;
	p1 = 0.1557282793158363491416585283D+04;
	p2 = 0.1033384651675161628243434662D+04;
	p3 = 0.2485841954911840502660889866D+03;
	p4 = 0.1566564964979791769948970100D+02;
	q0 = 0.7698297257888171026986294911D+03;
	q1 = 0.1813892701754635858982709369D+04;
	q2 = 0.1484049607102276827437401170D+04;
	q3 = 0.4904645326203706217748848797D+03;
	q4 = 0.5593479839280348664778328000D+02;
	q5 = 0.1000000000000000000000000000D+01;
    VAR
	xsqr: LONGREAL;
	neg: BOOLEAN;
	i: INTEGER;
  BEGIN
	IF NOT arctaninit THEN
		arctaninit := TRUE;
		WITH precomp[0] DO
			X := 0.19891236737965800691159762264467622D;
			arctan := 0.0D;
			OneOverXn := 0.0D;
			OneOverXnSquarePlusone := 0.0D;
		END;
		WITH precomp[1] DO
			X := 0.66817863791929891999775768652308076D;
			arctan := 0.39269908169872415480783042290993786D;
			OneOverXn := 2.41421356237309504880168872420969808D;
			OneOverXnSquarePlusone := 6.82842712474619009760337744841939616D;
		END;
		WITH precomp[2] DO
			X := 1.49660576266548901760113513494247691D;
			arctan := longquartpi;
			OneOverXn := 1.0;
			OneOverXnSquarePlusone := 2.0;
		END;
		WITH precomp[3] DO
			X := 5.02733949212584810451497507106407238D;
			arctan := 1.17809724509617246442349126872981358D;
			OneOverXn := 0.41421356237309504880168872420969808D;
			OneOverXnSquarePlusone := 1.17157287525380998659662255158060384D;
		END;
		WITH precomp[4] DO
			X := 0.0D;
			arctan := longhalfpi;
			OneOverXn := 0.0D;
			OneOverXnSquarePlusone := 1.0D;
		END;
	END;
	neg := FALSE;
	IF x < 0.0D THEN
		neg := TRUE;
		x := -x;
	END;
	i := 0;
	WHILE (i <= 3) AND (x >= precomp[i].X) DO
		INC(i);
	END;
	IF (i # 0) THEN 
	    WITH precomp[i] DO
		x := arctan + longarctan(OneOverXn-OneOverXnSquarePlusone/(OneOverXn+x));
	    END
	ELSE
		xsqr := x * x;
		x := x *  ((((p4*xsqr+p3)*xsqr+p2)*xsqr+p1)*xsqr+p0) /
			(((((q5*xsqr+q4)*xsqr+q3)*xsqr+q2)*xsqr+q1)*xsqr+q0);
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

BEGIN
	arctaninit := FALSE;
END Mathlib.
