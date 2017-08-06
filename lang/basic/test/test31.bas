10 a$(0)= "fbc"
11 a$(1)= "fcc"
12 a$(2)= "abb"
90 'bubble sort array
100 j=2: flips=1 'one more pass
110 while flips
115	flips=0
120	for i=0 to j-1
130	if a$(i)>a$(i+1) then swap a$(i),a$(i+1): flips=1
140	nexti
150 wend
