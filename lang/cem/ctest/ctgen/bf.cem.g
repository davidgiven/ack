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
bfs.bf3 = bfs.bf1  -1
bfs.bf3 += bfs.bf1  -2
bfs.bf3 -= bfs.bf1  0
bfs.bf3 /= bfs.bf1  1
bfs.bf3 %= bfs.bf1  0
bfs.bf3 *= bfs.bf1  1
bfs.bf3 &= bfs.bf1  -1
bfs.bf3 |= bfs.bf1  -1
bfs.bf3 ^= bfs.bf1  0
bfs.bf3 <<= 1 -2
bfs.bf3 >>= 1 -1
bfs.bf3 ++ 0
bfs.bf3 -- -2
-- bfs.bf3 -2
++ bfs.bf3 0
bfs.bf4 = ( bfs.bf1 + bfs.bf2 ) -9
bfs.bf4 = ( bfs.bf1 - bfs.bf2 ) 7
bfs.bf4 = ( bfs.bf1 / bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 % bfs.bf2 ) -1
bfs.bf4 = ( bfs.bf1 * bfs.bf2 ) 8
bfs.bf4 = ( bfs.bf1 & bfs.bf2 ) -8
bfs.bf4 = ( bfs.bf1 | bfs.bf2 ) -1
bfs.bf4 = ( bfs.bf1 ^ bfs.bf2 ) 7
bfs.bf4 = ( bfs.bf1 || bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 && bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 << 1 ) -2
bfs.bf4 = ( bfs.bf1 >> 1 ) -1
bfs.bf4 = ( -bfs.bf1  ) 1
bfs.bf4 = ( !bfs.bf1  ) 0
bfs.bf4 = ( ~bfs.bf1  ) 0
bfs.bf4 = ( bfs.bf1 == bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 != bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 <= bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 >= bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 < bfs.bf2 ) 0
bfs.bf4 = ( bfs.bf1 > bfs.bf2 ) 1
bfs.bf4 = ( bfs.bf1 ? bfs.bf1 : bfs.bf2 ) -1
bfs.bf4 = ( bfs.bf3 = bfs.bf1  ) -1 -1
bfs.bf4 = ( bfs.bf3 += bfs.bf1  ) -2 -2
bfs.bf4 = ( bfs.bf3 -= bfs.bf1  ) 0 0
bfs.bf4 = ( bfs.bf3 /= bfs.bf1  ) 1 1
bfs.bf4 = ( bfs.bf3 %= bfs.bf1  ) 0 0
bfs.bf4 = ( bfs.bf3 *= bfs.bf1  ) 1 1
bfs.bf4 = ( bfs.bf3 &= bfs.bf1  ) -1 -1
bfs.bf4 = ( bfs.bf3 |= bfs.bf1  ) -1 -1
bfs.bf4 = ( bfs.bf3 ^= bfs.bf1  ) 0 0
bfs.bf4 = ( bfs.bf3 <<= 1 ) -2 -2
bfs.bf4 = ( bfs.bf3 >>= 1 ) -1 -1
bfs.bf4 = ( bfs.bf3 ++ ) 0 -1
bfs.bf4 = ( bfs.bf3 -- ) -2 -1
bfs.bf4 = ( -- bfs.bf3 ) -2 -2
bfs.bf4 = ( ++ bfs.bf3 ) 0 0
yes if ( bfs.bf1 + bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 - bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 / bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 % bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 * bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 & bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 | bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 ^ bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 || bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 && bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 << 1 ) yes() ; else no()
yes if ( bfs.bf1 >> 1 ) yes() ; else no()
yes if ( -bfs.bf1  ) yes() ; else no()
no if ( !bfs.bf1  ) yes() ; else no()
no if ( ~bfs.bf1  ) yes() ; else no()
no if ( bfs.bf1 == bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 != bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 <= bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 >= bfs.bf2 ) yes() ; else no()
no if ( bfs.bf1 < bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 > bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf1 ? bfs.bf1 : bfs.bf2 ) yes() ; else no()
yes if ( bfs.bf3 = bfs.bf1  ) yes() ; else no() -1
yes if ( bfs.bf3 += bfs.bf1  ) yes() ; else no() -2
no if ( bfs.bf3 -= bfs.bf1  ) yes() ; else no() 0
yes if ( bfs.bf3 /= bfs.bf1  ) yes() ; else no() 1
no if ( bfs.bf3 %= bfs.bf1  ) yes() ; else no() 0
yes if ( bfs.bf3 *= bfs.bf1  ) yes() ; else no() 1
yes if ( bfs.bf3 &= bfs.bf1  ) yes() ; else no() -1
yes if ( bfs.bf3 |= bfs.bf1  ) yes() ; else no() -1
no if ( bfs.bf3 ^= bfs.bf1  ) yes() ; else no() 0
yes if ( bfs.bf3 <<= 1 ) yes() ; else no() -2
yes if ( bfs.bf3 >>= 1 ) yes() ; else no() -1
yes if ( bfs.bf3 ++ ) yes() ; else no() 0
yes if ( bfs.bf3 -- ) yes() ; else no() -2
yes if ( -- bfs.bf3 ) yes() ; else no() -2
no if ( ++ bfs.bf3 ) yes() ; else no() 0
