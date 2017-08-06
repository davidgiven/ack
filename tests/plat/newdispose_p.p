#
(*$U+ -- enables underscores in identifiers *)

program markrelease;

type
    iptr = ^integer;

var
    ptr1 : iptr;
    ptr2 : iptr;

procedure finished;
    extern;

procedure fail(line: integer);
    extern;

#define ASSERT(cond) \
    if (not (cond)) then fail(__LINE__)

begin
    New(ptr1);
    New(ptr2);
    ASSERT(ptr1 <> ptr2);

    Dispose(ptr1);
    Dispose(ptr2);
    (* Not required by the Pascal standard, but our implementation sets the
     * pointers to NULL after freeing them. *)
    ASSERT(ptr1 = ptr2);

    finished
end.
