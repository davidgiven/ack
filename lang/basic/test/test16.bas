100 rem two dimensional array
110 dim table(3,3)
120 for i=0 to 3
130 for j=0 to 3
135 print i,j
140 table(i,j)= i*j
150 next j,i
160 print "filled"
170 for i=0 to 3
180 print table(i,i)
190 nexti
