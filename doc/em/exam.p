  program example(output);
  {This program just demonstrates typical EM code.}
  type rec = record r1: integer; r2:real; r3: boolean end;
  var mi: integer;  mx:real;  r:rec;

  function sum(a,b:integer):integer;
  begin
    sum := a + b
  end;

  procedure test(var r: rec);
  label 1;
  var   i,j: integer;
	x,y: real;
	b: boolean;
	c: char;
	a: array[1..100] of integer;

  begin
	j := 1;
	i := 3 * j + 6;
	x := 4.8;
	y := x/0.5;
	b := true;
	c := 'z';
	for i:= 1 to 100 do a[i] := i * i;
	r.r1 := j+27;
	r.r3 := b;
	r.r2 := x+y;
	i := sum(r.r1, a[j]);
	while i > 0 do begin j := j + r.r1; i := i - 1 end;
	with r do begin r3 := b;  r2 := x+y;  r1 := 0 end;
	goto 1;
  1:    writeln(j, i:6, x:9:3, b)
  end; {test}
  begin {main program}
    mx := 15.96;
    mi := 99;
    test(r)
  end.
