#
 mes 2,EM_WSIZE,EM_PSIZE

 ; _LtoUset is called for set displays containing { expr1 .. expr2 }.
 ; It has five parameters, of which the caller must pop four:
 ; - The set in which bits must be set.
 ; - The set size in bytes.
 ; - The upper bound of set elements, specified by the set-type.
 ; - "expr2", the upper bound
 ; - "expr1", the lower bound

#define SETBASE	4*EM_WSIZE
#define SETSIZE 3*EM_WSIZE
#define USETSIZ 2*EM_WSIZE
#define LWB	EM_WSIZE
#define UPB	0
 exp $_LtoUset
 pro $_LtoUset,0
 lal SETBASE	; address of initial set
 lol SETSIZE
 los EM_WSIZE	; load initial set
1
 lol LWB	; low bound
 lol UPB	; high bound
 bgt *2		; while low <= high
 lol LWB
 lol SETSIZE
 set ?		; create [low]
 lol SETSIZE
 ior ?		; merge with initial set
 inl LWB	; increment low bound
 bra *1		; loop back
2
 lal SETBASE
 lol SETSIZE
 sts EM_WSIZE	; store result over initial set
 ret 0
 end 0
