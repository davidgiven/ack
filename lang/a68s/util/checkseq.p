program checkseq(output);

(* Rewritten to allow a list of files to be passed in on the command line *)
(*   This version : 24 August 1987 by Jon Abbott *)

type buf = packed array [1..20] of char;
     string = ^buf;

var
  this, last, nargs: integer;
  s: string;
  inf: text;

  function argc: integer; extern;
  function argv(i: integer): string; extern;
  procedure popen(var f: text; s: string); extern;

begin
  nargs := argc;
  while nargs>1 do
  begin
    nargs := nargs-1;
    s := argv(nargs);
    popen(inf,s);
    writeln('checkseq: ',s^);
    this := 0;
    while not eof(inf) do
    begin
      last := this;
      readln(inf,this);
      if this <= last then writeln(last, this)
    end
  end
end.
