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
program t2(input,output);

{ This program can be used to test out PASCAL compilers }

const
   rcsversion='$Id$';
   kew='q';
#ifndef NOFLOAT
   eps = 2.0e-7;  { This constant is machine dependent }
#endif

type wavelength = (red,blue,yellow);
  tp2=  record c1:char; i,j:integer; p:boolean; x:real end;
  single= array [0..0] of integer;
  spectrum= set of wavelength;
  np= ^node;
  node = record val:integer; next: np end;

var t,pct,ect:integer;
 i,j,k,l:integer;
#ifndef NOFLOAT
 w,x,y,z:real;
#endif
 p:boolean;
 d:char;
 color: wavelength;
 head: np;


function twice(k:integer):integer; begin twice := 2*k end;
function inc(k:integer):integer; begin inc := k+1 end;

procedure e(n:integer); 
begin 
  ect := ect + 1;
  writeln(' Error', n:3,' in test ', t) 
end;





{************************************************************************}
procedure tst21;
{ Test things packed }
var i:integer;  c:char;
    r1: packed record c:char; b:boolean;  i:integer end;
    r2: packed record c:char; i:integer; b:boolean; j:integer end;
#ifndef NOFLOAT
    r3: packed record c:char; r:real end;
#else
    r3: packed record c:char end;
#endif
    r4: packed record i:0..10; j:integer end;
    r5: packed record x:array[1..3] of char; i:integer end;
    r6: packed record x: packed array[1..3] of char; i:integer end;
    r7: packed record c:char; x:packed array[1..3] of char end;
    r8: packed record c:char; x:packed array[1..3] of integer end;
    r9: record x:packed record c:char; i:integer end; i:integer; c:char end;
     r10:packed record a:0..100; b:0..100; c:char; d:char end;

    a1: packed array[1..3] of char;
    a2: packed array[1..3] of integer;
#ifndef NOFLOAT
    a3: packed array[1..7] of real;
#endif
    a4: packed array[1..7] of array[1..11] of char;
    a5: packed array[1..5] of array[1..11] of integer;
    a6: packed array[1..9] of packed array[1..11] of char;
    a7: packed array[1..3] of packed array[1..5] of integer;
begin t:=21;  pct := pct + 1;
#ifndef NOFLOAT
  i:=4;  x:=3.5;  c:='x'; p:=true;
#else
  i:=4;  c:='x'; p:=true;
#endif

  r1.c:='a';  r1.b:=true;  r1.i:=i;   p:=r1.b;  j:=r1.i;
  r2.c:=c;  r2.i:=i;  r2.b:=p;  r2.j:=i;  j:=r2.i;  j:=r2.j;
#ifndef NOFLOAT
  r3.c:=c;  r3.r:=x;  y:=r3.r;
#else
  r3.c:=c;
#endif
  r4.i:=i;  r4.j:=i;  j:=r4.i;  j:=r4.j;
  r5.x[i-2]:=c;  r5.i:=i;  j:=r5.i;
  r6.x[i-1]:=c;  r6.i:=i;  j:=r6.i;
  r7.c:=c;  r7.x[i-1]:=c;  d:=r7.c;  d:=r7.x[i-1];
  r8.c:=c;  r8.x[i-1]:=5;  j:=r8.x[i-1];
  r9.x.c:=c;  r9.x.i:=i;  r9.c:=c;  j:=r9.x.i;

  if (r1.c <> 'a') or (r1.b <> true) or (r1.i <> 4) then e(1);
  if (r2.c<>'x') or (r2.i<>4) or (r2.b<>p) or (r2.j<>4) then e(2);
#ifndef NOFLOAT
  if (r3.c<>'x') or (r3.r<>3.5) then e(3);
#else
  if (r3.c<>'x') then e(3);
#endif
  if (r4.i<>4) or (r4.j<>4) then e(4);
  if (r5.x[2]<>'x') or (r5.i<>4) then e(5);
  if (r6.x[3]<>'x') or (r6.i<>4) then e(6);
  if (r7.c<>'x') or (r7.x[3]<>'x') or (c<>d) then e(7);
  if (r8.c<>'x') or (r8.x[3]<>5) then e(8);
  if (r9.x.c<>'x') or (r9.x.i<>4) or (r9.c<>'x') then e(9);

#ifndef NOFLOAT
  i:=4;  a1[i-1]:=c;    a2[i-1]:=i;   a3[i]:=x;
#else
  i:=4;  a1[i-1]:=c;    a2[i-1]:=i;
#endif
  a4[i][i+1]:=c;
  a5[i][i+1]:=i;  j:=a5[i][i+1];
  a6[i][i+1]:=c;
  a7[i-1][i+1]:=i;  j:=a7[i-1][i+1];

  if a1[i-1] <> 'x' then e(10);
  if a2[i-1] <> 4 then e(11);
#ifndef NOFLOAT
  if a3[i] <> 3.5 then e(12);
#endif
  if a4[i][i+1] <> 'x' then e(13);
  if a5[i][i+1] <> 4 then e(14);
  if a6[i][i+1] <> 'x' then e(15);
  if a7[i-1][i+1] <> 4 then e(16);

  i:=75; c:='s';
  r10.a:=i;  r10.b:=i+1;  r10.c:='x';  r10.d:=c;
  if (r10.a<>i) or (r10.b<>76) or (r10.c<>'x') or (r10.d<>'s') then e(17);
  i:=r10.a;  if i<>75 then e(18);
  i:=r10.b;  if i<>76 then e(19);
  c:=r10.c;  if c<>'x'then e(20);
  c:=r10.d;  if c<>'s'then e(21);
end;


{************************************************************************}
 procedure tst22;
{ References to intermediate lexical levels }
 type wavelength = (pink,green,orange);
     ww2= 1939..1945;
#ifndef NOFLOAT
     tp2=  record c1:char; i,j:integer; p:boolean; x:real end;
#else
     tp2=  record c1:char; i,j:integer; p:boolean end;
#endif
     single= array [0..0] of integer;
     spectrum= set of wavelength;
     pnode = ^node;
     node = record val:integer; next: pnode end;
     vec1 = array[-10..+10] of integer;

 var j,k,m:integer;
#ifndef NOFLOAT
    x,y,z:real;
#endif
    p,q,r:boolean;
    c1,c2,c3:char;
    sr1,sr2,sr3: 1939..1945;
    color,hue,tint: wavelength;
    a1: vec1;
#ifndef NOFLOAT
    a2: array [ww2] of real;
#endif
    a3: array[wavelength] of boolean;
    a4: array[(mouse,house)] of char;
    a5: array[50..52,(bat,cat,rat),boolean,ww2] of integer;
    a6: packed array[0..10,0..3,0..3] of char;
    r1,r2: tp2;
#ifndef NOFLOAT
    r3: packed record c1:char; i,j:integer; p:boolean; x:real end;
#else
    r3: packed record c1:char; i,j:integer; p:boolean end;
#endif
    colors: spectrum;
    beasts: set of (pig,chicken,farmersdaughter);
    bits: set of 0..1;
    p1: ^integer;
    p2: ^tp2;
    p3: ^single;
    p4: ^spectrum;
    tail: np;




 procedure tst2201;
 { Arithmetic on intermediate level integer variables }
 begin t:=2201; pct := pct + 1;
  i:=1;  j:=2;  k:=3;  l:=4;  m:=10;
  if i+j <> k then e(1);
  if i+k <> l then e(2);
  if j-k <> -i then e(3);
  if j*(j+k) <> m then e(4);
  if -m <> -(k+k+l) then e(5);
  if i div i <> 1 then e(6);
  if m*m div m <> m then e(7);
  if 10*m <> 100 then e(8);
  if m*(-10) <> -100 then e(9);
  if j div k <> 0 then e(10);
  if 100 div k <> 33 then e(11);
  if i+j*k+l+m mod j + 50 div k <> 27 then e(12);
  if j*k*m div 6 <> 10 then e(13);
  if (k>4) or (k>=4) or (k=4) then e(14);
  if (m<j) or (m<=j) or (m=j) then e(15);
  if k <> i+j then e(16);
 end;

#ifndef NOFLOAT

 procedure tst2202;
 { Real arithmetic using intermediate level variables }
 begin t:=2202; pct := pct + 1;

  x:=1.50;  y:=3.00; z:= 0.10;
  if abs(5*y*z-x) > eps then e(10);
  if abs(y*y*y/z*x-405) > eps then e(11);
  x:=1.1;  y:= 1.2;  
  if y<x then e(12);
  if y <= x then e(13);
  if y = x then e(14);
  if x <> x then e(15);
  if x >= y then e(16);
  if x >y then e(17);
 end;

#endif
 procedure tst2203;
 { Boolean expressions using intermediate level varibales }
 begin t:=2203; pct := pct + 1;
  p:=true; q:=true; r:=false;
  if not p then e(7);
  if r then e(8);
  if p and r then e(9);
  if p and not q then e(10);
  if not p or not q then e(11);
  if (p and r) or (q and r) then e(12);
  if p and q and r then e(13);
  if (p or q) = r then e(14);
 end;

 procedure tst2204;
 { Characters, Subranges, Enumerated types using intermediate level vars }
 begin t:=2204; pct := pct + 1;
  if 'q' <> kew then e(1);
  c1 := 'a'; c2 := 'b'; c3 := 'a';
  if c1 = c2 then e(2);
  if c1 <> c3 then e(3);

  sr1:=1939; sr2:=1945; sr3:=1939;
  if sr1=sr2 then e(4);
  if sr1<>sr3 then e(5);

  color := orange; hue := green; tint := orange;
  if color = hue then e(6);
  if color <> tint then e(7);
 end;


 procedure tst2205;
 { Intermediate level arrays }
 var i,l,o:integer;
 begin t:=2205; pct := pct + 1;
  for i:= -10 to 10 do a1[i] := i*i;
  if (a1[-10]<>100) or (a1[9]<>81) then e(1);

#ifndef NOFLOAT
  for i:=1939 to 1945 do a2[i]:=i-1938.5;
  if (abs(a2[1939]-0.5) > eps) or (abs(a2[1945]-6.5) > eps) then e(2);
#endif

  color := orange;
  a3[green] := true;  a3[orange] := true;
  if (a3[green]<>true) or (a3[orange]<>true) then e(3);
  a3[green] := false;  a3[orange] := false;
  if (a3[green]<>false) or (a3[orange]<>false) then e(4);

  a4[mouse]:='m'; a4[house]:='h';
  if (a4[mouse] <> 'm') or (a4[house]<>'h' ) then e(5);

  for i:=1939 to 1945 do a5[51,bat,false,i]:=300+i;
  if a5[51,bat,false,1940] <> 2240 then e(6);
  for i:=50 to 52 do a5[i,cat,true,1943]:=200+i;
  if (a5[50,cat,true,1943] <> 250) or (a5[52,cat,true,1943] <> 252) then e(7);

  for i:= -10 to 10 do a1[i]:= 0;
  for i:= 0 to 10 do a1[i div 2 + i div 2]:= i+1;
  if(a1[0]<>2) or (a1[5]<>0) or (a1[8]<>10) then e(8);

  for i:= 0 to 10 do
  for l:= 0 to 3 do
  for o:= 0 to 3 do
    if ( (i+l+o) div 2) * 2 = i+l+o then a6[i,l,o]:='e' else a6[i,l,o]:='o';
  if (a6[2,2,2]<>'e') or (a6[2,2,3]<>'o') or (a6[0,3,1]<>'e') then e(9);
 end;

#ifndef NOFLOAT

 procedure tst2206;
 { Intermediate level records }
 begin t:=2206; pct := pct + 1;
  r1.c1:='x'; r1.i:=40; r1.j:=50; r1.p:=true; r1.x:=3.0;
  c1:='a'; i:=0;  j:=0; p:=false; x:=100.0;
  if (r1.c1<>'x') or (r1.i<>40) or (r1.p<>true) or (r1.x<>3.0) then e(1);
  r2:=r1;
  if (r2.c1<>'x') or (r2.i<>40) or (r2.p<>true) or (r2.x<>3.0) then e(2);
  i:=r1.i;  p:=r1.p;  c1:=r1.c1; x:=r1.x;
  if (c1<>'x') or (i<>40) or (p<>true) or (x<>3.0) then e(3);
  r3.c1:='x'; r3.i:=40; r3.j:=50; r3.p:=true; r3.x:=3.0;
  if (r3.c1<>'x') or (r3.i<>40) or (r3.p<>true) or (r3.x<>3.0) then e(4);
 end;

#else

 procedure tst2206;
 { Intermediate level records }
 begin t:=2206; pct := pct + 1;
  r1.c1:='x'; r1.i:=40; r1.j:=50; r1.p:=true;
  c1:='a'; i:=0;  j:=0; p:=false; 
  if (r1.c1<>'x') or (r1.i<>40) or (r1.p<>true) then e(1);
  r2:=r1;
  if (r2.c1<>'x') or (r2.i<>40) or (r2.p<>true) then e(2);
  i:=r1.i;  p:=r1.p;  c1:=r1.c1;
  if (c1<>'x') or (i<>40) or (p<>true) then e(3);
  r3.c1:='x'; r3.i:=40; r3.j:=50; r3.p:=true;
  if (r3.c1<>'x') or (r3.i<>40) or (r3.p<>true) then e(4);
 end;

#endif
 procedure tst2207;
 { Intermediate level sets }
 begin t:=2207; pct := pct + 1;
  colors := [];
  colors := colors + [];
  if colors <> [] then e(1);
  colors := colors + [pink];
  if colors <> [pink] then e(2);
  colors := colors + [green];
  if colors <> [pink,green] then e(3);
  if colors <> [green,pink] then e(4);
  colors := colors - [pink];
  if colors <> [green] then e(5);
  beasts := [chicken] + [chicken,pig];
  if beasts <> [pig,chicken] then e(6);
  beasts := [] - [farmersdaughter];
  if beasts <> [] then e(7);
  bits := [0] + [1] - [0];
  if bits <> [1] then e(8);
 end;


 procedure tst2208;
 { Pointers }
 begin t:=2208; pct := pct + 1;
  new(p1); new(p2); new(p3); new(p4);
  p1^ := 1066;
  if p1^ <> 1066 then e(1);
  p2^.i := 1215;
  if p2^.i <> 1215 then e(2);
  p3^[0]:= 1566;
  if p3^[0] <> 1566 then e(3);
  p4^ := [pink];
  if p4^ <> [pink] then e(4);
 end;


 procedure tst2209;
 var i:integer;
 begin t:=2209; pct := pct + 1;
  head := nil;
  for i:= 1 to 100 do
    begin new(tail); tail^.val:=100+i; tail^.next :=head; head:= tail end;
  if (tail^.val<>200) or (tail^.next^.val<>199) then e(1);
  if tail^.next^.next^.next^.next^.next^.next^.next^.next^.val<> 192 then e(2);
  tail^.next^.next^.next^.val := 30;
  if tail^.next^.next^.next^.val <> 30 then e(3);
 end;
begin t:=22; pct:=pct+1;
#ifndef NOFLOAT
      tst2201; tst2202; tst2203; tst2204; tst2205; tst2206;
#else
      tst2201; tst2203; tst2204; tst2205; tst2206;
#endif
      tst2207; tst2208; tst2209;  
end;





{************************************************************************}
procedure tst25;
{ Statement sequencing }
label 0,1,2,3;
 procedure tst2501;
 begin t:=2501;
   goto 0;
 e(1);
 end;
begin t:=25; pct:=pct+1;
  tst2501;
  e(1);
  0:
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  i:=0;
1:  if i>10 then goto 3 else goto 2;
  e(2);
2: i:=i+1;  goto 1;
  e(3);
3:
end;




{************************************************************************}
procedure tst26;
{ More data structures }
type x = array[1..5] of integer;
     ta = array [1..5] of array  [1..5] of x;
     tb = array [1..5] of record p1: ^x;  p2: ^x end;
     tr = record c: record b: record a: integer end  end  end ;

var low,i,j,k:integer; a:ta;  b:tb;  r:tr;  hi:integer;

procedure tst2601(w:ta; x:tb; y:tr);
var i,j,k: integer;
begin t:=2601; pct:=pct+1;
  for i:= 1 to 5 do for j:= 1 to 5 do for k:=1 to 5 do
     if w[i][j][k] <> i*i + 7*j + k then e(1);
  if (x[1].p1^[1] <> -9) or (x[2].p2^[4]<> -39) then e(2);
  if y.c.b.a <> 102 then e(3);
end;

begin t:=26; pct:=pct+1;
  low := 1000; hi := 1001;
  for i:= 1 to 5 do for j:=1 to 5 do for k:= 1 to 5 do a[i][j][k] :=i*i+7*j+k;
  new(b[1].p1);  new(b[2].p2);
  b[1].p1^[1] := -9;  b[2].p2^[4] := -39;
  r.c.b.a := 102;
  tst2601(a,b,r);
  t:=26;
  if(low <> 1000) or (hi <> 1001) then e(1);
end;



{************************************************************************}
procedure tst27;
{ Assignments }
begin t:=27; pct := pct+1;
  i:=3; j:=2; k:= -100;
  l:= 1+(i*(j+(i*(j+(i*(j+(i*(3+j*(i*1+j*2)))))))));
  if l <> 1456 then e(1);
  l:= ((((((((((((((((((((((((((((((((0))))))))))))))))))))))))))))))));
  if l <> 0 then e(2);
  l:=(((i*j)+(3*i)-5) div 10)*(((i*j)+(3*i)-5) div 10)*(((i*j)+(3*i)-5) div 10)
   + (((i*j)+(3*i)-5) div 10)*(((i*j)+(3*i)-5) div 10)*(((i*j)+(3*i)-5) div 10);
  if l <> 2 then e(3);

  l:=((j+j) div 4) * ((j+j) div 4) * ((j+j) div 4)* (j div 3 + j div 4 + 3)+
     ((j+j) div 4) * ((j+j) div 4) * ((j+j) div 4)* (j div 3 + j div 4 + 3);
  if l <> 6 then e(4);
  i:=j*j*j*j*j*j*j*j*j*j*j*j*j*j - 16383;
  if i <>1 then e(5);
  l:=(i+(i+(i+(i+(i+(i+(i+(i+(i+(i+(i+(i+(i+(i+(i+(i))))))))))))))));
  if l <> 16 then e(6);
  l:= (((((((((((((((((j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j)+j);
  if l <> 34 then e(7);
  l:= (-(-(-(-(-(-(-(-(-(j))))))))));
  if l <> -2 then e(8);

#ifndef NOFLOAT
  x:= 0.1;  y:=0.2;  z:=0.3;
  w:=(((((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0))*
      ((((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0))*
      ((((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0))*
      ((((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0))*
      ((((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0)+(((x+y)/z)*2.0)))-1;
  if abs(w-32767) > 0.0001 then e(9);

  i:= trunc(100*y+0.5);  if i <> 20 then e(10);
  i:= 32767;  w:=i;  if w <> 32767 then e(11);
#endif
end;



{************************************************************************}
procedure tst28;
{ Calls }
var i:integer;
function ack(m,n:integer):integer;
begin if m=0
         then ack := n+1
         else if n=0
                 then ack := ack(m-1,1)
                 else ack := ack(m-1,ack(m,n-1))
end;

procedure fib(a:integer; var b:integer); { Fibonacci nrs }
var i,j:integer;
begin
  if (a=1) or (a=2) then b:=1 else
     begin fib(a-1,i);  fib(a-2,j);  b:=i+j end
end;

begin t:=28;  pct:= pct+1;
  if ack(2,2) <> 7 then e(1);
  if ack(3,3) <> 61 then e(2);
  if ack(3,5) <> 253 then e(3);
  if ack(2,100) <> 203 then e(4);
  fib(10,i);  if i <> 55 then e(5);
  fib(20,i);  if i <> 6765 then e(6);
end;


{************************************************************************}
procedure tst29;
{ Loops }
var i,l:integer; p:boolean;
begin t:= 29; pct:=pct+1;
  j:=5;
  k:=0; for i:=1 to j do k:=k+1; if k<>5 then e(1);
  k:=0; for i:=5 to j do k:=k+1; if k<>1 then e(2);
  k:=0; for i:=6 to j do k:=k+1; if k<>0 then e(3);
  k:=0; for i:=-1 downto -j do k:=k+1; if k<>5 then e(4);
  k:=0; for i:=-5 downto -j do k:=k+1; if k<>1 then e(5);
  k:=0; for i:=-6 downto j do k:=k+1; if k<>0 then e(6);
  k:=0; for i:=1 downto 10 do k:=k+1; if k<>0 then e(7);

  k:=0; for l:=1 to j do k:=k+1; if k<>5 then e(8);
  k:=0; for l:=5 to j do k:=k+1; if k<>1 then e(9);
  k:=0; for l:=6 to j do k:=k+1; if k<>0 then e(10);
  k:=0; for l:=-1 downto -j do k:=k+1; if k<>5 then e(11);
  k:=0; for l:=-5 downto -j do k:=k+1; if k<>1 then e(12);
  k:=0; for l:=-6 downto j do k:=k+1; if k<>0 then e(13);
  k:=0; for l:=1 downto 10 do k:=k+1; if k<>0 then e(14);
  k:=0; for p:= true downto false do k:=k+1; if k<>2 then e(15);
  k:=0; for p:= false to true do k:=k+1; if k<>2 then e(16);

  k:=0; while k<0 do k:=k+1; if k<>0 then e(17);
  k:=0; repeat k:=k+1; until k>0; if k<> 1 then e(18);
  k:=0; repeat k:=k+1; until k > 15; if k <> 16 then e(18);
  k:=0; while k<=10 do k:=k+1;  if k<> 11 then e(19);
end;

{************************************************************************}
procedure tst30;
{ case statements }
begin t:=30; pct:=pct+1;
  i:=3; k:=0;
  case i*i-7 of
   0: k:=0;  1: k:=0;  2: k:=1;  3,4: k:=0
  end;
  if k<>1 then e(1);

  color := red; k:=0;
  case color of
    red: k:=1;  blue: k:=0;  yellow: k:=0
  end;
  if k<>1 then e(2);

  k:=0;
  case color of
    red,blue: k:=1;  yellow: k:=0
  end;
  if k<>1 then e(3);
end;
#ifndef NOFLOAT

{************************************************************************}
procedure tst31;
{ with statements }
var ra: record i:integer; x:real; p:tp2; q:single;
               a2: record a3: tp2 end
        end;
     rb: record j: integer; y:real; pp:tp2; qq:single end;
begin t:=31; pct:=pct+1;
  i:=0;  x:=0;
  ra.i:=-3006;  ra.x:=-6000.23;  ra.q[0]:=35;  ra.p.i:=20;
  with ra do
    begin if (i<>-3006) or (x<>-6000.23) or (q[0]<>35)
              or (p.i<>20) then e(2);

      i:=300;   x:= 200.5;  q[0]:=35;  p.i:=-10
    end;
  if (ra.i<>300) or (ra.x<>200.5) or (ra.q[0]<>35) or (ra.p.i<>-10) then e(3);
  with ra.p do if i <> -10 then e(4);

  i:= -23;
  ra.a2.a3.i := -909;
  with ra do if a2.a3.i <> -909 then e(5);
  with ra.a2 do if a3.i <> -909 then e(6);
  with ra.a2.a3 do if i <> -909 then e(7);
  with ra.a2 do i:=5;
  if (i<>5) or (ra.a2.a3.i <> -909) then e(8);
  with ra.a2.a3 do i:= 6;
  if i<>5 then e(9);
  if ra.a2.a3.i <> 6 then e(10);

  with ra,rb do
   begin x:=3.5;  y:=6.5;  i:=3;  j:=9 end;
  if (ra.x<>3.5) or (rb.y<>6.5) or (ra.i<>3) or (rb.j<>9) then e(11);
end;

#else

{************************************************************************}
procedure tst31;
{ with statements }
var ra: record i:integer; p:tp2; q:single;
               a2: record a3: tp2 end
        end;
     rb: record j: integer; pp:tp2; qq:single end;
begin t:=31; pct:=pct+1;
#ifndef NOFLOAT
  i:=0;  x:=0;
#else
  i:=0;
#endif
  ra.i:=-3006; ra.q[0]:=35;  ra.p.i:=20;
  with ra do
    begin if (i<>-3006) or (q[0]<>35)
              or (p.i<>20) then e(2);

      i:=300;    q[0]:=35;  p.i:=-10
    end;
  if (ra.i<>300) or (ra.q[0]<>35) or (ra.p.i<>-10) then e(3);
  with ra.p do if i <> -10 then e(4);

  i:= -23;
  ra.a2.a3.i := -909;
  with ra do if a2.a3.i <> -909 then e(5);
  with ra.a2 do if a3.i <> -909 then e(6);
  with ra.a2.a3 do if i <> -909 then e(7);
  with ra.a2 do i:=5;
  if (i<>5) or (ra.a2.a3.i <> -909) then e(8);
  with ra.a2.a3 do i:= 6;
  if i<>5 then e(9);
  if ra.a2.a3.i <> 6 then e(10);

  with ra,rb do
   begin  i:=3;  j:=9 end;
  if  (ra.i<>3) or (rb.j<>9) then e(11);
end;


#endif






{************************************************************************}
procedure tst32;
{ Standard procedures }
begin t:=32;  pct:=pct+1;
  if abs(-1) <> 1 then e(1);
  i:= -5;  if abs(i) <> 5 then e(2);
#ifndef NOFLOAT
  x:=-2.0;  if abs(x) <> 2.0 then e(3);
#endif
  if odd(5) = false then e(4);
  if odd(4) then e(5);
  if sqr(i) <> 25 then e(6);
  if succ(i) <> -4 then e(7);
  if succ(red) <> blue then e(8);
  if pred(blue) <> red then e(9);
  if ord(red) <> 0 then e(10);
  if ord(succ(succ(red))) <> 2 then e(11);
  if chr(ord(chr(ord(chr(ord('u')))))) <> 'u' then e(12);
  if ord(chr(ord(chr(ord(chr(50))))))  <> 50 then e(13);
#ifndef NOFLOAT
  if abs(trunc(5.2)-5.0) > eps then e(14);
  if abs(sin(3.1415926536)) >  10*eps then e(15);
  if abs(exp(1.0)-2.7182818) > 0.0001 then e(16);
  if abs(ln(exp(1.0))- 1.0) > 3*eps then e(17);
  if abs(sqrt(25.0)-5.0) > eps then e(18);
  if abs(arctan(1.0) - 3.1415926535/4.0) > 0.0001 then e(19);
  if abs(ln(arctan(1)*4) - 1.144729886) > 0.000001 then e(20);
  if abs(sin(1) - 0.841470985 ) > 0.000001 then e(21);
  if abs(cos(1) - 0.540302306) > 0.000001 then e(22);
  if abs(sqrt(2) - 1.4142135623) > 0.000001 then e(23);
  if abs(sqrt(10) - 3.1622776601) > 0.000001 then e(24);
  if abs(sqrt(1000.0) - 31.622776602) > 0.00001 then e(25);
#endif
end;


{***************************************************************************}
procedure tst33;
{ Functions }
var i,j,k,l,m: integer;
begin t:=33;  pct := pct+1;
  i:=1; j:=2;  k:=3;  l:=4;  m:=10;
  if twice(k) <> m-l then e(1);
  if twice(1) <> 2 then e(2);
  if twice(k+1) <> twice(l) then e(3);
  if twice(twice(twice(inc(twice(inc(3)))))) <> 72 then e(4);
  if twice(inc(j+twice(inc(twice(i+1+inc(k)+inc(k))+twice(2)))))<>106
		then e(5);
  if twice(1) + twice(2) * twice(3) <> 26 then e(6);
  if 3 <>  0 + twice(1) + 1 then e(7);
  if 0 <> 0 * twice(m) then e(8);
end;



{**********************************************************************}

{ Main Program }
begin ect := 0;  pct := 0;
tst21; tst22; tst25; tst26; tst27; tst28; tst29; tst30; tst31; tst32; tst33;

write('Program t2:',pct:3,' tests completed.');
writeln('Number of errors = ',ect:1);
end.
