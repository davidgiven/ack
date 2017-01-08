110 rem second trap test
120 on error goto 200
130 print 1/o
140 print "division error not catched"
200 print "division error catched", err, erl
210 on error goto 500
220 print "waiting for interrupt"
230 goto 230
500 print "interrupt catched"
510 print "generate error 352"
520 error 352
