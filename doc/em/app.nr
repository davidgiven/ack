.BP
.AP "EM INTERPRETER"
.nf
.ta 8 16 24 32 40 48 56 64 72 80
.so em.i
.fi
.BP
.AP "EM CODE TABLES"
The following table is used by the assembler for EM machine
language.
It specifies the opcodes used for each instruction and
how arguments are mapped to machine language arguments.
The table is presented in three columns,
each line in each column contains three or four fields.
Each line describes a range of interpreter opcodes by
specifying for which instruction the range is used, the type of the
opcodes (mini, shortie, etc..) and range for the instruction
argument.
.A
The first field on each line gives the EM instruction mnemonic,
the second field gives some flags.
If the opcodes are minis or shorties the third field specifies
how many minis/shorties are used.
The last field gives the number of the (first) interpreter
opcode.
.N 1
Flags :
.IS 3
.N 1
Opcode type, only one of the following may be specified.
.PS - 5 "  "
.PT -
opcode without argument
.PT m
mini
.PT s
shortie
.PT 2
opcode with 2-byte signed argument
.PT 4
opcode with 4-byte signed argument
.PT 8
opcode with 8-byte signed argument
.PE
Secondary (escaped) opcodes.
.PS - 5 "  "
.PT e
The opcode thus marked is in the secondary opcode group instead
of the primary
.PE
restrictions on arguments
.PS - 5 "  "
.PT N
Negative arguments only
.PT P
Positive and zero arguments only
.PE
mapping of arguments
.PS - 5 "  "
.PT w
argument must be divisible by the wordsize and is divided by the
wordsize before use as opcode argument.
.PT o
argument ( possibly after division ) must be >= 1 and is
decremented before use as opcode argument
.PE
.IE
If the opcode type is 2,4 or 8 the resulting argument is used as
opcode argument (least significant byte first).
.N
If the opcode type is mini, the argument is added
to the first opcode - if in range - .
If the argument is negative, the absolute value minus one is
used in the algorithm above.
.N
For shorties with positive arguments the first opcode is used
for arguments in the range 0..255, the second for the range
256..511, etc..
For shorties with negative arguments the first opcode is used
for arguments in the range -1..-256, the second for the range
-257..-512, etc..
The byte following the opcode contains the least significant
byte of the argument.
First some examples of these specifications.
.PS - 5
.PT "aar mwPo 1 34"
Indicates that opcode 34 is used as a mini for Positive
instruction arguments only.
The w and o indicate division and decrementing of the
instruction argument.
Because the resulting argument must be zero ( only opcode 34 may be used
), this mini can only be used for instruction argument 2.
Conclusion: opcode 34 is for "AAR 2".
.PT "adp sP 1 41"
Opcode 41 is used as shortie for ADP with arguments in the range
0..255.
.PT "bra sN 2 60"
Opcode 60 is used as shortie for BRA with arguments -1..-256,
61 is used for arguments -257..-512.
.PT "zer e- 145"
Escaped opcode 145 is used for ZER.
.PE
The interpreter opcode table:
.N 1
.IS 3
.DS B
.so itables
.DE 0
.IE
.P
The table above results in the following dispatch tables.
Dispatch tables are used by interpreters to jump to the
routines implementing the EM instructions, indexed by the next opcode.
Each line of the dispatch tables gives the routine names
of eight consecutive opcodes, preceded by the first opcode number
on that line.
Routine names consist of an EM mnemonic followed by a suffix.
The suffices show the encoding used for each opcode.
.N
The following suffices exist:
.N 1
.VS 1 0
.IS 4
.PS - 11
.PT .z
no arguments
.PT .l
16-bit argument
.PT .lw
16-bit argument divided by the wordsize
.PT .p
positive 16-bit argument
.PT .pw
positive 16-bit argument divided by the wordsize
.PT .n
negative 16-bit argument
.PT .nw
negative 16-bit argument divided by the wordsize
.PT .s<num>
shortie with <num> as high order argument byte
.PT .sw<num>
shortie with argument divided by the wordsize
.PT .<num>
mini with <num> as argument
.PT .<num>W
mini with <num>*wordsize as argument
.PE 3
<num> is a possibly negative integer.
.VS 1 1
.IE
The dispatch table for the 256 primary opcodes:
.DS B
   0   loc.0    loc.1    loc.2    loc.3    loc.4    loc.5    loc.6    loc.7
   8   loc.8    loc.9    loc.10   loc.11   loc.12   loc.13   loc.14   loc.15
  16   loc.16   loc.17   loc.18   loc.19   loc.20   loc.21   loc.22   loc.23
  24   loc.24   loc.25   loc.26   loc.27   loc.28   loc.29   loc.30   loc.31
  32   loc.32   loc.33   aar.1W   adf.s0   adi.1W   adi.2W   adp.l    adp.1
  40   adp.2    adp.s0   adp.s-1  ads.1W   and.1W   asp.1W   asp.2W   asp.3W
  48   asp.4W   asp.5W   asp.w0   beq.l    beq.s0   bge.s0   bgt.s0   ble.s0
  56   blm.s0   blt.s0   bne.s0   bra.l    bra.s-1  bra.s-2  bra.s0   bra.s1
  64   cal.1    cal.2    cal.3    cal.4    cal.5    cal.6    cal.7    cal.8
  72   cal.9    cal.10   cal.11   cal.12   cal.13   cal.14   cal.15   cal.16
  80   cal.17   cal.18   cal.19   cal.20   cal.21   cal.22   cal.23   cal.24
  88   cal.25   cal.26   cal.27   cal.28   cal.s0   cff.z    cif.z    cii.z
  96   cmf.s0   cmi.1W   cmi.2W   cmp.z    cms.s0   csa.1W   csb.1W   dec.z
 104   dee.w0   del.w-1  dup.1W   dvf.s0   dvi.1W   fil.l    inc.z    ine.lw
 112   ine.w0   inl.-1W  inl.-2W  inl.-3W  inl.w-1  inn.s0   ior.1W   ior.s0
 120   lae.l    lae.w0   lae.w1   lae.w2   lae.w3   lae.w4   lae.w5   lae.w6
 128   lal.p    lal.n    lal.0    lal.-1   lal.w0   lal.w-1  lal.w-2  lar.W
 136   ldc.0    lde.lw   lde.w0   ldl.0    ldl.w-1  lfr.1W   lfr.2W   lfr.s0
 144   lil.w-1  lil.w0   lil.0    lil.1W   lin.l    lin.s0   lni.z    loc.l
 152   loc.-1   loc.s0   loc.s-1  loe.lw   loe.w0   loe.w1   loe.w2   loe.w3
 160   loe.w4   lof.l    lof.1W   lof.2W   lof.3W   lof.4W   lof.s0   loi.l
 168   loi.1    loi.1W   loi.2W   loi.3W   loi.4W   loi.s0   lol.pw   lol.nw
 176   lol.0    lol.1W   lol.2W   lol.3W   lol.-1W  lol.-2W  lol.-3W  lol.-4W
 184   lol.-5W  lol.-6W  lol.-7W  lol.-8W  lol.w0   lol.w-1  lxa.1    lxl.1
 192   lxl.2    mlf.s0   mli.1W   mli.2W   rck.1W   ret.0    ret.1W   ret.s0
 200   rmi.1W   sar.1W   sbf.s0   sbi.1W   sbi.2W   sdl.w-1  set.s0   sil.w-1
 208   sil.w0   sli.1W   ste.lw   ste.w0   ste.w1   ste.w2   stf.l    stf.W
 216   stf.2W   stf.s0   sti.1    sti.1W   sti.2W   sti.3W   sti.4W   sti.s0
 224   stl.pw   stl.nw   stl.0    stl.1W   stl.-1W  stl.-2W  stl.-3W  stl.-4W
 232   stl.-5W  stl.w-1  teq.z    tgt.z    tlt.z    tne.z    zeq.l    zeq.s0
 240   zeq.s1   zer.s0   zge.s0   zgt.s0   zle.s0   zlt.s0   zne.s0   zne.s-1
 248   zre.lw   zre.w0   zrl.-1W  zrl.-2W  zrl.w-1  zrl.nw   escape1  escape2
.DE 2
The list of secondary opcodes (escape1):
.N  1
.DS  B
   0   aar.l    aar.z    adf.l    adf.z    adi.l    adi.z    ads.l    ads.z
   8   adu.l    adu.z    and.l    and.z    asp.lw   ass.l    ass.z    bge.l
  16   bgt.l    ble.l    blm.l    bls.l    bls.z    blt.l    bne.l    cai.z
  24   cal.l    cfi.z    cfu.z    ciu.z    cmf.l    cmf.z    cmi.l    cmi.z
  32   cms.l    cms.z    cmu.l    cmu.z    com.l    com.z    csa.l    csa.z
  40   csb.l    csb.z    cuf.z    cui.z    cuu.z    dee.lw   del.pw   del.nw
  48   dup.l    dus.l    dus.z    dvf.l    dvf.z    dvi.l    dvi.z    dvu.l
  56   dvu.z    fef.l    fef.z    fif.l    fif.z    inl.pw   inl.nw   inn.l
  64   inn.z    ior.l    ior.z    lar.l    lar.z    ldc.l    ldf.l    ldl.pw
  72   ldl.nw   lfr.l    lil.pw   lil.nw   lim.z    los.l    los.z    lor.s0
  80   lpi.l    lxa.l    lxl.l    mlf.l    mlf.z    mli.l    mli.z    mlu.l
  88   mlu.z    mon.z    ngf.l    ngf.z    ngi.l    ngi.z    nop.z    rck.l
  96   rck.z    ret.l    rmi.l    rmi.z    rmu.l    rmu.z    rol.l    rol.z
 104   ror.l    ror.z    rtt.z    sar.l    sar.z    sbf.l    sbf.z    sbi.l
 112   sbi.z    sbs.l    sbs.z    sbu.l    sbu.z    sde.l    sdf.l    sdl.pw
 120   sdl.nw   set.l    set.z    sig.z    sil.pw   sil.nw   sim.z    sli.l
 128   sli.z    slu.l    slu.z    sri.l    sri.z    sru.l    sru.z    sti.l
 136   sts.l    sts.z    str.s0   tge.z    tle.z    trp.z    xor.l    xor.z
 144   zer.l    zer.z    zge.l    zgt.l    zle.l    zlt.l    zne.l    zrf.l
 152   zrf.z    zrl.pw   dch.z    exg.s0   exg.l    exg.z    lpb.z    gto.l
.DE 2
Finally, the list of opcodes with four byte arguments (escape2).
.DS

   0  loc
.DE 0
.BP
.AP "AN EXAMPLE PROGRAM"
.DS B
 1      program example(output);
 2      {This program just demonstrates typical EM code.}
 3      type rec = record r1: integer; r2:real; r3: boolean end;
 4      var mi: integer;  mx:real;  r:rec;
 5
 6      function sum(a,b:integer):integer;
 7      begin
 8        sum := a + b
 9      end;
10
11      procedure test(var r: rec);
12      label 1;
13      var i,j: integer;
14          x,y: real;
15          b: boolean;
16          c: char;
17          a: array[1..100] of integer;
18
19      begin
20              j := 1;
21              i := 3 * j + 6;
22              x := 4.8;
23              y := x/0.5;
24              b := true;
25              c := 'z';
26              for i:= 1 to 100 do a[i] := i * i;
27              r.r1 := j+27;
28              r.r3 := b;
29              r.r2 := x+y;
30              i := sum(r.r1, a[j]);
31              while i > 0 do begin j := j + r.r1; i := i - 1 end;
32              with r do begin r3 := b;  r2 := x+y;  r1 := 0 end;
33              goto 1;
34      1:      writeln(j, i:6, x:9:3, b)
35      end; {test}
36      begin {main program}
37        mx := 15.96;
38        mi := 99;
39        test(r)
40      end.
.DE 0
.BP
The EM code as produced by the Pascal-VU compiler is given below. Comments
have been added manually.  Note that this code has already been  optimized.
.DS B
  mes 2,2,2              ; wordsize 2, pointersize 2
 .1
  rom 't.p\e000'         ; the name of the source file
  hol 552,-32768,0       ; externals and buf occupy 552 bytes
  exp $sum               ; sum can be called from other modules
  pro $sum,2             ; procedure sum; 2 bytes local storage
  lin 8                  ; code from source line 8
  ldl 0                  ; load two locals ( a and b )
  adi 2                  ; add them
  ret 2                  ; return the result
  end 2                  ; end of procedure ( still two bytes local storage )
 .2
  rom 1,99,2             ; descriptor of array a[]
  exp $test              ; the compiler exports all level 0 procedures
  pro $test,226          ; procedure test, 226 bytes local storage
 .3
  rom 4.8F8              ; assemble Floating point 4.8 (8 bytes) in
 .4                              ; global storage
  rom 0.5F8              ; same for 0.5
  mes 3,-226,2,2         ; compiler temporary not referenced by address
  mes 3,-24,2,0          ; the same is true for i, j, b and c in test
  mes 3,-22,2,0
  mes 3,-4,2,0
  mes 3,-2,2,0
  mes 3,-20,8,0          ; and for x and y
  mes 3,-12,8,0
  lin 20                 ; maintain source line number
  loc 1
  stl -4                 ; j := 1
  lni                    ; lin 21 prior to optimization
  lol -4
  loc 3
  mli 2
  loc 6
  adi 2
  stl -2                 ; i := 3 * j + 6
  lni                    ; lin 22 prior to optimization
  lae .3
  loi 8
  lal -12
  sti 8                  ; x := 4.8
  lni                    ; lin 23 prior to optimization
  lal -12
  loi 8
  lae .4
  loi 8
  dvf 8
  lal -20
  sti 8                  ; y := x / 0.5
  lni                    ; lin 24 prior to optimization
  loc 1
  stl -22                ; b := true
  lni                    ; lin 25 prior to optimization
  loc 122
  stl -24                ; c := 'z'
  lni                    ; lin 26 prior to optimization
  loc 1
  stl -2                 ; for i:= 1
 2
  lol -2
  dup 2
  mli 2                  ; i*i
  lal -224
  lol -2
  lae .2
  sar 2                  ; a[i] :=
  lol -2
  loc 100
  beq *3                 ; to 100 do
  inl -2                 ; increment i and loop
  bra *2
 3
  lin 27
  lol -4
  loc 27
  adi 2                  ; j + 27
  sil 0                  ; r.r1 :=
  lni                    ; lin 28 prior to optimization
  lol -22                ; b
  lol 0
  stf 10                 ; r.r3 :=
  lni                    ; lin 29 prior to optimization
  lal -20
  loi 16
  adf 8                  ; x + y
  lol 0
  adp 2
  sti 8                  ; r.r2 :=
  lni                    ; lin 23 prior to optimization
  lal -224
  lol -4
  lae .2
  lar 2                  ; a[j]
  lil 0                  ; r.r1
  cal $sum               ; call now
  asp 4                  ; remove parameters from stack
  lfr 2                  ; get function result
  stl -2                 ; i :=
 4
  lin 31
  lol -2
  zle *5                 ; while i > 0 do
  lol -4
  lil 0
  adi 2
  stl -4                 ; j := j + r.r1
  del -2                 ; i := i - 1
  bra *4                 ; loop
 5
  lin 32
  lol 0
  stl -226               ; make copy of address of r
  lol -22
  lol -226
  stf 10                 ; r3 := b
  lal -20
  loi 16
  adf 8
  lol -226
  adp 2
  sti 8                  ; r2 := x + y
  loc 0
  sil -226               ; r1 := 0
  lin 34                 ; note the abscence of the unnecesary jump
  lae 22                 ; address of output structure
  lol -4
  cal $_wri              ; write integer with default width
  asp 4                  ; pop parameters
  lae 22
  lol -2
  loc 6
  cal $_wsi              ; write integer width 6
  asp 6
  lae 22
  lal -12
  loi 8
  loc 9
  loc 3
  cal $_wrf              ; write fixed format real, width 9, precision 3
  asp 14
  lae 22
  lol -22
  cal $_wrb              ; write boolean, default width
  asp 4
  lae 22
  cal $_wln              ; writeln
  asp 2
  ret 0                  ; return, no result
  end 226
  exp $_main
  pro $_main,0           ; main program
 .6
  con 2,-1,22            ; description of external files
 .5
  rom 15.96F8
  fil .1                 ; maintain source file name
  lae .6                 ; description of external files
  lae 0                  ; base of hol area to relocate buffer addresses
  cal $_ini              ; initialize files, etc...
  asp 4
  lin 37
  lae .5
  loi 8
  lae 2
  sti 8                  ; mx := 15.96
  lni                    ; lin 38 prior to optimization
  loc 99
  ste 0                  ; mi := 99
  lni                    ; lin 39 prior to optimization
  lae 10                 ; address of r
  cal $test
  asp 2
  loc 0                  ; normal exit
  cal $_hlt              ; cleanup and finish
  asp 2
  end 0
  mes 5                  ; reals were used
.DE 0
The compact code corresponding to the above program is listed below.
Read it horizontally, line by line, not column by column.
Each number represents a byte of compact code, printed in decimal.
The first two bytes form the magic word.
.N 1
.IS 3
.DS B
173   0 159 122 122 122 255 242   1 161 250 124 116  46 112   0
255 156 245  40   2 245   0 128 120 155 249 123 115 117 109 160
249 123 115 117 109 122  67 128  63 120   3 122  88 122 152 122
242   2 161 121 219 122 255 155 249 124 116 101 115 116 160 249
124 116 101 115 116 245 226   0 242   3 161 253 128 123  52  46
 56 255 242   4 161 253 128 123  48  46  53 255 159 123 245  30
255 122 122 255 159 123  96 122 120 255 159 123  98 122 120 255
159 123 116 122 120 255 159 123 118 122 120 255 159 123 100 128
120 255 159 123 108 128 120 255  67 140  69 121 113 116  68  73
116  69 123  81 122  69 126   3 122 113 118  68  57 242   3  72
128  58 108 112 128  68  58 108  72 128  57 242   4  72 128  44
128  58 100 112 128  68  69 121 113  98  68  69 245 122   0 113
 96  68  69 121 113 118 182  73 118  42 122  81 122  58 245  32
255  73 118  57 242   2  94 122  73 118  69 220  10 123  54 118
 18 122 183  67 147  73 116  69 147   3 122 104 120  68  73  98
 73 120 111 130  68  58 100  72 136   2 128  73 120   4 122 112
128  68  58 245  32 255  73 116  57 242   2  59 122  65 120  20
249 123 115 117 109   8 124  64 122 113 118 184  67 151  73 118
128 125  73 116  65 120   3 122 113 116  41 118  18 124 185  67
152  73 120 113 245  30 255  73  98  73 245  30 255 111 130  58
100  72 136   2 128  73 245  30 255   4 122 112 128  69 120 104
245  30 255  67 154  57 142  73 116  20 249 124  95 119 114 105
  8 124  57 142  73 118  69 126  20 249 124  95 119 115 105   8
126  57 142  58 108  72 128  69 129  69 123  20 249 124  95 119
114 102   8 134  57 142  73  98  20 249 124  95 119 114  98   8
124  57 142  20 249 124  95 119 108 110   8 122  88 120 152 245
226   0 155 249 125  95 109  97 105 110 160 249 125  95 109  97
105 110 120 242   6 151 122 119 142 255 242   5 161 253 128 125
 49  53  46  57  54 255  50 242   1  57 242   6  57 120  20 249
124  95 105 110 105   8 124  67 157  57 242   5  72 128  57 122
112 128  68  69 219 110 120  68  57 130  20 249 124 116 101 115
116   8 122  69 120  20 249 124  95 104 108 116   8 122 152 120
159 124 160 255 159 125 255
.DE 0
.IE
.MS T A 0
.ME
.BP
.MS B A 0
.ME
.CT
