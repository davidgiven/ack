  mes 2,2,2              ; wordsize 2, pointersize 2
 .1
  rom 't.p\000'          ; the name of the source file
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
  mes 3,-226,2,2         ; compiler temporary not referenced indirect
  mes 3,-24,2,0          ; the same is true for i, j, b and c in test
  mes 3,-22,2,0
  mes 3,-4,2,0
  mes 3,-2,2,0
  mes 3,-20,8,0          ; and for x and y
  mes 3,-12,8,0
  lin 20                 ; maintain source line number
  loc 1
  stl -4                 ; j := 1
  lni                    ; was lin 21 prior to optimization
  lol -4
  loc 3
  mli 2
  loc 6
  adi 2
  stl -2                 ; i := 3 * j + 6
  lni                    ; was lin 22 prior to optimization
  lae .3
  loi 8
  lal -12
  sti 8                  ; x := 4.8
  lni                    ; was lin 23 prior to optimization
  lal -12
  loi 8
  lae .4
  loi 8
  dvf 8
  lal -20
  sti 8                  ; y := x / 0.5
  lni                    ; was lin 24 prior to optimization
  loc 1
  stl -22                ; b := true
  lni                    ; was lin 25 prior to optimization
  loc 122
  stl -24                ; c := 'z'
  lni                    ; was lin 26 prior to optimization
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
  lni                    ; was lin 28 prior to optimization
  lol -22                ; b
  lol 0
  stf 10                 ; r.r3 :=
  lni                    ; was lin 29 prior to optimization
  lal -20
  loi 16
  adf 8                  ; x + y
  lol 0
  adp 2
  sti 8                  ; r.r2 :=
  lni                    ; was lin 23 prior to optimization
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
  sti 8                  ; x := 15.9
  lni                    ; was lin 38 prior to optimization
  loc 99
  ste 0                  ; mi := 99
  lni                    ; was lin 39 prior to optimization
  lae 10                 ; address of r
  cal $test
  asp 2
  loc 0                  ; normal exit
  cal $_hlt              ; cleanup and finish
  asp 2
  end 0
  mes 4,40               ; length of source file is 40 lines
  mes 5                  ; reals were used
