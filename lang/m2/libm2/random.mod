(*$R-*)
IMPLEMENTATION MODULE random;

FROM Unix IMPORT getpid, time;
TYPE index = [0..54];

VAR	X: ARRAY index OF CARDINAL;
	k, j: index;

PROCEDURE Random(): CARDINAL;
BEGIN
	IF k+1 > 54 THEN k := 0; ELSE INC(k) END;
	IF j+1 > 54 THEN j := 0; ELSE INC(j) END;
	X[k] := X[k] + X[j];
	RETURN X[k]
END Random;

PROCEDURE Uniform (lwb, upb: CARDINAL): CARDINAL;
BEGIN
    	IF upb <= lwb THEN RETURN lwb; END;
    	RETURN lwb + (Random() MOD (upb - lwb + 1));
END Uniform;

BEGIN
	X[0] := time(NIL);
	X[0] := CARDINAL(getpid()) * X[0];
	FOR k := 1 TO 54 DO
		X[k] := X[k-1] * 1297;
	END;
	k := 54;
	j := 30;
END random.
