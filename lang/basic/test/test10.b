100 rem compound goto
110 leti=1:print i
120 on i goto 130, 140, 150
121 printi"end":stop
130 print "line130":i=i+1:goto120
140 print "line140":i=i+1:goto120
150 print "line150":i=i+1:goto120
