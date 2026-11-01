10 rem DATA, READ and RESTORE. The DATA text is part of the program, so
20 rem it runs anywhere (it used to be read from a file next to the source).
30 read a, b, c
40 l = 40 : if a <> 1 or b <> -2 or c <> 3.5 then 900
50 read s$, t$
60 l = 60 : if s$ <> "hello" or t$ <> "a, b" then 900
70 read d
80 l = 80 : if d <> 100 then 900
90 restore
100 read a
110 l = 110 : if a <> 1 then 900
120 restore 520
130 read d, e
140 l = 140 : if d <> 100 or e <> 7 then 900
150 print "@@FINISHED"
160 end
500 data 1, -2, 3.5
510 data "hello", "a, b"
520 data 100, 7
900 print "@@FAIL at line"; l
910 end
