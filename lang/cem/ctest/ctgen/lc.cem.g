'0' + '1'
'0' - '1'
'0' / '1'
'0' % '1'
'0' * '1'
'0' & '1'
'0' | '1'
'0' ^ '1'
'0' || '1'
'0' && '1'
'0' << 4
'0' >> 4
-'0' 
!'0' 
~'0' 
'0' == '1'
'0' != '1'
'0' <= '1'
'0' >= '1'
'0' < '1'
'0' > '1'
'0' ? '0' : '1'
x = '0'  48
x += '0'  58
x -= '0'  218
x /= '0'  0
x %= '0'  10
x *= '0'  224
x &= '0'  0
x |= '0'  58
x ^= '0'  58
x <<= 4 160
x >>= 4 0
x ++ 11
x -- 9
-- x 9
++ x 11
y = ( '0' + '1' ) 97
y = ( '0' - '1' ) 255
y = ( '0' / '1' ) 0
y = ( '0' % '1' ) 48
y = ( '0' * '1' ) 48
y = ( '0' & '1' ) 48
y = ( '0' | '1' ) 49
y = ( '0' ^ '1' ) 1
y = ( '0' || '1' ) 1
y = ( '0' && '1' ) 1
y = ( '0' << 4 ) 0
y = ( '0' >> 4 ) 3
y = ( -'0'  ) 208
y = ( !'0'  ) 0
y = ( ~'0'  ) 207
y = ( '0' == '1' ) 0
y = ( '0' != '1' ) 1
y = ( '0' <= '1' ) 1
y = ( '0' >= '1' ) 0
y = ( '0' < '1' ) 1
y = ( '0' > '1' ) 0
y = ( '0' ? '0' : '1' ) 48
y = ( x = '0'  ) 48 48
y = ( x += '0'  ) 58 58
y = ( x -= '0'  ) 218 218
y = ( x /= '0'  ) 0 0
y = ( x %= '0'  ) 10 10
y = ( x *= '0'  ) 224 224
y = ( x &= '0'  ) 0 0
y = ( x |= '0'  ) 58 58
y = ( x ^= '0'  ) 58 58
y = ( x <<= 4 ) 160 160
y = ( x >>= 4 ) 0 0
y = ( x ++ ) 11 10
y = ( x -- ) 9 10
y = ( -- x ) 9 9
y = ( ++ x ) 11 11
yes if ( '0' + '1' ) yes() ; else no()
yes if ( '0' - '1' ) yes() ; else no()
no if ( '0' / '1' ) yes() ; else no()
yes if ( '0' % '1' ) yes() ; else no()
yes if ( '0' * '1' ) yes() ; else no()
yes if ( '0' & '1' ) yes() ; else no()
yes if ( '0' | '1' ) yes() ; else no()
yes if ( '0' ^ '1' ) yes() ; else no()
yes if ( '0' || '1' ) yes() ; else no()
yes if ( '0' && '1' ) yes() ; else no()
yes if ( '0' << 4 ) yes() ; else no()
yes if ( '0' >> 4 ) yes() ; else no()
yes if ( -'0'  ) yes() ; else no()
no if ( !'0'  ) yes() ; else no()
yes if ( ~'0'  ) yes() ; else no()
no if ( '0' == '1' ) yes() ; else no()
yes if ( '0' != '1' ) yes() ; else no()
yes if ( '0' <= '1' ) yes() ; else no()
no if ( '0' >= '1' ) yes() ; else no()
yes if ( '0' < '1' ) yes() ; else no()
no if ( '0' > '1' ) yes() ; else no()
yes if ( '0' ? '0' : '1' ) yes() ; else no()
yes if ( x = '0'  ) yes() ; else no() 48
yes if ( x += '0'  ) yes() ; else no() 58
yes if ( x -= '0'  ) yes() ; else no() 218
no if ( x /= '0'  ) yes() ; else no() 0
yes if ( x %= '0'  ) yes() ; else no() 10
yes if ( x *= '0'  ) yes() ; else no() 224
no if ( x &= '0'  ) yes() ; else no() 0
yes if ( x |= '0'  ) yes() ; else no() 58
yes if ( x ^= '0'  ) yes() ; else no() 58
yes if ( x <<= 4 ) yes() ; else no() 160
no if ( x >>= 4 ) yes() ; else no() 0
yes if ( x ++ ) yes() ; else no() 11
yes if ( x -- ) yes() ; else no() 9
yes if ( -- x ) yes() ; else no() 9
yes if ( ++ x ) yes() ; else no() 11
