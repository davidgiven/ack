0010 print " beginwaarde =a"
0020 input a
0030 print "eindwaarde =b"
0040 input b
0050 if b<a then 0130
0060 n=a
0070 if n<0 then 0200
0080 print "n=",n;" wortel uit n=",sqr(n)
0090 if n=b then 0120
0100 n=n+1
0110 goto 0070
0120 end
0130 print "b moet groter zijn dan a"
0140 end
0200 print "n=",n,"n is negatief, de oplossing is leeg"
0210 goto 0100

