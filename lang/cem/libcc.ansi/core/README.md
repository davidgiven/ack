This directory contains the libc core: functions here may depend on other
libc functions _only_ (so, no `unistd.h` stuff). Plats don't get to configure
this, so nothing here should add any overhead if the function isn't linked.

Examples of what goes here:

  - `strcmp()` --- because it's pure code.
  - `setjmp()` --- in EM, it's portable.
  - `assert()` --- because it only calls other core functions (`fputs()`
    and `abort()`).

Examples of what doesn't go here:

  - `malloc()` --- because it calls the `unistd.h` function `sbrk()` (and so a
    plat might want to swap it out).
  - stdio --- because it calls the `unistd.h` functions `read()`, `write()`
    etc.
  - `signal()` --- because it can't be implemented portably and needs to go
    in the plat.
