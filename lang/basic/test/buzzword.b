100 print "This computer program demonstration us a new aid for"
110 print "preparing speeches and briefings. It's a buzzword"
120 print "generator which provides you with a set of three higly"
130 print "acceptablewords to work into your material. The words"
140 print "don't actually mean anything, but they sound great."
150 print
160 print "the procedure:"
170 print ,"Think of any three numbers between 0 and 9, enter"
180 print , "them after the '?' separated by commas. your"
190 print ,"buzzword will be printed out. Typing "100" for"
200 print ,"each of your choices stops this program."
210 print "What are your three numbers";
220 goto 260
230 print
240 print 
250 print "Three more numbers";
260 input n,m,p
265 if n= 100 then 1290
267 print "continue"
270 if n<0 then 1240
280 if p<0 then 1240
290 if m<0 then 1240
300 if m>9 then 1240
310 if p>9 then 1240
320 if n>9 then 1240
330 print
340 on n+1 goto 640,660,680,700,720,740,760,780,800,820
440 on m+1 goto 840,860,880,900,920,940,960,980,1000,1020
540 on p+1 goto 1040,1060,1080,1100,1120,1140,1160,1180,1200,1220
640 print " integrated";: goto 440
660 print " total";: goto 440
680 print " systematized";: goto 440
700 print " parallel";: goto 440
720 print " functional";: goto 440
740 print " responsive";: goto 440
760 print " optimal";: goto 440
780 print " synchronized";: goto 440
800 print " compatible";: goto 440
820 print " balanced";: goto 440
840 print " management"; : goto 540
860 print " organizational"; : goto 540
880 print " monitored"; : goto 540
900 print " reciprocal"; : goto 540
920 print " digital"; : goto 540
940 print " logistical"; : goto 540
960 print " transitional"; : goto 540
980 print " incremental"; : goto 540
1000 print " fifth-generation"; : goto 540
1020 print " policy"; : goto 540
1040 print " options";: goto 230
1060 print " flexibility";: goto 230
1080 print " capability";: goto 230
1100 print " mobility";: goto 230
1120 print " programming";: goto 230
1140 print " concept";: goto 230
1160 print " time-phase";: goto 230
1180 print " projection";: goto 230
1200 print " hardware";: goto 230
1220 print " contingency";: goto 230
1240 print 
1260 print
1270 print "numbers must be between 0 and 9, please select three more."
1280 goto 260
1290 print "Goodbye for now."
1300 print:print:print
1310 end
