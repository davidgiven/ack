bfs.bf1 + bfs.bf2
bfs.bf1 - bfs.bf2
bfs.bf1 / bfs.bf2
bfs.bf1 % bfs.bf2
bfs.bf1 * bfs.bf2
bfs.bf1 & bfs.bf2
bfs.bf1 | bfs.bf2
bfs.bf1 ^ bfs.bf2
bfs.bf1 || bfs.bf2
bfs.bf1 && bfs.bf2
bfs.bf1 << 1
bfs.bf1 >> 1
-bfs.bf1 
!bfs.bf1 
~bfs.bf1 
bfs.bf1 == bfs.bf2
bfs.bf1 != bfs.bf2
bfs.bf1 <= bfs.bf2
bfs.bf1 >= bfs.bf2
bfs.bf1 < bfs.bf2
bfs.bf1 > bfs.bf2
bfs.bf1 ? bfs.bf1 : bfs.bf2
bfs.bf3 = bfs.bf1  1
bfs.bf3 += bfs.bf1  0
bfs.bf3 -= bfs.bf1  254
bfs.bf3 /= bfs.bf1  255
bfs.bf3 %= bfs.bf1  0
bfs.bf3 *= bfs.bf1  255
bfs.bf3 &= bfs.bf1  1
bfs.bf3 |= bfs.bf1  255
bfs.bf3 ^= bfs.bf1  254
bfs.bf3 <<= 1 254
bfs.bf3 >>= 1 127
bfs.bf3 ++ 0
bfs.bf3 -- 254
-- bfs.bf3 254
++ bfs.bf3 0
bfs.bf4 = ( bfs.bf1 + bfs.bf2 ) 9
bfs.bf4 = ( bfs.bf1 - bfs.bf2 ) -7
bfs.bf4 = ( bfs.bf1 / bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 % bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 * bfs.bf2 ) 8
bfs.bf4 = ( bfs.bf1 & bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 | bfs.bf2 ) 9
bfs.bf4 = ( bfs.bf1 ^ bfs.bf2 ) 9
bfs.bf4 = ( bfs.bf1 || bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 && bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 << 1 ) 2
bfs.bf4 = ( bfs.bf1 >> 1 ) 0
bfs.bf4 = ( -bfs.bf1  ) -1
bfs.bf4 = ( !bfs.bf1  ) 0
bfs.bf4 = ( ~bfs.bf1  ) -2
bfs.bf4 = ( bfs.bf1 == bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 != bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 <= bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 >= bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 < bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 > bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 ? bfs.bf1 : bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf3 = bfs.bf1  ) 1 1
bfs.bf4 = ( bfs.bf3 += bfs.bf1  ) 0 0
bfs.bf4 = ( bfs.bf3 -= bfs.bf1  ) 254 254
bfs.bf4 = ( bfs.bf3 /= bfs.bf1  ) 255 255
bfs.bf4 = ( bfs.bf3 %= bfs.bf1  ) 0 0
bfs.bf4 = ( bfs.bf3 *= bfs.bf1  ) 255 255
bfs.bf4 = ( bfs.bf3 &= bfs.bf1  ) 1 1
bfs.bf4 = ( bfs.bf3 |= bfs.bf1  ) 255 255
bfs.bf4 = ( bfs.bf3 ^= bfs.bf1  ) 254 254
bfs.bf4 = ( bfs.bf3 <<= 1 ) 254 254
bfs.bf4 = ( bfs.bf3 >>= 1 ) 127 127
bfs.bf4 = ( bfs.bf3 ++ ) 0 255
bfs.bf4 = ( bfs.bf3 -- ) 254 255
bfs.bf4 = ( -- bfs.bf3 ) 254 254
bfs.bf4 = ( ++ bfs.bf3 ) 0 0
yes if ( bfs.bf1 + bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 - bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 / bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 % bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 * bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 & bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 | bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 ^ bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 || bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 && bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 << 1 ) yes() ; else no()
no if ( bfs.bf1 >> 1 ) yes() ; else no()
yes if ( -bfs.bf1  ) yes() ; else no()
no if ( !bfs.bf1  ) yes() ; else no()
yes if ( ~bfs.bf1  ) yes() ; else no()
no if ( bfs.bf1 == bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 != bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 <= bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 >= bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 < bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 > bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 ? bfs.bf1 : bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf3 = bfs.bf1  ) yes() ; else no() 1
no if ( bfs.bf3 += bfs.bf1  ) yes() ; else no() 0
yes if ( bfs.bf3 -= bfs.bf1  ) yes() ; else no() 254
yes if ( bfs.bf3 /= bfs.bf1  ) yes() ; else no() 255
no if ( bfs.bf3 %= bfs.bf1  ) yes() ; else no() 0
yes if ( bfs.bf3 *= bfs.bf1  ) yes() ; else no() 255
yes if ( bfs.bf3 &= bfs.bf1  ) yes() ; else no() 1
yes if ( bfs.bf3 |= bfs.bf1  ) yes() ; else no() 255
yes if ( bfs.bf3 ^= bfs.bf1  ) yes() ; else no() 254
yes if ( bfs.bf3 <<= 1 ) yes() ; else no() 254
yes if ( bfs.bf3 >>= 1 ) yes() ; else no() 127
yes if ( bfs.bf3 ++ ) yes() ; else no() 0
yes if ( bfs.bf3 -- ) yes() ; else no() 254
yes if ( -- bfs.bf3 ) yes() ; else no() 254
no if ( ++ bfs.bf3 ) yes() ; else no() 0
