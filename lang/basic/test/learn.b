10 REM Learning program I
15 dim variable(10), rank(10), varname$(10)
20 input "how many variable have you";v
30 for i=1to v :variable(i)=0:rank(i)=0:next i
40 print "please name these variable"
50 for i=1to v: input "variable name";varname$(i):next i
60 print "Please name the outcomes"
70 input "outcome 1";q1$
80 input "outcome 2"; q2$
90 for i=1 to v:variable(i)=0
100 print "variable ";varname(i);
110 input "is this variable the case";a$
120 if a$="y" then variable(i)=1
130 next i
140 d=0
150 for i=1 to v
160 d=d+variable(i)* rank(i)
170 next i
175 print "conclusion: ";d;
180 if d>=0 then print q1$
190 if d<0 then print q2$
195 input "is this right";a$:if a$="y" then:goto 90
200 if d>=0 and a$="n" then: for i=1 to v:rank(i)=rank(i)-variable(i):next i
210 if d<0 and a$="n" then: for i=1 to v:rank(i)=rank(i)+variable(i):next i
220 goto 90
