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

program t4(input,output);
{ Tests for the EM-1 compiler }
const rcsversion='$Id$';
type vec = array[1..1000] of integer;
     spectrum = set of (red,blue,yellow);
#ifndef NOFLOAT
     tp2 = record c1:char;i,j:integer; p:boolean; x:real end;
#else
     tp2 = record c1:char;i,j:integer; p:boolean end;
#endif
     cmat = array[0..3,0..7] of ^spectrum;
     single = array [0..0] of integer;
     np = ^node;
     node = record val: integer;  next: np end;

var t,ect,pct:integer;
    r1: tp2;
    pt1,pt2: ^vec;
    pt3:^integer;
    mk: ^integer;
    i,j: integer;



procedure e(n:integer); 
begin
  ect := ect + 1;
  writeln(' Error', n:3,' in test ', t) 
end;

function inc(k:integer):integer; begin inc := k+1 end;
function twice(k:integer):integer; begin twice := 2*k end;
function decr(k:integer):integer; begin decr := k-1 end;



procedure tst40;
{ Mark and Release }
var i:integer;
  procedure grab;
  var i:integer;
  begin
    for i:=1 to 10 do new(pt1);
    for i:=1 to 1000 do new(pt3);
  end;

begin t:= 40;  pct:=pct+1;
  for i:=1 to 10 do
     begin
        mark(mk);
        new(pt2);
        grab;
        release(mk)
     end;
end;


procedure tst41;
{ Empty sets }
begin  t:=41;  pct := pct + 1;
  if red in [] then e(1);
  if ([] <> []) then e(2);
  if not ([] = []) then e(3);
  if not([] <=[]) then e(4);
  if not ( [] >= []) then e(5);
end;


{************************************************************************}
procedure tst42;
{ Record variants.  These tests are machine dependent }
var s:record b:boolean; case t:boolean of false:(c:char);true:(d:cmat) end;
    w: packed record
          case z:boolean of
            false: (x:array[0..20] of integer);
            true: (x1,x2,x3,a,b,c,d,e,f,g,h,i,j,k,l:char)
       end;

    y: record
          case z:boolean of
            false: (x:array[0..20] of integer);
            true: (a,b,c,d,e,f,g,h,i,j,k,l:char)
       end;
    i:integer;
begin t:=42; pct:=pct+1;
  s.t:=false;  s.c:='x';  if s.c <> 'x' then e(1);
  for i:=0 to 20 do begin w.x[i]:=-1; y.x[i]:=-1 end;
  w.a:=chr(0);  w.f:=chr(0);
  y.a:=chr(0);  y.f:=chr(0);
  if (ord(w.a) <> 0) or (ord(w.b) <> 255) then e(3);
  if (ord(w.c) <> 255) or (ord(w.d)<>255) then e(4);
  if (ord(w.e) <> 255) or (ord(w.f) <> 0) then e(5);
  if ord(y.a) <> 0 then e(6);
  if ord(y.f) <> 0 then e(7);
end;




{************************************************************************}
procedure tst43;
{ Procedure and function parameters }
  function incr(k:integer):integer; begin incr := k+1 end;
  function double(k:integer):integer; begin double := 2*k end;
  function eval(function f(a:integer):integer; a:integer):integer;
  begin eval:=f(a) end;
  function apply(function f(a:integer):integer; a:integer):integer; 
      begin apply:=eval(f,a) end;

  procedure x1(function f(a:integer):integer;  a:integer; var r:integer);
      procedure x2(function g(c:integer):integer;  b:integer;  var s:integer);
      begin s:=apply(g,b); end;
  begin x2(f, a+a, r) end;

procedure p0(procedure p(x:integer); i,j:integer);
begin
  if j=0 then p(i) else p0(p,i+j,j-1)
end;

procedure p1(a,b,c,d:integer);
var k:integer;
  procedure p2(x:integer);
  begin k:= x*x end;
begin k:=0;
  p0(p2,a,b);
  if k <> c then e(d);
end;



begin t:=43; pct := pct+1;
  i:=10;  j:=20;
  if incr(0) <> 1 then e(1);
  if decr(i) <> 9 then e(2);
  if double(i+j) <> 60 then e(3);
  if incr(double(j)) <> 41 then e(4);
  if decr(double(incr(double(i)))) <> 41 then e(5);
  if incr(incr(incr(incr(incr(5))))) <> 10 then e(6);
  if eval(incr,i) <> 11 then e(7);
  if eval(decr,3) <> 2 then e(8);
  if incr(eval(double,15)) <> 31 then e(9);
  if apply(incr,3) <> 4 then e(10);

  x1(double,i,j);  if j <> 40 then e(11);
  x1(incr,i+3,j);  if j <> 27 then e(12);
  p1(3,5,324,13);
  p1(10,4,400,14);
  p1(1,8,1369,15);
  j:=1;
  if inc(incr(twice(double(inc(incr(twice(double(j)))))))) <> 26 then e(13);
end;


{************************************************************************}
 procedure tst44;
{ Value parameters }
   type ww2 = array[-10..+10] of tp2;
        arra = array[-10..+10] of integer;
        reca = record k:single; s:spectrum end;
        pa = np;
#ifndef NOFLOAT
var l1:integer;  xr:real;  xb:boolean;  xc:char;  xar:cmat;  xnode:pa;
#else
var l1:integer;  xb:boolean;  xc:char;  xar:cmat;  xnode:pa;
#endif
    vec1: arra;   vec2: ww2;
    s2:spectrum;  rec1: reca;
    zero:0..0;

#ifndef NOFLOAT
procedure tst4401(pl1:integer; pxr:real;   pxb:boolean;  pxc:char;
#else
procedure tst4401(pl1:integer;  pxb:boolean;  pxc:char;
#endif
                   pxar:cmat;   pxnode:pa;  pxtp2:tp2;
                   pvec1:arra;  pvec2:ww2; prec1:reca;
                   ps1,ps2:spectrum;  psin:single; i,j:integer);
begin t:=4401; pct:=pct+1;
  if pl1<>29 then e(1);
#ifndef NOFLOAT
  if pxr<>-0.31 then e(2);
#endif
  if pxb <> false then e(3);
  if pxc <> 'k' then e(4);
  if (pxar[1,1]^<>[red,blue]) or (pxar[2,2]^ <> [yellow]) then e(5);
  if (pxnode^.val <> 105) or (pxnode^.next^.val <> 106) then e(6);
#ifndef NOFLOAT
  if (pxtp2.c1 <> 'w') or (pxtp2.x <> 20.3) then e(7);
#else
  if (pxtp2.c1 <> 'w')  then e(7);
#endif
  if pvec1[10] <> -996 then e(8);
#ifndef NOFLOAT
  if pvec2[zero].x <> -300 then e(9);
#endif
  if (prec1.k[zero] <> -421) or (prec1.s <> []) then e(10);
  if (ps1<>[]) or (ps2<>[red]) then e(11);
  if psin[zero] <> -421 then e(12);
  if i <> -421 then e(13);
  if j <> 106 then e(14);

  pl1:=0;  pxc:=' ';  pxb:=true;
  pxar[1,1]^:=[];  pxar[2,2]^:=[];
  pxnode^.val:=0;  pxnode^.next^.val:=1;
  pxtp2.c1:=' ';
  pvec1[10]:=0;
#ifndef NOFLOAT
  pvec2[zero].x:=0;
#endif
  prec1.k[zero]:=0;
  psin[0]:=0;  i:=0;  j:=0;
end;

begin t:=44; pct:=pct+1;
  zero:=0;
#ifndef NOFLOAT
  l1:=29;  xr:=-0.31;  xb:=false;  xc:='k';
#else
  l1:=29;  xb:=false;  xc:='k';
#endif
  new(xar[1,1]);  xar[1,1]^ := [red,blue];
  new(xar[2,2]);  xar[2,2]^ := [yellow];
  new(xar[1,2]);  xar[1,2]^ := [yellow];
  new(xnode);  xnode^.val :=105;
  new(xnode^.next); xnode^.next^.val :=106;
#ifndef NOFLOAT
  r1.c1:='w';  r1.x:=20.3;
  vec1[10] := -996;  vec2[zero].x := -300;
#else
  r1.c1:='w';
  vec1[10] := -996;
#endif
  rec1.k[zero]:=-421;  rec1.s :=[];
  s2:=[red];

#ifndef NOFLOAT
  tst4401(l1, xr, xb, xc, xar, xnode, r1, vec1, vec2, rec1, 
#else
  tst4401(l1, xb, xc, xar, xnode, r1, vec1, vec2, rec1, 
#endif
           [], s2, rec1.k, rec1.k[zero], xnode^.next^.val);;
  t:=44;

  if l1<>29 then e(1);
#ifndef NOFLOAT
  if xr<> -0.31 then e(2);
#endif
  if xb <> false then e(3);
  if xc <> 'k' then e(4);
  if (xar[1,1]^ <> [])  or (xar[2,2]^ <> []) then e(5);
  if xar[1,2]^ <> [yellow] then e(6);
  if (xnode^.val <> 0) or (xnode^.next^.val <> 1) then e(7);
#ifndef NOFLOAT
  if (r1.c1 <> 'w') or (r1.x <> 20.3) then e(8);
#else
  if (r1.c1 <> 'w') then e(8);
#endif
  if vec1[10] <> -996 then e(9);
#ifndef NOFLOAT
  if vec2[zero].x <> -300 then e(10);
#endif
  if (rec1.k[zero] <> -421) or (rec1.s <> []) then e(11);
  if s2 <> [red] then e(12);
end;


{************************************************************************}
 procedure tst45;
{ Var parameters }
   type ww2 = array[-10..+10] of tp2;
        arra = array[-10..+10] of integer;
        reca = record k:single; s:spectrum end;
        pa = np;
#ifndef NOFLOAT
var l1:integer;  xr:real;  xb:boolean;  xc:char;  xar:cmat;  xnode:pa;
#else
var l1:integer;   xb:boolean;  xc:char;  xar:cmat;  xnode:pa;
#endif
    vec1: arra;   vec2: ww2;
    s1,s2:spectrum;  rec1: reca;
    zero:0..0;

#ifndef NOFLOAT
procedure tst4501(var pl1:integer; var pxr:real; var pxb:boolean; var pxc:char; 
#else
procedure tst4501(var pl1:integer;  var pxb:boolean; var pxc:char; 
#endif
                   var pxar:cmat;   var pxnode:pa;  var pxtp2:tp2;
                   var pvec1:arra;  var pvec2:ww2; var prec1:reca;
                   var ps1,ps2:spectrum;  var psin:single; var i,j:integer);
begin t:=4501; pct:=pct+1;
  if pl1<>29 then e(1);
#ifndef NOFLOAT
  if pxr<>-0.31 then e(2);
#endif
  if pxb <> false then e(3);
  if pxc <> 'k' then e(4);
  if (pxar[1,1]^<>[red,blue]) or (pxar[2,2]^ <> [yellow]) then e(5);
  if (pxnode^.val <> 105) or (pxnode^.next^.val <> 106) then e(6);
#ifndef NOFLOAT
  if (pxtp2.c1 <> 'w') or (pxtp2.x <> 20.3) then e(7);
#else
  if (pxtp2.c1 <> 'w') then e(7);
#endif
  if pvec1[10] <> -996 then e(8);
#ifndef NOFLOAT
  if pvec2[zero].x <> -300 then e(9);
#endif
  if (prec1.k[zero] <> -421) or (prec1.s <> []) then e(10);
  if (ps1<>[]) or (ps2<>[red]) then e(11);
  if psin[zero] <> -421 then e(12);
  if i <> -421 then e(13);
  if j <> 106 then e(14);

#ifndef NOFLOAT
  pl1:=0;  pxr:=0;  pxc:=' ';  pxb:=true;
#else
  pl1:=0;   pxc:=' ';  pxb:=true;
#endif
  pxar[1,1]^:=[];  pxar[2,2]^:=[];
  pxnode^.val:=0;  pxnode^.next^.val:=1;
  pxtp2.c1:=' ';
#ifndef NOFLOAT
  pxtp2.x := 0;
#endif
  pvec1[10]:=0;
#ifndef NOFLOAT
  pvec2[zero].x:=0;
#endif
  prec1.k[zero]:=0;
  psin[0]:=0;  i:=223;  j:=445;
end;

begin t:=45; pct:=pct+1;
  zero:=0;
#ifndef NOFLOAT
  l1:=29;  xr:=-0.31;  xb:=false;  xc:='k';
#else
  l1:=29;  xb:=false;  xc:='k';
#endif
  new(xar[1,1]);  xar[1,1]^ := [red,blue];
  new(xar[2,2]);  xar[2,2]^ := [yellow];
  new(xar[1,2]);  xar[1,2]^ := [yellow];
  new(xnode);  xnode^.val :=105;
  new(xnode^.next); xnode^.next^.val :=106;
#ifndef NOFLOAT
  r1.c1:='w';  r1.x:=20.3;
  vec1[10] := -996;  vec2[zero].x := -300;
#else
  r1.c1:='w';
  vec1[10] := -996;
#endif
  rec1.k[zero]:=-421;  rec1.s :=[];
  s1:=[];  s2:=[red];

#ifndef NOFLOAT
  tst4501(l1, xr, xb, xc, xar, xnode, r1, vec1, vec2, rec1, 
#else
  tst4501(l1, xb, xc, xar, xnode, r1, vec1, vec2, rec1, 
#endif
           s1, s2, rec1.k, rec1.k[zero], xnode^.next^.val);;
  t:=45;

  if l1<>0 then e(1);
#ifndef NOFLOAT
  if xr<> 0 then e(2);
#endif
  if xb <> true then e(3);
  if xc <> ' ' then e(4);
  if (xar[1,1]^ <> [])  or (xar[2,2]^ <> []) then e(5);
  if xar[1,2]^ <> [yellow] then e(6);
  if (xnode^.val <> 0) or (xnode^.next^.val <> 445) then e(7);
#ifndef NOFLOAT
  if (r1.c1 <> ' ') or (r1.x <> 0) then e(8);
#else
  if (r1.c1 <> ' ') then e(8);
#endif
  if vec1[10] <> 0 then e(9);
#ifndef NOFLOAT
  if vec2[zero].x <> 0 then e(10);
#endif
  if (rec1.k[zero] <> 223) or (rec1.s <> []) then e(11);
  if (s1 <> []) or (s2 <> [red]) then e(12);
end;




begin ect:=0; pct:=0;
  tst40; tst41; tst42; tst43; tst44; tst45;
  write('Program t4:',pct:3,' tests completed.');
  writeln('Number of errors = ',ect:1);
end.
