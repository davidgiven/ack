IMPLEMENTATION MODULE MathLib0;
(*	Rewritten in Modula-2.
	The originals came from the Pascal runtime library.
*)

FROM FIFFEF	IMPORT	FIF, FEF;

CONST
	HUGE =	1.701411733192644270E38;

PROCEDURE sinus(arg: REAL; quad: INTEGER): REAL;

(*	Coefficients for sin/cos are #3370 from Hart & Cheney (18.80D).
*)
CONST
	twoopi	= 0.63661977236758134308;
	p0	= 0.1357884097877375669092680E8;
	p1	= -0.4942908100902844161158627E7;
	p2	= 0.4401030535375266501944918E6;
	p3	= -0.1384727249982452873054457E5;
	p4	= 0.1459688406665768722226959E3;
	q0	= 0.8644558652922534429915149E7;
	q1	= 0.4081792252343299749395779E6;
	q2	= 0.9463096101538208180571257E4;
	q3	= 0.1326534908786136358911494E3;
VAR
	e, f: REAL;
	ysq: REAL;
	x,y: REAL;
	k: INTEGER;
	temp1, temp2: REAL;
BEGIN
	x := arg;
	IF x < 0.0 THEN
		x := -x;
		quad := quad + 2;
	END;
	x := x*twoopi;	(*underflow?*)
	IF x>32764.0 THEN
		y := FIF(x, 10.0, e);
		e := e + FLOAT(quad);
		temp1 := FIF(0.25, e, f);
		quad := TRUNC(e - 4.0*f);
	ELSE
		k := TRUNC(x);
		y := x - FLOAT(k);
		quad := (quad + k) MOD 4;
	END;
	IF ODD(quad) THEN
		y := 1.0-y;
	END;
	IF quad > 1 THEN
		y := -y;
	END;

	ysq := y*y;
	temp1 := ((((p4*ysq+p3)*ysq+p2)*ysq+p1)*ysq+p0)*y;
	temp2 := ((((ysq+q3)*ysq+q2)*ysq+q1)*ysq+q0);
	RETURN temp1/temp2;
END sinus;

PROCEDURE cos(arg: REAL): REAL;
BEGIN
	IF arg < 0.0 THEN
		arg := -arg;
	END;
	RETURN sinus(arg, 1);
END cos;

PROCEDURE sin(arg: REAL): REAL;
BEGIN
	RETURN sinus(arg, 0);
END sin;

(*
	floating-point arctangent

	arctan returns the value of the arctangent of its
	argument in the range [-pi/2,pi/2].

	coefficients are #5077 from Hart & Cheney. (19.56D)
*)

CONST
	sq2p1	= 2.414213562373095048802E0;
	sq2m1	= 0.414213562373095048802E0;
	pio2	= 1.570796326794896619231E0;
	pio4	= 0.785398163397448309615E0;
	p4	= 0.161536412982230228262E2;
	p3	= 0.26842548195503973794141E3;
	p2	= 0.11530293515404850115428136E4;
	p1	= 0.178040631643319697105464587E4;
	p0	= 0.89678597403663861959987488E3;
	q4	= 0.5895697050844462222791E2;
	q3	= 0.536265374031215315104235E3;
	q2	= 0.16667838148816337184521798E4;
	q1	= 0.207933497444540981287275926E4;
	q0	= 0.89678597403663861962481162E3;

(*
	xatan evaluates a series valid in the
	range [-0.414...,+0.414...].
*)

PROCEDURE xatan(arg: REAL) : REAL;
VAR
	argsq, value: REAL;
BEGIN
	argsq := arg*arg;
	value := ((((p4*argsq + p3)*argsq + p2)*argsq + p1)*argsq + p0);
	value := value/(((((argsq + q4)*argsq + q3)*argsq + q2)*argsq + q1)*argsq + q0);
	RETURN value*arg;
END xatan;

PROCEDURE satan(arg: REAL): REAL;
BEGIN
	IF arg < sq2m1 THEN
		RETURN xatan(arg);
	ELSIF arg > sq2p1 THEN
		RETURN pio2 - xatan(1.0/arg);
	ELSE
		RETURN pio4 + xatan((arg-1.0)/(arg+1.0));
	END;
END satan;

(*
	atan makes its argument positive and
	calls the inner routine satan.
*)

PROCEDURE arctan(arg: REAL): REAL;
BEGIN
	IF arg>0.0 THEN
		RETURN satan(arg);
	ELSE
		RETURN -satan(-arg);
	END;
END arctan;

(*
	sqrt returns the square root of its floating
	point argument. Newton's method.
*)

PROCEDURE sqrt(arg: REAL): REAL;
VAR
	x, temp: REAL;
	exp, i: INTEGER;
BEGIN
	IF arg <= 0.0 THEN
		IF arg < 0.0 THEN
			(* ??? *)
			;
		END;
		RETURN 0.0;
	END;
	x := FEF(arg,exp);
	(*
	 * NOTE
	 * this wont work on 1's comp
	 *)
	IF ODD(exp) THEN
		x := 2.0 * x;
		DEC(exp);
	END;
	temp := 0.5*(1.0 + x);

	WHILE exp > 28 DO
		temp := temp * 16384.0;
		exp := exp - 28;
	END;
	WHILE exp < -28 DO
		temp := temp / 16384.0;
		exp := exp + 28;
	END;
	WHILE exp >= 2 DO
		temp := temp * 2.0;
		exp := exp - 2;
	END;
	WHILE exp <= -2 DO
		temp := temp / 2.0;
		exp := exp + 2;
	END;
	FOR i := 0 TO 4 DO
		temp := 0.5*(temp + arg/temp);
	END;
	RETURN temp;
END sqrt;

(*
	ln returns the natural logarithm of its floating
	point argument.

	The coefficients are #2705 from Hart & Cheney. (19.38D)
*)
PROCEDURE ln(arg: REAL): REAL;
CONST
	log2	= 0.693147180559945309E0;
	sqrto2	= 0.707106781186547524E0;
	p0	= -0.240139179559210510E2;
	p1	= 0.309572928215376501E2;
	p2	= -0.963769093368686593E1;
	p3	= 0.421087371217979714E0;
	q0	= -0.120069589779605255E2;
	q1	= 0.194809660700889731E2;
	q2	= -0.891110902798312337E1;
VAR
	x,z, zsq, temp: REAL;
	exp: INTEGER;
BEGIN
	IF arg <= 0.0 THEN
		(* ??? *)
		RETURN -HUGE;
	END;
	x := FEF(arg,exp);
	IF x<sqrto2 THEN
		x := x + x;
		DEC(exp);
	END;

	z := (x-1.0)/(x+1.0);
	zsq := z*z;

	temp := ((p3*zsq + p2)*zsq + p1)*zsq + p0;
	temp := temp/(((zsq + q2)*zsq + q1)*zsq + q0);
	temp := temp*z + FLOAT(exp)*log2;
	RETURN temp;
END ln;

(*
	exp returns the exponential function of its
	floating-point argument.

	The coefficients are #1069 from Hart and Cheney. (22.35D)
*)

PROCEDURE floor(d: REAL): REAL;
BEGIN
	IF d < 0.0 THEN
		d := -d;
		IF FIF(d, 1.0, d) # 0.0 THEN
			d := d + 1.0;
		END;
		d := -d;
	ELSE
		IF FIF(d, 1.0, d) # 0.0 THEN
			(* just ignore result of FIF *)
			;
		END;
	END;
	RETURN d;
END floor;

PROCEDURE ldexp(fr: REAL; exp: INTEGER): REAL;
VAR
	neg,i: INTEGER;
BEGIN
	neg := 1;
	IF fr < 0.0 THEN
		fr := -fr;
		neg := -1;
	END;
	fr := FEF(fr, i);
	exp := exp + i;
	IF exp > 127 THEN
		(* Too large. ??? *)
		RETURN FLOAT(neg) * HUGE;
	END;
	IF exp < -127 THEN
		RETURN 0.0;
	END;
	WHILE exp > 14 DO
		fr := fr * 16384.0;
		exp := exp - 14;
	END;
	WHILE exp < -14 DO
		fr := fr / 16384.0;
		exp := exp + 14;
	END;
	WHILE exp > 0 DO
		fr := fr + fr;
		DEC(exp);
	END;
	WHILE exp < 0 DO
		fr := fr / 2.0;
		INC(exp);
	END;
	RETURN FLOAT(neg) * fr;
END ldexp;

PROCEDURE exp(arg: REAL): REAL;
CONST
	p0	= 0.2080384346694663001443843411E7;
	p1	= 0.3028697169744036299076048876E5;
	p2	= 0.6061485330061080841615584556E2;
	q0	= 0.6002720360238832528230907598E7;
	q1	= 0.3277251518082914423057964422E6;
	q2	= 0.1749287689093076403844945335E4;
	log2e	= 1.4426950408889634073599247;
	sqrt2	= 1.4142135623730950488016887;
	maxf	= 10000.0;
VAR
	fract: REAL;
	temp1, temp2, xsq: REAL;
	ent: INTEGER;
BEGIN
	IF arg = 0.0 THEN
		RETURN 1.0;
	END;
	IF arg < -maxf THEN
		RETURN 0.0;
	END;
	IF arg > maxf THEN
		(* result too large ??? *)
		RETURN HUGE;
	END;
	arg := arg * log2e;
	ent := TRUNC(floor(arg));
	fract := (arg-FLOAT(ent)) - 0.5;
	xsq := fract*fract;
	temp1 := ((p2*xsq+p1)*xsq+p0)*fract;
	temp2 := ((xsq+q2)*xsq+q1)*xsq + q0;
	RETURN ldexp(sqrt2*(temp2+temp1)/(temp2-temp1), ent);
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
