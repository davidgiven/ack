  10 rem The Creator
  20 rem 80 micro jan 1983
  80 print"This is the Creator. It will allow you to generate"
  90 print"a progam which will create and access a data file."
 100 print"For later use. Please type the proposed program name."
 110 print"You are limited to 8 alphabetic characters."
 120 print" program name=";:lineinputpn$
 130 fori=1tolen(pn$):a$=mid$(pn$,i,1):ifa$>"z"ora$<"A"thenprint"alpha characters only!":goto 120
 140 nexti
 150 iflen(pn$)>8thenprint"too long":goto 120
 160 print"when the proposed program is run, which drive contains the"
 170 print"data file(0-3)?";
 180 an$=inkey$:ifan$="" then 180 elseif(an$>"3"oran$<"0")then170
 190 printan$
 200 print"which drive do you want the program written on? (0-3)";
 210 dn$=inkey$:ifdn$="" then 210 elseif(dn$>"3"ordn$<"0")then200

 240 print#1," 1 rem*******Program name:";pn$;"*******"
 250 print#1," 2 rem*******Data File name:";df$;"*****"
 260 print#1," 3 rem*******Data File is on drive";ans$;"******"
 270 input"What is the maximum data file size, in # of records";ms
 280 input"What is the record length(1-255)";rr:ifrr<0orrr>255then280 else r%=256/rr
 290 ifms*256/r%>85760thenprint"not enough room on a single disk for this.":goto 270
 300 print#1,"4 rem ******maximum file size is";ms;"records******"
 310 print#1,"5 rem****** record length is";rr;"packed";r%;"per sector"
 320 q$=chr$(34)
 330 print"please type in a title for your generated program.":lineinputti$
 340 ln=ln+10:print#1,ln;"rem change disks reinitialize here"
 350 ln=ln+10
