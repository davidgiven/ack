1000 rem Temperature conversion
1010 print "fahrenheit","centrigrade","kelvin","rankin"
1020 for s= 1 to 5
1030	print
1040 next s
1050 for i= 1 to 12
1060	read f
1070	let c= 5/9 *(f-32)
1080	let k = c+273
1090	let r= f+ 460
1100	print f,c,k,r
1110 next i
1120 data 144, 36,110,98,63,26,14,78,66,51,107,2
1130 end
