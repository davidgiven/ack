This is just a naive domestic PowerPC simulator, but I think you'll be amused
by its presumption.

The simulator implements just enough of the instruction set to make the tests
pass. Certain features aren't supported at all (and an effort has been made
to detect this and error out). The FPU is crudely approximated using the
native floating-point support, doesn't support reading and writing FPSCR, and
will almost certainly produce incorrect results. Plus, there are bugs. It's
also likely to be very, very slow.

However, it should be easily extensible and the emulator core is only about
500 lines of code.

Instructions are defined in `instructions.dat`; `mkdispatcher.lua` reads
these in and generates the instruction decoder. `emu.c` contains the main
emulator core. `main.c` contains the application front end and the incredibly
crude syscall interface.

TODO:

  - overflow bit support (instructions that try to set OV error out)
  - mtcrf
  - read string / write string
  - factor out the ELF loader, and linux68k/emu uses it too
  - floating point condition bits
  - bit-for-bit FPU emulation, although this looks like a huge amount of work

It was written from scratch for the ACK by me, David Given.
