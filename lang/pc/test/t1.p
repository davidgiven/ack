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

program t1(input,output);

{ This program can be used to test out PASCAL compilers }

const 
   rcsversion='$Id$';
   ONE=1;  TWO=2;  TEN=10; FIFTY=50; MINONE=-1;
#ifndef NOFLOAT
   RR1=1.0; RR1H=1.5; RR2=2.0; RR3=3.0; RR4=4.0; RRMINONE=-1.0; 
#endif
   yes=true; no=false;
   kew='q';
#ifndef NOFLOAT
   eps = 2.0e-7;  { This constant is machine dependent }
#endif

type wavelength = (red,blue,yellow,purple,white,gray,pink,black,fuchia,maple,
             violet,darkred,darkblue,darkyellow,darkwhite,darkpink,darkblack);
#if EM_WSIZE < 4
  ww2= 1939..1945;
#else
  ww2= 1000939..1000945;
#endif
#ifndef NOFLOAT
  tp2=  record c1:char; i,j:integer; p:boolean; x:real end;
#else
  tp2=  record c1:char; i,j:integer; p:boolean end;
#endif
  single= array [0..0] of integer;
  spectrum= set of wavelength;
  np = ^node;
  node = record val:integer; next: np end;

var t,pct,ect:integer;
 i,j,k,l,m:integer;
#ifndef NOFLOAT
 x,y,z:real;
#endif
 p,q,r:boolean;
 c1,c2,c3:char;
#if EM_WSIZE < 4
 sr1,sr2,sr3: 1939..1945;
#else
 sr1,sr2,sr3: 1000939..1000945;
#endif
 bar: packed array[0..3] of 0..255;
 color,hue,tint: wavelength;
 grat:spectrum;
 a1: array [-10..+10] of integer;
#ifndef NOFLOAT
 a2: array [ww2] of real;
#endif
 a3: array[wavelength] of boolean;
 a4: array[(mouse,house)] of char;
 a5: array[50..52,(bat,cat),boolean,ww2] of integer;
 a6: packed array[0..10,0..3,0..3] of char;
 r1,r2: tp2;
#ifndef NOFLOAT
 r3: packed record c1:char; i,j:integer; p:boolean; x:real end;
#else
 r3: packed record c1:char; i,j:integer; p:boolean end;
#endif
 colors: set of wavelength;
 beasts: set of (pig,cow,chicken,farmersdaughter);
 bits: set of 0..1;
 p1: ^integer;
 p2: ^tp2;
 p3: ^single;
 p4: ^spectrum;
 head,tail: np;



procedure e(n:integer); 
begin 
  ect := ect + 1;
  writeln(' Error', n:3,' in test ', t) 
end;




function inc(k:integer):integer; begin inc := k+1 end;



{************************************************************************}
procedure tst1;
{ Arithmetic on constants }
begin t:=1; pct := pct + 1;
  if 1+1 <> 2 then e(1);
  if ONE+ONE <> TWO then e(2);
  if ONE+MINONE <> 0 then e(3);
  if ONE-TWO <> MINONE then e(4);
  if TWO-MINONE <> 3 then e(5);
  if TWO*TWO <> 4 then e(6);
  if 100*MINONE <> -100 then e(7);
  if 50*ONE <> 50 then e(8);
  if 50*9 <> 450 then e(9);
  if 50*TEN <> 500 then e(10);
  if 60 div TWO <> 30 then e(11);
  if FIFTY div TWO <> 25 then e(12);
  if -2 div 1 <> -2 then e(13);
  if -3 div 1 <> -3 then e(14);
  if -3 div 2 <> -1 then e(15);
  if ((1+2+3) * (2+3+4) * (3+5+5)) div 2 <> ((3 * ((5+3+2)*10)+51)*6) div 6
       then e(16);
#if EM_WSIZE < 4
  if (1000*2 + 5*7 + 13) div 8 <> 2*2*2*2*4*4 then e(17);
#else
  if (1000*2 + 5*7 + 13) * 128 div 8 <> 2*2*2*2*4*4*128 then e(17);
#endif
  if (1 * 2 * 3 * 4 * 5 * 6 * 7) div 5040  <> 
      5040 div 7 div 6 div 5 div 4 div 3 div 2 then e(18);
  if -(-(-(-(-(-(-(-(-(1))))))))) <> -1 then e(19);
  if -1 -1 -1 -1 -1 <> -5 then e(20);
  if -                          1 <> -(((((((((((((1))))))))))))) then e(21);
  if -4 * (-5) <> 20 then e(22);
  if (9999-8) mod 97 <> 309 mod 3 then e(23);
  if 2<1 then e(24);
  if 2 <= 1 then e(25);
  if 2 = 3 then e(26);
  if 2 <> 2 then e(27);
  if 2 >= 3 then e(28);
  if 2 > 3 then e(29);
  if 2+0 <> 2 then e(30);
  if 2-0 <> 2 then e(31);
  if 2*0 <> 0 then e(32);
  if 0+2 <> 2 then e(33);
  if 0-2 <> -2 then e(34);
  if 0*2 <> 0 then e(35);
  if 0 div 1 <> 0 then e(36);
  if -0 <> 0 then e(37);
  if 0 - 0 <> 0 then e(38);
  if 0 * 0 <> 0 then e(39);
end;

{************************************************************************}
procedure tst2;
{ Arithmetic on global integer variables }
begin t:=2; pct := pct + 1;
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
  if j < i then e(17);
  if j <= i then e(18);
  if j = i then e(19);
  if j <> j then e(20);
  if i >= j then e(21);
  if i > j then e(22);
end;

#ifndef NOFLOAT

{************************************************************************}
procedure tst3;
{ Real arithmetic }
begin t:=3; pct := pct + 1;
  if abs(1.0+1.0-2.0) > eps then e(1);
  if abs(1e10-1e10) > eps then e(2);
  if abs(RR1+RR1H+RR2+RR3+RR4+RRMINONE-10.5) > eps then e(3);
  if abs(1.0e-1 * 1.0e1 - 100e-2) > eps then e(4);
  if abs(10.0/3.0*3.0/10.0-100e-2) > eps then e(5);
  if 0.0e0 <> 0 then e(6);
  if abs(32767.0-32767.0) > eps then e(7);
  if abs(1.0+2+5+3.0e0+5.0e+0+140e-1-30.000)/100 > eps then e(8);
  if abs(-1+(-1)+(-1.0)+(-1e0)+(-1e+0)+(-1e-0) + ((((6)))) ) > eps then e(9);

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


{************************************************************************}
procedure tst4;
{ Boolean expressions }
begin t:=4; pct := pct + 1;
  if not yes = true then e(1);
  if not no = false then e(2);
  if yes = no then e(3);
  if not true = not false then e(4);
  if true and false then e(5);
  if false or false then e(6);

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

{************************************************************************}
procedure tst5;
{ Characters, Subranges, Enumerated types }
begin t:=5; pct := pct + 1;
  if 'q' <> kew then e(1);
  c1 := 'a'; c2 := 'b'; c3 := 'a';
  if c1 = c2 then e(2);
  if c1 <> c3 then e(3);

#if EM_WSIZE < 4
  sr1:=1939; sr2:=1945; sr3:=1939;
#else
  sr1:=1000939; sr2:=1000945; sr3:=1000939;
#endif
  if sr1=sr2 then e(4);
  if sr1<>sr3 then e(5);

  color := yellow; hue := blue; tint := yellow;
  if color = hue then e(6);
  if color <> tint then e(7);
end;


{************************************************************************}
procedure tst6;
{ Global arrays }
var i,j,k:integer;
begin t:=6; pct := pct + 1;
  for i:= -10 to 10 do a1[i] := i*i;
  if (a1[-10]<>100) or (a1[9]<>81) then e(1);

#ifndef NOFLOAT
#if EM_WSIZE < 4
  for i:=1939 to 1945 do a2[i]:=i-1938.5;
  if (abs(a2[1939]-0.5) > eps) or (abs(a2[1945]-6.5) > eps) then e(2);
#else
  for i:=1000939 to 1000945 do a2[i]:=i-1000938.5;
  if (abs(a2[1000939]-0.5) > eps) or (abs(a2[1000945]-6.5) > eps) then e(2);
#endif
#endif

  color := yellow;
  a3[blue] := true;  a3[yellow] := true;
  if (a3[blue]<>true) or (a3[yellow]<>true) then e(3);
  a3[blue] := false;  a3[yellow] := false;
  if (a3[blue]<>false) or (a3[yellow]<>false) then e(4);

  a4[mouse]:='m'; a4[house]:='h';
  if (a4[mouse] <> 'm') or (a4[house]<>'h' ) then e(5);

#if EM_WSIZE < 4
  for i:=1939 to 1945 do a5[51,bat,false,i]:=300+i;
  if a5[51,bat,false,1940] <> 2240 then e(6);
  for i:=50 to 52 do a5[i,cat,true,1943]:=200+i;
  if (a5[50,cat,true,1943] <> 250) or (a5[52,cat,true,1943] <> 252) then e(7);
#else
  for i:=1000939 to 1000945 do a5[51,bat,false,i]:=300+i;
  if a5[51,bat,false,1000940] <> 1001240 then e(6);
  for i:=50 to 52 do a5[i,cat,true,1000943]:=200+i;
  if (a5[50,cat,true,1000943] <> 250) or (a5[52,cat,true,1000943] <> 252) then e(7);
#endif

  for i:= -10 to 10 do a1[i]:= 0;
  for i:= 0 to 10 do a1[i div 2 + i div 2]:= i+1;
  if(a1[0]<>2) or (a1[5]<>0) or (a1[8]<>10) then e(8);

  for i:= 0 to 10 do
  for j:= 0 to 3 do
  for k:= 0 to 3 do
   if ( (i+j+k) div 2) * 2 = i+j+k then a6[i,j,k]:='e' else a6[i,j,k]:='o';
   if (a6[2,2,2]<>'e') or (a6[2,2,3]<>'o') or (a6[0,3,1]<>'e') then e(9);
end;


#ifndef NOFLOAT

{************************************************************************}
procedure tst7;
{ Global records }
begin t:=7; pct := pct + 1;
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

{************************************************************************}
procedure tst7;
{ Global records }
begin t:=7; pct := pct + 1;
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


{************************************************************************}
procedure tst8;
{ Global sets }
begin t:=8; pct := pct + 1;
  colors := [];
  colors := colors + [];
  if colors <> [] then e(1);
  colors := colors + [red];
  if colors <> [red] then e(2);
  colors := colors + [blue];
  if colors <> [red,blue] then e(3);
  if colors <> [blue,red] then e(4);
  colors := colors - [red];
  if colors <> [blue] then e(5);
  beasts := [chicken] + [chicken,pig];
  if beasts <> [pig,chicken] then e(6);
  beasts := [] - [farmersdaughter] + [cow] - [cow];
  if beasts <> [] then e(7);
  bits := [0] + [1] - [0];
  if bits <> [1] then e(8);
  bits := [] + [] + [] -[] + [0] + [] + [] - [0];
  if bits <> [] then e(9);
  if not ([] <= [red]) then e(10);
  if [red] >= [blue] then e(11);
  if [red] <= [blue] then e(12);
  if [red] = [blue] then e(13);
  if not ([red] <= [red,blue]) then e(14);
  if not ([red,blue] <= [red,yellow,blue]) then e(15);
  if not ([blue,yellow] >= [blue] + [yellow]) then e(16);
  grat := [ red,blue,yellow,purple,white,gray,pink,black,fuchia,maple,
           violet,darkred,darkblue,darkyellow,darkwhite,darkpink,darkblack];
  if grat<>[red,blue,yellow,purple,white,gray,pink,black,fuchia,maple,violet,
   darkred,darkblue,darkyellow,darkwhite,darkpink,darkblack] then e(17);
  if not ([10] <= [10]) then e(18);
end;


{************************************************************************}
procedure tst9;
{ Global pointers }
begin t:=9; pct := pct + 1;
  new(p1); new(p2); new(p3); new(p4);
#if EM_WSIZE < 4
  p1^ := 1066;
  if p1^ <> 1066 then e(1);
#else
  p1^ := 1000066;
  if p1^ <> 1000066 then e(1);
#endif
  p2^.i := 1215;
  if p2^.i <> 1215 then e(2);
  p3^[0]:= 1566;
  if p3^[0] <> 1566 then e(3);
  p4^ := [red];
  if p4^ <> [red] then e(4);
end;


{************************************************************************}
procedure tst10;
{ More global pointers }
var i:integer;
begin t:=10; pct := pct + 1;
  head := nil;
  for i:= 1 to 100 do
    begin new(tail); tail^.val:=100+i; tail^.next :=head; head:= tail end;
  if (tail^.val<>200) or (tail^.next^.val<>199) then e(1);
  if tail^.next^.next^.next^.next^.next^.next^.next^.next^.val<> 192 then e(2);
  tail^.next^.next^.next^.val := 30;
  if tail^.next^.next^.next^.val <> 30 then e(3);
end;


{************************************************************************}
 procedure tst11;
 { Arithmetic on local integer variables }
 var i,j,k,l,m:integer;
 begin t:=11; pct := pct + 1;
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

{************************************************************************}
 procedure tst12;
 { Real arithmetic on locals }
 var x,y,z:real;
 begin t:=12; pct := pct + 1;

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


{************************************************************************}
 procedure tst13;
 { Boolean expressions using locals }
 var pp,qq,rr:boolean;
 begin t:=13; pct := pct + 1;
  if not yes = true then e(1);
  if not no = false then e(2);
  if yes = no then e(3);
  if not true = not false then e(4);
  if true and false then e(5);
  if false or false then e(6);

  pp:=true; qq:=true; rr:=false;
  if not pp then e(7);
  if rr then e(8);
  if pp and rr then e(9);
  if pp and not qq then e(10);
  if not pp or not qq then e(11);
  if (pp and rr) or (qq and rr) then e(12);
  if pp and qq and rr then e(13);
  if (pp or qq) = rr then e(14);
 end;

{************************************************************************}
 procedure tst14;
 { Characters, Subranges, Enumerated types using locals }
 var cc1,cc2,cc3:char;
#if EM_WSIZE < 4
   sr1,sr2,sr3: 1939..1945;
#else
   sr1,sr2,sr3: 1000939..1000945;
#endif
   color,hue,tint: (ochre,magenta);
 begin t:=14; pct := pct + 1;
  if 'q' <> kew then e(1);
  cc1 := 'a'; cc2 := 'b'; cc3 := 'a';
  if cc1 = cc2 then e(2);
  if cc1 <> cc3 then e(3);

#if EM_WSIZE < 4
  sr1:=1939; sr2:=1945; sr3:=1939;
#else
  sr1:=1000939; sr2:=1000945; sr3:=1000939;
#endif
  if sr1=sr2 then e(4);
  if sr1<>sr3 then e(5);
  bar[0]:=200;  bar[1]:=255;  bar[2]:=255; bar[3]:=203;
  if (bar[0]<>200) or (bar[1]<>255) or (bar[2]<>255) or (bar[3]<>203) then e(6);

  color := ochre; hue:=magenta; tint := ochre;
  if color = hue then e(7);
  if color <> tint then e(8);
 end;


{************************************************************************}
 procedure tst15;
 { Local arrays }
 type colour = (magenta,ochre);
 var aa1: array [-10..+10] of integer;
#ifndef NOFLOAT
    aa2: array [ww2] of real;
#endif
    aa3: array[colour] of boolean;
    aa4: array[(mouse,house,louse)] of char;
    aa5: array[50..52,(bat,cat),boolean,ww2] of integer;
    aa6: packed array[0..10,0..3,0..3] of char;
    i,j,k:integer;
 begin t:=15; pct := pct + 1;
  for i:= -10 to 10 do aa1[i] := i*i;
  if (aa1[-10]<>100) or (aa1[9]<>81) then e(1);

#ifndef NOFLOAT
#if EM_WSIZE < 4
  for i:=1939 to 1945 do aa2[i]:=i-1938.5;
  if (abs(aa2[1939]-0.5) > eps) or (abs(aa2[1945]-6.5) > eps) then e(2);
#else
  for i:=1000939 to 1000945 do aa2[i]:=i-1000938.5;
  if (abs(aa2[1000939]-0.5) > eps) or (abs(aa2[1000945]-6.5) > eps) then e(2);
#endif
#endif

  aa3[magenta] := true;  aa3[ochre] := true;
  if (aa3[magenta]<>true) or (aa3[ochre]<>true) then e(3);
  aa3[magenta] := false;  aa3[ochre] := false;
  if (aa3[magenta]<>false) or (aa3[ochre]<>false) then e(4);

  aa4[mouse]:='m'; aa4[house]:='h';  aa4[louse]:='l';
  if (aa4[mouse] <> 'm') or (aa4[house]<>'h' ) or (aa4[louse]<>'l') then e(5);

#if EM_WSIZE < 4
  for i:=1939 to 1945 do aa5[51,bat,false,i]:=300+i;
  if aa5[51,bat,false,1940] <> 2240 then e(6);
  for i:=50 to 52 do aa5[i,cat,true,1943]:=200+i;
  if (aa5[50,cat,true,1943] <> 250) or (aa5[52,cat,true,1943] <> 252) then e(7);
#else
  for i:=1000939 to 1000945 do aa5[51,bat,false,i]:=300+i;
  if aa5[51,bat,false,1000940] <> 1001240 then e(6);
  for i:=50 to 52 do aa5[i,cat,true,1000943]:=200+i;
  if (aa5[50,cat,true,1000943] <> 250) or (aa5[52,cat,true,1000943] <> 252) then e(7);
#endif

  for i:= -10 to 10 do aa1[i]:= 0;
  for i:= 0 to 10 do aa1[i div 2 + i div 2]:= i+1;
  if(aa1[0]<>2) or (aa1[5]<>0) or (aa1[8]<>10) then e(8);

  for i:= 0 to 10 do
  for j:= 0 to 3 do
  for k:= 0 to 3 do
    if ( (i+j+k) div 2) * 2 = i+j+k then aa6[i,j,k]:='e' else aa6[i,j,k]:='o';
  if (aa6[2,2,2]<>'e') or (aa6[2,2,3]<>'o') or (aa6[0,3,1]<>'e') then e(9);
 end;


#ifndef NOFLOAT

{************************************************************************}
 procedure tst16;
 { Local records }
 var r1,r2: tp2;
     r3: packed record c1:char; i,j:integer; p:boolean; x:real end;
 begin t:=16; pct := pct + 1;
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
{************************************************************************}
 procedure tst16;
 { Local records }
 var r1,r2: tp2;
     r3: packed record c1:char; i,j:integer; p:boolean end;
 begin t:=16; pct := pct + 1;
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

{************************************************************************}
 procedure tst17;
 { Local sets }
 var colors: set of (pink,green,orange,red);
     beasts: set of (pig,cow,chicken,farmersdaughter);
     bits: set of 0..1;
 begin t:=17; pct := pct + 1;
  colors := [];
  colors := colors + [];
  if colors <> [] then e(1);
  colors := colors + [pink];
  if colors <> [pink] then e(2);
  colors := colors + [green];
  if colors <> [pink,green] then e(3);
  if colors <> [green,pink] then e(4);
  colors := colors - [pink,orange];
  if colors <> [green] then e(5);
  beasts := [chicken] + [chicken,pig];
  if beasts <> [pig,chicken] then e(6);
  beasts := [] - [farmersdaughter] + [cow] - [cow];
  if beasts <> [] then e(7);
  bits := [0] + [1] - [0];
  if bits <> [1] then e(8);
  bits := [] + [] + [] + [0] + [] + [0];
  if bits <> [0] then e(9);
  if ord(red) <> 3 then e(10);
 end;


{************************************************************************}
 procedure tst18;
 { Local pointers }
    type rainbow = set of (pink,purple,chartreuse);
    var p1: ^integer;
    p2: ^tp2;
    p3: ^single;
    p4: ^rainbow;
 begin t:=18; pct := pct + 1;
  new(p1); new(p2); new(p3); new(p4);
  p1^ := 1066;
  if p1^ <> 1066 then e(1);
  p2^.i := 1215;
  if p2^.i <> 1215 then e(2);
  p3^[0]:= 1566;
  if p3^[0] <> 1566 then e(3);
  p4^ := [pink] + [purple] + [purple,chartreuse] - [purple];
  if p4^ <> [pink,chartreuse] then e(4);
 end;


{************************************************************************}
 procedure tst19;
 var head,tail: np; i:integer;
 begin t:=19; pct := pct + 1;
  head := nil;
  for i:= 1 to 100 do
    begin new(tail); tail^.val:=100+i; tail^.next :=head; head:= tail end;
  if (tail^.val<>200) or (tail^.next^.val<>199) then e(1);
  if tail^.next^.next^.next^.next^.next^.next^.next^.next^.val<> 192 then e(2);
  tail^.next^.next^.next^.val := 30;
  if tail^.next^.next^.next^.val <> 30 then e(3);
 end;

#ifndef NOFLOAT

{************************************************************************}
procedure tst20;
{ Mixed local and global }
var li:integer;
    lx:real;
begin t:=20; pct := pct + 1;
  li:=6;  i:=li;  if i<>6 then e(1);
  i:=6;  li:=i;  if li <> 6 then e(2);
  lx := 3.5;  x:=lx;  if x <> 3.5 then e(3);
  x:= 4.5;  lx:= x;  if lx <> 4.5 then e(4);
end;

#else
{************************************************************************}
procedure tst20;
{ Mixed local and global }
var li:integer;
begin t:=20; pct := pct + 1;
  li:=6;  i:=li;  if i<>6 then e(1);
  i:=6;  li:=i;  if li <> 6 then e(2);
end;

#endif


{************************************************************************}

{ Main Program }
begin ect := 0;  pct := 0;
#ifndef NOFLOAT
tst1;	tst2;	tst3;	tst4;	tst5;	tst6;	tst7;	tst8;
tst9;	tst10;	tst11;	tst12;	tst13;	tst14;	tst15;	tst16;
tst17;	tst18;	tst19;	tst20;

#else

tst1;	tst2;	tst4;	tst5;	tst6;	tst7;	tst8;
tst9;	tst10;	tst11;	tst13;	tst14;	tst15;	tst16;
tst17;	tst18;	tst19;	tst20;

#endif
write('Program t1:',pct:3,' tests completed.');
writeln('Number of errors = ',ect:1);
end.
