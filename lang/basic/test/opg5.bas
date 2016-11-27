
 10 rem find largest number (57)
  20 read l
  30 let r=0
  40 for z=1 to l
  50   read a
  60	if a<=r then 80
  70	let r=a
  80 next z
  90 print "largest number is",r
100  data 10
110 data 106, 42,-12,111,88,91,3,263,-1042,7
120 end
