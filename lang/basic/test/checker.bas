100 print "This program will play checkers. The computer us X,"
110 print "and you are 0. The computer will go first, -note: squares"
120 print "are printed in the form-(X,Y) and sq. 1.1 is the bottom left!"
130 print "do not attempt a double jump or your piece might just"
140 print "disappear (same for triple!)"
150 print "	Wait for the computer to move!!!!!!"
700 g=-1
800 dim r(50)
900 let l=-1
1000 dim s(10,10)
1100 data 1,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,-1,15
1200 for x=1to8
1300 fory=1to8
1400 read j
1500 if j=15 then 1800
1600 s(x,y)=j
1700 goto 2000
1800 restore
1900 reads(x,y)
2000 nexty
2100 nextx
2200 rem
2300 l=-1*l
2400 for x=1to 8
2500 for y=1to8
2600 if s(x,y)=0 then 3500
2700 if g>0 then 3000
2800 if s(x,y)>0 then 3500
2900 goto 3100
3000 if s(x,y)<0 then 3500
3100 if abs(s(x,y))<>1 then 3300
3200 gosub 4300
3300 if abs(s(x,y))<>2 then 3500
3400 gosub 6500
3500 if x<>8 then 3800
3600 if l=1 then 3800
3700 rem return
3800 nexty
3900 nextx
4000 print
4100 gosub 11400
4200 goto 2300
4300 for a=-1 to 1 step2
4400 let u=x+a
4500 let v=y+g
4600 if u<1 then 6300
4700 if u>8 then 6300
4800 if v<1 then 6300
4900 if v>8 then 6300
5000 if s(u,v) <> 0 then 5300
5100 gosub 9100
5200 goto 6300
5300 if s(u,v)=g then 6300
5400 if s(u,v)=2*g then 6300
5500 u=u+a
5600 v=v+g

5700 if u<1 then 6300
5800 if u>8 then 6300
5900 if v<1 then 6300
6000 if v>8 then 6300
6100 if s(u,v)<>0 then 6300
6200 gosub 9100
6300 next a
6400 return
6500 rem king moves
6600 for a=-1 to 1 step2
6700 forb=-1to 1step2
6800 u=x+a
6900 v=y+b
7000 if u<1 then 8700
7100 if u>8 then 8700
7200 if v<1 then 8700
7300 if v>8 then 8700
7400 if s(u,v)<>0 then 7700
7500 gosub 9100
7600 goto 8700
7700 if s(umv)=g then 8700
7800 if s(u,v)=2*g then 8700
7900 u=u+a
8000 v=v+b
8100 if u<1 then 8700
8200 if u>8 then 8700
8300 if v<1 then 8700
8400 if v>8 then 8700
8500 if s(u,v)<>0 then 8700
8600 gosub 9100
8700 next b
8800 next a
8900return
9000 goto 14200
9100 rem
9200 p=p+1
9300 if p=k then 12300
9400 if v<>(4.5+(3.5*g)) then 9600
9500 q=q+2
9600 if x<>(4.5-(3.5*g)) then9800
9700 q=q-2
9800 rem
9900 if u<>1 then 10100
10000 q=q+1
10100 if u<> 8 then 10300
10200 q=q+1
10300 for c=-1 to 1 step 2
10400 if s(u+c,v+g)<1 then 10800
10500 q=q-1
10600 if s(u-c,v-g) <> 0 then 10800
10700 q=q-1
10800 rem this was the evaluation section
10900 rem
11000 next c
11100 r(p)=q
11200 q=0
11300 return
11400 if p=0 then 18800
11500 for j=10to-10step -1
11600for f=1to p
11700 if r(f)=j then 12000
11800 next f
11900 next j
12000 let k=f+p
12100 print "retry": gosub 2300
12200 return
12300 print " I move from ("X Y") to("U;V")"
12400 letf=0
12500 p=0
12600 k=0
12700 if v<>(4.5+(3.5*g)) then 13000
12800 s(u,v)=s*g
12900 goto 13100
13000 let s(u,v)=s(x,y)
13100 let s(x,y)=0
13200 if(abs(x-u))<>2 then 13400
13300 s((x+u)/2,(y+v)/2)=0
13400 print "board";
13500 input d$
13600 if d$<>"yes" then 13900
13700 gosub 14100
13800 return
13900 gosub 15800
14000 return
14100 print
14200 for y=8to1step -1
14300 for x=1to8
14400 i=2*x
14500 if s(x,y)<>0 then14700
14600 print tab(i)".";
14700 if s(x,y)<>1 then 14900
14800 print tab(i)"0"
14900 if s(x,y)<>-1then 15100
15000 print tab(i)"X"
15100 if s(x,y)<>-2 then 15300
15200 print tab(i)"X";tab(I)"*"
15300 if s(x,y)<>2 then 15500
15400 print tab(i)"O";tab(I)"*"
15500 next x
15600 print
15700 next y
15800 print
15900 print "from";
16000 input e,h
16100 x=e
16200 y=h
16300 if s(x,y)<>0 then 16700
16400 print "there is no one occupying that space"
16500 print
16600 goto 15900
16700 print "to";
16800 input a,b
16900 x=a
17000 y=b
17100 if s(x,y)=0 then 17500
17200 print "that space is already occupied"
17300 print
17400 goto 16700
17500 rem
17600 s(a,b)=s(e,h)
17700 s(e,h)=0
17800 t=(4.5-(3.5*g))
17900 if abs(e-a)<>2 then 18100
18000 s((e+a)/2,(h+b)/2)=0
18100 if b<>t then 18300
18200 s(a,b)= -2*g
18300 for x=8to8
18400 for y=8to8
18500 return
18600 nexty
18700 next x
18800 print " very good, you win"
18900 print:print
19100 print "		chuck out"
19200 end
