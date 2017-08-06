10 rem *** bullseye
20 print "game of bullseye":print:randomize
30 print "in this game, up to 20 players throw darts at a target"
40 print "with 10,20,30, and 40 point zones. The objective is"
50 print "toget 200 point.":print
60 print "throw","description",,"probable score"
70 print " 1","fast overarm",,"bullseye or complete miss"
80 print " 2","controlled overarm","10,20,30 points"
90 print " 3","underarm",,"anything":print
100 dim a$(20),s(20),w(10):r=0:m=0: for i=1 to 20:s(i)=0:nexti
110 input ;"how many players";n:print
120 for i=1 to n
130 print "name of player" i; : input a$(i)
140 nexti
150 r=r+1:print:print "round" r
160 for i=1 to n
170 print:print a$(i)"'s throw";:input t
180if t<1 or t>3 then print "input 1, 2, or 3":goto 170
190 on t goto 200,210,220
200 p1=.65:p2=.55:p3=.5:p4=.5:goto 230
210p1=.99:p2=.77:p3=.43:p4=.01:goto 230
220 p1=.95:p2=.75:p3=.45:p4=.05
230 u=rnd(0):print "rnd="u
240 if u>=p1 then print "BULLSEYE!! 40 points":b=40:goto 290
250 if u>=p2 then print "30-point zone":b=30:goto 290
260 if u>=p3 then print "20-point zone":b=20:goto 290
270 if u>=p4 then print "WHEH! 10 points":b=10:goto 290
280 print "missed the target. too bad!":b=0
290 s(i)=s(i)+b:print "total score="s(i):next i
300 fori=1 to n
310 if s(i)>=200 then m=m+1:w(m)=i
320 nexti
330 if m=0 then150
340 print :print "We have a winner!!":print
350 print a$(w(m))" scored"s(w(m))"points."
360 for i=1 to m 
370 print:print "thanks for the game!":nexti:end
