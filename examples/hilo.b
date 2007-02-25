1 ' $Source$
2 ' $State$
3 ' $Revision$

10 print "Hi there! Before we start, what is your name?"
20 input "> ", PlayerName$
30 print
40 print "Hello, "; PlayerName$; "!"

100 gosub 1000
110 print
120 print "Would you like another go?"
130 input "> ", s$
140 s$ = left$(s$, 1)
150 if s$ = "n" or s$ = "N" then goto 200
160 print
170 print "Excellent! ";
180 goto 100
200 print
210 print "Thanks for playing --- goodbye!"
220 end

1000 print "See if you can guess my number."
1010 Number% = rnd(1) mod 100
1020 Attempts% = 1

1030 print
1040 input "> ", guess%
1050 if guess% < Number% then print: print "Try a bit higher."
1060 if guess% > Number% then print: print "Try a bit lower."
1070 if guess% = Number% then goto 1100
1080 Attempts% = Attempts% + 1
1090 goto 1030
1100 print
1110 print "You got it right in only"; Attempts%;
1120 if Attempts% = 1 then print "go"; else print "goes";
1130 print "!"
1140 return

