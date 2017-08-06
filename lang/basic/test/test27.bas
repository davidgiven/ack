0 rem string test
110 yes$= "yes"
120 no$="no"
130 if "yes"="no" then  print "130 true" else print "130 f"
140 if "yes"="yes" then  print "140 true" else print "140 f"
150 if "yes"="" then print "150 true" else print "150 f"
160 if "yes"=yes then print "160 true" else print "160 f"
170 if yes="yes" then print "170 true" else print "170 f"
180 print "yes or no"
190 input answer$
200 print answer$="yes"
210 print answer$=no
