10 rem gunner program
90 randomize
100 print "this program simulates the results "
110 print "of firing a field artillery weapon"
120 print
130 print "you are the officer-in-charge, giving orders to the gun"
140 print "crew, telling them the degrees of elevation you estimate"
150 print "will place the projectile on target. A hit within 100 yards"
160 print "of the target will destroy it. Take more than 5 shots,"
170 print "end the enemy will destroy you!":print
180 print "maximum range of your gun is 46500 yards."
185 z=0
190 print
195 s1=0
200 t= 43000-30000*rnd(x)
210 s=0
220 goto 370
230 print"minimum elevation of the gun is one degree."
240 goto 390
250 print "maximum elevation of gun is 89 degrees."
260 goto 390
270 print "over target by";abs(e);"yards"
280 goto 390
290 print "short of target by";abs(e);"yards"
300 goto 390
310 goto 320
320 print "*****target destroyed ****";s;"rounds of ammunition expended"
322 gosub 600
325 s1=s1+s
330 if z=4 then 490
340 z=z+1
345 print
350 print "the forward observer has sighted more enemy activity."
360 goto 200
370 print "	distance to the target is";int(t);"yards...."
380 print
390 print
400 print "elevation";
410 input b
420 if b>89 then 250
430 if b<1 then 230
440 s=s+1
442 if s<6 then 450
444 print:print "BOOM !!! You have been destroyed";
445 gosub 600
446 print "by the enemy":print:print:goto 495
450 b2=2*b/57.3:let i=46500*sin(b2):x=t-i:e=int(x)
460 if abs(e)<100 then 310
470 if e>100 then 290
480 if e<-100 then 270
490 print:print:print "total rounds expended were";s1
491 if s1>15 then 495:print "nice shooting!!":gosub 600:goto 500
495 print "better go back to fort silly for refresher training!"
500 print:print "thank you for playing!"
505 print:print "try again....":print:goto 180
600 rem for n=1 to 10 print chr$(7): rem next n
610 return
999 end
