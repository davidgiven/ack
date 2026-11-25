# ack-6800

Porting of the Amsterdam Compiler Kit to the Motorola MC6800 is underway.
The target environment is emu6800 from the Fuzix Compiler Kit.

## Current status

Results for the ACK regression tests (run on emu6800).

- `core` — All 42 tests passed.
- `bugs` — All 6 tests passed.
- `m2` — All 6 tests passed.
- `basic` — The `DATA` test stopped at a floating-point operation.
- `floats` — Floating point is not supported. The 6 tests were not run.
- `long-long` — `long long` is not supported. All 8 tests failed to compile.

It is still incomplete, with work remaining especially for languages other than C.

In the emu6800 environment (-me6800), support for stdin, stdout, and stderr has been added.
Currently, examples/hilo.c and paranoia.c are working.
On emu6800, no file system, so FILE I/O is unavailable.

The implementation has passed the tarai (Takeuchi) function test.
Below are the execution cycles compared to other C compilers for the 6800.

```
27,965,947,146  ack
26,566,321,956	CC68
21,832,185,026	Fuzix CC
16,339,666,935	chibicc-6800 -O2
23,601,741,099	chibicc-6800 -Os
```

Test program:

```
int tarai(int x, int y, int z)
{

	if (x>y){
		return tarai(
			tarai(x-1,y,z),
			tarai(y-1,z,x),
			tarai(z-1,x,y));
	}
	return y;
}

int main(int argc, char **argv)
{
	if(tarai(13,7,0)!=13)		// call 91924989 times
		return 1;

	return 0;
}
```

Now, passed 8queen and knight tour, Mandelbrot(integer version).

- https://github.com/zu2/chibicc-6800-v1/blob/main/ztest/9001-8queen.c
- https://github.com/zu2/chibicc-6800-v1/blob/main/ztest/9006-knight.c
- https://github.com/zu2/chibicc-6800-v1/blob/main/ztest/9017-asciiart.c

```
emu6800 -d 6800 e6800.img /dev/null
CPU cycles = 210
111111111111111111222222222333334568BC67443322222211111111111000000000000000000
11111111111111111122222222233344598C  77943333222221111111110000000000000000000
111111111111112222222233324444556       955433332211111111100000000000000000000
111111111211112222222333455665778       976554444222211111100000000000000000000
11111112222222233333334457 AB9              787B5432111111111100000000000000000
11112222222222333333444667                       532222111111111000000000000000
11111222333444444444555A                       96443322211111110000000000000000
12222223345D6657 6555679                        AA43322211111110000000000000000
222233334569  8C  E8789                          B43322111111110000000000000000
223333345578D        E                            43322211111111111100000000000
3344444789A                                      543322111111111111100000000000
5555658A                                       C6433222222111111111100000000000
                                              975443322221111111111100000000000
5555658A                                       C6433222222111111111100000000000
3344444789A                                      543322111111111111100000000000
223333345578D        E                            43322211111111111100000000000
222233334569  8C  E8789                          B43322111111110000000000000000
12222223345D6657 6555679                        AA43322211111110000000000000000
11111222333444444444555A                       96443322211111110000000000000000
11112222222222333333444667                       532222111111111000000000000000
11111112222222233333334457 AB9              787B5432111111111100000000000000000
111111111211112222222333455665778       976554444222211111100000000000000000000
111111111111112222222233324444556       955433332211111111100000000000000000000
11111111111111111122222222233344598C  77943333222221111111110000000000000000000
111111111111111111222222222333334568BC67443322222211111111111000000000000000000
CPU cycles = 112374329
```

Cycle time of Mandelbrot.

- 112,374,119 ack-6800
- 133,893,286 CC68
- 146,818,028 Fuzix CC
-  90,918,135 chibicc-6800 -O2

## Other language

- BASIC: PRINT works fine, but INPUT is still acting up.
- Pascal: All tests pass ( tests/plat/core/ ).
- Modula-2: All tests pass ( tests/plat/m2/ ).

## TODO

- Add essential source files under plat/e6800/
- Avoid generating redundant ldx LB/LBl instructions.
- Support routines need to be extended further. 
- Other languages: Compilation fails due to insufficient library support.

## DONE

- The directory was renamed from mach/6800 to mach/mc6800, though it is not clear if this is the best approach.
  - Renamed to prevent the program from treating names starting with 'em' as EM code platforms.
- Compilation of some test program.

## Note on Disk Space

This project generates a large number of files, which can lead to i-node exhaustion on smaller disks. Please consider allocating extra disk space as needed; for reference, I am using a 64GB disk.

## Install & compile

To participate in development, follow these steps to test code generation. The environment assumed is Ubuntu Linux.

```
$ sudo apt install git make gcc pkg-config lua5.4 flex bison
$ git clone https://github.com/zu2/ack-6800.git
$ cd ack-6800
$ make
(debug:  make V=1 -j1)
$ make install
$ export PATH=/opt/pkg/ack/bin:$PATH
$ cd examples
$ ack  -me6800 -v -v  hilo.c
$ emu6800 6800   e6800.img /dev/null 
```

By default, 'make' builds for all CPUs and platforms, leading to long build times. To compile only mc6800/e6800, update the Makefile as shown below.

```
@@ -10,8 +10,9 @@ DEFAULT_PLATFORM ?= pc86
 # Which architectures should get built?
 
 $(if $(PLATS), $(error Don't set PLATS on the command line, because reasons. Edit the Makefile instead.))
-PLATS = all
+#PLATS = all
 # PLATS = linux386 linuxppc linuxmips
+PLATS = e6800
 
 # Where should the ACK put its temporary files?
```

During the initial 'make', build.py scans for compilation dependencies and generates the Makefile.
When files are added or removed, regenerating the Makefile requires a 'make clean'.
This is just my guess; there might be a better approach.


The following patch is required to use stdin on emu6800.

```
--- a/test/emu6800.c
+++ b/test/emu6800.c
@@ -5,6 +5,7 @@
 #include <unistd.h>
 #include <fcntl.h>
 #include "6800.h"
+#include <termios.h>
 
 static uint8_t ram[65536];
 
@@ -76,6 +77,19 @@ uint8_t m6800_debug_read(struct m6800 *cpu, uint16_t addr)
 
 uint8_t m6800_read(struct m6800 *cpu, uint16_t addr)
 {
+       switch(addr) {
+           case 0xFEFE: {
+               struct termios old,new;
+               int c;
+               tcgetattr(STDIN_FILENO, &old);
+               new = old;
+               new.c_lflag &= ~(ICANON);
+               tcsetattr(STDIN_FILENO, TCSANOW, &new);
+               c = getchar();
+               tcsetattr(STDIN_FILENO, TCSANOW, &old);
+               return c;
+           }
+       }
        return m6800_read_op(cpu, addr, 0);
 }
```

### debugging

make map file:
  map file: ack -e6800 -c.out test.c
  anm test.out > test.map

disassemble:
  f9dasm -6800 e6800.img

run with disassembler:
  emu6800 -d 6800 e6800.img /dev/null 

### links

emu6800: https://github.com/EtchedPixels/Fuzix-Compiler-Kit/test/
f9dasm:  https://github.com/Arakula/f9dasm


The following is the original README.

- [zu2/ack-6800/README](https://github.com/zu2/ack-6800/blob/default/README)
- https://github.com/davidgiven/ack/blob/default/README
