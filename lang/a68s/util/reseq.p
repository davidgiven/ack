(*                            reseq.p                                        *)
(*                            *******                                        *)

(*  A program to renumber a text file. To use this utility type :
    reseq <file1 >file2 start step
    to  create file2 as a renumbered version of file1, starting 
    with line number start, with increments of step.
    NOTE : file1 and file2 had better be different !!!                       *)


(*  Version 1.1  written Friday 31 July 1987 by Jon Abbott.                  *)

program reseq(input,output);

type buf = packed array [1..10] of char;
     string = ^ buf;

var c : char;
    start,step,i : integer;
    numbered : boolean;

  function argc: integer; extern;
  function argv(i: integer): string; extern;

  procedure number;
  begin
    if i<10 then write('0000',i:1)
    else if i<100 then write('000',i:2)
    else if i<1000 then write('00',i:3)
    else if i<10000 then write('0',i:4)
    else write(i:5);
    if not numbered then write('  ');
    if not eoln then write(c);
    while not eoln do
    begin
      read(c);
      write(c)
    end;
    readln;
    writeln;
    i:=i+step
  end;

  function getarg(n:integer) : integer;
  var s : string;
      i,g :integer;
  begin
    s := argv(n);
    i:=1;
    while (not (s^[i] in ['0'..'9'])) and (i<10) do i:=i+1;
    g := 0;
    if not (s^[i] in ['0'..'9']) then g := 100
    else
    while (i<11) and (s^[i] in ['0'..'9']) do begin
      g := g*10+ord(s^[i])-ord('0');
      i := i+1
    end;
    getarg := g
  end;

begin
  start := 100;
  step := 10;
  if argc>1 then start := getarg(1);
  if argc>2 then step := getarg(2);
  if argc>3 then
    writeln('Syntax : reseq <file1 >file2 start step : subsequent args ignored');
  read(c);
  numbered := (c in ['0'..'9']);
  i := start;
  while not eof do
  begin
    if numbered then
      while c in ['0'..'9'] do
        read(c);
    number;
    if not eof then
      if  not eoln then read(c)
  end
end.
