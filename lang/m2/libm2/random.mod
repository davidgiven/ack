IMPLEMENTATION MODULE random;

VAR	seed: CARDINAL;

PROCEDURE Random(): CARDINAL;
BEGIN
	seed := seed * 77 + 153;
	RETURN seed;
END Random;

PROCEDURE Uniform (lwb, upb: CARDINAL): CARDINAL;
BEGIN
    	IF upb <= lwb THEN RETURN lwb; END;
    	RETURN lwb + (Random() MOD (upb - lwb + 1));
END Uniform;

BEGIN
	seed := 253B;
END random.
