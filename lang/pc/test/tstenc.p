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
program tstenc(output);
const   rcsversion='$Id$';
	trapno=150;
var     level:integer;
	beenhere:boolean;
	e:integer;
procedure trap(erno:integer); extern;
procedure encaps(procedure p;procedure q(erno:integer)); extern;
procedure p1;
    label   1;
    var     plevel:integer;
    procedure p2;
	var     plevel:integer;
	begin plevel:=3 ; trap(trapno) ;
	  writeln('executing unreachable code in p2') ; e:=e+1 ;
	end;
   procedure q2(no:integer);
	var     qlevel:integer;
	begin qlevel:=-3 ;
	  if no<>trapno then
	    begin writeln('wrong trapno ',no,' in q2'); e:=e+1 end ;
	  if plevel<>2 then
	    begin writeln('wrong level ',plevel,' in q2'); e:=e+1 end ;
	  trap(trapno) ;
	  goto 1;
	  writeln('executing unreachable code in q2') ; e:=e+1 ;
	end;
    begin plevel:=2 ;  encaps(p2,q2) ;
      writeln('executing unreachable code in p1'); e:=e+1;
1:    if plevel<>2 then
	begin writeln('wrong level ', plevel, 'in p1') ; e:=e+1 end ;
      beenhere:=true ;
    end; { body of p1 }
procedure q1(no:integer);
    var     qlevel:integer;
    begin qlevel:=-2 ;
      if no<>trapno then
	begin writeln('wrong trapno ',no,' in q1'); e:=e+1 end ;
      if level<>1 then
	begin writeln('wrong level ',level,' in q1'); e:=e+1 end ;
    end;
begin
  level:=1 ;
  e:=0 ;
  beenhere:=false ;
  encaps(p1,q1);
  if not beenhere then
    begin writeln('illegaly skipped code in p1') ; e:=e+1 end;
  if e=0 then writeln('encaps OK')
end.
