#
{
  (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 
           This product is part of the Amsterdam Compiler Kit.
 
  Permission to use, sell, duplicate or disclose this software must be
  obtained in writing. Requests for such permissions may be sent to
 
       Dr. Andrew S. Tanenbaum
       Wiskundig Seminarium
       Vrije Universiteit
       Postbox 7161
       1007 MC Amsterdam
       The Netherlands
 
}
{$i64 : sets of integers contain 64 bits}
program t3(input,output,f1,f2,f3,f4,f5,f6);

{ The Berkeley and EM-1 compilers both can handle this program }

const rcsversion='$Id$';
type wavelength = (red,blue,yellow,q1,q2,q3,q4,q5,q6,q7,q8,q9,q10,q11,
                   pink,green,orange);
  spectrum= set of wavelength;
  bit = 0..1;

#ifndef NOFLOAT
  tp3=  packed record c1:char; i:integer; p:boolean; x:real end;
  tp4=  record c1:char; i:integer; p:boolean; x:real end;
#else
  tp3=  packed record c1:char; i:integer; p:boolean; end;
  tp4=  record c1:char; i:integer; p:boolean; end;
#endif

  vec1 =  array [-10..+10] of integer;

#ifndef NOFLOAT
  vrec = record case t:boolean of false:(r:real); true:(b:bit) end;
#else
  vrec = record case t:boolean of false:(); true:(b:bit) end;
#endif

var t,pct,ect:integer;
 i,j,k,l:integer;

#ifndef NOFLOAT
 x,y: real;
#endif

 p:boolean;
 c2:char;
 a1: vec1;
 c: array [1..20] of char;
 r3: tp3;
 r4: tp4;
 vr: vrec;
 colors: spectrum;
 letters,cset:set of char;
 f1: text;
 f2: file of spectrum;
 f3: file of tp3;
 f4: file of tp4;
 f5: file of vec1;
 f6: file of vrec;



procedure e(n:integer); 
begin 
  ect := ect + 1;
  writeln(' Error', n:3,' in test ', t) 
end;








{************************************************************************}
procedure tst34;
{ Global files }
var i:integer; c1:char;
begin t:=34; pct := pct + 1;
  rewrite(f1);
  if not eof(f1) then e(1);
  write(f1,'abc',20+7:2,'a':2); writeln(f1);
  write(f1,'xyz');
  i:=-3000;  write(f1,i:5);
  reset(f1);
  if eof(f1) or eoln(f1) then e(2);
  for i:=1 to 17 do read(f1,c[i]);
  if(c[1]<>'a') or (c[3]<>'c') or (c[5]<>'7') or (c[8]<>' ') or
     (c[12]<>'-') or (c[13]<>'3') or (c[16]<>'0') then e(3);
  if not eof(f1) then e(4);
  rewrite(f1);
  for i:= 32 to 127 do write(f1,chr(i));
  reset(f1);  p:= false;
  for i:= 32 to 127 do begin read(f1,c1); if ord(c1) <> i then p:=true end;
  if p then e(5);
  rewrite(f1);
  for c1 := 'a' to 'z' do write(f1,c1);
  reset(f1);  p:= false;
  for c1 := 'a' to 'z' do begin read(f1,c2); if c2 <> c1 then p:=true end;
  if p then e(6);
end;

procedure tst36;
var i,j:integer;
begin t:=36; pct:=pct+1;
  rewrite(f2); rewrite(f3); rewrite(f4); rewrite(f5); rewrite(f6);
  colors := []; f2^ := colors; put(f2);
  colors := [red]; f2^ := colors; put(f2);
  colors := [red,blue]; f2^ := colors; put(f2);
  colors := [yellow,blue]; f2^ := colors; put(f2);
  reset(f2);
  colors := f2^;  get(f2);  if colors <> [] then e(4);
  colors := f2^;  get(f2);  if colors <> [red] then e(5);
  colors := f2^;  get(f2);  if colors <> [blue,red] then e(6);
  colors := f2^;  get(f2);  if colors <> [blue,yellow] then e(7);

#ifndef NOFLOAT
  r3.c1:='w';  r3.i:= -100; r3.x:=303.56;  r3.p:=true; f3^:=r3; put(f3);
  r3.c1:='y';  r3.i:= -35;  r3.x:=26.32;   f3^:=r3; put(f3);
  r3.c1:='q';  r3.i:= +29;  r3.x:=10.00;   f3^:=r3; put(f3);
  r3.c1:='j';  r3.i:=   8;  r3.x:=10000;   f3^:=r3; put(f3);
#else
  r3.c1:='w';  r3.i:= -100; r3.p:=true; f3^:=r3; put(f3);
  r3.c1:='y';  r3.i:= -35;  f3^:=r3; put(f3);
  r3.c1:='q';  r3.i:= +29;  f3^:=r3; put(f3);
  r3.c1:='j';  r3.i:=   8;  f3^:=r3; put(f3);
#endif

  for i:= 1 to 1000 do begin f3^ := r3; put(f3) end;
  reset(f3);

#ifndef NOFLOAT
  r3 := f3^; get(f3);
  if (r3.c1<>'w') or (r3.i<>-100) or (r3.x<>303.56) then e(8);
  r3 := f3^; get(f3);
  if (r3.c1<>'y') or (r3.i<> -35) or (r3.x<> 26.32) then e(9);
  r3 := f3^; get(f3);
  if (r3.c1<>'q') or (r3.i<>  29) or (r3.x<> 10.00) then e(10);
  r3 := f3^; get(f3);
  if (r3.c1<>'j') or (r3.i<>   8) or (r3.x<> 10000) then e(11);
#else
  r3 := f3^; get(f3);
  if (r3.c1<>'w') or (r3.i<>-100) then e(8);
  r3 := f3^; get(f3);
  if (r3.c1<>'y') or (r3.i<> -35) then e(9);
  r3 := f3^; get(f3);
  if (r3.c1<>'q') or (r3.i<>  29) then e(10);
  r3 := f3^; get(f3);
  if (r3.c1<>'j') or (r3.i<>   8) then e(11);
#endif


#ifndef NOFLOAT
  r4.c1:='w';  r4.i:= -100; r4.x:=303.56;  r4.p:=true; f4^:=r4; put(f4);
  r4.c1:='y';  r4.i:= -35;  r4.x:=26.32;   f4^:=r4; put(f4);
  r4.c1:='q';  r4.i:= +29;  r4.x:=10.00;   f4^:=r4; put(f4);
  r4.c1:='j';  r4.i:=   8;  r4.x:=10000;   f4^:=r4; put(f4);
#else
  r4.c1:='w';  r4.i:= -100; r4.p:=true; f4^:=r4; put(f4);
  r4.c1:='y';  r4.i:= -35;  f4^:=r4; put(f4);
  r4.c1:='q';  r4.i:= +29;  f4^:=r4; put(f4);
  r4.c1:='j';  r4.i:=   8;  f4^:=r4; put(f4);
#endif 

  for i:= 1 to 1000 do begin f4^ := r4; put(f4) end;
  reset(f4);

#ifndef NOFLOAT
  r4 := f4^; get(f4);
  if (r4.c1<>'w') or (r4.i<>-100) or (r4.x<>303.56) then e(12);
  r4 := f4^; get(f4);
  if (r4.c1<>'y') or (r4.i<> -35) or (r4.x<> 26.32) then e(13);
  r4 := f4^; get(f4);
  if (r4.c1<>'q') or (r4.i<>  29) or (r4.x<> 10.00) then e(13);
  r4 := f4^; get(f4);
  if (r4.c1<>'j') or (r4.i<>   8) or (r4.x<> 10000) then e(14);
#else
  r4 := f4^; get(f4);
  if (r4.c1<>'w') or (r4.i<>-100) then e(12);
  r4 := f4^; get(f4);
  if (r4.c1<>'y') or (r4.i<> -35) then e(13);
  r4 := f4^; get(f4);
  if (r4.c1<>'q') or (r4.i<>  29) then e(13);
  r4 := f4^; get(f4);
  if (r4.c1<>'j') or (r4.i<>   8) then e(14);
#endif


  for j:= 1 to 100 do
    begin for i:= -10 to +10 do a1[i] := i*j; f5^ := a1; put(f5); end;
  reset(f5);
  for j:= 1 to 99 do
    begin a1:=f5^; get(f5); for i:= -10 to +10 do if a1[i]<> i*j then e(14) end;

#ifndef NOFLOAT
  vr.t:=false;
  for i:= 1 to 1000 do begin vr.r:=i+0.5;   f6^:=vr; put(f6) ;  p:=true; end;
  reset(f6);   p:=false;
  for i:= 1 to 999 do 
     begin  vr:=f6^; get(f6); if vr.r <> i+0.5 then p:=true end;
  if p then e(15);
#endif

  rewrite(f6);
  if not eof(f6) then e(16);
  for i:= 1 to 1000 do begin vr.b:=i mod 2; f6^:=vr; put(f6) end;
  reset(f6);
  if eof(f6) then e(17);
  p:=false;
  for i:= 1 to 1000 do 
     begin  vr:=f6^; get(f6); if vr.b <> i mod 2 then p:=true end;
  if not eof(f6) then e(18);
  if p then e(19);

  rewrite(f1);
  f1^:=chr(10); 
  put(f1);
  reset(f1);
  if ord(f1^) <> 32 then e(20);

#ifndef NOFLOAT
  rewrite(f1);
  x:=0.0625;  write(f1,x:6:4, x:6:2);
  reset(f1);  read(f1,y);  if y <> 0.0625 then e(21);
  reset(f1);  for i:= 1 to 12 do begin c[i]:= f1^; get(f1) end;
  if (c[1]<>'0') or (c[2]<>'.') or (c[4]<>'6') then e(22);
  if (c[7]<>' ') or (c[9]<>'0') or (c[10]<>'.') or (c[12]<>'6') then e(23);
#endif

end;

{************************************************************************}
procedure tst35;
{ Local files }
var g1: text;
    g2: file of spectrum;
    g3: file of tp4;
    g4: file of vec1;
    i,j:integer;
 begin t:=35; pct := pct + 1;
  rewrite(g1); rewrite(g2); rewrite(g3); rewrite(g4);
  if (not (eof(g1) and eof(g4))) then e(1);
  writeln(g1,'abc', 20+7:2,'a':2);
  write(g1,'xyz');
  reset(g1);
  if eof(g1) or eoln(g1) then e(2);
  read(g1,c[1]); read(g1,c[2]); read(g1,c[3],c[4],c[5],c[6],c[7]);
  if (c[1]<>'a') or (c[3]<>'c') or (c[4]<>'2') or (c[7]<>'a') then e(3);
  if not eoln(g1) then e(4)
  else readln(g1);
  for i:=1 to 2 do read(g1,c[8+i]);
  if c[10]<>'y' then e(5);
  if eof(g1) or eoln(g1) then e(6);
  colors := []; g2^ := colors; put(g2);
  colors := [pink]; g2^ := colors; put(g2);
  colors := [pink,green];  g2^ := colors;  put(g2); 
  colors := [orange,green];  g2^ := colors;  put(g2); 
  reset(g2); 
  colors := g2^;  get(g2); if colors <> [] then e(7); 
  colors := g2^; get(g2); if colors <> [pink] then e(8);
  colors := g2^; get(g2); if colors <> [green,pink] then e(9);
  colors := g2^; get(g2); if colors <> [green,orange] then e(10);

#ifndef NOFLOAT
  r4.c1:='w';  r4.i:= -100; r4.x:=303.56;  g3^:=r4; put(g3);
  r4.c1:='y';  r4.i:= -35;  r4.x:=26.32;   g3^:=r4; put(g3);
  r4.c1:='q';  r4.i:= +29;  r4.x:=10.00;   g3^:=r4; put(g3);
  r4.c1:='j';  r4.i:=   8;  r4.x:=10000;   g3^:=r4; put(g3);
#else
  r4.c1:='w';  r4.i:= -100; g3^:=r4; put(g3);
  r4.c1:='y';  r4.i:= -35;  g3^:=r4; put(g3);
  r4.c1:='q';  r4.i:= +29;  g3^:=r4; put(g3);
  r4.c1:='j';  r4.i:=   8;  g3^:=r4; put(g3);
#endif

  for i:= 1 to 1000 do begin g3^ := r4; put(g3) end;
  reset(g3);
  if eof(g3) then e(11);

#ifndef NOFLOAT
  r4 := g3^;  get(g3);
  if (r4.c1<>'w') or (r4.i<>-100) or (r4.x<>303.56) then e(12);
  r4 := g3^;  get(g3);
  if (r4.c1<>'y') or (r4.i<> -35) or (r4.x<> 26.32) then e(13);
  r4 := g3^;  get(g3);
  if (r4.c1<>'q') or (r4.i<>  29) or (r4.x<> 10.00) then e(14);
  r4 := g3^;  get(g3);
  if (r4.c1<>'j') or (r4.i<>   8) or (r4.x<> 10000) then e(15);
#else
  r4 := g3^;  get(g3);
  if (r4.c1<>'w') or (r4.i<>-100) then e(12);
  r4 := g3^;  get(g3);
  if (r4.c1<>'y') or (r4.i<> -35) then e(13);
  r4 := g3^;  get(g3);
  if (r4.c1<>'q') or (r4.i<>  29) then e(14);
  r4 := g3^;  get(g3);
  if (r4.c1<>'j') or (r4.i<>   8) then e(15);
#endif


  for j:= 1 to 100 do
    begin for i:= -10 to +10 do a1[i] := i*j; g4^ := a1; put(g4) end;
  reset(g4);
  for j:= 1 to 100 do
    begin a1:=g4^; get(g4); for i:= -10 to +10 do if a1[i]<>i*j then e(16) end;
  if not eof(g2) then e(17);
colors:=[q1,q2,q3,q4,q5,q6,q7,q8,q9,q10,q11];
end;


{***********************************************************************}
procedure tst37;
{ Intermediate level files }
var g1: text;
    g2: file of spectrum;
    g3: file of tp4;
    g4: file of vec1;

 procedure tst3701;
 var i,j:integer;
 begin t:=3701; pct := pct + 1;
  rewrite(g1); rewrite(g2); rewrite(g3); rewrite(g4);
  if (not (eof(g1) and eof(g4))) then e(1);
  writeln(g1,'abc', 20+7:2,'a':2);
  write(g1,'xyz');
  reset(g1);
  if eof(g1) or eoln(g1) then e(2);
  read(g1,c[1]); read(g1,c[2]); read(g1,c[3],c[4],c[5],c[6],c[7]);
  if (c[1]<>'a') or (c[3]<>'c') or (c[4]<>'2') or (c[7]<>'a') then e(3);
  if not eoln(g1) then e(4)
  else readln(g1);
  for i:=1 to 2 do read(g1,c[8+i]);
  if c[10]<>'y' then e(5);
  if eof(g1) or eoln(g1) then e(6);
  colors := []; g2^ := colors; put(g2);
  colors := [pink]; g2^ := colors; put(g2);
  colors := [pink,green];  g2^ := colors;  put(g2); 
  colors := [orange,green];  g2^ := colors;  put(g2); 
  reset(g2); 
  colors := g2^;  get(g2); if colors <> [] then e(7); 
  colors := g2^; get(g2); if colors <> [pink] then e(8);
  colors := g2^; get(g2); if colors <> [green,pink] then e(9);
  colors := g2^; get(g2); if colors <> [green,orange] then e(10);

#ifndef NOFLOAT
  r4.c1:='w';  r4.i:= -100; r4.x:=303.56;  g3^:=r4; put(g3);
  r4.c1:='y';  r4.i:= -35;  r4.x:=26.32;   g3^:=r4; put(g3);
  r4.c1:='q';  r4.i:= +29;  r4.x:=10.00;   g3^:=r4; put(g3);
  r4.c1:='j';  r4.i:=   8;  r4.x:=10000;   g3^:=r4; put(g3);
#else
  r4.c1:='w';  r4.i:= -100; g3^:=r4; put(g3);
  r4.c1:='y';  r4.i:= -35;  g3^:=r4; put(g3);
  r4.c1:='q';  r4.i:= +29;  g3^:=r4; put(g3);
  r4.c1:='j';  r4.i:=   8;  g3^:=r4; put(g3);
#endif

  for i:= 1 to 1000 do begin g3^ := r4; put(g3) end;
  reset(g3);
  if eof(g3) then e(11);

#ifndef NOFLOAT
  r4 := g3^;  get(g3);
  if (r4.c1<>'w') or (r4.i<>-100) or (r4.x<>303.56) then e(12);
  r4 := g3^;  get(g3);
  if (r4.c1<>'y') or (r4.i<> -35) or (r4.x<> 26.32) then e(13);
  r4 := g3^;  get(g3);
  if (r4.c1<>'q') or (r4.i<>  29) or (r4.x<> 10.00) then e(14);
  r4 := g3^;  get(g3);
  if (r4.c1<>'j') or (r4.i<>   8) or (r4.x<> 10000) then e(15);
#else
  r4 := g3^;  get(g3);
  if (r4.c1<>'w') or (r4.i<>-100) then e(12);
  r4 := g3^;  get(g3);
  if (r4.c1<>'y') or (r4.i<> -35) then e(13);
  r4 := g3^;  get(g3);
  if (r4.c1<>'q') or (r4.i<>  29) then e(14);
  r4 := g3^;  get(g3);
  if (r4.c1<>'j') or (r4.i<>   8) then e(15);
#endif

  for j:= 1 to 100 do
    begin for i:= -10 to +10 do a1[i] := i*j; g4^ := a1; put(g4) end;
  reset(g4);
  for j:= 1 to 100 do
    begin a1:=g4^; get(g4); for i:= -10 to +10 do if a1[i]<>i*j then e(16) end;
 end;

begin t:=37;  pct := pct+1;
  tst3701;
  t:=37;
  if not eof(g2) then e(1);
end;



{***********************************************************************}
procedure tst38;
{ Advanced set theory }
begin t:=38;  pct := pct + 1;
  if [50] >= [49,51] then e(1);
  if [10] <= [9,11] then e(2);
  if not ([50] <= [49..51]) then e(3);
  i:=1;  j:=2; k:=3;  l:=5;
  if [i] + [j] <> [i,j] then e(4);
  if [i] + [j] <> [i..j] then e(5);
  if [j..i] <> [] then e(6);
  if [j..l] + [j..k] <> [2,3,4,5] then e(7);
  if ([1..k, l..8] + [10]) * [k..7, 2, l] <> [2,3,l..7] then e(8);
  if [i..9] - [j..l] <> [1,l+1..k*k] then e(9);
  if [k..j] <> [i..j] * [k..l] then e(10);
  if not ([k..10] <= [i..15]) then e(11);
  if not ([k-1..k*l] <= [i..15]) then e(12);

  letters := ['a','b', 'z'];
  if letters <> ['a', 'b', 'z'] then e(13);
  cset := ['a'] + ['b', 'c', 'z'] - ['c','d'];
  if cset <> letters then e(14);
  cset := ['a'..'e'];
  if cset <> ['a', 'b', 'c', 'd', 'e'] then e(15);
  cset := ['a'..'z', '0'..'9', '+','-','*','/','.',':','(',')','{','}']; 
  if not ('+' in cset) or not ('.' in cset) or not ('}' in cset) then e(16);
  letters := ['a'..'z' , '0'..'9'];
  if letters >= cset then e(17);
end;


{***********************************************************************}

{ Main program }
begin ect:=0; pct:=0;
  tst34;   tst35;   tst36;   tst37;   tst38;
  write('Program t3:',pct:3,' tests completed.');
  writeln('Number of errors = ',ect:1);
end.
