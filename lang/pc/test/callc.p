#
program callc(input,output) ;
var success: integer ;
procedure rcsid ; begin writeln('$Id$') end ;
function kwad(val:integer) : integer ; extern ;
procedure cmain ; extern ;
procedure incs ; begin success:=success+1 end ;
procedure pptr( function ptwice(val:integer):integer ) ; extern ;
#ifndef NOFLOAT
function ceval( function pinside(val:integer):real ): boolean ; extern ;
function outside(val:integer):real ;
begin
	outside:= 1.411
end ;
procedure envellop ;
var testval: integer ;
function  inside(val:integer):real ;
begin
	if testval<>1234 then writeln('The static link is incorrect')
		 else success:=success+1 ;
	inside:=sqrt(val)
end ;
begin
	testval:=1234 ;
	if ceval(inside) then success:=success+1
		 else writeln('Calling inside through C doesn''t work');
	if ceval(outside) then success:=success+1
		 else writeln('Calling outside through C doesn''t work')
end;
#endif
procedure cptr( function pkwad(val:integer):integer ) ;
begin
	if ( pkwad(-2)<>4 ) and (pkwad(-8)<>64) then
		writeln('Using C function pointers in Pascal doesn''t work')
	else
		success:=success+1
end ;
function twice(val:integer) : integer ;
begin
	twice:= 2*val
end ;
begin
	success:=0 ;
	if (kwad(2)<>4) and (kwad(8)<>64) then
		writeln('C cals don''t work')
	else
		success:=success+1 ;
	cmain;
	pptr(twice) ;
#ifndef NOFLOAT
	envellop ;
#endif
	if success <>
#ifdef NOFLOAT
	4
#else
	7
#endif
	then writeln('Only ',success,' tests passed')
		      else writeln('All tests passed')
end.
