MODULE PowersOf2;
  FROM InOut IMPORT Write, WriteLn, WriteString, WriteCard;

  CONST
	M = 11;		(* M ~ N*log(2) *)
	N = 32;
  VAR
	i,j,k,exp: CARDINAL;
	c,r,t:	CARDINAL;
	d: ARRAY [0..M] OF CARDINAL;
	f: ARRAY[0..N] OF CARDINAL;
  BEGIN
	d[0] := 1;
	k := 1;
	FOR exp := 1 TO N DO
		(* compute d = 2 ^ exp by d = 2*d *)
		c := 0;		(* carry *)
		FOR i := 0 TO k-1 DO
			t := 2 * d[i] + c;
			IF t >= 10 THEN
				d[i] := t - 10;
				c := 1;
			ELSE
				d[i] := t;
				c := 0;
			END
		END;
		IF c > 0 THEN
			d[k] := 1;
			k := k + 1
		END;
		(* output d[k-1] .. d[0] *)
		i := M;
		REPEAT
			i := i - 1; 
			Write(" ")
		UNTIL i = k;
		REPEAT
			i := i - 1;
			Write(CHR(d[i]+ORD("0")))
		UNTIL i = 0;
		WriteCard(exp, 4);
		(* compute and output f = 2^(-exp) by f := f DIV 2 *)
		WriteString(" 0.");
		r := 0;		(* remainder *)
		FOR j := 1 TO exp-1 DO
			r := 10 * r + f[j];
			f[j] := r DIV 2;
			r := r MOD 2;
			Write(CHR(f[j]+ORD("0")))
		END;
		f[exp] := 5;
		Write("5");
		WriteLn
	END
  END PowersOf2.

