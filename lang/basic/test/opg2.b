100 rem Square root program (47)
110 rem this program uses the newton raphson
120 rem technique to calculate the square root 
130 read a,e
140 let x =(a+2)/3
150 let x1= (x+a/x)*.5
160 rem determine absolute value of x1-x
170 let n= x1-x
180 if n>= 0 then 200
190 let n= 0-n
200 if n<e then 230
210 let x=x1
220 goto 150
230 print "square root of",a,"equals",x1
240 end
250 data 81, .01
