.bp
.AP "EM INTERPRETER"
.nf
.ft CW
.lg 0
.nr x \w'        '
.ta \nxu +\nxu +\nxu +\nxu +\nxu +\nxu +\nxu +\nxu +\nxu +\nxu

{ This  is an interpreter for EM.  It serves as  the official machine
  definition.  This interpreter must run on a machine which supports
  arithmetic with words and memory offsets.

  Certain aspects of the definition are over specified.  In particular:

    1. The representation of  an  address on the stack  need not be the
       numerical value of the memory location.

    2. The state of  the stack is not defined  after a trap has aborted
       an instruction in the middle.  For example, it is officially un-
       defined  whether the second  operand of an  ADD  instruction has
       been popped  or  not  if the  first one is undefined ( -32768 or
       unsigned 32768).

    3. The memory layout is implementation dependent. Only the most
       basic checks are performed whenever memory is accessed.

    4. The representation of an integer or set on the stack is not fixed
       in bit order.

    5. The format and existence of the procedure descriptors depends on
       the implementation.

    6. The result of the compare operators  CMI etc.  are -1, 0  and  1
       here, but other negative  and  positive values will do  and they
       need not be the same each time.

    7. The shift count for SHL, SHR, ROL and ROR must be in the range 0
       to object size in bits - 1.  The effect of a  count  not in this
       range is undefined.
}
.bp
{$i256} {$d+}
program em(tables,prog,input,output);

label 8888,9999;

const
  t15   = 32768;        { 2**15    }
  t15m1 = 32767;        { 2**15 -1 }
  t16   = 65536;        { 2**16    }
  t16m1 = 65535;        { 2**16 -1 }
  t31m1 = 2147483647;   { 2**31 -1 }

  wsize = 2;            { number of bytes in a word }
  asize = 2;            { number of bytes in an address }
  fsize = 4;            { number of bytes in a floating point number }
  maxret =4;            { number of words in the return value area }

  signbit = t15;        { the power of two indicating the sign bit }
  negoff  = t16;        { the next power of two }
  maxsint = t15m1;      { the maximum signed integer }
  maxuint = t16m1;      { the maximum unsigned integer }
  maxdbl  = t31m1;      { the maximum double signed integer }
  maxadr  = t16m1;      { the maximum address }
  maxoffs = t15m1;      { the maximum offset from an address }
  maxbitnr= 15;         { the number of the highest bit }

  lineadr = 0;          { address of the line number }
  fileadr = 4;          { address of the file name }
  maxcode = 8191;       { highest byte in code address space }
  maxdata = 8191;       { highest byte in data address space }

  { format of status save area }
  statd   = 4;          { how far is static link from lb }
  dynd    = 2;          { how far is dynamic link from lb }
  reta    = 0;          { how far is the return address from lb }
  savsize = 4;          { size of save area in bytes }

  { procedure descriptor format }
  pdlocs  = 0;          { offset for size of local variables in bytes }
  pdbase  = asize;      { offset for the procedure base }
  pdsize  = 4;          { size of procedure descriptor in bytes = 2*asize }

  { header words }
  NTEXT   = 1;
  NDATA   = 2;
  NPROC   = 3;
  ENTRY   = 4;
  NLINE   = 5;
  SZDATA  = 6;

  escape1 = 254;        { escape to secondary opcodes }
  escape2 = 255;        { escape to tertiary opcodes }
  undef   = signbit;    { the range of integers is -32767 to +32767 }

  { error codes }
  EARRAY   =  0; ERANGE   =  1; ESET     =  2; EIOVFL   =  3; EFOVFL   =  4; 
  EFUNFL   =  5; EIDIVZ   =  6; EFDIVZ   =  7; EIUND    =  8; EFUND    =  9; 
  ECONV    = 10; ESTACK   = 16; EHEAP    = 17; EILLINS  = 18; EODDZ    = 19; 
  ECASE    = 20; EMEMFLT  = 21; EBADPTR  = 22; EBADPC   = 23; EBADLAE  = 24;
  EBADMON  = 25; EBADLIN  = 26; EBADGTO  = 27;
.ne 20
.bp
{---------------------------------------------------------------------------}
{                             Declarations                                  }
{---------------------------------------------------------------------------}

type
  bitval= 0..1;             { one bit }
  bitnr=  0..maxbitnr;      { bits in machine words are numbered 0 to 15 }
  byte=   0..255;           { memory is an array of bytes }
  adr=    {0..maxadr} long; { the range of addresses }
  word=   {0..maxuint} long;{ the range of unsigned integers }
  offs=  -maxoffs..maxoffs; { the range of signed offsets from addresses }
  size=   0..maxoffs;       { the range of sizes is the positive offsets }
  sword= {-signbit..maxsint} long; { the range of signed integers }
  full=  {-maxuint..maxuint} long; { intermediate results need this range }
  double={-maxdbl..maxdbl} long;   { double precision range }
  bftype= (andf,iorf,xorf); { tells which boolean operator needed }
  insclass=(prim,second,tert); { tells which opcode table is in use }
  instype=(implic,explic);  { does opcode have implicit or explicit operand }
  iflags= (mini,short,sbit,wbit,zbit,ibit);
  ifset=  set of iflags;

  mnem = ( NON,
	   AAR, ADF, ADI, ADP, ADS, ADU,XAND, ASP, ASS, BEQ,
	   BGE, BGT, BLE, BLM, BLS, BLT, BNE, BRA, CAI, CAL,
	   CFF, CFI, CFU, CIF, CII, CIU, CMF, CMI, CMP, CMS,
	   CMU, COM, CSA, CSB, CUF, CUI, CUU, DCH, DEC, DEE,
	   DEL, DUP, DUS, DVF, DVI, DVU, EXG, FEF, FIF, FIL,
	   GTO, INC, INE, INL, INN, IOR, LAE, LAL, LAR, LDC,
	   LDE, LDF, LDL, LFR, LIL, LIM, LIN, LNI, LOC, LOE,
	   LOF, LOI, LOL, LOR, LOS, LPB, LPI, LXA, LXL, MLF,
	   MLI, MLU, MON, NGF, NGI, NOP, RCK, RET, RMI, RMU,
	   ROL, ROR, RTT, SAR, SBF, SBI, SBS, SBU, SDE, SDF,
	   SDL,XSET, SIG, SIL, SIM, SLI, SLU, SRI, SRU, STE,
	   STF, STI, STL, STR, STS, TEQ, TGE, TGT, TLE, TLT,
	   TNE, TRP, XOR, ZEQ, ZER, ZGE, ZGT, ZLE, ZLT, ZNE,
	   ZRE, ZRF, ZRL);

  dispatch = record
		iflag: ifset;
		instr: mnem;
		case instype of
		implic: (implicit:sword);
		explic: (ilength:byte);
	     end;


var
  code: packed array[0..maxcode] of byte;      { code space }
  data: packed array[0..maxdata] of byte;      { data space }
  retarea: array[1..maxret ] of word;          { return area }
  pc,lb,sp,hp,pd: adr;  { internal machine registers }
  i: integer;           { integer scratch variable }
  s,t :word;            { scratch variables }
  sz:size;              { scratch variables }
  ss,st: sword;         { scratch variables }
  k :double;            { scratch variables }
  j:size;               { scratch variable used as index }
  a,b:adr;              { scratch variable used for addresses }
  dt,ds:double;         { scratch variables for double precision }
  rt,rs,x,y:real;       { scratch variables for real }
  found:boolean;        { scratch }
  opcode: byte;         { holds the opcode during execution }
  iclass: insclass;     { true for escaped opcodes }
  dispat: array[insclass,byte] of dispatch;
  retsize:size;         { holds size of last LFR }
  insr: mnem;           { holds the instruction number }
  halted: boolean;      { normally false }
  exitstatus:word;      { parameter of MON 1 }
  ignmask:word;         { ignore mask for traps }
  uerrorproc:adr;       { number of user defined error procedure }
  intrap:boolean;       { Set when executing trap(), to catch recursive calls}
  trapval:byte;         { Set to number of last trap }
  header: array[1..8] of adr;

  tables: text;         { description of EM instructions }
  prog: file of byte;   { program and initialized data }
.ne 20
.sp 2
{---------------------------------------------------------------------------}
{                        Various check routines                             }
{---------------------------------------------------------------------------}

{ Only the most basic checks are performed. These routines are inherently
  implementation dependent. }

procedure trap(n:byte); forward;

procedure memadr(a:adr);
begin if (a>maxdata) or ((a<sp) and (a>=hp)) then trap(EMEMFLT) end;

procedure wordadr(a:adr);
begin memadr(a); if (a mod wsize<>0) then trap(EBADPTR) end;

procedure chkadr(a:adr; s:size);
begin memadr(a); memadr(a+s-1); { assumption: size is ok }
      if s<wsize
      then begin if a mod s<>0  then trap(EBADPTR) end
      else       if a mod wsize<>0 then trap(EBADPTR)
end;

procedure newpc(a:double);
begin if (a<0) or (a>maxcode) then trap(EBADPC); pc:=a end;

procedure newsp(a:adr);
begin if (a>lb) or (a<hp) or (a mod wsize<>0) then trap(ESTACK); sp:=a end;

procedure newlb(a:adr);
begin if (a<sp) or (a mod wsize<>0) then trap(ESTACK); lb:=a end;

procedure newhp(a:adr);
begin if (a>sp) or (a>maxdata+1) or (a mod wsize<>0)
      then trap(EHEAP)
      else hp:=a
end;

function argc(a:double):sword;
begin if (a<-signbit) or (a>maxsint) then trap(EILLINS); argc:=a end;

function argd(a:double):double;
begin if (a<-maxdbl) or (a>maxdbl) then trap(EILLINS); argd:=a end;

function argl(a:double):offs;
begin if (a<-maxoffs) or (a>maxoffs) then trap(EILLINS); argl:=a end;

function argg(k:double):adr;
begin if (k<0) or (k>maxadr) then trap(EILLINS); argg:=k end;

function argf(a:double):offs;
begin if (a<-maxoffs) or (a>maxoffs) then trap(EILLINS); argf:=a end;

function argn(a:double):word;
begin if (a<0) or (a>maxuint) then trap(EILLINS); argn:=a end;

function args(a:double):size;
begin if (a<=0) or (a>maxoffs)
	then trap(EODDZ)
	else if (a mod wsize)<>0 then trap(EODDZ);
      args:=a ;
end;

function argz(a:double):size;
begin if (a<0) or (a>maxoffs)
	then trap(EODDZ)
	else if (a mod wsize)<>0 then trap(EODDZ);
      argz:=a ;
end;

function argo(a:double):size;
begin if (a<=0) or (a>maxoffs)
	then trap(EODDZ)
	else if (a mod wsize<>0) and (wsize mod a<>0) then trap(EODDZ);
      argo:=a ;
end;

function argw(a:double):size;
begin if (a<=0) or (a>maxoffs) or (a>maxuint)
	then trap(EODDZ)
	else if (a mod wsize)<>0 then trap(EODDZ);
      argw:=a ;
end;

function argp(a:double):size;
begin if (a<0) or (a>=header[NPROC]) then trap(EILLINS); argp:=a end;

function argr(a:double):word;
begin if (a<0) or (a>2) then trap(EILLINS); argr:=a end;

procedure argwf(s:double);
begin if argw(s)<>fsize then trap(EILLINS) end;

function szindex(s:double):integer;
begin s:=argw(s); if (s mod wsize <> 0) or (s>2*wsize) then trap(EILLINS);
      szindex:=s div wsize
end;

function locadr(l:double):adr;
begin l:=argl(l); if l<0 then locadr:=lb+l else locadr:=lb+l+savsize end;

function signwd(w:word):sword;
begin if w = undef then trap(EIUND);
      if w >= signbit then signwd:=w-negoff else signwd:=w
end;

function dosign(w:word):sword;
begin if w >= signbit then dosign:=w-negoff else dosign:=w end;

function unsign(w:sword):word;
begin if w<0 then unsign:=w+negoff else unsign:=w end;

function chopw(dw:double):word;
begin chopw:=dw mod negoff end;

function fitsw(w:full;trapno:byte):word;
{ checks whether value fits in signed word, returns unsigned representation}
begin
  if (w>maxsint) or (w<-signbit) then
    begin trap(trapno);
      if w<0 then fitsw:=negoff- (-w)mod negoff
	     else fitsw:=w mod negoff;
    end
  else fitsw:=unsign(w)
end;

function fitd(w:full):double;
begin
  if abs(w) > maxdbl then trap(ECONV);
  fitd:=w
end;
.ne 20
.sp 2
{---------------------------------------------------------------------------}
{                        Memory access routines                             }
{---------------------------------------------------------------------------}

{ memw returns a machine word as an unsigned integer
  memb returns a single byte as a positive integer: 0 <= memb <= 255
  mems(a,s) fetches an object smaller than a word and returns a word
  store(a,v) stores the word v at machine address a
  storea(a,v) stores the address v at machine address a
  storeb(a,b) stores the byte b at machine address a
  stores(a,s,v) stores the s least significant bytes of a word at address a
  memi returns an offset from the instruction space
       Note that the procedure descriptors are part of instruction space.
  nextpc returns the next byte addressed by pc, incrementing pc

  lino changes the line number word.
  filna changes the pointer to the file name.

  All routines check to make sure the address is within range and valid for
  the size of the object. If an addressing error is found, a trap occurs.
}


function memw(a:adr):word;
var b:word; i:integer;
begin wordadr(a); b:=0;
      for i:=wsize-1 downto 0 do b:=256*b + data[a+i] ;
      memw:=b
end;

function memd(a:adr):double; { Always signed }
var b:double; i:integer;
begin wordadr(a); b:=data[a+2*wsize-1];
      if b>=128 then b:=b-256;
      for i:=2*wsize-2 downto 0 do b:=256*b + data[a+i] ;
      memd:=b
end;

function mema(a:adr):adr;
var b:adr; i:integer;
begin wordadr(a); b:=0;
      for i:=asize-1 downto 0 do b:=256*b + data[a+i] ;
      mema:=b
end;

function mems(a:adr;s:size):word;
var i:integer; b:word;
begin chkadr(a,s); b:=0; for i:=1 to s do b:=b*256+data[a+s-i]; mems:=b end;

function memb(a:adr):byte;
begin memadr(a); memb:=data[a] end;

procedure store(a:adr; x:word);
var i:integer;
begin wordadr(a);
  for i:=0 to wsize-1 do
     begin data[a+i]:=x mod 256; x:=x div 256 end
end;

procedure storea(a:adr; x:adr);
var i:integer;
begin wordadr(a);
  for i:=0 to asize-1 do
     begin data[a+i]:=x mod 256; x:=x div 256 end
end;

procedure stores(a:adr;s:size;v:word);
var i:integer;
begin chkadr(a,s);
  for i:=0 to s-1 do begin data[a+i]:=v mod 256; v:=v div 256 end;
end;

procedure storeb(a:adr; b:byte);
begin memadr(a); data[a]:=b end;

function memi(a:adr):adr;
var b:adr; i:integer;
begin if (a mod wsize<>0) or (a+asize-1>maxcode) then trap(EBADPTR); b:=0;
      for i:=asize-1 downto 0 do b:=256*b + code[a+i] ;
      memi:=b
end;

function nextpc:byte;
begin if pc>=pd then trap(EBADPC); nextpc:=code[pc]; newpc(pc+1) end;

procedure lino(w:word);
begin store(lineadr,w) end;

procedure filna(a:adr);
begin storea(fileadr,a) end;
.ne 20
.sp 2
{---------------------------------------------------------------------------}
{                    Stack Manipulation Routines                            }
{---------------------------------------------------------------------------}

{ push puts a word on the stack
  pushsw takes a signed one word integer and pushes it on the stack
  pop removes a machine word from the stack and delivers it as a word
  popsw removes a machine word from the stack and delivers a signed integer
  pusha pushes an address on the stack
  popa removes a machine word from the stack and delivers it as an address
  pushd pushes a double precision number on the stack
  popd removes two machine words and returns a double precision integer
  pushr pushes a float (floating point) number on the stack
  popr removes several machine words and returns a float number
  pushx puts an object of arbitrary size on the stack
  popx removes an object of arbitrary size
									  }

procedure push(x:word);
begin newsp(sp-wsize); store(sp,x) end;

procedure pushsw(x:sword);
begin newsp(sp-wsize); store(sp,unsign(x)) end;

function pop:word;
begin pop:=memw(sp); newsp(sp+wsize) end;

function popsw:sword;
begin popsw:=signwd(pop) end;

procedure pusha(x:adr);
begin newsp(sp-asize); storea(sp,x) end;

function popa:adr;
begin popa:=mema(sp); newsp(sp+asize) end;

procedure pushd(y:double);
begin { push double integer onto the stack } newsp(sp-2*wsize) end;

function popd:double;
begin { pop double integer from the stack } newsp(sp+2*wsize); popd:=0 end;

procedure pushr(z:real);
begin { Push a float onto the stack } newsp(sp-fsize) end;

function popr:real;
begin { pop float from the stack } newsp(sp+fsize); popr:=0.0 end;

procedure pushx(objsize:size; a:adr);
var i:integer;
begin
  if objsize<wsize
     then push(mems(a,objsize))
     else for i:=1 to objsize div wsize do push(memw(a+objsize-wsize*i))
end;

procedure popx(objsize:size; a:adr);
var i:integer;
begin
  if objsize<wsize
     then stores(a,objsize,pop)
     else for i:=1 to objsize div wsize do store(a-wsize+wsize*i,pop)
end;
.ne 20
.sp 2
{---------------------------------------------------------------------------}
{              Bit manipulation routines (extract, shift, rotate)           }
{---------------------------------------------------------------------------}

procedure sleft(var w:sword);  { 1 bit left shift   }
begin w:= dosign(fitsw(2*w,EIOVFL)) end;

procedure suleft(var w:word);  { 1 bit left shift   }
begin w := chopw(2*w) end;

procedure sdleft(var d:double);  { 1 bit left shift   }
begin { shift two word signed integer } end;

procedure sright(var w:sword);  { 1 bit right shift with sign extension }
begin if w >= 0 then w := w div 2 else w := (w-1) div 2 end;

procedure suright(var w:word);    { 1 bit right shift without sign extension }
begin w := w div 2 end;

procedure sdright(var d:double);  { 1 bit right shift   }
begin { shift two word signed integer } end;

procedure rleft(var w:word);  { 1 bit left rotate }
begin if w >= t15
	then w:=(w-t15)*2 + 1
	else w:=w*2
end;

procedure rright(var w:word);  { 1 bit right rotate }
begin if w mod 2 = 1
	then w:=w div 2 + t15
	else w:=w div 2
end;

function sextend(w:word;s:size):word;
var i:size;
begin
  for i:=1 to (wsize-s)*8 do rleft(w);
  for i:=1 to (wsize-s)*8 do sright(w);
  sextend:=w;
end;

function bit(b:bitnr; w:word):bitval; { return bit b of the word w }
var i:bitnr;
begin for i:= 1 to b do rright(w); bit:= w mod 2 end;

function bf(ty:bftype; w1,w2:word):word;  { return boolean fcn of 2 words }
var i:bitnr; j:word;
begin j:=0;
      for i:= maxbitnr downto 0 do
	begin j := 2*j;
	      case ty of
		andf: if bit(i,w1)+bit(i,w2) = 2 then j:=j+1;
		iorf: if bit(i,w1)+bit(i,w2) > 0 then j:=j+1;
		xorf: if bit(i,w1)+bit(i,w2) = 1 then j:=j+1
	      end
	end;
      bf:=j
end;

{---------------------------------------------------------------------------}
{                           Array indexing                                  }
{---------------------------------------------------------------------------}

function arraycalc(c:adr):adr; { subscript calculation }
var j:full; objsize:size; a:adr;
begin j:= popsw - signwd(memw(c));
  if (j<0) or (j>memw(c+wsize)) then trap(EARRAY);
  objsize := argo(memw(c+wsize+wsize));
  a := j*objsize+popa; chkadr(a,objsize);
  arraycalc:=a
end;
.ne 20
.sp 2
{---------------------------------------------------------------------------}
{                       Double and Real Arithmetic                          }
{---------------------------------------------------------------------------}

{ All routines for doubles and floats are dummy routines, since the format of
  doubles and floats is not defined in EM.
}

function doadi(ds,dt:double):double;
begin { add two doubles } doadi:=0 end;

function dosbi(ds,dt:double):double;
begin { subtract two doubles } dosbi:=0 end;

function domli(ds,dt:double):double;
begin { multiply two doubles } domli:=0 end;

function dodvi(ds,dt:double):double;
begin { divide two doubles } dodvi:=0 end;

function dormi(ds,dt:double):double;
begin { modulo of two doubles } dormi:=0 end;

function dongi(ds:double):double;
begin { negative of a double } dongi:=0 end;

function doadf(x,y:real):real;
begin { add two floats } doadf:=0.0 end;

function dosbf(x,y:real):real;
begin { subtract two floats } dosbf:=0.0 end;

function domlf(x,y:real):real;
begin { multiply two floats } domlf:=0.0 end;

function dodvf(x,y:real):real;
begin { divide two floats } dodvf:=0.0 end;

function dongf(x:real):real;
begin { negate a float } dongf:=0.0 end;

procedure dofif(x,y:real;var intpart,fraction:real);
begin { dismember x*y into integer and fractional parts }
  intpart:=0.0;  { integer part of x*y, same sign as x*y }
  fraction:=0.0;
	{ fractional part of x*y, 0<=abs(fraction)<1 and same sign as x*y }
end;

procedure dofef(x:real;var mantissa:real;var exponent:sword);
begin { dismember x into mantissa and exponent parts }
  mantissa:=0.0;  { mantissa of x , >= 1/2 and <1 }
  exponent:=0;    { base 2 exponent of x }
end;
.bp
{---------------------------------------------------------------------------}
{                            Trap and Call                                  }
{---------------------------------------------------------------------------}

procedure call(p:adr); { Perform the call }
begin
  pusha(lb);pusha(pc);
  newlb(sp);newsp(sp - memi(pd + pdsize*p + pdlocs));
  newpc(memi(pd + pdsize*p+ pdbase))
end;

procedure dotrap(n:byte);
var i:size;
begin
  if (uerrorproc=0) or intrap then
    begin
      if intrap then
	writeln('Recursive trap, first trap number was ', trapval:1);
      writeln('Error ', n:1);
      writeln('With',ord(insr):4,' arg ',k:1);
      goto 9999
    end;
  { Deposit all interpreter variables that need to be saved on
    the stack. This includes all scratch variables that can
    be in use at the moment and ( not possible in this interpreter )
    the internal address of the interpreter where the error occurred.
    This would make it possible to execute an RTT instruction totally
    transparent to the user program.
    It can, for example, occur within an ADD instruction that both
    operands are undefined and that the result overflows.
    Although this will generate 3 error traps it must be possible
    to ignore them all.
}
  intrap:=true; trapval:=n;
  for i:=retsize div wsize downto 1 do push(retarea[i]);
  push(retsize);              { saved return area }
  pusha(mema(fileadr));       { saved current file name pointer }
  push(memw(lineadr));        { saved line number }
  push(n);                    { push error number }
  a:=argp(uerrorproc);
  uerrorproc:=0;              { reset signal }
  call(a);                    { call the routine }
  intrap:=false;              { Don't catch recursive traps anymore }
  goto 8888;                  { reenter main loop }
end;

procedure trap;
{ This routine is invoked for overflow, and other run time errors.
  For non-fatal errors, trap returns to the calling routine
}
begin
  if n>=16 then dotrap(n) else if bit(n,ignmask)=0 then dotrap(n);
end;

procedure dortt;
{ The restoration of file address and line number is not essential.
  The restoration of the return save area is.
}
var i:size;
    n:word;
begin
  newsp(lb); lb:=maxdata+1 ; { to circumvent ESTACK for the popa + pop }
  newpc(popa); newlb(popa); { So far a plain RET 0 }
  n:=pop; if (n>=16) and (n<64) then goto 9999 ;
  lino(pop); filna(popa); retsize:=pop;
  for i:=1 to retsize div wsize do retarea[i]:=pop ;
end;
.sp 2
{---------------------------------------------------------------------------}
{                              monitor calls                                }
{---------------------------------------------------------------------------}


procedure domon(entry:word);
var     index:  1..63;
	dummy:  double;
	count,rwptr:    adr;
	token:  byte;
	i:      integer;
begin
  if (entry<=0) or (entry>63) then entry:=63 ;
  index:=entry;
  case index of
   1: begin { exit } exitstatus:=pop; halted:=true end;
   3: begin { read }  dummy:=pop; { All input is from stdin }
	rwptr:=popa; count:=popa;
	i:=0 ;
	while (not eof(input)) and (i<count) do
	begin
	  if eoln(input) then begin storeb(rwptr,10) ; count:=i end
			 else storeb(rwptr,ord(input^)) ;
	  get(input); rwptr:=rwptr+1 ; i:=i+1 ;
	end;
	pusha(i); push(0)
      end;
   4: begin { write } dummy:=pop; { All output is to stdout }
	rwptr:=popa; count:=popa;
	for i:=1 to count do
	  begin token:=memb(rwptr); rwptr:=rwptr+1 ;
	    if token=10 then writeln else write(chr(token))
	  end ;
	pusha(count);
	push(0)
      end;
  54: begin { ioctl, faked } dummy:=popa;dummy:=popa;dummy:=pop;push(0) end ;
       2,          5,  6,  7,  8,  9, 10,
  11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
  21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53,     55, 56, 57, 58, 59, 60,
  61, 62:
      begin push(22); push(22) end;
  63: { exists only for the trap }
      trap(EBADMON)
  end
end;
.bp
{---------------------------------------------------------------------------}
{                       Initialization and debugging                        }
{---------------------------------------------------------------------------}

procedure doident; { print line number and file name }
var a:adr; i,c:integer; found:boolean;
begin
  write('at line ',memw(lineadr):1,' ');
  a:=mema(fileadr); if a<>0 then
  begin i:=20; found:=false;
    while (i<>0) and not found do
    begin c:=memb(a); a:=a+1; found:=true; i:=i-1;
      if (c>=48) and (c<=57) then
	begin found:=false; write(chr(ord('0')+c-48)) end;
      if (c>=65) and (c<=90) then
	begin found:=false; write(chr(ord('A')+c-65)) end;
      if (c>=97) and (c<=122) then
	begin found:=false; write(chr(ord('a')+c-97)) end;
    end;
  end;
  writeln;
end;

procedure initialize;  { start the ball rolling }
{ This is not part of the machine definition }
var cset:set of char;
    f:ifset;
    iclass:insclass;
    insno:byte;
    nops:integer;
    opcode:byte;
    i,j,n:integer;
    wtemp:sword;
    count:integer;
    repc:adr;
    nexta,firsta:adr;
    elem:byte;
    amount,ofst:size;
    c:char;

    function readb(n:integer):double;
    var b:byte;
    begin read(prog,b); if n>1 then readb:=readb(n-1)*256+b else readb:=b end;

    function readbyte:byte;
    begin readbyte:=readb(1) end;

    function readword:word;
    begin readword:=readb(wsize) end;

    function readadr:adr;
    begin readadr:=readb(asize) end;

    function ifind(ordinal:byte):mnem;
    var loopvar:mnem;
	found:boolean;
    begin ifind:=NON;
      loopvar:=insr; found:=false;
      repeat
	if ordinal=ord(loopvar) then
	  begin found:=true; ifind:=loopvar end;
	if loopvar<>ZRL then loopvar:=succ(loopvar) else loopvar:=NON;
      until found or (loopvar=insr) ;
   end;

    procedure readhdr;
    type hdrw=0..32767 ; { 16 bit header words }
    var  hdr: hdrw;
	 i: integer;
    begin
      for i:=0 to 7 do
      begin hdr:=readb(2);
	case i of
	0: if hdr<>3757 then { 07255 }
	   begin writeln('Not an em load file'); halt end;
	2: if hdr<>0 then
	   begin writeln('Unsolved references'); halt end;
	3: if hdr<>3 then
	   begin writeln('Incorrect load file version'); halt end;
	4: if hdr<>wsize then
	   begin writeln('Incorrect word size'); halt end;
	5: if hdr<>asize then
	   begin writeln('Incorrect pointer size'); halt end;
	1,6,7:;
	end
      end
    end;

    procedure noinit;
    begin writeln('Illegal initialization'); halt end;

    procedure readint(a:adr;s:size);
    var i:size;
    begin { construct integer out of byte sequence }
      for i:=1 to s do { construct the value and initialize at a }
	begin storeb(a,readbyte); a:=a+1 end
    end;

    procedure readuns(a:adr;s:size);
    begin { construct unsigned out of byte sequence }
      readint(a,s) { identical to readint }
    end;

    procedure readfloat(a:adr;s:size);
    var i:size; b:byte;
    begin { construct float out of string}
      if (s<>4) and (s<>8) then noinit; i:=0;
      repeat { eat the bytes, construct the value and intialize at a }
	b:=readbyte; i:=i+1;
      until b=0 ;
    end;

begin
  halted:=false;
  exitstatus:=undef;
  uerrorproc:=0; intrap:=false;

  { initialize tables }
  for i:=0 to maxcode do code[i]:=0;
  for i:=0 to maxdata do data[i]:=0;
  for iclass:=prim to tert do
    for i:=0 to 255 do
      with dispat[iclass][i] do
	begin instr:=NON; iflag:=[zbit] end;

  { read instruction table file. see appendix B }
  { The table read here is a simple transformation of the table on page xx }
  { - instruction names were transformed to numbers }
  { - the '-' flag was transformed to an 'i' flag for 'w' type instructions }
  { - the 'S' flag was added for instructions having signed operands }
  reset(tables);
  insr:=NON;
  repeat
    read(tables,insno) ; cset:=[]; f:=[];
    insr:=ifind(insno);
    if insr=NON then begin writeln('Incorrect table'); halt end;
    repeat read(tables,c) until c<>' ' ;
    repeat
      cset:=cset+[c];
      read(tables,c)
    until c=' ' ;
    if 'm' in cset then f:=f+[mini];
    if 's' in cset then f:=f+[short];
    if '-' in cset then f:=f+[zbit];
    if 'i' in cset then f:=f+[ibit];
    if 'S' in cset then f:=f+[sbit];
    if 'w' in cset then f:=f+[wbit];
    if (mini in f) or (short in f) then read(tables,nops) else nops:=1 ;
    readln(tables,opcode);
    if ('4' in cset) or ('8' in cset) then
      begin iclass:=tert end
    else if 'e' in cset then
      begin iclass:=second end
    else iclass:=prim;
    for i:=0 to nops-1 do
    begin
      with dispat[iclass,opcode+i] do
      begin
	iflag:=f; instr:=insr;
	if '2' in cset      then ilength:=2
	else if 'u' in cset then ilength:=2
	else if '4' in cset then ilength:=4
	else if '8' in cset then ilength:=8
	else if (mini in f) or (short in f) then
	  begin
	    if 'N' in cset then wtemp:=-1-i else wtemp:=i ;
	    if 'o' in cset then wtemp:=wtemp+1 ;
	    if short in f then wtemp:=wtemp*256 ;
	    implicit:=wtemp
	  end
      end
    end
  until eof(tables);

  { read in program text, data and procedure descriptors }
  reset(prog);
  readhdr;                               { verify first header }
  for i:=1 to 8 do header[i]:=readadr;  { read second header }
  hp:=maxdata+1; sp:=maxdata+1; lino(0);
  { read program text }
  if header[NTEXT]+header[NPROC]*pdsize>maxcode then
    begin writeln('Text size too large'); halt end;
  if header[SZDATA]>maxdata then
    begin writeln('Data size too large'); halt end;
  for i:=0 to header[NTEXT]-1 do code[i]:=readbyte;
  { read data blocks }
  nexta:=0;
  for i:=1 to header[NDATA] do
    begin
      n:=readbyte;
      if n<>0 then
	begin
	  elem:=readbyte; firsta:=nexta;
	  case n of
	  1: { uninitialized words }
	     for j:=1 to elem do
	     begin store(nexta,undef); nexta:=nexta+wsize end;
	  2: { initialized bytes }
	     for j:=1 to elem do
	     begin storeb(nexta,readbyte); nexta:=nexta+1 end;
	  3: { initialized words }
	     for j:=1 to elem do
	     begin store(nexta,readword); nexta:=nexta+wsize end;
	  4,5: { instruction and data pointers }
	     for j:=1 to elem do
	     begin storea(nexta,readadr); nexta:=nexta+asize end;
	  6: { signed integers }
	     begin readint(nexta,elem); nexta:=nexta+elem end;
	  7: { unsigned integers }
	     begin readuns(nexta,elem); nexta:=nexta+elem end;
	  8: { floating point numbers }
	     begin readfloat(nexta,elem); nexta:=nexta+elem end;
	  end
	end
      else
	begin
	  repc:=readadr; amount:=nexta-firsta;
	  for count:=1 to repc do
	  begin
	    for ofst:=0 to amount-1 do data[nexta+ofst]:=data[firsta+ofst];
	    nexta:=nexta+amount;
	  end
	end
    end;
  if header[SZDATA]<>nexta then writeln('Data initialization error');
  hp:=nexta;
  { read descriptor table }
  pd:=header[NTEXT];
  for i:=1 to header[NPROC]*pdsize do code[pd+i-1]:=readbyte;
  { call the entry point routine }
  ignmask:=0;	   { catch all traps, higher numbered traps cannot be ignored}
  retsize:=0;
  lb:=maxdata;     { illegal dynamic link }
  pc:=maxcode;     { illegal return address }
  push(0); a:=sp;  { No environment }
  push(0); b:=sp;  { No args }
  pusha(a);        { envp }
  pusha(b);        { argv }
  push(0);         { argc }
  call(argp(header[ENTRY]));
end;
.bp
{---------------------------------------------------------------------------}
{                       MAIN LOOP OF THE INTERPRETER                        }
{---------------------------------------------------------------------------}
{ It should be noted that the interpreter (microprogram) for  an  EM
  machine  can be written in two fundamentally different ways: (1) the
  instruction operands are fetched in the main loop, or  (2)  the  in-
  struction operands are fetched after the 256 way branch, by the exe-
  cution routines themselves.  In this interpreter, method (1) is used
  to  simplify  the  description  of execution  routines. The dispatch
  table dispat is used to determine how the operand is  encoded. There
  are 4 possibilities:

     0. There is no operand
     1. The operand and  instruction are  together in 1  byte (mini)
     2. The operand is  one byte long and follows the opcode byte(s)
     3. The operand is two bytes long and follows the opcode byte(s)
     4. The operand is four bytes long and follows the opcode byte(s)

  In  this  interpreter,  the  main  loop determines the operand type,
  fetches it, and leaves it in the global variable k for the execution
  routines  to use.  Consequently, instructions such as LOL, which use
  three different formats, need only be described once in the body  of
  the interpreter.
      However, for  a  production  interpreter,  or  a  hardware  EM
  machine,  it  is  probably better to use method (2), i.e. to let the
  execution routines themselves fetch their own operands.  The  reason
  for this is that each opcode uniquely determines the operand format,
  so no table lookup in the dispatch table is needed.  The whole table
  is not needed. Method (2) therefore executes much faster.
      However, separate execution routines will be needed for LOL with
  a one byte offset,  and  LOL with a two byte offset.  It is to avoid
  this additional clutter that method (1) is used here.  In a  produc-
  tion interpreter, it is envisioned that the main loop will fetch the
  next instruction byte, and use it as an index into a 256 word  table
  to  find  the  address  of  the interpreter routine to jump to.  The
  routine jumped to will  begin  by  fetching  its  operand,  if  any,
  without  any  table  lookup,  since it knows which format to expect.
  After doing the work, it returns to the main  loop  by  jumping  in-
  directly  to  a register that contains the address of the main loop.
      A slight variation on this idea is to have the  register contain
  the address of the branch table, rather than the address of the main
  loop.
      Another  issue  is whether the execution routines for LOL 0, LOL
  2, LOL 4, etc. should all be have distinct execution routines. Doing
  so  provides for the maximum speed, since the operand is implicit in
  the routine itself.  The disadvantage is that many nearly  identical
  execution  routines will then be needed.  Another way of doing it is
  to keep the instruction byte fetched from memory (LOL 0, LOL 2,  LOL
  4, etc.) in some register, and have all the LOL mini format instruc-
  tions branch to a common routine.  This routine can  then  determine
  the  operand  by  subtracting  the code for LOL 0 from the register,
  leaving the true operand in the register  (as  a  word  quantity  of
  course).   This  method  makes the interpreter smaller, but is a bit
  slower.
.bp
       To make this important point a little clearer, consider how a
  production interpreter for the PDP-11 might appear.  Let us assume the
  following opcodes have been assigned:

       31: LOL -2     (2 bytes, i.e. next word)
       32: LOL -4
       33: LOL -6
       34: LOL b      (format with a one byte offset)
       35: LOL w      (format with a one word, i.e. two byte offset)

  Further assume that each of the 5 opcodes will have its own execution
  routine, i.e. we are making a tradeoff in favor of fast execution and
  a slightly larger interpreter.
       Register r5 is the em program counter.
       Register r4 is the em LB register
       Register r3 is the em SP register (the stack grows toward low core)
       Register r2 contains the interpreter address of the main loop

  The main loop looks like this:

       movb (r5)+,r0           /fetch the opcode into r0 and increment r5
       asl r0                  /shift r0 left 1 bit. Now: -256<=r0<=+254
       jmp *table(r0)          /jump to execution routine

  Notice that no operand fetching has been done. The execution routines for
  the 5 sample instructions given above might be as follows:

  lol2: mov -2(r4),-(sp)       /push local -2 onto stack
	jmp (r2)               /go back to main loop
  lol4: mov -4(r4),-(sp)       /push local -4 onto stack
	jmp (r2)               /go back to main loop
  lol6: mov -6(r4),-(sp)       /push local -6 onto stack
	jmp (r2)               /go back to main loop
  lolb: mov $177400,r0         /prepare to fetch the 1 byte operand
	bisb (r5)+,r0          /operand is now in r0
	asl r0                 /r0 is now offset from LB in bytes, not words
	add r4,r0              /r0 is now address of the needed local
	mov (r0),-(sp)         /push the local onto the stack
	jmp (r2)
  lolw: clr r0                 /prepare to fetch the 2 byte operand
	bisb (r5)+,r0          /fetch high order byte first !!!
	swab r0                /insert high order byte in place
	bisb (r5)+,r0          /insert low order byte in place
	asl r0                 /convert offset to bytes, from words
	add r4,r0              /r0 is now address of needed local
	mov (r0),-(sp)         /stack the local
	jmp (r2)               /done

  The important thing to notice is where and how the operand fetch occurred:
       lol2, lol4, and lol6, (the mini's) have implicit operands
       lolb knew it had to fetch one byte, and did so without any table lookup
       lolw knew it had to fetch a word, and did so, high order byte first }
.bp
.sp 4
{---------------------------------------------------------------------------}
{           Routines for the individual instructions                        }
{---------------------------------------------------------------------------}
procedure loadops;
var j:integer;
begin
  case insr of
    { LOAD GROUP }
    LDC: pushd(argd(k));
    LOC: pushsw(argc(k));
    LOL: push(memw(locadr(k)));
    LOE: push(memw(argg(k)));
    LIL: push(memw(mema(locadr(k))));
    LOF: push(memw(popa+argf(k)));
    LAL: pusha(locadr(k));
    LAE: pusha(argg(k));
    LXL: begin a:=lb; for j:=1 to argn(k) do a:=mema(a+savsize); pusha(a) end;
    LXA: begin a:=lb;
	   for j:=1 to argn(k) do a:= mema(a+savsize);
	   pusha(a+savsize)
	 end;
    LOI: pushx(argo(k),popa);
    LOS: begin k:=argw(k); if k<>wsize then trap(EILLINS);
	   k:=pop; pushx(argo(k),popa)
	 end;
    LDL: begin a:=locadr(k); push(memw(a+wsize)); push(memw(a)) end;
    LDE: begin k:=argg(k); push(memw(k+wsize)); push(memw(k)) end;
    LDF: begin k:=argf(k);
	   a:=popa; push(memw(a+k+wsize)); push(memw(a+k))
	 end;
    LPI: push(argp(k))
  end
end;

procedure storeops;
begin
  case insr of
    { STORE GROUP }
    STL: store(locadr(k),pop);
    STE: store(argg(k),pop);
    SIL: store(mema(locadr(k)),pop);
    STF: begin a:=popa; store(a+argf(k),pop) end;
    STI: popx(argo(k),popa);
    STS: begin k:=argw(k); if k<>wsize then trap(EILLINS);
	   k:=popa; popx(argo(k),popa)
	 end;
    SDL: begin a:=locadr(k); store(a,pop); store(a+wsize,pop) end;
    SDE: begin k:=argg(k); store(k,pop); store(k+wsize,pop) end;
    SDF: begin k:=argf(k); a:=popa; store(a+k,pop); store(a+k+wsize,pop) end
  end
end;

procedure intarith;
var i:integer;
begin
  case insr of
    { SIGNED INTEGER ARITHMETIC }
    ADI: case szindex(argw(k)) of
	 1: begin st:=popsw; ss:=popsw; push(fitsw(ss+st,EIOVFL)) end;
	 2: begin dt:=popd; ds:=popd; pushd(doadi(ds,dt)) end;
	 end ;
    SBI: case szindex(argw(k)) of
	 1: begin st:=popsw; ss:= popsw; push(fitsw(ss-st,EIOVFL)) end;
	 2: begin dt:=popd; ds:=popd; pushd(dosbi(ds,dt)) end;
	 end ;
    MLI: case szindex(argw(k)) of
	 1: begin st:=popsw; ss:= popsw; push(fitsw(ss*st,EIOVFL)) end;
	 2: begin dt:=popd; ds:=popd; pushd(domli(ds,dt)) end;
	 end ;
    DVI: case szindex(argw(k)) of
	 1: begin st:= popsw; ss:= popsw;
	       if st=0 then trap(EIDIVZ) else pushsw(ss div st)
	    end;
	 2: begin dt:=popd; ds:=popd; pushd(dodvi(ds,dt)) end;
	 end;
    RMI: case szindex(argw(k)) of
	 1: begin st:= popsw; ss:=popsw;
	       if st=0 then trap(EIDIVZ) else pushsw(ss - (ss div st)*st)
	    end;
	 2: begin dt:=popd; ds:=popd; pushd(dormi(ds,dt)) end
	 end;
    NGI: case szindex(argw(k)) of
	 1: begin st:=popsw; pushsw(-st) end;
	 2: begin ds:=popd; pushd(dongi(ds)) end
	 end;
    SLI: begin t:=pop;
	   case szindex(argw(k)) of
	   1: begin ss:=popsw;
		 for i:= 1 to t do sleft(ss); pushsw(ss)
	      end
	   end
	 end;
    SRI: begin t:=pop;
	   case szindex(argw(k)) of
	   1: begin ss:=popsw;
		 for i:= 1 to t do sright(ss); pushsw(ss)
	      end;
	   2: begin ds:=popd;
		 for i:= 1 to t do sdright(ss); pushd(ss)
	      end
	   end
	 end
  end
end;

procedure unsarith;
var i:integer;
begin
  case insr of
    { UNSIGNED INTEGER ARITHMETIC }
    ADU: case szindex(argw(k)) of
	 1: begin t:=pop; s:= pop; push(chopw(s+t)) end;
	 2: trap(EILLINS);
	 end ;
    SBU: case szindex(argw(k)) of
	 1: begin t:=pop; s:= pop; push(chopw(s-t)) end;
	 2: trap(EILLINS);
	 end ;
    MLU: case szindex(argw(k)) of
	 1: begin t:=pop; s:= pop; push(chopw(s*t)) end;
	 2: trap(EILLINS);
	 end ;
    DVU: case szindex(argw(k)) of
	 1: begin t:= pop; s:= pop;
	       if t=0 then trap(EIDIVZ) else push(s div t)
	    end;
	 2: trap(EILLINS);
	 end;
    RMU: case szindex(argw(k)) of
	 1: begin t:= pop; s:=pop;
	       if t=0 then trap(EIDIVZ) else push(s - (s div t)*t)
	    end;
	 2: trap(EILLINS);
	 end;
    SLU: case szindex(argw(k)) of
	 1: begin t:=pop; s:=pop;
	       for i:= 1 to t do suleft(s); push(s)
	    end;
	 2: trap(EILLINS);
	 end;
    SRU: case szindex(argw(k)) of
	 1: begin t:=pop; s:=pop;
	       for i:= 1 to t do suright(s); push(s)
	    end;
	 2: trap(EILLINS);
	 end
  end
end;

procedure fltarith;
begin
  case insr of
    { FLOATING POINT ARITHMETIC }
    ADF: begin argwf(k); rt:=popr; rs:=popr; pushr(doadf(rs,rt)) end;
    SBF: begin argwf(k); rt:=popr; rs:=popr; pushr(dosbf(rs,rt)) end;
    MLF: begin argwf(k); rt:=popr; rs:=popr; pushr(domlf(rs,rt)) end;
    DVF: begin argwf(k); rt:=popr; rs:=popr; pushr(dodvf(rs,rt)) end;
    NGF: begin argwf(k); rt:=popr; pushr(dongf(rt)) end;
    FIF: begin argwf(k); rt:=popr; rs:=popr;
	       dofif(rt,rs,x,y); pushr(y); pushr(x)
	 end;
    FEF: begin argwf(k); rt:=popr; dofef(rt,x,ss); pushr(x); pushsw(ss) end
  end
end;

procedure ptrarith;
begin
  case insr of
    { POINTER ARITHMETIC }
    ADP: pusha(popa+argf(k));
    ADS: case szindex(argw(k)) of
	 1: begin st:=popsw; pusha(popa+st) end;
	 2: begin dt:=popd; pusha(popa+dt) end;
	 end;
    SBS: begin
	   a:=popa; b:=popa;
	   case szindex(argw(k)) of
	     1: push(fitsw(b-a,EIOVFL));
	     2: pushd(b-a)
	   end
	 end
  end
end;

procedure incops;
var j:integer;
begin
  case insr of
    { INCREMENT/DECREMENT/ZERO }
    INC: push(fitsw(popsw+1,EIOVFL));
    INL: begin a:=locadr(k); store(a,fitsw(signwd(memw(a))+1,EIOVFL)) end;
    INE: begin a:=argg(k); store(a,fitsw(signwd(memw(a))+1,EIOVFL)) end;
    DEC: push(fitsw(popsw-1,EIOVFL));
    DEL: begin a:=locadr(k); store(a,fitsw(signwd(memw(a))-1,EIOVFL)) end;
    DEE: begin a:=argg(k); store(a,fitsw(signwd(memw(a))-1,EIOVFL)) end;
    ZRL: store(locadr(k),0);
    ZRE: store(argg(k),0);
    ZER: for j:=1 to argw(k) div wsize do push(0);
    ZRF: pushr(0);
  end
end;

procedure convops;
begin
  case insr of
    { CONVERT GROUP }
    CII: begin s:=pop; t:=pop;
	   if t<wsize then begin push(sextend(pop,t)); t:=wsize end;
	   case szindex(argw(t)) of
	   1: if szindex(argw(s))=2 then pushd(popsw);
	   2: if szindex(argw(s))=1 then push(fitsw(popd,ECONV))
	   end
	 end;
    CIU: case szindex(argw(pop)) of
	 1: if szindex(argw(pop))=2 then push(unsign(popd mod negoff));
	 2: trap(EILLINS);
	 end;
    CIF: begin argwf(pop);
	   case szindex(argw(pop)) of 1:pushr(popsw); 2:pushr(popd) end
	 end;
    CUI: case szindex(argw(pop)) of
	 1: case szindex(argw(pop)) of
	    1: begin s:=pop; if s>maxsint then trap(ECONV); push(s) end;
	    2: trap(EILLINS);
	    end;
	 2: case szindex(argw(pop)) of
	    1: pushd(pop);
	    2: trap(EILLINS);
	    end;
	 end;
    CUU: case szindex(argw(pop)) of
	 1: if szindex(argw(pop))=2 then trap(EILLINS);
	 2: trap(EILLINS);
	 end;
    CUF: begin argwf(pop);
	   if szindex(argw(pop))=1 then pushr(pop) else trap(EILLINS)
	 end;
    CFI: begin sz:=argw(pop); argwf(pop); rt:=popr;
	   case szindex(sz) of
	   1: push(fitsw(trunc(rt),ECONV));
	   2: pushd(fitd(trunc(rt)));
	   end
	 end;
    CFU: begin sz:=argw(pop); argwf(pop); rt:=popr;
	   case szindex(sz) of
	   1: push( chopw(trunc(abs(rt)-0.5)) );
	   2: trap(EILLINS);
	   end
	 end;
    CFF: begin argwf(pop); argwf(pop) end
  end
end;

procedure logops;
var i,j:integer;
begin
  case insr of
    { LOGICAL GROUP }
    XAND:
	 begin k:=argw(k);
	   for j:= 1 to k div wsize do
	     begin a:=sp+k; t:=pop; store(a,bf(andf,memw(a),t)) end;
	 end;
    IOR:
	 begin k:=argw(k);
	   for j:= 1 to k div wsize do
	     begin a:=sp+k; t:=pop; store(a,bf(iorf,memw(a),t)) end;
	 end;
    XOR:
	 begin k:=argw(k);
	   for j:= 1 to k div wsize do
	     begin a:=sp+k; t:=pop; store(a,bf(xorf,memw(a),t)) end;
	 end;
    COM:
	 begin k:=argw(k);
	   for j:= 1 to k div wsize do
	     begin
	       store(sp+k-wsize*j, bf(xorf,memw(sp+k-wsize*j), negoff-1))
	     end
	 end;
    ROL: begin k:=argw(k); if k<>wsize then trap(EILLINS);
	   t:=pop; s:=pop; for i:= 1 to t do rleft(s); push(s)
	 end;
    ROR: begin k:=argw(k); if k<>wsize then trap(EILLINS);
	   t:=pop; s:=pop; for i:= 1 to t do rright(s); push(s)
	 end
  end
end;

procedure setops;
var i,j:integer;
begin
  case insr of
    { SET GROUP }
    INN:
	 begin k:=argw(k);
	   t:=pop;
	   i:= t mod 8; t:= t div 8;
	   if t>=k then
	     begin trap(ESET); s:=0 end
	   else
	     begin s:=memb(sp+t) end;
	   newsp(sp+k); push(bit(i,s));
	 end;
    XSET:
	 begin k:=argw(k);
	   t:=pop;
	   i:= t mod 8; t:= t div 8;
	   for j:= 1 to k div wsize do push(0);
	   if t>=k then
	     trap(ESET)
	   else
	     begin s:=1; for j:= 1 to i do rleft(s); storeb(sp+t,s) end
	 end
  end
end;

procedure arrops;
begin
  case insr of
    { ARRAY GROUP }
    LAR:
	 begin k:=argw(k); if k<>wsize then trap(EILLINS); a:=popa;
	   pushx(argo(memw(a+2*k)),arraycalc(a))
	 end;
    SAR:
	 begin k:=argw(k); if k<>wsize then trap(EILLINS); a:=popa;
	   popx(argo(memw(a+2*k)),arraycalc(a))
	 end;
    AAR:
	 begin k:=argw(k); if k<>wsize then trap(EILLINS); a:=popa;
	   push(arraycalc(a))
	 end
  end
end;

procedure cmpops;
begin
  case insr of
    { COMPARE GROUP }
    CMI: case szindex(argw(k)) of
	 1: begin st:=popsw; ss:=popsw;
	      if ss<st then pushsw(-1) else if ss=st then push(0) else push(1)
	    end;
	 2: begin dt:=popd; ds:=popd;
	      if ds<dt then pushsw(-1) else if ds=dt then push(0) else push(1)
	    end;
	 end;
    CMU: case szindex(argw(k)) of
	 1: begin t:=pop; s:=pop;
	      if s<t then pushsw(-1) else if s=t then push(0) else push(1)
	    end;
	 2: trap(EILLINS);
	 end;
    CMP: begin a:=popa; b:=popa;
	  if b<a then pushsw(-1) else if b=a then push(0) else push(1)
	 end;
    CMF: begin argwf(k); rt:=popr; rs:=popr;
	   if rs<rt then pushsw(-1) else if rs=rt then push(0) else push(1)
	 end;
    CMS: begin k:=argw(k);
	   t:= 0; j:= 0;
	   while (j < k) and (t=0) do
	     begin if memw(sp+j) <> memw(sp+k+j) then t:=1;
	       j:=j+wsize
	     end;
	   newsp(sp+wsize*k); push(t);
	 end;

    TLT: if popsw <  0 then push(1) else push(0);
    TLE: if popsw <= 0 then push(1) else push(0);
    TEQ: if pop   =  0 then push(1) else push(0);
    TNE: if pop   <> 0 then push(1) else push(0);
    TGE: if popsw >= 0 then push(1) else push(0);
    TGT: if popsw >  0 then push(1) else push(0);
  end
end;

procedure branchops;
begin
  case insr of
    { BRANCH GROUP }
    BRA: newpc(pc+k);

    BLT: begin st:=popsw; if popsw <  st then newpc(pc+k) end;
    BLE: begin st:=popsw; if popsw <= st then newpc(pc+k) end;
    BEQ: begin t :=pop  ; if pop   =   t then newpc(pc+k) end;
    BNE: begin t :=pop  ; if pop   <>  t then newpc(pc+k) end;
    BGE: begin st:=popsw; if popsw >= st then newpc(pc+k) end;
    BGT: begin st:=popsw; if popsw >  st then newpc(pc+k) end;

    ZLT: if popsw <  0 then newpc(pc+k);
    ZLE: if popsw <= 0 then newpc(pc+k);
    ZEQ: if pop   =  0 then newpc(pc+k);
    ZNE: if pop   <> 0 then newpc(pc+k);
    ZGE: if popsw >= 0 then newpc(pc+k);
    ZGT: if popsw >  0 then newpc(pc+k)
  end
end;

procedure callops;
var j:integer;
begin
  case insr of
    { PROCEDURE CALL GROUP }
    CAL: call(argp(k));
    CAI: begin call(argp(popa)) end;
    RET: begin k:=argz(k); if k div wsize>maxret  then trap(EILLINS);
	   for j:= 1 to k div wsize do retarea[j]:=pop; retsize:=k;
	   newsp(lb); lb:=maxdata+1; { To circumvent stack overflow error }
	   newpc(popa);
	   if pc=maxcode then
	   begin
	     halted:=true;
	     if retsize=wsize then exitstatus:=retarea[1]
	       else exitstatus:=undef
	  end
	  else
	     newlb(popa);
	 end;
    LFR: begin k:=args(k); if k<>retsize then trap(EILLINS);
	   for j:=k div wsize downto 1 do push(retarea[j]);
	 end
  end
end;

procedure miscops;
var i,j:integer;
begin
  case insr of
    { MISCELLANEOUS GROUP }
    ASP,ASS:
	 begin if insr=ASS then
	   begin k:=argw(k); if k<>wsize then trap(EILLINS); k:=popsw end;
	   k:=argf(k);
	   if k<0
	     then for j:= 1 to -k div wsize do push(undef)
	     else newsp(sp+k);
	 end;
    BLM,BLS:
	 begin if insr=BLS then
	   begin k:=argw(k); if k<>wsize then trap(EILLINS); k:=pop end;
	   k:=argz(k);
	   b:=popa; a:=popa;
	   for j := 1 to k div wsize do
	     store(b-wsize+wsize*j,memw(a-wsize+wsize*j))
	 end;
    CSA: begin k:=argw(k); if k<>wsize then trap(EILLINS);
	   a:=popa;
	   st:= popsw - signwd(memw(a+asize));
	   if (st>=0) and (st<=memw(a+wsize+asize)) then
	      b:=mema(a+2*wsize+asize+asize*st) else b:=mema(a);
	   if b=0 then trap(ECASE) else newpc(b)
	 end;
    CSB: begin k:=argw(k); if k<>wsize then trap(EILLINS); a:=popa;
	   t:=pop; i:=1; found:=false;
	   while (i<=memw(a+asize)) and not found do
	     if t=memw(a+(asize+wsize)*i) then found:=true else i:=i+1;
	   if found then b:=memw(a+(asize+wsize)*i+wsize) else b:=memw(a);
	   if b=0 then trap(ECASE) else newpc(b);
	 end;
    DCH: begin pusha(mema(popa+dynd)) end;
    DUP,DUS:
	 begin if insr=DUS then
	      begin k:=argw(k); if k<>wsize then trap(EILLINS); k:=pop end;
	   k:=args(k);
	   for i:=1 to k div wsize do push(memw(sp+k-wsize));
	 end;
    EXG: begin
	   k:=argw(k);
	   for i:=1 to k div wsize do push(memw(sp+k-wsize));
	   for i:=0 to k div wsize - 1 do
	     store(sp+k+i*wsize,memw(sp+k+k+i*wsize));
	   for i:=1 to k div wsize do
	   begin t:=pop ; store(sp+k+k-wsize,t) end;
	 end;
    FIL: filna(argg(k));
    GTO: begin k:=argg(k);
	   newlb(mema(k+2*asize)); newsp(mema(k+asize)); newpc(mema(k))
	 end;
    LIM: push(ignmask);
    LIN: lino(argn(k));
    LNI: lino(memw(0)+1);
    LOR: begin i:=argr(k);
	   case i of 0:pusha(lb); 1:pusha(sp); 2:pusha(hp) end;
	 end;
    LPB: pusha(popa+statd);
    MON: domon(pop);
    NOP: writeln('NOP at line ',memw(0):5) ;
    RCK: begin a:=popa;
	   case szindex(argw(k)) of
	   1: if (signwd(memw(sp))<signwd(memw(a))) or
		(signwd(memw(sp))>signwd(memw(a+wsize))) then trap(ERANGE);
	   2: if (memd(sp)<memd(a)) or
		(memd(sp)>memd(a+2*wsize)) then trap(ERANGE);
	   end
	 end;
    RTT: dortt;
    SIG: begin a:=popa; pusha(uerrorproc); uerrorproc:=a end;
    SIM: ignmask:=pop;
    STR: begin i:=argr(k);
	   case i of 0: newlb(popa); 1: newsp(popa); 2: newhp(popa) end;
	 end;
    TRP: trap(pop)
  end
end;
.bp
{---------------------------------------------------------------------------}
{                               Main Loop                                   }
{---------------------------------------------------------------------------}

begin initialize;
8888:
  repeat
    opcode := nextpc;       { fetch the first byte of the instruction }
    if opcode=escape1 then iclass:=second
    else if opcode=escape2 then iclass:=tert
    else iclass:=prim;
    if iclass<>prim then opcode := nextpc;
    with dispat[iclass][opcode] do
      begin insr:=instr;
	if not (zbit in iflag) then
	  if ibit in iflag then k:=pop else
	    begin
	      if mini in iflag then k:=implicit else
		begin
		  if short in iflag then k:=implicit+nextpc else
		    begin k:=nextpc;
		      if (sbit in iflag) and (k>=128) then k:=k-256;
		      for i:=2 to ilength do k:=256*k + nextpc
		    end
		end;
	      if wbit in iflag then k:=k*wsize;
	    end
      end;
case insr of

  NON: trap(EILLINS);

  { LOAD GROUP }
  LDC,LOC,LOL,LOE,LIL,LOF,LAL,LAE,LXL,LXA,LOI,LOS,LDL,LDE,LDF,LPI:
      loadops;

  { STORE GROUP }
  STL,STE,SIL,STF,STI,STS,SDL,SDE,SDF:
      storeops;

  { SIGNED INTEGER ARITHMETIC }
  ADI,SBI,MLI,DVI,RMI,NGI,SLI,SRI:
      intarith;

  { UNSIGNED INTEGER ARITHMETIC }
  ADU,SBU,MLU,DVU,RMU,SLU,SRU:
      unsarith;

  { FLOATING POINT ARITHMETIC }
  ADF,SBF,MLF,DVF,NGF,FIF,FEF:
      fltarith;

  { POINTER ARITHMETIC }
  ADP,ADS,SBS:
      ptrarith;

  { INCREMENT/DECREMENT/ZERO }
  INC,INL,INE,DEC,DEL,DEE,ZRL,ZRE,ZER,ZRF:
      incops;

  { CONVERT GROUP }
  CII,CIU,CIF,CUI,CUU,CUF,CFI,CFU,CFF:
      convops;

  { LOGICAL GROUP }
 XAND,IOR,XOR,COM,ROL,ROR:
      logops;

  { SET GROUP }
  INN,XSET:
      setops;

  { ARRAY GROUP }
  LAR,SAR,AAR:
      arrops;

  { COMPARE GROUP }
  CMI,CMU,CMP,CMF,CMS,  TLT,TLE,TEQ,TNE,TGE,TGT:
      cmpops;

  { BRANCH GROUP }
  BRA,  BLT,BLE,BEQ,BNE,BGE,BGT,  ZLT,ZLE,ZEQ,ZNE,ZGE,ZGT:
      branchops;

  { PROCEDURE CALL GROUP }
  CAL,CAI,RET,LFR:
      callops;

  { MISCELLANEOUS GROUP }
  ASP,ASS,BLM,BLS,CSA,CSB,DCH,DUP,DUS,EXG,FIL,GTO,LIM,
  LIN,LNI,LOR,LPB,MON,NOP,RCK,RTT,SIG,SIM,STR,TRP:
      miscops;

    end;        { end of case statement }
    if not ( (insr=RET) or (insr=ASP) or (insr=BRA) or (insr=GTO) ) then
	retsize:=0 ;
  until halted;
9999:
  writeln('halt with exit status: ',exitstatus:1);
  doident;
end.
.ft P
.lg 1
.fi
