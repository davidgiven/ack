100 rem writing to a file
110 open "o", #2, "tst/output"
113 open "i", #3, "tst/output"
120 a$="CAMERA": b$="93604-1"
130 write a$,b$
140 write #2, a$, b$
145 input #3, b$: print b$
150 write #3, a$, b$
