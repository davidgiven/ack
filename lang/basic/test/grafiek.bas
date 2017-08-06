0015  print
0020 for f =2 to 72 step 2
0030 print tab (f) "-" ;
0040 next f
0043 print
0045 c= 30
0050 for x=6 to 360 step 12
0060 a=30 +25*sin(x*0.01745329)
0070 b=30+25*cos(x*0.01745329)
0080 if a=c then 0130
0090 if b=c then 0190
0100 if a=b then 0230
0110 if a>c then 0290
0120 if c>a then 0380
0130 if a>b then 0360
0140 ifb>a then 0170
0150 print tab(b) "." tab(a) "*"
0160 goto  0460
0170 print tab(a) "*" tab(b) "."
0180 goto 0460
0190 if a>b then 0210
0200 if b>a then 0220
0210 goto 0150
0220 goto 0170
0230 if a>c then 0250
0240 if c>a then 0270
0250 print tab(c) "I" tab(a) "*"
0260 goto  0460
0270 print tab(a) "*" tab(c) "I"
0280 goto 0460
0290 if b>a then 0340
0300 if a>b then 0302
0302 if c>b then 0320  
0305 if b<a then 0360     
0310 if b>c then 0360
0320 print tab(b) "." tab(c) "I" tab(a) "*"
0330 goto 0460
0340 print tab(c) "I" tab(a) "*" tab(b) "."
0350 goto 0460
0360 print tab(c) "I" tab(b) "." tab(a) "*"
0370 goto 0460
0380 if a>b then 0430
0385 if c>b then 0410
0387 if  b>c then 0450
0395 if a>b then 0410
0400 if b>a then 0450
0410 print tab(a) "*" tab(b) "." tab(c) "I"
0420 goto 0460
0430 print tab(b) "." tab(a) "*" tab(c) "I"
0440 goto 0460
0450 print tab(a) "*" tab(c) "I" tab(b) "."
0460 next x
0470 end


