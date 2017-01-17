#
(*$U+ -- enables underscores in identifiers *)

program pascalsets;

type
    charset = set of char;

var
    s : charset;
    i : integer;

procedure finished;
    extern;

procedure fail(line: integer);
    extern;

#define ASSERT(cond) \
    if (not (cond)) then fail(__LINE__)

begin
    s := [];
    for i := 0 to 255 do
        s := s + [chr(i)];

    i := 99; (* to defeat optimisation *)
    ASSERT(chr(42) in s);
    ASSERT(chr(142) in s);
    ASSERT(chr(i) in s);
    s := s - [chr(42)];
    ASSERT(not(chr(42) in s));
    ASSERT(chr(142) in s);
    ASSERT(chr(i) in s);

    finished
end.
