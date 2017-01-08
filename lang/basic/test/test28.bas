10 rem reading beyond eof
11 open "r",#1,"tst/test01"
13 print eof(#1)
20 while eof(#1) <> -1
22 line  #1, l$
23 print eof(#1), l
24 wend
