.bp
.AP "AN EXAMPLE PROGRAM"
.PP
.na
.ta 4n 8n 12n 16n 20n
.nf
 1	program example(output);
 2	{This program just demonstrates typical EM code.}
 3	type rec = record r1: integer; r2:real; r3: boolean end;
 4	var mi: integer;  mx:real;  r:rec;
 5
 6	function sum(a,b:integer):integer;
 7	begin
 8		sum := a + b
 9	end;
10
11	procedure test(var r: rec);
12	label 1;
13	var i,j: integer;
14		x,y: real;
15		b: boolean;
16		c: char;
17		a: array[1..100] of integer;
18
19	begin
20		j := 1;
21		i := 3 * j + 6;
22		x := 4.8;
23		y := x/0.5;
24		b := true;
25		c := 'z';
26		for i:= 1 to 100 do a[i] := i * i;
27		r.r1 := j+27;
28		r.r3 := b;
29		r.r2 := x+y;
30		i := sum(r.r1, a[j]);
31		while i > 0 do begin j := j + r.r1; i := i - 1 end;
32		with r do begin r3 := b;  r2 := x+y;  r1 := 0 end;
33		goto 1;
34	1:	writeln(j, i:6, x:9:3, b)
35	end; {test}
36	begin {main program}
37		mx := 15.96;
38		mi := 99;
39		test(r)
40	end.
.fi
.ad
.bp
The EM code as produced by the Pascal-VU compiler is given below. Comments
have been added manually.  Note that this code has already been  optimized.
.LP
.na
.nf
.ta 1n 24n
	mes 2,2,2	; wordsize 2, pointersize 2
\&.1
	rom 't.p\e000'	; the name of the source file
	hol 552,\-32768,0	; externals and buf occupy 552 bytes
	exp $sum	; sum can be called from other modules
	pro $sum,2	; procedure sum	; 2 bytes local storage
	lin 8	; code from source line 8
	ldl 0	; load two locals ( a and b )
	adi 2	; add them
	ret 2	; return the result
	end 2	; end of procedure ( still two bytes local storage )
\&.2
	rom 1,99,2	; descriptor of array a[]
	exp $test	; the compiler exports all level 0 procedures
	pro $test,226	; procedure test, 226 bytes local storage
\&.3
	rom 4.8F8	; assemble Floating point 4.8 (8 bytes) in
\&.4		; global storage
	rom 0.5F8	; same for 0.5
	mes 3,\-226,2,2	; compiler temporary not referenced by address
	mes 3,\-24,2,0	; the same is true for i, j, b and c in test
	mes 3,\-22,2,0
	mes 3,\-4,2,0
	mes 3,\-2,2,0
	mes 3,\-20,8,0	; and for x and y
	mes 3,\-12,8,0
	lin 20	; maintain source line number
	loc 1
	stl \-4	; j := 1
	lni	; lin 21 prior to optimization
	lol \-4
	loc 3
	mli 2
	loc 6
	adi 2
	stl \-2	; i := 3 * j + 6
	lni	; lin 22 prior to optimization
	lae .3
	loi 8
	lal \-12
	sti 8	; x := 4.8
	lni	; lin 23 prior to optimization
	lal \-12
	loi 8
	lae .4
	loi 8
	dvf 8
	lal \-20
	sti 8	; y := x / 0.5
	lni	; lin 24 prior to optimization
	loc 1
	stl \-22	; b := true
	lni	; lin 25 prior to optimization
	loc 122
	stl \-24	; c := 'z'
	lni	; lin 26 prior to optimization
	loc 1
	stl \-2	; for i:= 1
2
	lol \-2
	dup 2
	mli 2	; i*i
	lal \-224
	lol \-2
	lae .2
	sar 2	; a[i] :=
	lol \-2
	loc 100
	beq *3	; to 100 do
	inl \-2	; increment i and loop
	bra *2
3
	lin 27
	lol \-4
	loc 27
	adi 2	; j + 27
	sil 0	; r.r1 :=
	lni	; lin 28 prior to optimization
	lol \-22	; b
	lol 0
	stf 10	; r.r3 :=
	lni	; lin 29 prior to optimization
	lal \-20
	loi 16
	adf 8	; x + y
	lol 0
	adp 2
	sti 8	; r.r2 :=
	lni	; lin 30 prior to optimization
	lal \-224
	lol \-4
	lae .2
	lar 2	; a[j]
	lil 0	; r.r1
	cal $sum	; call now
	asp 4	; remove parameters from stack
	lfr 2	; get function result
	stl \-2	; i :=
4
	lin 31
	lol \-2
	zle *5	; while i > 0 do
	lol \-4
	lil 0
	adi 2
	stl \-4	; j := j + r.r1
	del \-2	; i := i - 1
	bra *4	; loop
5
	lin 32
	lol 0
	stl \-226	; make copy of address of r
	lol \-22
	lol \-226
	stf 10	; r3 := b
	lal \-20
	loi 16
	adf 8
	lol \-226
	adp 2
	sti 8	; r2 := x + y
	loc 0
	sil \-226	; r1 := 0
	lin 34	; note the absence of the unnecessary jump
	lae 22	; address of output structure
	lol \-4
	cal $_wri	; write integer with default width
	asp 4	; pop parameters
	lae 22
	lol \-2
	loc 6
	cal $_wsi	; write integer width 6
	asp 6
	lae 22
	lal \-12
	loi 8
	loc 9
	loc 3
	cal $_wrf	; write fixed format real, width 9, precision 3
	asp 14
	lae 22
	lol \-22
	cal $_wrb	; write boolean, default width
	asp 4
	lae 22
	cal $_wln	; writeln
	asp 2
	ret 0	; return, no result
	end 226
	exp $_main
	pro $_main,0	; main program
\&.6
	con 2,\-1,22	; description of external files
\&.5
	rom 15.96F8
	fil .1	; maintain source file name
	lae .6	; description of external files
	lae 0	; base of hol area to relocate buffer addresses
	cal $_ini	; initialize files, etc...
	asp 4
	lin 37
	lae .5
	loi 8
	lae 2
	sti 8	; mx := 15.96
	lni	; lin 38 prior to optimization
	loc 99
	ste 0	; mi := 99
	lni	; lin 39 prior to optimization
	lae 10	; address of r
	cal $test
	asp 2
	loc 0	; normal exit
	cal $_hlt	; cleanup and finish
	asp 2
	end 0
	mes 5	; reals were used
.fi
.ad
.PP
The compact code corresponding to the above program is listed below.
Read it horizontally, line by line, not column by column.
Each number represents a byte of compact code, printed in decimal.
The first two bytes form the magic word.
.LP
.Dr 33
 173   0 159 122 122 122 255 242   1 161 250 124 116  46 112   0
 255 156 245  40   2 245   0 128 120 155 249 123 115 117 109 160
 249 123 115 117 109 122  67 128  63 120   3 122  88 122 152 122
 242   2 161 121 219 122 255 155 249 124 116 101 115 116 160 249
 124 116 101 115 116 245 226   0 242   3 161 253 128 123  52  46
  56 255 242   4 161 253 128 123  48  46  53 255 159 123 245  30
 255 122 122 255 159 123  96 122 120 255 159 123  98 122 120 255
 159 123 116 122 120 255 159 123 118 122 120 255 159 123 100 128
 120 255 159 123 108 128 120 255  67 140  69 121 113 116  68  73
 116  69 123  81 122  69 126   3 122 113 118  68  57 242   3  72
 128  58 108 112 128  68  58 108  72 128  57 242   4  72 128  44
 128  58 100 112 128  68  69 121 113  98  68  69 245 122   0 113
  96  68  69 121 113 118 182  73 118  42 122  81 122  58 245  32
 255  73 118  57 242   2  94 122  73 118  69 220  10 123  54 118
  18 122 183  67 147  73 116  69 147   3 122 104 120  68  73  98
  73 120 111 130  68  58 100  72 136   2 128  73 120   4 122 112
 128  68  58 245  32 255  73 116  57 242   2  59 122  65 120  20
 249 123 115 117 109   8 124  64 122 113 118 184  67 151  73 118
 128 125  73 116  65 120   3 122 113 116  41 118  18 124 185  67
 152  73 120 113 245  30 255  73  98  73 245  30 255 111 130  58
 100  72 136   2 128  73 245  30 255   4 122 112 128  69 120 104
 245  30 255  67 154  57 142  73 116  20 249 124  95 119 114 105
   8 124  57 142  73 118  69 126  20 249 124  95 119 115 105   8
 126  57 142  58 108  72 128  69 129  69 123  20 249 124  95 119
 114 102   8 134  57 142  73  98  20 249 124  95 119 114  98   8
 124  57 142  20 249 124  95 119 108 110   8 122  88 120 152 245
 226   0 155 249 125  95 109  97 105 110 160 249 125  95 109  97
 105 110 120 242   6 151 122 119 142 255 242   5 161 253 128 125
  49  53  46  57  54 255  50 242   1  57 242   6  57 120  20 249
 124  95 105 110 105   8 124  67 157  57 242   5  72 128  57 122
 112 128  68  69 219 110 120  68  57 130  20 249 124 116 101 115
 116   8 122  69 120  20 249 124  95 104 108 116   8 122 152 120
 159 124 160 255 159 125 255
.De
