program tstgto(output);
type int=integer;
     pint=^integer;
var ga0,ga1,ga2,ga3,ga4,ga5:int;
    gp0,gp1,gp2,gp3,gp4,gp5:pint;

procedure level0(a1,a2:int;p1,p2:pint);
label 1;
var a3,a4,a5:int;p3,p4,p5:pint;

procedure level1(a1,a2:int;p1,p2:pint);
var a3,a4,a5:int;p3,p4,p5:pint;

procedure level2(a1,a2:int;p1,p2:pint);
var a3,a4,a5:int;p3,p4,p5:pint;
begin
  a1:= -5;a2:=a1;a3:=a2;a4:=a3;a5:=a4;
  a1:= -4;a2:=a1;a3:=a2;a4:=a3;
  a1:= -3;a2:=a1;a3:=a2;
  a1:= -2;a2:=a1;
  a1:=a5+a5;a1:= -1;
  p1:=gp0;p2:=p1;p3:=p2;p4:=p3;p5:=p4;
  p1:=gp1;p2:=p1;p3:=p2;p4:=p3;
  p1:=gp2;p2:=p1;p3:=p2;
  p1:=gp3;p2:=p1;
  p1:=p5;p1:=gp4;
  goto 1;
end; { level 2 }

begin
  a1:=ga4;a2:=a1;a3:=a2;a4:=a3;a5:=a4;
  a1:=ga3;a2:=a1;a3:=a2;a4:=a3;
  a1:=ga2;a2:=a1;a3:=a2;
  a1:=ga1;a2:=a1;
  a1:=ga0;
  p1:=gp4;p2:=p1;p3:=p2;p4:=p3;p5:=p4;
  p1:=gp3;p2:=p1;p3:=p2;p4:=p3;
  p1:=gp2;p2:=p1;p3:=p2;
  p1:=gp1;p2:=p1;
  p1:=gp0;
  level2(a5,a4,p5,p4);
  writeln('Error, goto failed');
end; { level 1 }

begin
  a1:=ga5;a2:=a1;a3:=a2;a4:=a3;a5:=a4;
  a1:=ga4;a2:=a1;a3:=a2;a4:=a3;
  a1:=ga3;a2:=a1;a3:=a2;
  a1:=ga2;a2:=a1;
  a1:=ga1;
  p1:=gp5;p2:=p1;p3:=p2;p4:=p3;p5:=p4;
  p1:=gp4;p2:=p1;p3:=p2;p4:=p3;
  p1:=gp3;p2:=p1;p3:=p2;
  p1:=gp2;p2:=p1;
  p1:=gp1;
  level1(a5,a4,p5,p4);
  writeln('Error, goto failed');
1:
  if (a1 <> ga1) then writeln('level0:a1 has wrong value');
  if (a2 <> ga2) then writeln('level0:a2 has wrong value');
  if (a3 <> ga3) then writeln('level0:a3 has wrong value');
  if (a4 <> ga4) then writeln('level0:a4 has wrong value');
  if (a5 <> ga5) then writeln('level0:a5 has wrong value');
  if (p1 <> gp1) then writeln('level0:p1 has wrong value');
  if (p2 <> gp2) then writeln('level0:p2 has wrong value');
  if (p3 <> gp3) then writeln('level0:p3 has wrong value');
  if (p4 <> gp4) then writeln('level0:p4 has wrong value');
  if (p5 <> gp5) then writeln('level0:p5 has wrong value');
end; { level 0 }

begin 
  ga0:=0;ga1:=1;ga2:=2;ga3:=3;ga4:=4;ga5:=5;
  new(gp0);new(gp1);new(gp2);new(gp3);new(gp4);new(gp5);
  level0(ga5,ga4,gp5,gp4);
end.
