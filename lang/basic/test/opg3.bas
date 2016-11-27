010 rem values of n using if-then loop
020 print "n","n2","n3","1/n","square root"
030 let n=1
040 let n2= n*n
050 let n3= n*n*n
060 let r= 1/n
070 let s= n^ .5
080 print n,n2,n3,r,s
090 if n>= 20 then 120
100 let n= n+1
110 goto 40
120 end
