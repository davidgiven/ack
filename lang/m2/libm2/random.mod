(*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*)

(*$R-*)
IMPLEMENTATION MODULE random;
(*
  Module:       random numbers
  Author:       Ceriel J.H. Jacobs
  Version:      $Header$
*)

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

PROCEDURE StartSeed(seed: CARDINAL);
BEGIN
	X[0] := seed;
	FOR k := 1 TO 54 DO
		X[k] := X[k-1] * 1297;
	END;
	k := 54;
	j := 30;
END StartSeed;

BEGIN
 	X0 := time(NIL);
	StartSeed(CARDINAL(getpid()) * X[0]);
END random.
