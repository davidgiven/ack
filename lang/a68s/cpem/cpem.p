#include        <em_spec.h>
#include        <em_pseu.h>
#include	<em_mnem.h>
#include	<em_mes.h>
#include	<em_reg.h>
#include	<pc_size.h>

{
  (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 
           This product is part of the Amsterdam Compiler Kit.
 
  Permission to use, sell, duplicate or disclose this software must be
  obtained in writing. Requests for such permissions may be sent to
 
       Dr. Andrew S. Tanenbaum
       Wiskundig Seminarium
       Vrije Universiteit
       Postbox 7161
       1007 MC Amsterdam
       The Netherlands
 
}

{if next line is included the compiler itself is written in standard pascal}
{#define        STANDARD        1}
{if next line is included the compiler contains the extra features needed
 for ALGOL 68S}
#define         CHL             1

{if next line is included, the compiler won't generate static exchanges}
{#define	NO_EXC	1}

{Author:        Johan Stevenson                 Version:        32}
{$l- : no source line numbers}
{$r- : no subrange checking}
{$a- : no assertion checking}
#ifdef STANDARD
{$s+ : test conformancy to standard}
#endif

program pem(input,em,errors,output);
{/*
   This Pascal compiler produces EM code as described in
   - A.S.Tanenbaum, J.W.Stevenson & H. van Staveren,
	"Description of a machine architecture for use with
	 block structured languages" Informatika rapport 81.
  NOTE: this version is modified to produce the modified EM code of
	januari 1981. it is not possible, using this compiler, to
	generate code for machines with 1 byte wordsize.
  NOTE: this version is modified by Kees Visser in such a way that
	the compiler can now run on 2 and 4 byte machines. It is also
	able to generate em-code for a 2 bytes machine while running
	on a 4-bytes machine. Cross-compilation from a 2 bytes to a
	four bytes machine is also possible with the following
	exception: large integers that don't fit in an integer of 
	the compiler are treated like longs and are thus not allowed
	in types.

  A description of Pascal is given in
   - K.Jensen & N.Wirth, "PASCAL user manual and report", Springer-Verlag.
  Several options may be given in the normal pascal way. Moreover,
  a positive number may be used instead of + and -. The options are:
	a:      interpret assertions (+)
	c:      C-type strings allowed (-)
	d:      type long may be used (-)
	i:      controls the number of elements in integer sets
			default: (wordsize in bits)
	l:      insert code to keep track of source lines (+)
	o:      optimize (+)
	r:      check subranges (+)
	s:      accept only standard pascal programs (-)
	t:      trace procedure entry and exit (-)
	u:      treat '_' as letter (-)
#ifdef  CHL
	e:      all procedure names external (even inner ones) (-)
	g:      global declarations compiled (+)
		when compiling segments of procedures:
		  g- implies that global declarations are merely repetitions
		  (which had better be correct!) of declarations in some master
		  segment.
		when compiling main program:
		  g- implies that such a master segment is being compiled.
	w:      generate warnings as well as error messages (+)
  
  all the above options are translated to lower case, thus are case
  independent. This applies to options in the source file and to those
  passed via C_EM_PC.
#endif
*/}
{===================================================================}
#ifdef STANDARD
label 9999;
#endif

const
{fundamental constants}
  MB1 = 7;
  NB1 = 8;
  MI1 = 127;
  NI1 = 128;
  MI2 = 32767;
  MU1 = 255;
  NU1 = 256;

{string constants}
  imax = 10;
#ifdef CHL
  max2bytes   = '0000032768';
  max4bytes   = '2147483648';
#else
  max2bytes   = '0000032767';
  max4bytes   = '2147483647';
#endif

#if EM_WSIZE == 4
  {this can only be compiled with a compiler that has integer size 4}
  MU2 = 65535;
  NU2 = 65536;

  {characteristics of the machine on which the compiler will run}
  {wordsize and integer size are 4}
  MI = 2147483647;
  maxcompintstring = max4bytes;
#endif
#if EM_WSIZE == 2
  MU2 = 0;	{not used}
  NU2 = 0;	{not used}

  MI = MI2;
  maxcompintstring = max2bytes;
#endif
#if EM_WSIZE != 2 && EM_WSIZE != 4
Something wrong here!
#endif

{maximal indices}
  idmax         = 8;
  fnmax         = 14;
  smax          = 72;

{opt values}
  off           = 0;
  on            = 1;

{for push and pop: }
  global        = false;
  local         = true;

{for sizeof and posaddr: }
  wordmult      = false;
  wordpart      = true;

{ASCII characters}
  ascht         = 9;
  ascnl         = 10;
  ascvt         = 11;
  ascff         = 12;
  asccr         = 13;

{miscellaneous}
  maxcharord    = 127;          {maximal ordinal number of chars}
  maxargc       = 13;           {maximal index in argv}
  rwlim         = 34;           {number of reserved words}
  spaces        = '        ';

{-------------------------------------------------------------------}
type
{scalar types}
  symbol=       (comma,semicolon,colon1,colon2,notsy,lbrack,ident,
		 intcst,charcst,realcst,longcst,stringcst,nilcst,minsy,
		 plussy,lparent,arrow,arraysy,recordsy,setsy,filesy,
		 packedsy,progsy,labelsy,constsy,typesy,varsy,procsy,
		 funcsy,beginsy,gotosy,ifsy,whilesy,repeatsy,forsy,
		 withsy,casesy,becomes,starsy,divsy,modsy,slashsy,
		 andsy,orsy,eqsy,nesy,gtsy,gesy,ltsy,
		 lesy,insy,endsy,elsesy,untilsy,ofsy,dosy,
		 downtosy,tosy,thensy,rbrack,rparent,period
		);                      {the order is important}
  chartype=     (lower,upper,digit,layout,tabch,
		   quotech,dquotech,colonch,periodch,lessch,
		   greaterch,lparentch,lbracech,
						{different entries}
		 rparentch,lbrackch,rbrackch,commach,semich,arrowch,
		   plusch,minch,slash,star,equal,
						{also symbols}
		 others
		);
  standpf=      (pread,preadln,pwrite,pwriteln,pput,pget,
		 preset,prewrite,pnew,pdispose,ppack,punpack,
#ifdef  CHL
		 penew,pedispose,
#endif
		 pmark,prelease,ppage,
#ifdef  CHL
		 psetstktop,pmoveleft,
#endif
		 phalt,
						{all procedures}
		 feof,feoln,fabs,fsqr,ford,fchr,fpred,fsucc,fodd,
#ifdef  CHL
		 fasptr,fincptr,fgetstktop,
#endif
		 ftrunc,fround,fsin,fcos,fexp,fsqt,flog,fatn
						{all functions}
		);                      {the order is important}
  libmnem=      (ELN ,EFL ,CLS ,WDW ,           {input and output}
		 OPN ,GETX,RDI ,RDC ,RDR ,RDL ,RLN ,
						{on inputfiles}
		 CRE ,PUTX,WRI ,WSI ,WRC ,WSC ,WRS ,WSS ,WRB ,
		 WSB ,WRR ,WSR ,WRL, WSL, WRF ,WRZ ,WSZ ,WLN ,PAG ,
				{on outputfiles, order important}
		 ABR ,RND ,SINX,COSX,EXPX,SQT ,LOG ,ATN ,
						{floating point}
		 ABI ,ABL ,BCP ,BTS ,NEWX,SAV ,RST ,INI ,HLT ,
		 ASS ,GTO ,PAC ,UNP, DIS, ASZ, MDI, MDL
						{miscellaneous}
		);
  structform=   (scalar,subrange,pointer,power,files,arrays,carray,
		 records,variant,tag);          {order important}
  structflag=   (spack,withfile);
  identflag=    (refer,used,assigned,noreg,loopvar,samesect);
  idclass=      (types,konst,vars,field,carrbnd,proc,func);
  kindofpf=     (standard,formal,actual,extern,varargs,forward);
  where=        (blck,rec,wrec);
  attrkind=     (cst,fixed,pfixed,loaded,ploaded,indexed);
  twostruct=    (eq,subeq,ir,ri,il,li,lr,rl,es,se,noteq);  {order important}

{subrange types}
  rwrange=      0..rwlim;
  byte=         0..MU1;

{pointer types}
  sp=   ^structure;
  ip=   ^identifier;
  lp=   ^labl;
  bp=   ^blockinfo;
  np=   ^nameinfo;
#ifdef NO_EXC
  mp=	^mmark;
  op=	^outrec;
#endif NO_EXC

{set types}
  sos=          set of symbol;
  setofids=     set of idclass;
  formset=      set of structform;
  sflagset=     set of structflag;
  iflagset=     set of identflag;

{array types}
  idarr=packed array[1..idmax] of char;
  fnarr=packed array[1..fnmax] of char;

{record types}
  position=record               {the addr info of certain variable}
    ad:integer;                 {for locals it is the byte offset}
    lv:integer;                 {the level of the beast}
  end;

{records of type attr are used to remember qualities of
  expression parts to delay the loading of them.
  Reasons to delay the loading of one word constants:
	- bound checking
	- set building.
  Reasons to delay the loading of direct accessible objects:
	- efficient handling of read/write
	- efficient handling of the with statement.
}
  attr=record
    asp:sp;                             {type of expression}
    packbit:boolean;                    {true for part of packed structure}
    ak:attrkind;                        {access method}
    pos:position;                       {lv and ad}
    {If ak=cst then the value is stored in ad}
  end;

  nameinfo=record               {one for each separate name space}
    nlink:np;                   {one deeper}
    fname:ip;                   {first name: root of tree}
    case occur:where of
      blck:();
      rec: ();
      wrec:(wa:attr)            {name space opened by with statement}
  end;

  blockinfo=record      {all info of the current procedure}
    nextbp:bp;          {pointer to blockinfo of surrounding proc}
    reglb:integer;      {data location counter (from begin of proc) }
    minlb:integer;      {keeps track of minimum of reglb}
    ilbno:integer;      {number of last local label}
#ifdef  CHL
    tip:ip;             {ip of current proc/func}
#endif
    forwcount:integer;  {number of not yet specified forward procs}
    lchain:lp;          {first label: header of chain}
  end;

  structure=record
    size:integer;                       {size of structure in bytes}
    sflag:sflagset;                     {flag bits}
    case form:structform of
      scalar  :(scalno:integer;         {number of range descriptor}
		fconst:ip               {names of constants}
	       );
      subrange:(min,max:integer;        {lower and upper bound}
		rangetype:sp;           {type of bounds}
		subrno:integer          {number of subr descriptor}
	       );
      pointer :(eltype:sp);             {type of pointed object}
      power   :(elset:sp);              {type of set elements}
      files   :(filtype:sp);            {type of file elements}
      arrays,carray:
	       (aeltype:sp;             {type of array elements}
		inxtype:sp;             {type of array index}
		arpos:position          {position of array descriptor}
	       );
      records :(fstfld:ip;              {points to first field}
		tagsp:sp                {points to tag if present}
	       );
      variant :(varval:integer;         {tag value for this variant}
		nxtvar:sp;              {next equilevel variant}
		subtsp:sp               {points to tag for sub-case}
	       );
      tag     :(fstvar:sp;              {first variant of case}
		tfldsp:sp               {type of tag}
	       )
  end;

  identifier=record
    idtype:sp;                          {type of identifier}
    name:idarr;                         {name of identifier}
    llink,rlink:ip;                     {see enterid,searchid}
    next:ip;                            {used to make several chains}
    iflag:iflagset;                     {several flag bits}
    case klass:idclass of
      types   :();
      konst   :(value:integer);         {for integers the value is
		  computed and stored in this field.
		  For strings and reals an assembler constant is
		  defined labeled '.1', '.2', ...  This '.' number is then
		  stored in value. For reals value may be negated to
		  indicate that the opposite of the assembler constant
		  is needed. }
      vars    :(vpos:position);         {position of var}
      field   :(foffset:integer);       {offset to begin of record}
      carrbnd :();                      {idtype points to carray struct}
      proc,func:
	(case pfkind:kindofpf of
	   standard:(key:standpf);      {identification}
	   formal,actual,forward,extern,varargs:
	     (pfpos:position;           {lv gives declaration level.
			ad is relevant for formal pf s and for
			functions (no conflict!!).
			for functions: ad is the result address.
			for formal pf s: ad is the address of the
			descriptor }
	      pfno:integer;             {unique pf number}
	      maxlb:integer;		{bytes of parameters}
	      parhead:ip                {head of parameter list}
	     )
	)
  end;

  labl=record
    nextlp:lp;          {chain of labels}
    seen:boolean;
    labval:integer;     {label number given by the programmer}
    labname:integer;    {label number given by the compiler}
    labdlb:integer      {zero means only locally used,
			  otherwise dlbno of label information}
  end;

#ifdef NO_EXC
  outrec=record
    next:op;		{chain of records}
    bytes:array[1..16] of byte;
    cnt:0..16;
  end;

  mmark=record
    next:mp;		{chain of marks}
    count,where:integer;
  end;
#endif NO_EXC
{-------------------------------------------------------------------}
var  {the most frequent used externals are declared first}
  sy:symbol;            {last symbol}
  a:attr;               {type,access method,position,value of expr}
{returned by insym}
  ch:char;              {last character}
  chsy:chartype;        {type of ch, used by insym}
  val:integer;          {if last symbol is an constant }
  ix:integer;           {string length}
  eol:boolean;          {true of current ch is a space, replacing a newline}
  zerostring:boolean;   {true for strings in " "}
  id:idarr;             {if last symbol is an identifier}
{some counters}
  lino:integer;         {line number on code file (1..n) }
  dlbno:integer;        {number of last global number}
#ifdef CHL
  glbdata:integer;	{start of hol area}
  holdone:boolean;	{set after genhol called}
#endif
  holeb:integer;        {size of hol-area}
  level:integer;        {current static level}
  argc:integer;         {index in argv}
  lastpfno:integer;     {unique pf number counter}
  copt:integer;         {C-type strings allowed if on}
  dopt:integer;         {longs allowed if on}
  iopt:integer;         {number of bits in sets with base integer}
  sopt:integer;         {standard option}
  srcchno:integer;      {column count for errors}
  srclino:integer;      {source line number after preprocessing}
  srcorig:integer;      {source line number before preprocessing}
  fildlb:integer;       {label number of source string}
{pointers pointing to standard types}
  realptr,intptr,textptr,nullset,boolptr:sp;
  charptr,nilptr,zeroptr,procptr,longptr:sp;
{flags}
  giveline:boolean;     {give source line number at next statement}
  including:boolean;    {no LINs for included code}
  eofexpected:boolean;  {quit without error if true (nextch) }
  main:boolean;         {complete programme or a module}
  intypedec:boolean;    {true if nested in typedefinition}
  fltused:boolean;      {true if floating point instructions are used}
  seconddot:boolean;    {indicates the second dot of '..'}
#ifdef CHL
  ioaddressfixed:boolean; {indicates if argv[0..1].ad set up}
#endif
{pointers}
  fwptr:ip;             {head of chain of forward reference pointers}
  progp:ip;             {program identifier}
  currproc:ip;          {current procedure/function ip (see selector)}
  top:np;               {pointer to the most recent name space}
  lastnp:np;            {pointer to nameinfo of last searched ident }
{records}
  b:blockinfo;          {all info to be stacked at pfdeclaration}
  fa:attr;              {attr for current file name}
{arrays}
  sizes:array[0 .. sz_last] of integer;
  maxintstring,maxlongstring:packed array[1..imax] of char;
  strbuf:array[1..smax] of char;
  rw:array[rwrange] of idarr;
			{reserved words}
  frw:array[0..idmax] of integer;
			{indices in rw}
  rsy:array[rwrange] of symbol;
			{symbol for reserved words}
  cs:array[char] of chartype;
			{chartype of a character}
  csy:array[rparentch..equal] of symbol;
			{symbol for single character symbols}
  lmn:array[libmnem] of packed array[1..4] of char;
			{mnemonics of pascal library routines}
  opt:array['a'..'z'] of integer;
  forceopt:array['a'..'z'] of boolean;
			{26 different options}
  undefip:array[idclass] of ip;
			{used in searchid}
  iop:array[boolean] of ip;
			{false:standard input, true:standard output}
  argv:array[0..maxargc] of
	 record name:idarr; ad:integer end;
			{save here the external heading names}
{files}
   em:file of byte;      {the EM code}
  errors:text;          {the compilation errors}
  source:fnarr;
#ifdef NO_EXC
  ohead: op;	{head of outrec list}
  mhead: mp;	{head of marks list}
  bcnt: integer;
#define newmark setmark
#define relmark(xx) freemark(xx)
#else not NO_EXC
#define newmark lino
#define relmark(xx)
#endif NO_EXC

{===================================================================}

procedure initpos(var p:position);
begin p.lv:=level; p.ad:=0; end;

procedure inita(fsp:sp; fad:integer);
begin with a do begin
  asp:=fsp; packbit:=false; ak:=fixed; pos.ad:=fad; pos.lv:=level;
end end;

function newip(kl:idclass; n:idarr; idt:sp; nxt:ip):ip;
var p:ip; f:iflagset;
begin f:=[];
  case kl of
    types,carrbnd:  {similar structure}
      new(p,types);
    konst:
      begin new(p,konst); p^.value:=0 end;
    vars:
      begin new(p,vars); f:=[used,assigned]; initpos(p^.vpos) end;
    field:
      begin new(p,field); p^.foffset:=0 end;
    proc,func:  {same structure}
      begin new(p,proc,actual); p^.pfkind:=actual;
	initpos(p^.pfpos); p^.pfno:=0; p^.maxlb:=0; p^.parhead:=nil;
      end
  end;
  p^.name:=n; p^.klass:=kl; p^.idtype:=idt; p^.next:=nxt;
  p^.llink:=nil; p^.rlink:=nil; p^.iflag:=f; newip:=p
end;

function newsp(sf:structform; sz:integer):sp;
var p:sp; sflag:sflagset;
begin sflag:=[];
  case sf of
    scalar:
      begin new(p,scalar); p^.scalno:=0; p^.fconst:=nil end;
    subrange:
      new(p,subrange);
    pointer:
      begin new(p,pointer); p^.eltype:=nil end;
    power:
      new(p,power);
    files:
      begin new(p,files); sflag:=[withfile] end;
    arrays,carray:  {same structure}
      new(p,arrays);
    records:
      new(p,records);
    variant:
      new(p,variant);
    tag:
      new(p,tag);
  end;
  p^.form:=sf; p^.size:=sz; p^.sflag:=sflag; newsp:=p;
end;

function sizeof(fsp:sp; partword:boolean):integer;
var s:integer;
begin if fsp=nil then s:=0 else s:=fsp^.size;
  if s<>0 then
    if partword and (s<sz_word) then
      while sz_word mod s <> 0 do s:=s+1
    else
      while s mod sz_word <> 0 do s:=s+1;
  sizeof:=s
end;

function formof(fsp:sp; forms:formset):boolean;
begin if fsp=nil then formof:=false else formof:=fsp^.form in forms end;

{===================================================================}

#ifdef NO_EXC
procedure newoutrec;
var p:op;
begin
  new(p);
  bcnt := bcnt+1;
  with p^ do begin cnt := 0; next := ohead end;
  ohead := p
end;

procedure put1(b:byte);
begin
  if mhead = nil then write(em,b)
  else begin
    if ohead^.cnt = 16 then newoutrec;
    with ohead^ do
      begin cnt := cnt + 1; bytes[cnt] := b end
  end
end;
#else not NO_EXC
procedure put1(b:byte);
begin write(em,b) end;
#endif NO_EXC

procedure put2(i:integer);
var i1,i2:byte;
begin
  if i<0 then
    begin i:=-(i+1); i1:=MU1 - i mod NU1; i2:=MU1 - i div NU1 end
  else
    begin i1:=i mod NU1; i2:=i div NU1 end;
  put1(i1); put1(i2)
end;

#if EM_WSIZE == 4
procedure put4(i:integer);
var i1,i2:integer;
begin
  if i<0 then
    begin i:=-(i+1); i1:=MU2 - i mod NU2; i2:=MU2 - i div NU2 end
  else
    begin i1:=i mod NU2; i2:=i div NU2 end;
  put1(i1 mod NU1); put1(i1 div NU1);
  put1(i2 mod NU1); put1(i2 div NU1)
end;
#endif

procedure argend;
begin put1(sp_cend) end;

procedure argcst(i:integer);
begin
  if (i >= -sp_zcst0) and (i < sp_ncst0-sp_zcst0) then
    	 put1(i + sp_zcst0 + sp_fcst0)
  else
#if EM_WSIZE == 4
	if (i >= -MI2-1) and (i <= MI2) then
#endif
    	 begin put1(sp_cst2); put2(i) end
#if EM_WSIZE == 4
  else   begin put1(sp_cst4); put4(i) end
#endif
end;

procedure argnil;
begin put1(sp_icon); argcst(sz_addr); argcst(1); put1(ord('0')) end;

procedure argilb(i:integer);
begin
  if i<=MU1 then
    begin put1(sp_ilb1); put1(i) end
  else
    begin put1(sp_ilb2); put2(i) end
end;

procedure argdlb(i:integer);
begin
  if i<=MU1 then
    begin put1(sp_dlb1); put1(i) end
  else
    begin put1(sp_dlb2); put2(i) end
end;

procedure argident(var a:idarr);
var i,j:integer;
#ifdef CHL
      k:integer;
#endif
begin i:=idmax;
  while (a[i]=' ') and (i>1) do i:=i-1;
  put1(sp_pnam); argcst(i);
#ifdef CHL
  if a='m_a_i_n ' then
      for j:=1 to i do put1(ord(a[j]))
  else
      for j:=1 to i do
      begin
          k:=ord(a[j]);
          if (k>96) and (k<123) then k:=k-32; (*translate to upper case*)
          put1(k);
      end;
#else
  for j:=1 to i do put1(ord(a[j]))
#endif
end;

#ifdef CHL
procedure dlbident(var a:idarr);
var i,j,k:integer;
begin i:=idmax;
  while (a[i]=' ') and (i>1) do i:=i-1;
  put1(sp_dnam); argcst (i);
  for j:=1 to i do
  begin
      k:=ord(a[j]);
      if (k>96) and (k<123) then k:=k-32; (*translate to upper case*)
      put1(k);
  end;
end;
#endif

procedure genop(b:byte);
begin put1(b); lino:=lino+1 end;

procedure gencst(b:byte; i:integer);
begin genop(b); argcst(i) end;

#ifdef CHL
procedure genarg(b:byte; i:integer);
begin
genop(b);
put1(sp_doff); 
argdlb(glbdata);
argcst(i)
end;
#endif

procedure genglb(b:byte; i:integer);
begin
#ifdef CHL
  if opt['g']=off then genarg(b,i) else gencst(b,i);
#else
  gencst(b,i);
#endif
end;

procedure gensp(m:libmnem; s:integer);
var i:integer;
begin genop(op_cal); put1(sp_pnam); argcst(4);
  for i:=1 to 4 do put1(ord(lmn[m][i]));
  gencst(op_asp,s)
end;

procedure genpnam(b:byte; fip:ip);
var n:idarr; i,j:integer;
begin
#ifdef  CHL
  if (fip^.pfpos.lv<=1) or (opt['e']=on) then
#else
  if fip^.pfpos.lv<=1 then
#endif
    n:=fip^.name
  else
    begin n:='_       '; j:=1; i:=fip^.pfno;
      while i<>0 do
	begin j:=j+1; n[j]:=chr(i mod 10 + ord('0')); i:=i div 10 end;
    end;
  genop(b); argident(n)
end;

#ifdef CHL
procedure gengotonam(b:byte; fip:ip; l:integer);
var index,count,val : integer;
    n : idarr;
begin
   n:='_       ';
   index:=2;
   val:=l;
   while val<>0 do
   begin
       n[index]:= chr(val mod 10 +ord('0'));
       val:=val div 10;
       index:=index+1;
   end;
   val:=1;
   for count:=index to idmax do
   begin
       n[count]:=fip^.name[val];
       val:=val+1;
   end;
   genop(b);
   dlbident(n);
   dlbident(n);      (* the last three instructions produce *)
		     (* exa _xxxnnn *)
		     (*_xxxnnn      *)
		     (* where xxx is the number of the label supplied by the *)
		     (*              programmer *)
		     (* and   nnn is the name of the routine the label is in *)
end;
#endif

procedure genasp(m:byte);
begin gencst(m,sizeof(a.asp,wordmult)) end;

procedure genlin;
begin giveline:=false;
  if opt['l']<>off then if main then gencst(op_lin,srcorig)
end;

procedure genreg(sz,ad,regval:integer);
begin gencst(ps_mes,ms_reg);
  argcst(ad); argcst(sz); argcst(regval); argend
end;

procedure laedlb(d:integer);
begin genop(op_lae); argdlb(d) end;

#ifdef NO_EXC
procedure reloutrec;
var i,j,k:integer;
  q, r, p:op;
  m : mp;
begin p := ohead; q := p;
  if mhead <> nil then
    begin
      m := mhead; while m^.next <> nil do m := m^.next;
      k := (bcnt - m^.where) + 1
    end
  else begin k := 0; ohead := nil; bcnt := 0 end;
  for i := 1 to k do begin q := p; p := p^.next end;
  if q <> p then q^.next := nil;
  if p <> nil then
    begin r := nil;
      while p <> nil do
	begin q := p^.next; p^.next := r; r := p; p := q end;
      while r <> nil do with r^ do
        begin
	  for j := 1 to cnt do write(em, bytes[j]);
	  r := next
        end
    end
end;

function setmark:integer;
var p:mp; nm:boolean;
begin nm := false;
  if mhead <> nil then with mhead^ do
    if (where = bcnt) and (ohead^.cnt = 0) then
      begin count := count + 1; nm := true end;
  if not nm then
    begin new(p); newoutrec;
      with p^ do
        begin where := bcnt; count := 1; next := mhead end;
      mhead := p;
    end;
  setmark := bcnt
end;

procedure freemark(m : integer);
var p, q : mp;
begin assert(mhead <> nil); p := mhead; q := p;
  while p^.where <> m do
    begin q := p; p := p^.next; assert(p <> nil) end;
  with p^ do
    begin assert(count > 0); count := count - 1; if count = 0 then
      begin
	if p = mhead then begin mhead := next; reloutrec end
	else q^.next := next
      end
end end;

procedure exchange(n,m:integer);
var i:integer;
    p,q,r:op;
begin assert(m >= n);
  if n <> m then
    begin
      p := ohead;
      for i := bcnt downto m+1 do p := p^.next;
      q := p;
      for i := m downto n+1 do q := q^.next;
      r := ohead; ohead := p^.next; p^.next := q^.next; q^.next := r
    end
end;
#else not NO_EXC
procedure exchange(l1,l2:integer);
var d1,d2:integer;
begin d1:=l2-l1; d2:=lino-l2;
  if (d1<>0) and (d2<>0) then
    begin gencst(ps_exc,d1); argcst(d2) end
end;
#endif NO_EXC

procedure newilb(i:integer);
begin lino:=lino+1;
  if i<sp_nilb0 then put1(i+sp_filb0) else argilb(i)
end;

function newdlb:integer;
begin lino:=lino+1; dlbno:=dlbno+1;
#ifdef  CHL
  if (opt['g']=off) and (not holdone) then begin genop(ps_exa); argdlb(dlbno) end;
  if main or holdone then argdlb(dlbno);
#else
  argdlb(dlbno);
#endif
  newdlb:=dlbno;
end;

function romstr(typ:byte; siz:integer):integer;
var i:integer;
begin romstr:=newdlb;
#ifdef  CHL
  if main or holdone then
#endif
    begin genop(ps_rom);
      put1(typ); if typ<>sp_scon then argcst(siz); argcst(ix);
      for i:=1 to ix do put1(ord(strbuf[i])); argend
    end;
end;

{===================================================================}

procedure error(err:integer);
{as you will notice, all error numbers are preceded by '+' and '0' to
  ease their renumbering in case of new errornumbers.
}
begin
#ifdef CHL
	if ((opt['w']=on) and (err<0)) or (err>0) then
#endif
	begin 
		writeln(errors,err,srclino,srcchno);
  		if err>0 then begin gencst(ps_mes,ms_err); argend end
	end
end;

procedure errid(err:integer; var id:idarr);
begin
#ifdef CHL
	if ((opt['w']=on) and (err<0)) or (err>0) then
#endif
		begin write(errors,'''',id); error(err) end;
end;

procedure errint(err:integer; i:integer);
begin write(errors,i:1); error(err) end;

procedure errasp(err:integer);
begin if a.asp<>nil then begin error(err); a.asp:=nil end end;

procedure teststandard;
begin if sopt<>off then error(-(+01)) end;

procedure enterid(fip: ip);
{enter id pointed at by fip into the name-table,
  which on each declaration level is organised as
  an unbalanced binary tree}
var nam:idarr; lip,lip1:ip; lleft,again:boolean;
begin nam:=fip^.name; again:=false; assert nam[1]<>' ';
  lip:=top^.fname;
  if lip=nil then top^.fname:=fip else
    begin
      repeat lip1:=lip;
	if lip^.name>nam then
	  begin lip:=lip^.llink; lleft:=true end
	else
	  begin if lip^.name=nam then again:=true;  {name conflict}
	    lip:=lip^.rlink; lleft:=false;
	  end;
      until lip=nil;
      if lleft then lip1^.llink:=fip else lip1^.rlink:=fip
    end;
  fip^.llink:=nil; fip^.rlink:=nil;
  if again then errid(+02,nam);
end;

{===================================================================}

procedure trace(tname:idarr; fip:ip; var namdlb:integer);
var i:integer;
begin
  if opt['t']<>off then
    begin
      if namdlb=0 then
	begin namdlb:=newdlb; genop(ps_rom); put1(sp_scon); argcst(8);
	  for i:=1 to 8 do put1(ord(fip^.name[i])); argend;
	end;
      laedlb(namdlb); genop(op_cal); argident(tname);
      gencst(op_asp,sz_addr);
    end;
end;

procedure expandnullset(fsp:sp);
var s:integer;
begin s:=sizeof(fsp,wordmult)-sz_word;
  if s<>0 then gencst(op_zer,s); a.asp:=fsp
end;

procedure push(local:boolean; ad:integer; sz:integer);
begin assert sz mod sz_word = 0;
  if sz=sz_word then
    if local then gencst(op_lol,ad) else genglb(op_loe,ad)
  else if sz=2*sz_word then
    if local then gencst(op_ldl,ad) else genglb(op_lde,ad)
  else
    begin if local then gencst(op_lal,ad) else genglb(op_lae,ad);
      gencst(op_loi,sz)
    end
end;

procedure pop(local:boolean; ad:integer; sz:integer);
begin assert sz mod sz_word = 0;
  if sz=sz_word then
    if local then gencst(op_stl,ad) else genglb(op_ste,ad)
  else if sz=2*sz_word then
    if local then gencst(op_sdl,ad) else genglb(op_sde,ad)
  else
    begin if local then gencst(op_lal,ad) else genglb(op_lae,ad);
      gencst(op_sti,sz)
    end
end;

procedure lexaddr(lv:integer; ad:integer);
begin assert level>=lv;
  if ad>=0 then gencst(op_lxa,level-lv) else gencst(op_lxl,level-lv);
  gencst(op_adp,ad)
end;

procedure loadpos(var p:position; sz:integer);
begin with p do
  if lv<=0 then push(global,ad,sz) else
  if lv=level then push(local,ad,sz) else
    begin lexaddr(lv,ad); gencst(op_loi,sz) end;
end;

procedure descraddr(var p:position);
begin if p.lv=0 then laedlb(p.ad) else loadpos(p,sz_addr) end;

procedure loadaddr;
begin with a,pos do begin
  case ak of
    fixed:
      if lv<=0 then genglb(op_lae,ad) else
      if lv=level then gencst(op_lal,ad) else lexaddr(lv,ad);
    pfixed:
      loadpos(pos,sz_addr);
    ploaded:
      ;
    indexed:
      gencst(op_aar,sz_word);
  end;  {case}
  ak:=ploaded;
end end;

procedure load;
var sz:integer;
begin with a do begin sz:=sizeof(asp,packbit);
  if asp<>nil then begin
    case ak of
      cst:
	gencst(op_loc,pos.ad);  {only one-word scalars}
      fixed:
	loadpos(pos,sz);
      pfixed:
	begin loadpos(pos,sz_addr); gencst(op_loi,sz) end;
      loaded:
	;
      ploaded:
	gencst(op_loi,sz);
      indexed:
	gencst(op_lar,sz_word);
    end;  {case}
  if asp^.form = subrange then
     if sz < sz_word then
	if asp^.min < 0 then
	   { do sign extension }
	   begin gencst(op_loc, sz); gencst(op_loc, sz_word); genop(op_cii) end;
  end;
  ak:=loaded;
end end;

procedure store;
var sz:integer;
begin with a,pos do begin sz:=sizeof(asp,packbit);
  if asp<>nil then
    case ak of
      fixed:
	if lv<=0 then pop(global,ad,sz) else
	if level=lv then pop(local,ad,sz) else
	  begin lexaddr(lv,ad); gencst(op_sti,sz) end;
      pfixed:
	begin loadpos(pos,sz_addr); gencst(op_sti,sz) end;
      ploaded:
	gencst(op_sti,sz);
      indexed:
	gencst(op_sar,sz_word);
    end;  {case}
end end;

procedure fieldaddr(off:integer);
begin with a do
  if (ak=fixed) and not packbit then pos.ad:=pos.ad+off else
    begin loadaddr; gencst(op_adp,off) end
end;

procedure loadcheap;
begin if formof(a.asp,[arrays..records]) then loadaddr else load end;

{===================================================================}

procedure nextch;
begin
  eol:=eoln(input); read(input,ch); chsy:=cs[ch];
  if chsy <> tabch then srcchno:=srcchno+1
end;

procedure nextln;
begin
  if eof(input) then
    begin
      if not eofexpected then error(+03) else
	if fltused then begin gencst(ps_mes,ms_flt); argend end;
#ifdef STANDARD
      goto 9999
#else
      halt
#endif
    end;
  srcchno:=0; srclino:=srclino+1;
  if not including then
    begin srcorig:=srcorig+1; giveline:=true end;
end;

procedure options(normal:boolean);
var ci:char; i:integer;

procedure getc;
begin if normal then nextch else read(errors,ch) end;

begin
  repeat getc;
#ifdef CHL
    if cs[ch]=upper then
      ch:= chr( ord(ch) + (ord('a')-ord('A')) );  (* shift to lower case *)
#endif
    if (ch>='a') and (ch<='z') then
      begin ci:=ch; getc; i:=0;
	if ch='+' then begin i:=1; getc end else
	if ch='-' then getc else
	if cs[ch]=digit then
	  repeat i:=i*10 + ord(ch) - ord('0'); getc;
	  until cs[ch]<>digit
	else i:=-1;
	if i>=0 then
	  if not normal then
	    begin forceopt[ci]:=true; opt[ci]:=i end
	  else
	    if not forceopt[ci] then opt[ci]:=i;
      end;
  until ch<>',';
end;

procedure linedirective;
var i:integer; fname:fnarr;
begin
  repeat nextch until (ch='"') or eol;
  if eol then error(+04) else
    begin nextch; i:=0;
      while (ch<>'"') and not eol do
	begin
	  if ch='/' then i:=0 else
	    begin i:=i+1; if i<=fnmax then fname[i]:=ch end;
	  nextch
	end;
      while i<fnmax do begin i:=i+1; fname[i]:=' ' end;
      including:=fname<>source; while not eol do nextch
    end;
end;

procedure putdig;
begin ix:=ix+1; if ix<=smax then strbuf[ix]:=ch; nextch end;

procedure inident;
label 1;
var i,k:integer;
begin k:=0; id:=spaces;
  repeat
    if chsy=upper then ch:=chr(ord(ch)-ord('A')+ord('a'));
    if k<idmax then begin k:=k+1; id[k]:=ch end;
    nextch
  until chsy>digit;
	{lower=0,upper=1,digit=2. ugly but fast}
  for i:=frw[k-1] to frw[k] - 1 do
    if rw[i]=id then
      begin sy:=rsy[i]; goto 1 end;
  sy:=ident;
1:
end;

procedure innumber;
label   1;
var     i,j:integer;
	is:packed array[1..imax] of char;
begin ix:=0; sy:=intcst; val:=0;
  repeat putdig until chsy<>digit;
  if (ch='.') or (ch='e') or (ch='E') then
    begin
      if ch='.' then
	begin putdig;
	  if ch='.' then
	    begin seconddot:=true; ix:=ix-1; goto 1 end;
	  if chsy<>digit then error(+05) else
	    repeat putdig until chsy<>digit;
	end;
      if (ch='e') or (ch='E') then
	begin putdig;
	  if (ch='+') or (ch='-') then putdig;
	  if chsy<>digit then error(+06) else
	    repeat putdig until chsy<>digit;
	end;
      if ix>smax then begin error(+07); ix:=smax end;
      sy:=realcst; fltused:=true; val:=romstr(sp_fcon,sz_real);
    end;
1:if (chsy=lower) or (chsy=upper) then teststandard;
  if sy=intcst then
    if ix>imax then error(+08) else
      begin is:='0000000000'; i:=ix; j:=imax;
	repeat is[j]:=strbuf[i]; j:=j-1; i:=i-1 until i=0;
	if (is<=maxintstring) and (is<=maxcompintstring) then
	  repeat j:=j+1; val:=val*10 - ord('0') + ord(is[j]) until j=imax
	else if (is<=maxlongstring) and (dopt<>off) then
	  begin sy:=longcst; val:=romstr(sp_icon,sz_long) end
	else error(+09)
      end
end;

procedure instring(qc:char);
begin ix:=0; zerostring:=qc='"';
  repeat
    repeat nextch; ix:=ix+1; if ix<=smax then strbuf[ix]:=ch;
    until (ch=qc) or eol;
    if ch=qc then nextch else error(+010);
  until ch<>qc;
  if not zerostring then
    begin ix:=ix-1; if ix=0 then error(+011) end
  else
    begin strbuf[ix]:=chr(0); if copt=off then error(+012) end;
  if (ix=1) and not zerostring then
    begin sy:=charcst; val:=ord(strbuf[1]) end
  else
    begin if ix>smax then begin error(+013); ix:=smax end;
      sy:=stringcst; val:=romstr(sp_scon,0);
    end
end;

procedure incomment;
var stopc:char;
begin nextch; stopc:='}';
  if ch='$' then options(true);
  while (ch<>'}') and (ch<>stopc) do
    begin stopc:='}'; if ch='*' then stopc:=')';
      if eol then nextln; nextch
    end;
  if ch<>'}' then teststandard;
  nextch
end;

procedure insym;
  {read next basic symbol of source program and return its
  description in the global variables sy, op, id, val and ix}
label 1;
begin
1:case chsy of
    tabch:
      begin srcchno:=srcchno - srcchno mod 8 + 8; nextch; goto 1 end;
    layout:
      begin if eol then nextln; nextch; goto 1 end;
    lower,upper: inident;
    digit: innumber;
    quotech,dquotech:
      instring(ch);
    colonch:
      begin nextch;
	if ch='=' then begin sy:=becomes; nextch end else sy:=colon1
      end;
    periodch:
      begin nextch;
	if seconddot then begin seconddot:=false; sy:=colon2 end else
	if ch='.' then begin sy:=colon2; nextch end else sy:=period
      end;
    lessch:
      begin nextch;
	if ch='=' then begin sy:=lesy; nextch end else
	if ch='>' then begin sy:=nesy; nextch end else sy:=ltsy
      end;
    greaterch:
      begin nextch;
	if ch='=' then begin sy:=gesy; nextch end else sy:=gtsy
      end;
    lparentch:
      begin nextch;
	if ch<>'*' then sy:=lparent else
	  begin teststandard; incomment; goto 1 end;
      end;
    lbracech:
      begin incomment; goto 1 end;
    rparentch,lbrackch,rbrackch,commach,semich,arrowch,
    plusch,minch,slash,star,equal:
      begin sy:=csy[chsy]; nextch end;
    others:
      begin
	if (ch='#') and (srcchno=1) then linedirective else
	  begin error(+014); nextch end;
	goto 1
      end;
  end {case}
end;

procedure nextif(fsy:symbol; err:integer);
begin if sy=fsy then insym else error(-err) end;

function find1(sys1,sys2:sos; err:integer):boolean;
{symbol of sys1 expected. return true if sy in sys1}
begin
  if not (sy in sys1) then
    begin error(err); while not (sy in sys1+sys2) do insym end;
  find1:=sy in sys1
end;

function find2(sys1,sys2:sos; err:integer):boolean;
{symbol of sys1+sys2 expected. return true if sy in sys1}
begin
  if not (sy in sys1+sys2) then
    begin error(err); repeat insym until sy in sys1+sys2 end;
  find2:=sy in sys1
end;

function find3(sy1:symbol; sys2:sos; err:integer):boolean;
{symbol sy1 or one of sys2 expected. return true if sy1 found and skip it}
begin find3:=true;
  if not (sy in [sy1]+sys2) then
    begin error(err); repeat insym until sy in [sy1]+sys2 end;
  if sy=sy1 then insym else find3:=false
end;

function endofloop(sys1,sys2:sos; sy3:symbol; err:integer):boolean;
begin endofloop:=false;
  if find2(sys2+[sy3],sys1,err) then nextif(sy3,err+1)
  else endofloop:=true;
end;

function lastsemicolon(sys1,sys2:sos; err:integer):boolean;
begin lastsemicolon:=true;
  if not endofloop(sys1,sys2,semicolon,err) then
    if find2(sys2,sys1,err+2) then lastsemicolon:=false
end;

{===================================================================}

function searchid(fidcls: setofids):ip;
{search for current identifier symbol in the name table}
label 1;
var lip:ip; ic:idclass;
begin lastnp:=top;
  while lastnp<>nil do
    begin lip:=lastnp^.fname;
      while lip<>nil do
	if lip^.name=id then
	  if lip^.klass in fidcls then
	    begin
	      if lip^.klass=vars then if lip^.vpos.lv<>level then
		lip^.iflag:=lip^.iflag+[noreg];
	      goto 1
	    end
	  else lip:=lip^.rlink
	else
	  if lip^.name< id then lip:=lip^.rlink else lip:=lip^.llink;
      lastnp:=lastnp^.nlink;
    end;
  errid(+015,id);
  if types in fidcls then ic:=types else
  if vars  in fidcls then ic:=vars  else
  if konst in fidcls then ic:=konst else
  if proc  in fidcls then ic:=proc  else
  if func  in fidcls then ic:=func  else ic:=field;
  lip:=undefip[ic];
1:
  searchid:=lip
end;

function searchsection(fip: ip):ip;
{to find record fields and forward declared procedure identifiers
  -->procedure pfdeclaration
  -->procedure selector}
label 1;
begin
  while fip<>nil do
    if fip^.name=id then goto 1 else
      if fip^.name< id then fip:=fip^.rlink else fip:=fip^.llink;
1:  searchsection:=fip
end;

function searchlab(flp:lp; val:integer):lp;
label 1;
begin
  while flp<>nil do
    if flp^.labval=val then goto 1 else flp:=flp^.nextlp;
1:searchlab:=flp
end;

procedure opconvert(ts:twostruct);
var op:integer;
begin with a do begin genasp(op_loc);
  case ts of
    ir, lr: begin asp:=realptr; op:=op_cif; fltused:=true end;
    ri:     begin asp:=intptr ; op:=op_cfi; fltused:=true end;
    rl:     begin asp:=longptr; op:=op_cfi; fltused:=true end;
    li:     begin asp:=intptr ; op:=op_cii end;
    il:     begin asp:=longptr; op:=op_cii end;
  end;
  genasp(op_loc); genop(op)
end end;

procedure negate;
begin if a.asp=realptr then genasp(op_ngf) else genasp(op_ngi) end;

function desub(fsp:sp):sp;
begin if formof(fsp,[subrange]) then fsp:=fsp^.rangetype; desub:=fsp end;

function nicescalar(fsp:sp):boolean;
begin
  if fsp=nil then nicescalar:=true else
    nicescalar:=(fsp^.form=scalar) and (fsp<>realptr) and (fsp<>longptr)
end;

function bounded(fsp:sp):boolean;
begin bounded:=false;
  if fsp<>nil then
    if fsp^.form=subrange then bounded:=true else
    if fsp^.form=scalar then bounded:=fsp^.fconst<>nil
end;

procedure bounds(fsp:sp; var fmin,fmax:integer);
begin
  if fsp=nil then
    begin fmin:=0; fmax:=0 end
  else
    case fsp^.form of
    subrange:
      begin fmin:=fsp^.min; fmax:=fsp^.max end;
    scalar:
      begin fmin:=0; fmax:=fsp^.fconst^.value end
    end
end;

procedure genrck(fsp:sp);
var min,max,sno:integer;
begin
  if opt['r']<>off then if bounded(fsp) then
    begin
      if fsp^.form=scalar then sno:=fsp^.scalno else sno:=fsp^.subrno;
      if sno=0 then
	begin bounds(fsp,min,max); sno:=newdlb;
	  gencst(ps_rom,min); argcst(max); argend;
	  if fsp^.form=scalar then fsp^.scalno:=sno else fsp^.subrno:=sno
	end;
      laedlb(sno); gencst(op_rck,sz_word);
    end
end;

procedure checkbnds(fsp:sp);
var min1,max1,min2,max2:integer;
begin
  if bounded(fsp) then
    if not bounded(a.asp) then genrck(fsp) else
      begin bounds(fsp,min1,max1); bounds(a.asp,min2,max2);
	if (min2<min1) or (max2>max1) then
	  genrck(fsp);
      end;
  a.asp:=fsp;
end;

function eqstruct(p,q:sp):boolean;
begin eqstruct:=(p=q) or (p=nil) or (q=nil) end;

function string(fsp:sp):boolean;
var lsp:sp;
begin string:=false;
  if formof(fsp,[arrays]) then
    if eqstruct(fsp^.aeltype,charptr) then
      if spack in fsp^.sflag then
	begin lsp:=fsp^.inxtype;
	  if lsp=nil then string:=true else
	    if lsp^.form=subrange then
	      if lsp^.rangetype=intptr then
		if lsp^.min=1 then
		  string:=true
	end
end;

function compat(p,q:sp):twostruct;
begin compat:=noteq;
  if eqstruct(p,q) then compat:=eq else
    begin p:=desub(p); q:=desub(q);
      if eqstruct(p,q) then compat:=subeq else
      if p^.form=q^.form then
	case p^.form of
	  scalar:
	    if (p=intptr) and (q=realptr) then compat:=ir else
	    if (p=realptr) and (q=intptr) then compat:=ri else
	    if (p=intptr) and (q=longptr) then compat:=il else
	    if (p=longptr) and (q=intptr) then compat:=li else
	    if (p=longptr) and (q=realptr) then compat:=lr else
	    if (p=realptr) and (q=longptr) then compat:=rl else
		;
	  pointer:
	    if (p=nilptr) or (q=nilptr) then compat:=eq;
	  power:
	    if p=nullset then compat:=es else
	    if q=nullset then compat:=se else
	    if compat(p^.elset,q^.elset) <= subeq then
	      if p^.sflag=q^.sflag then compat:=eq;
	  arrays:
	    if string(p) and string(q) and (p^.size=q^.size) then compat:=eq;
	  files,carray,records: ;
	end;
    end
end;

procedure checkasp(fsp:sp; err:integer);
var ts:twostruct;
begin
  ts:=compat(a.asp,fsp);
  case ts of
    eq:
      if fsp<>nil then if withfile in fsp^.sflag then errasp(err);
    subeq:
      checkbnds(fsp);
    li:
      begin opconvert(ts); checkasp(fsp,err) end;
    il,rl,lr,ir:
      opconvert(ts);
    es:
      expandnullset(fsp);
    noteq,ri,se:
      errasp(err);
  end
end;

procedure force(fsp:sp; err:integer);
begin load; checkasp(fsp,err) end;

function newident(kl:idclass; idt:sp; nxt:ip; err:integer):ip;
begin newident:=nil;
  if sy<>ident then error(err) else
    begin newident:=newip(kl,id,idt,nxt); insym end
end;

function stringstruct:sp;
var lsp:sp;
begin {only used when ix and zerostring are still valid}
  if zerostring then lsp:=zeroptr else
    begin lsp:=newsp(arrays,ix*sz_char); lsp^.sflag:=[spack];
      lsp^.aeltype:=charptr; lsp^.inxtype:=nil;
    end;
  stringstruct:=lsp;
end;

function posaddr(var lb:integer; fsp:sp; partword:boolean):integer;
var sz:integer;
begin sz:=sizeof(fsp,partword);
  if sz_int = 2 then
    if lb >= MI2-sz-sz_word then begin error(+016); lb:=0 end;
  if not partword or (sz>=sz_word) then
    while lb mod sz_word <> 0 do lb:=lb+1;
  posaddr:=lb;
  lb:=lb+sz
end;

function negaddr(fsp:sp):integer;
var sz:integer;
begin with b do begin
  sz:=sizeof(fsp,wordmult);
  if sz_int = 2 then
    if reglb <= -MI2+sz+sz_word then begin error(+017); reglb:=0 end;
  reglb:=reglb-sz;
  while reglb mod sz_word <> 0 do reglb:=reglb-1;
  if reglb < minlb then minlb:=reglb;
  negaddr:=reglb
end end;

procedure temporary(fsp:sp;r:integer);
begin inita(fsp,negaddr(fsp));
  if r>=0 then genreg(sizeof(fsp,wordmult),a.pos.ad,r)
end;

procedure genhol;
var sz: integer;
begin
#ifdef  CHL
  holdone := true;
  if opt['g']=off then begin
    genop(ps_exa); argdlb(glbdata);
    if main then begin 
      sz:=posaddr(holeb,nil,false);
      argdlb(glbdata);
      gencst(ps_bss,sz);
      if sz_word = 4 then begin put1(sp_cst4); put1(0); put1(0); end
      else put1(sp_cst2);
      put1(0); put1(128);	 	{ 1000000000000000 pattern}
      argcst(0);
    end
  end
  else
  begin
    if main then sz:=posaddr(holeb,nil,false);
#else
  sz:=posaddr(holeb,nil,false);
  begin
#endif
    gencst(ps_hol,sz);
    if sz_word = 4 then begin put1(sp_cst4); put1(0); put1(0); end
    else put1(sp_cst2);
    put1(0); put1(128);	 	{ 1000000000000000 pattern}
    argcst(0);
  end;
  level:=1
end;

function arraysize(fsp:sp; pack:boolean):integer;
var sz,min,max,tot,n:integer;
begin sz:=sizeof(fsp^.aeltype,pack);
  bounds(fsp^.inxtype,min,max);
  fsp^.arpos.lv:=0; fsp^.arpos.ad:=newdlb;
#ifdef  CHL
  if main or holdone then
#endif
    begin
      gencst(ps_rom,min);
#ifdef	CHL
      if opt['r']<>on then 
	   if min<0 then argcst(maxint)
	   else argcst(maxint-min)
      else
           argcst(max-min); 
#else
      argcst(max-min);
#endif
      argcst(sz); argend
    end;
  n:=max-min+1; tot:=sz*n;
  if sz<>0 then if tot div sz <> n then begin error(+018); tot:=0 end;
  arraysize:=tot
end;

procedure treewalk(fip:ip);
var lsp:sp; i,sz:integer;
begin
  if fip<>nil then
    begin treewalk(fip^.llink); treewalk(fip^.rlink);
      if fip^.klass=vars then
	begin if not (used in fip^.iflag) then errid(-(+019),fip^.name);
	  if not (assigned in fip^.iflag) then errid(-(+020),fip^.name);
	  lsp:=fip^.idtype;
	  if level<>1 then 
	   if (refer in fip^.iflag) or not (noreg in fip^.iflag) then
	    if (refer in fip^.iflag) or formof(lsp,[pointer]) then
	      genreg(sz_addr,fip^.vpos.ad,reg_pointer)
	    else
	      begin sz:=sizeof(lsp,wordmult);
		if loopvar in fip^.iflag then
		  genreg(sz,fip^.vpos.ad,reg_loop)
		else if lsp=realptr then
		  genreg(sz,fip^.vpos.ad,reg_float)
		else
		  genreg(sz,fip^.vpos.ad,reg_any);
	      end;
	  if lsp<>nil then if withfile in lsp^.sflag then
	    if lsp^.form=files then
	      if level=1 then
		begin
		  for i:=2 to argc do with argv[i] do
		    if name=fip^.name then ad:=fip^.vpos.ad
		end
	      else
		begin
		  if not (refer in fip^.iflag) then
		    begin gencst(op_lal,fip^.vpos.ad); gensp(CLS,sz_addr)
		    end
		end
	    else
	      if level<>1 then errid(-(+021),fip^.name)
	end
    end
end;

procedure constant(fsys:sos; var fsp:sp; var fval:integer);
var signed,min:boolean; lip:ip;
begin signed:=(sy=plussy) or (sy=minsy);
  if signed then begin min:=sy=minsy; insym end else min:=false;
  if find1([ident..stringcst],fsys,+022) then
    begin fval:=val;
      case sy of
	stringcst: fsp:=stringstruct;
	charcst: fsp:=charptr;
	intcst: fsp:=intptr;
	realcst: fsp:=realptr;
	longcst: fsp:=longptr;
	ident:
	  begin lip:=searchid([konst]);
	    fsp:=lip^.idtype; fval:=lip^.value;
	  end
      end;  {case}
      if signed then
	if (fsp<>intptr) and (fsp<>realptr) and (fsp<>longptr) then
	  error(+023)
	else if min then fval:= -fval;
		{note: negating the v-number for reals and longs}
      insym;
    end
  else begin fsp:=nil; fval:=0 end;
end;

function cstinteger(fsys:sos; fsp:sp; err:integer):integer;
var lsp:sp; lval,min,max:integer;
begin constant(fsys,lsp,lval);
  if fsp<>lsp then
    if not eqstruct(desub(fsp),lsp) then
      begin error(err); lval:=0 end
    else if bounded(fsp) then
      begin bounds(fsp,min,max);
	if (lval<min) or (lval>max) then error(+024)
      end;
  cstinteger:=lval
end;

{===================================================================}

function typid(err:integer):sp;
var lip:ip; lsp:sp;
begin lsp:=nil;
  if sy<>ident then error(err) else
    begin lip:=searchid([types]); lsp:=lip^.idtype; insym end;
  typid:=lsp
end;

function simpletyp(fsys:sos):sp;
var lsp,lsp1:sp; lip,hip:ip; min,max:integer; lnp:np;
    newsubrange:boolean;
begin lsp:=nil;
  if find1([ident..lparent],fsys,+025) then
    if sy=lparent then
      begin insym; lnp:=top;   {decl. consts local to innermost block}
	while top^.occur<>blck do top:=top^.nlink;
	lsp:=newsp(scalar,sz_word); hip:=nil; max:=0;
	repeat lip:=newident(konst,lsp,hip,+026);
	  if lip<>nil then
	    begin enterid(lip); hip:=lip; lip^.value:=max; max:=max+1 end;
	until endofloop(fsys+[rparent],[ident],comma,+027);  {+028}
	if max<=MU1 then lsp^.size:=sz_byte
#if EM_WSIZE == 4
	else if max <= MU2 then lsp^.size := 2*sz_byte
#endif
	;
	lsp^.fconst:=hip; top:=lnp; nextif(rparent,+029);
      end
    else
      begin newsubrange:=true;
	if sy=ident then
	  begin lip:=searchid([types,konst]); insym;
	    if lip^.klass=types then
	      begin lsp:=lip^.idtype; newsubrange:=false end
	    else
	      begin lsp1:=lip^.idtype; min:=lip^.value end
	  end
	else constant(fsys+[colon2,ident..plussy],lsp1,min);
	if newsubrange then
	  begin lsp:=newsp(subrange,sz_word); lsp^.subrno:=0;
	    if not nicescalar(lsp1) then
	      begin error(+030); lsp1:=nil; min:=0 end;
	    lsp^.rangetype:=lsp1;
	    nextif(colon2,+031); max:=cstinteger(fsys,lsp1,+032);
	    if min>max then begin error(+033); max:=min end;
	    if ((min>=0) and (max<=MU1)) or ((min>=-NI1) and (max<=MI1)) then
		lsp^.size:=sz_byte
#if EM_WSIZE == 4
	    else if ((min>=0) and (max<=MU2)) or ((min>=-MI2-1) and (max<=MI2)) then
		lsp^.size := 2*sz_byte
#endif
		;
	    lsp^.min:=min; lsp^.max:=max
	  end
      end;
  simpletyp:=lsp
end;

function arraytyp(fsys:sos;
		  artyp:structform;
		  sflag:sflagset;
		  function element(fsys:sos):sp
		 ):sp;
var lsp,lsp1,hsp:sp; ok:boolean; sepsy:symbol; lip:ip;
    oksys:sos;
begin insym; nextif(lbrack,+034); hsp:=nil;
  repeat lsp:=newsp(artyp,0); initpos(lsp^.arpos);
    lsp^.aeltype:=hsp; hsp:=lsp;  {link reversed}
    if artyp=carray then
      begin sepsy:=semicolon; oksys:=[ident];
	lip:=newident(carrbnd,lsp,nil,+035); if lip<>nil then enterid(lip);
	nextif(colon2,+036);
	lip:=newident(carrbnd,lsp,lip,+037); if lip<>nil then enterid(lip);
	nextif(colon1,+038); lsp1:=typid(+039);
	ok:=nicescalar(desub(lsp1));
      end
    else
      begin sepsy:=comma; oksys:=[ident..lparent];
	lsp1:=simpletyp(fsys+[comma,rbrack,ofsy,ident..packedsy]);
	ok:=bounded(lsp1)
      end;
    if not ok then begin error(+040); lsp1:=nil end;
    lsp^.inxtype:=lsp1
  until endofloop(fsys+[rbrack,ofsy,ident..packedsy],oksys,
				sepsy,+041);  {+042}
  nextif(rbrack,+043); nextif(ofsy,+044);
  lsp:=element(fsys);
  if lsp<>nil then sflag:=sflag + lsp^.sflag * [withfile];
  repeat  {reverse links and compute size}
    lsp1:=hsp^.aeltype; hsp^.aeltype:=lsp; hsp^.sflag:=sflag;
    if artyp=arrays then hsp^.size:=arraysize(hsp,spack in sflag);
    lsp:=hsp; hsp:=lsp1
  until hsp=nil;  {lsp points to array with highest dimension}
  arraytyp:=lsp
end;

function typ(fsys:sos):sp;
var lsp,lsp1:sp; off,sz,min,errno:integer;
    sflag:sflagset; lnp:np;

function fldlist(fsys:sos):sp;
	{level 2: <<  typ}
var fip,hip,lip:ip; lsp:sp;

function varpart(fsys:sos):sp;
	{level 3: <<  fldlist <<  typ}
var tip,lip:ip; lsp,headsp,hsp,vsp,tsp,tsp1,tfsp:sp;
    minoff,maxoff,int,nvar:integer; lid:idarr;
begin insym; tip:=nil; lip:=nil;
  tsp:=newsp(tag,0);
  if sy<>ident then error(+045) else
    begin lid:=id; insym;
      if sy=colon1 then
	begin tip:=newip(field,lid,nil,nil); enterid(tip); insym;
	  if sy<>ident then error(+046) else
	    begin lid:=id; insym end;
	end;
      if sy=ofsy then  {otherwise you may destroy id}
	begin id:=lid; lip:=searchid([types]) end;
    end;
  if lip=nil then tfsp:=nil else tfsp:=lip^.idtype;
  if bounded(tfsp) then
    begin bounds(tfsp,int,nvar); nvar:=nvar-int+1 end
  else
    begin nvar:=0; if tfsp<>nil then begin error(+047); tfsp:=nil end end;
  tsp^.tfldsp:=tfsp;
  if tip<>nil then  {explicit tag}
    begin tip^.idtype:=tfsp;
      tip^.foffset:=posaddr(off,tfsp,spack in sflag)
    end;
  nextif(ofsy,+048); minoff:=off; maxoff:=minoff; headsp:=nil;
  repeat hsp:=nil;  {for each caselabel list}
    repeat nvar:=nvar-1;
      int:=cstinteger(fsys+[ident..plussy,comma,colon1,lparent,
		    semicolon,casesy,rparent],tfsp,+049);
      lsp:=headsp;  {each label may occur only once}
      while lsp<>nil do
	begin if lsp^.varval=int then error(+050);
	  lsp:=lsp^.nxtvar
	end;
      vsp:=newsp(variant,0); vsp^.varval:=int;
      vsp^.nxtvar:=headsp; headsp:=vsp;  {chain of case labels}
      vsp^.subtsp:=hsp; hsp:=vsp;
	    {use this field to link labels with same variant}
    until endofloop(fsys+[colon1,lparent,semicolon,casesy,rparent],
		    [ident..plussy],comma,+051);  {+052}
    nextif(colon1,+053); nextif(lparent,+054);
    tsp1:=fldlist(fsys+[rparent,semicolon,ident..plussy]);
    if off>maxoff then maxoff:=off;
    while vsp<>nil do
      begin vsp^.size:=off; hsp:=vsp^.subtsp;
	vsp^.subtsp:=tsp1; vsp:=hsp
      end;
    nextif(rparent,+055);
    off:=minoff;
  until lastsemicolon(fsys,[ident..plussy],+056);  {+057 +058}
  if nvar>0 then error(-(+059));
  tsp^.fstvar:=headsp; tsp^.size:=minoff; off:=maxoff; varpart:=tsp;
end;

begin  {fldlist}
  if find2([ident],fsys+[casesy],+060) then
    repeat lip:=nil; hip:=nil;
      repeat fip:=newident(field,nil,nil,+061);
	if fip<>nil then
	  begin enterid(fip);
	    if lip=nil then hip:=fip else lip^.next:=fip; lip:=fip;
	  end;
      until endofloop(fsys+[colon1,ident..packedsy,semicolon,casesy],
			      [ident],comma,+062);  {+063}
      nextif(colon1,+064);
      lsp:=typ(fsys+[casesy,semicolon]);
      if lsp<>nil then if withfile in lsp^.sflag then
	sflag:=sflag+[withfile];
      while hip<>nil do
	begin hip^.idtype:=lsp;
	  hip^.foffset:=posaddr(off,lsp,spack in sflag);
	  hip:=hip^.next
	end;
    until lastsemicolon(fsys+[casesy],[ident],+065);  {+066 +067}
  if sy=casesy then fldlist:=varpart(fsys) else fldlist:=nil;
end;


begin  {typ}
  sflag:=[]; lsp:=nil;
  if sy=packedsy then begin sflag:=[spack]; insym end;
  if find1([ident..filesy],fsys,+068) then
    if sy in [ident..arrow] then
      begin if spack in sflag then error(+069);
	if sy=arrow then
	  begin lsp:=newsp(pointer,sz_addr); insym;
	    if not intypedec then lsp^.eltype:=typid(+070) else
	      if sy<>ident then error(+071) else
		begin fwptr:=newip(types,id,lsp,fwptr); insym end
	  end
	else lsp:=simpletyp(fsys);
      end
    else
      case sy of
{<<<<<<<<<<<<}
arraysy:
  lsp:=arraytyp(fsys,arrays,sflag,typ);
recordsy:
  begin insym;
    new(lnp,rec); lnp^.occur:=rec; lnp^.nlink:=top; lnp^.fname:=nil; top:=lnp;
    off:=0; lsp1:=fldlist(fsys+[endsy]);  {fldlist updates off}
    lsp:=newsp(records,off); lsp^.tagsp:=lsp1;
    lsp^.fstfld:=top^.fname; lsp^.sflag:=sflag;
    top:=top^.nlink; nextif(endsy,+072)
  end;
setsy:
  begin insym; nextif(ofsy,+073);
    lsp:=simpletyp(fsys); lsp1:=desub(lsp); errno:=0;
    if bounded(lsp1) then
      begin bounds(lsp1,min,sz);
	if sz div NB1>=sz_mset then errno:=+074
      end
    else if bounded(lsp) then  {subrange of integer}
      begin bounds(lsp,min,sz);
	if (min<0) or (sz>=iopt) then errno:=+075;
	sz:=iopt-1
      end
    else if lsp=intptr then
      begin sz:=iopt-1; errno:=-(+076) end
    else
      errno:=+077;
    if errno<>0 then
      begin error(errno); if errno>0 then begin lsp1:=nil; sz:=0 end end;
    lsp:=newsp(power,sz div NB1 +1); lsp^.elset:=lsp1;
  end;
filesy:
  begin insym; nextif(ofsy,+078); lsp1:=typ(fsys);
    if lsp1<>nil then if withfile in lsp1^.sflag then error(-(+079));
    sz:=sizeof(lsp1,wordpart); if sz<sz_buff then sz:=sz_buff;
    lsp:=newsp(files,sz+sz_head); lsp^.filtype:=lsp1;
  end;
{>>>>>>>>>>>>}
      end;  {case}
  typ:=lsp;
end;

function vpartyp(fsys:sos):sp;
begin
  if find2([arraysy],fsys+[ident],+080) then
    vpartyp:=arraytyp(fsys,carray,[],vpartyp)
  else
    vpartyp:=typid(+081)
end;

{===================================================================}

procedure block(fsys:sos; fip:ip); forward;
	{pfdeclaration calls block. With a more obscure lexical
	  structure this forward declaration can be avoided}

procedure labeldeclaration(fsys:sos);
var llp:lp;
begin with b do begin
  repeat
    if sy<>intcst then error(+082) else
      begin
	if searchlab(lchain,val)<>nil then errint(+083,val) else
	  begin new(llp); llp^.labval:=val;
	    if val>9999 then teststandard;
	    ilbno:=ilbno+1; llp^.labname:=ilbno; llp^.labdlb:=0;
	    llp^.seen:=false; llp^.nextlp:=lchain; lchain:=llp;
	  end;
	insym
      end
  until endofloop(fsys+[semicolon],[intcst],comma,+084);  {+085}
  nextif(semicolon,+086)
end end;

procedure constdefinition(fsys:sos);
var lip:ip;
begin
  repeat lip:=newident(konst,nil,nil,+087);
    if lip<>nil then
      begin nextif(eqsy,+088);
	constant(fsys+[semicolon,ident],lip^.idtype,lip^.value);
	nextif(semicolon,+089); enterid(lip);
      end;
  until not find2([ident],fsys,+090);
end;

procedure typedefinition(fsys:sos);
var lip:ip;
begin fwptr:=nil; intypedec:=true;
  repeat lip:=newident(types,nil,nil,+091);
    if lip<>nil then
      begin nextif(eqsy,+092);
	lip^.idtype:=typ(fsys+[semicolon,ident]);
	nextif(semicolon,+093); enterid(lip);
      end;
  until not find2([ident],fsys,+094);
  assert sy<>ident;
  while fwptr<>nil do
    begin
      id:=fwptr^.name; lip:=searchid([types]);
      fwptr^.idtype^.eltype:=lip^.idtype; fwptr:=fwptr^.next
    end;
  intypedec:=false;
end;

procedure vardeclaration(fsys:sos);
var lip,hip,vip:ip; lsp:sp;
begin
  repeat hip:=nil; lip:=nil;
    repeat vip:=newident(vars,nil,nil,+095);
      if vip<>nil then
	begin enterid(vip); vip^.iflag:=[];
	  if lip=nil then hip:=vip else lip^.next:=vip; lip:=vip;
	end;
    until endofloop(fsys+[colon1,ident..packedsy],[ident],comma,+096);  {+097}
    nextif(colon1,+098);
    lsp:=typ(fsys+[semicolon,ident]);
    while hip<>nil do
      begin hip^.idtype:=lsp;
	if level<=1 then
	  hip^.vpos.ad:=posaddr(holeb,lsp,false)
	else
	  hip^.vpos.ad:=negaddr(lsp);
	hip:=hip^.next
      end;
    nextif(semicolon,+099);
  until not find2([ident],fsys,+0100);
end;

procedure pfhead(fsys:sos;var fip:ip;var again:boolean;param:boolean);
  forward;

procedure parlist(fsys:sos; slink:boolean; var tip:ip; var maxlb:integer);
var lastip,hip,lip,pip:ip; lsp,tsp:sp; iflag:iflagset; again:boolean;
#ifdef  CHL
  function posaddr(var lb:integer; fsp:sp; partword:boolean):integer;
  var sz:integer;
  begin sz:=sizeof(fsp,partword);
    lb:=lb+sz;
    posaddr:=sz;
  end;
#endif
begin tip:=nil; lastip:=nil;
  maxlb:=0; if slink then maxlb:=sz_addr;
  repeat  {once for each formal-parameter-section}
    if find1([ident,varsy,procsy,funcsy],fsys+[semicolon],+0101) then
      begin
	if (sy=procsy) or (sy=funcsy) then
	  begin
	    pfhead(fsys+[semicolon,ident,varsy,procsy,funcsy],hip,again,true);
	    hip^.pfpos.ad:=posaddr(maxlb,procptr,false);
	    hip^.pfkind:=formal; lip:=hip;
	    top:=top^.nlink; level:=level-1
	  end
	else
	  begin hip:=nil; lip:=nil; iflag:=[assigned];
	    if sy=varsy then
	      begin iflag:=[refer,assigned,used]; insym end;
	    repeat pip:=newident(vars,nil,nil,+0102);
	      if pip<>nil then
		begin enterid(pip); pip^.iflag:=iflag;
		  if lip=nil then hip:=pip else lip^.next:=pip; lip:=pip;
		end;
	      iflag:=iflag+[samesect];
	    until endofloop(fsys+[semicolon,colon1],[ident],comma,+0103);
	    {+0104}
	    nextif(colon1,+0105);
	    if refer in iflag then
	      begin lsp:=vpartyp(fsys+[semicolon]); tsp:=lsp;
		while formof(tsp,[carray]) do
		  begin tsp^.arpos.ad:=posaddr(maxlb,nilptr,false);
		    tsp:=tsp^.aeltype
		  end;
		tsp:=nilptr;
	      end
	    else
	      begin lsp:=typid(+0106); tsp:=lsp end;
	    pip:=hip;
	    while pip<>nil do
	      begin pip^.vpos.ad:=posaddr(maxlb,tsp,false); pip^.idtype:=lsp;
		pip:=pip^.next
	      end;
	  end;
	if lastip=nil then tip:=hip else lastip^.next:=hip; lastip:=lip;
     end;
  until endofloop(fsys,[ident,varsy,procsy,funcsy],semicolon,+0107);  {+0108}
#ifdef  CHL
  hip:=tip;
  while hip<>nil do
    begin
     if hip^.klass=vars then
	begin maxlb:=maxlb-hip^.vpos.ad; hip^.vpos.ad:=maxlb end
      else begin maxlb:=maxlb-hip^.pfpos.ad; hip^.pfpos.ad:=maxlb end;
      hip:=hip^.next;
    end;
#endif
end;

procedure pfhead;  {forward declared}
var lip:ip; lsp:sp; lnp:np; kl:idclass;
begin lip:=nil; again:=false;
  if sy=procsy then kl:=proc else
    begin kl:=func; fsys:=fsys+[colon1,ident] end;
  insym;
  if sy<>ident then begin error(+0109); id:=spaces end;
  if not param then lip:=searchsection(top^.fname);
  if lip<>nil then
    if (lip^.klass<>kl) or (lip^.pfkind<>forward) then errid(+0110,id) else
      begin b.forwcount:=b.forwcount-1; again:=true end;
  if again then insym else
    begin lip:=newip(kl,id,nil,nil);
      if sy=ident then begin enterid(lip); insym end;
      lastpfno:=lastpfno+1; lip^.pfno:=lastpfno;
    end;
  level:=level+1;
  new(lnp,blck); lnp^.occur:=blck; lnp^.nlink:=top; top:=lnp;
  if again then lnp^.fname:=lip^.parhead else
    begin lnp^.fname:=nil;
      if find3(lparent,fsys,+0111) then
	begin parlist(fsys+[rparent],lip^.pfpos.lv>1,lip^.parhead,lip^.maxlb);
	  nextif(rparent,+0112)
	end;
    end;
  if (kl=func) and not again then
    begin nextif(colon1,+0113); lsp:=typid(+0114);
      if formof(lsp,[power..tag]) then
	begin error(+0115); lsp:=nil end;
      lip^.idtype:=lsp;
    end;
  fip:=lip;
end;

procedure pfdeclaration(fsys:sos);
var lip:ip; again,headonly:boolean; markp:^integer; lbp:bp; kind:kindofpf;
begin with b do begin
  pfhead(fsys+[ident,semicolon,labelsy..beginsy],lip,again,false);
  nextif(semicolon,+0116);
  if find1([ident,labelsy..beginsy],fsys+[semicolon],+0117) then
    begin headonly:=sy=ident;
      if headonly then
	begin kind:=standard;
	  if id='forward ' then kind:=forward else
	  if id='extern  ' then kind:=extern  else
	  if id='varargs ' then kind:=varargs else errid(+0118,id);
	  if kind<>standard then
	    begin insym; lip^.pfkind:=kind;
	      if kind=forward then
		if again then errid(+0119,lip^.name) else
		  forwcount:=forwcount+1
	      else
		begin 
#ifdef CHL
		if kind<>extern then 
#endif
		    lip^.pfpos.lv:=1; 
		    teststandard 
	        end
	    end;
	end;
      if not again then
#ifdef  CHL
	if (lip^.pfpos.lv<=1) or (opt['e']=on) then
#else
	if lip^.pfpos.lv<=1 then
#endif
	  genpnam(ps_exp,lip) else genpnam(ps_inp,lip);
      if not headonly then
	begin lip^.pfkind:=actual;
#ifndef STANDARD
	  mark(markp);
#endif
	  new(lbp); lbp^:=b; nextbp:=lbp;
	  minlb:=0; ilbno:=0; forwcount:=0; lchain:=nil;
#ifdef  CHL
	  reglb:=-sz_word;
	  tip:=lip;
#else
	  reglb:=0;
#endif
	  block(fsys+[semicolon],lip);
	  b:=nextbp^;
#ifndef STANDARD
	  release(markp);
#endif
	end;
    end;
  if not main then eofexpected:=forwcount=0;
  nextif(semicolon,+0120);
  level:=level-1; top:=top^.nlink;
end end;

{===================================================================}

procedure expression(fsys:sos); forward;
	{this forward declaration cannot be avoided}

procedure selectarrayelement(fsys:sos);
var isp,lsp:sp;
begin
  repeat loadaddr; isp:=nil;
    if formof(a.asp,[arrays,carray]) then isp:=a.asp^.inxtype else
      errasp(+0121);
    lsp:=a.asp;
    expression(fsys+[comma]); force(desub(isp),+0122);
		{no range check}
    if lsp<>nil then
      begin a.packbit:=spack in lsp^.sflag;
	descraddr(lsp^.arpos); lsp:=lsp^.aeltype
      end;
    a.asp:=lsp; a.ak:=indexed;
  until endofloop(fsys,[notsy..lparent],comma,+0123);  {+0124}
end;

procedure selector(fsys: sos; fip:ip; iflag:iflagset);
{selector computes the address of any kind of variable.
  Four possibilities:
  1.for direct accessable variables (fixed), a contains offset and level,
  2.for indirect accessable variables (ploaded), the address is on the stack.
  3.for array elements, the top of stack gives the index (one word).
    The address of the array is beneath it.
  4.for variables with address in direct accessible pointer variable (pfixed),
    the offset and level of the pointer is stored in a.
  If a.asp=nil then an error occurred else a.asp gives
  the type of the variable.
}
var lip:ip;
begin inita(fip^.idtype,0);
  case fip^.klass of
    vars: with a do
      begin pos:=fip^.vpos; if refer in fip^.iflag then ak:=pfixed end;
    field:
      begin a:=lastnp^.wa; fieldaddr(fip^.foffset); a.asp:=fip^.idtype end;
    func: with a do
      if fip^.pfkind=standard then errasp(+0125) else
      if (fip^.pfpos.lv>=level-1) and (fip<>currproc) then error(+0126) else
      if fip^.pfkind<>actual then error(+0127) else
	begin pos:=fip^.pfpos; pos.lv:=pos.lv+1;
	  if sy=arrow then error(+0128);
	end
  end;  {case}
  if (sy=lbrack) or (sy=period) then iflag:=iflag+[noreg];
  while find2([lbrack,period,arrow],fsys,+0129) do with a do
    if sy=lbrack then
      begin insym; selectarrayelement(fsys+[rbrack,lbrack,period,arrow]);
	nextif(rbrack,+0130);
      end else
    if sy=period then
      begin insym;
	if sy<>ident then error(+0131) else
	  begin
	    if not formof(asp,[records]) then errasp(+0132) else
	      begin lip:=searchsection(asp^.fstfld);
		if lip=nil then begin errid(+0133,id); asp:=nil end else
		  begin packbit:=spack in asp^.sflag;
		    fieldaddr(lip^.foffset); asp:=lip^.idtype
		  end
	      end;
	    insym
	  end
      end
    else
      begin insym; iflag:=[used];
	if asp<>nil then
	  if asp=zeroptr then errasp(+0134) else
	  if asp^.form=pointer then
	    begin
	      if ak=fixed then ak:=pfixed else
		begin load; ak:=ploaded end;
	      asp:=asp^.eltype
	    end else
	  if asp^.form=files then
	    begin loadaddr; gensp(WDW,sz_addr); gencst(op_lfr,sz_addr);
	      asp:=asp^.filtype; ak:=ploaded; packbit:=true;
	    end
	  else errasp(+0135);
      end;
  fip^.iflag:=fip^.iflag+iflag;
end;

procedure variable(fsys:sos);
var lip: ip;
begin
  if sy=ident then
    begin lip:=searchid([vars,field]); insym;
      selector(fsys,lip,[used,assigned,noreg])
    end
  else begin error(+0136); inita(nil,0) end;
end;

{===================================================================}

function plistequal(p1,p2:ip):boolean;
var ok:boolean; q1,q2:sp;
begin plistequal:=eqstruct(p1^.idtype,p2^.idtype);
  p1:=p1^.parhead; p2:=p2^.parhead;
  while (p1<>nil) and (p2<>nil) do
    begin ok:=false;
      if p1^.klass=p2^.klass then
	if p1^.klass<>vars then ok:=plistequal(p1,p2) else
	  begin q1:=p1^.idtype; q2:=p2^.idtype; ok:=true;
	    while ok and formof(q1,[carray]) and formof(q2,[carray]) do
	      begin ok:=eqstruct(q1^.inxtype,q2^.inxtype);
		q1:=q1^.aeltype; q2:=q2^.aeltype;
	      end;
	    if not (eqstruct(q1,q2) and
		    (p1^.iflag*[refer,samesect] = p2^.iflag*[refer,samesect]))
	      then ok:=false;
	  end;
      if not ok then plistequal:=false;
      p1:=p1^.next; p2:=p2^.next
    end;
  if (p1<>nil) or (p2<>nil) then plistequal:=false
end;

procedure callnonstandard(fsys:sos; moreargs:boolean; fip:ip);
var nxt,lip:ip; l0,l1,l2,l3,sz:integer; lsp,savasp:sp;
begin with a do begin
  l0:=newmark; l1:=newmark; sz:=0; nxt:=fip^.parhead;
  while moreargs do
    begin
      if nxt=nil then
	begin if fip^.pfkind<>varargs then error(+0137);
	  expression(fsys); load; sz:=sz+sizeof(asp,wordmult)
	end
      else
	begin lsp:=nxt^.idtype;
	  if nxt^.klass<>vars then  {proc or func}
	    begin inita(procptr,0); sz:=sz+sz_proc;
	      if sy<>ident then error(+0138) else
		begin lip:=searchid([nxt^.klass]); insym;
		  if lip^.pfkind=standard then error(+0139) else
		  if not plistequal(nxt,lip) then error(+0140)
		  else
		    begin pos:=lip^.pfpos;
		      if lip^.pfkind=formal then load else
			begin
			  if lip^.pfpos.lv<=1 then gencst(op_zer,sz_addr) else
			    gencst(op_lxl,level-lip^.pfpos.lv);
			  genpnam(op_lpi,lip)
			end
		    end
		end
	    end
	  else if not (refer in nxt^.iflag) then  {call by value}
	    begin expression(fsys); force(lsp,+0141);
	      sz:=sz+sizeof(asp,wordmult);
	    end
	  else  {call by reference}
	    begin variable(fsys); loadaddr; sz:=sz+sz_addr;
	      if samesect in nxt^.iflag then lsp:=savasp else
		begin savasp:=asp; l2:=newmark;
		  while formof(lsp,[carray])
			and formof(asp,[arrays,carray]) do
		    if (compat(lsp^.inxtype,asp^.inxtype) > subeq) or
			  (lsp^.sflag<>asp^.sflag) then errasp(+0142) else
		      begin l3:=newmark; descraddr(asp^.arpos); exchange(l2,l3);
			relmark(l3);
			sz:=sz+sz_addr; asp:=asp^.aeltype; lsp:=lsp^.aeltype
		      end;
		  relmark(l2)
		end;
	      if not eqstruct(asp,lsp) then errasp(+0143);
	      if packbit then errasp(+0144);
	    end;
	  nxt:=nxt^.next
	end;
#ifndef CHL
      exchange(l0,l1);
#endif
      relmark(l1);
      l1:=newmark; moreargs:=find3(comma,fsys,+0145)
    end;
  relmark(l0); relmark(l1);
  if nxt<>nil then error(+0146);
  inita(procptr,0); pos:=fip^.pfpos;
  if fip^.pfkind=formal then
    with b do
      begin load; ilbno:=ilbno+2;
	gencst(op_exg,sz_addr);
	gencst(op_dup,sz_addr);
	gencst(op_zer,sz_addr);
	genop(op_cmp);
	gencst(op_zeq,ilbno-1);
	gencst(op_exg,sz_addr);
	genop(op_cai);
	gencst(op_asp,sz_addr);
	gencst(op_bra,ilbno);
	newilb(ilbno-1);
	gencst(op_asp,sz_addr);
	genop(op_cai);
	newilb(ilbno);
      end
  else
    begin
      if pos.lv>1 then
	begin gencst(op_lxl,level-pos.lv); sz:=sz+sz_addr end;
      genpnam(op_cal,fip)
    end;
  if sz<>0 then gencst(op_asp,sz);
  asp:=fip^.idtype;
  if asp<>nil then genasp(op_lfr)
end end;

procedure fileaddr;
var la:attr;
begin la:=a; a:=fa; loadaddr; a:=la end;

procedure callr(l1,l2:integer);
var la:attr; m:libmnem;
begin with a do begin
  la:=a; asp:=desub(asp); fileaddr; m:=RDI;
  if asp<>intptr then
  if asp=charptr then m:=RDC else
  if asp=realptr then m:=RDR else
  if asp=longptr then m:=RDL else errasp(+0147);
  gensp(m,sz_addr); genasp(op_lfr);
  if asp<>la.asp then checkbnds(la.asp);
  a:=la; exchange(l1,l2); store;
end end;

procedure callw(fsys:sos; l1,l2:integer);
var m:libmnem; s:integer;
begin with a do begin
  fileaddr; exchange(l1,l2); loadcheap; asp:=desub(asp);
  if string(asp) then
    begin gencst(op_loc,asp^.size); m:=WRS; s:=sz_addr+sz_word end
  else
    begin m:=WRI; s:=sizeof(asp,wordmult);
      if asp<>intptr then
      if asp=charptr then m:=WRC else
      if asp=realptr then m:=WRR else
      if asp=boolptr then m:=WRB else
      if asp=zeroptr then m:=WRZ else
      if asp=longptr then m:=WRL else errasp(+0148);
    end;
  if find3(colon1,fsys,+0149) then
    begin expression(fsys+[colon1]); force(intptr,+0150);
      m:=succ(m); s:=s+sz_int
    end;
  if find3(colon1,fsys,+0151) then
    begin expression(fsys); force(intptr,+0152); s:=s+sz_int;
      if m<>WSR then error(+0153) else m:=WRF;
    end;
  gensp(m,s+sz_addr);
end end;

procedure callrw(fsys:sos; lpar,w,ln:boolean);
var l1,l2,errno:integer; ftype,lsp,fsp:sp; (* savlb:integer;*) m:libmnem;
begin with b do begin (* savlb:=reglb; *) ftype:=textptr;
  inita(textptr,argv[ord(w)].ad); a.pos.lv:=0; fa:=a;
  if lpar then
    begin l1:=newmark; if w then expression(fsys+[colon1]) else variable(fsys);
      l2:=newmark;
      if formof(a.asp,[files]) then
	begin ftype:=a.asp;
	  if (a.ak<>fixed) and (a.ak<>pfixed) then
	    begin loadaddr; temporary(nilptr,reg_pointer);
	      store; a.ak:=pfixed
	    end;
	  fa:=a;  {store does not change a}
	  if (sy<>comma) and not ln then error(+0154);
	end
      else
	begin if iop[w]=nil then error(+0155);
	  if w then callw(fsys,l1,l2) else callr(l1,l2)
	end;
      relmark(l1); relmark(l2);
      while find3(comma,fsys,+0156) do with a do
	begin l1:=newmark;
	  if w then expression(fsys+[colon1]) else variable(fsys);
	  l2:=newmark;
	  if ftype=textptr then
	    if w then callw(fsys,l1,l2) else callr(l1,l2)
	  else
	    begin errno:=+0157; fsp:=ftype^.filtype;
	      if w then force(fsp,errno) else
		begin store; lsp:=asp; relmark(l2); l2 := newmark end;
	      fileaddr; gensp(WDW,sz_addr); gencst(op_lfr,sz_addr);
	      ak:=ploaded; packbit:=true; asp:=fsp;
	      if w then store else
		begin force(lsp,errno); exchange(l1,l2) end;
	      fileaddr; if w then m:=PUTX else m:=GETX; gensp(m,sz_addr)
	    end;
	  relmark(l1); relmark(l2);
	end;
    end
  else
    if not ln then error(+0158) else
      if iop[w]=nil then error(+0159);
  if ln then
    begin if ftype<>textptr then error(+0160);
      fileaddr; if w then m:=WLN else m:=RLN; gensp(m,sz_addr)
    end;
  (* reglb:=savlb *)
end end;

procedure callnd(fsys:sos);
label 1;
var lsp:sp; int:integer;
begin with a do begin
  if asp=zeroptr then errasp(+0161) else asp:=asp^.eltype;
  while find3(comma,fsys,+0162) do
    begin
      if asp<>nil then  {asp of form record or variant}
	if asp^.form=records then asp:=asp^.tagsp else
	if asp^.form=variant then asp:=asp^.subtsp else errasp(+0163);
      if asp=nil then constant(fsys,lsp,int) else
	begin assert asp^.form=tag;
	  int:=cstinteger(fsys,asp^.tfldsp,+0164); lsp:=asp^.fstvar;
	  while lsp<>nil do
	    if lsp^.varval<>int then lsp:=lsp^.nxtvar else
	      begin asp:=lsp; goto 1 end;
	end;
1:  end;
  genasp(op_loc)
end end;

#ifdef  CHL
function plen:integer;
{length of parameter space of current proc/func}
var pip:ip; maxlb,dummy:integer;
begin
  if b.tip<>nil then
    begin
      maxlb:=ord(b.tip^.pfpos.lv>1)*sz_addr;    {space for static chain}
      pip:=b.tip^.parhead;
      while pip<>nil do
	begin
	  case pip^.klass of
	    vars: if refer in pip^.iflag then dummy:=posaddr(maxlb,nilptr,false)
		  else dummy:=posaddr(maxlb,pip^.idtype,false);
	    proc,func: dummy:=posaddr(maxlb,procptr,false);
	  end;
	  pip:=pip^.next;
	end;
      plen:=maxlb;
    end
  else error(+0001);
end {of plen};
#endif

procedure call(fsys: sos; fip: ip);
var lkey: standpf; lpar:boolean; lsp,sp1,sp2:sp;
    m:libmnem; s:integer; b:byte;
#ifdef  CHL
    int:integer;
#endif
begin with a do begin fsys:=fsys+[comma];
  lpar:=find3(lparent,fsys,+0165); if lpar then fsys:=fsys+[rparent];
  if fip^.pfkind<>standard then callnonstandard(fsys,lpar,fip) else
    begin lkey:=fip^.key; m:=CLS; lsp:=nil;
      if not lpar then
	if lkey in [pput..prelease,
#ifdef  CHL
		    psetstktop,pmoveleft,
#endif
		    fabs..fatn] then error(+0166);
      if lkey in [pput..ppage,feof,feoln] then
	begin s:=sz_addr;
	  if lpar then
	    begin variable(fsys); loadaddr end
	  else
	    begin asp:=textptr;
	      if iop[lkey=ppage]=nil then errasp(+0167) else
		genglb(op_lae,argv[ord(lkey=ppage)].ad)
	    end;
	  if lkey in [pput..prewrite,ppage,feof,feoln] then
	    if not formof(asp,[files]) then
	      begin error(+0168); asp:=textptr end;
	  if lkey in [pnew,pdispose,
#ifdef  CHL
		      penew,pedispose,
#endif
		      pmark,prelease] then
	    if not formof(asp,[pointer]) then
	      begin error(+0169); asp:=nilptr end;
	end;
      case lkey of
	pread, preadln, pwrite, pwriteln:       {0,1,2,3 resp}
	  callrw(fsys,lpar,lkey>=pwrite,odd(ord(lkey)));
	pput: m:=PUTX;
	pget: m:=GETX;
	ppage: m:=PAG;
	preset: m:=OPN;
	prewrite: m:=CRE;
#ifdef  CHL
	penew,
#endif
	pnew: m:=NEWX;
#ifdef  CHL
	pedispose,
#endif
	pdispose: m:=DIS;
	ppack:
	  begin sp2:=asp; nextif(comma,+0170); expression(fsys); load;
	    lsp:=asp; nextif(comma,+0171); variable(fsys); loadaddr;
	    sp1:=asp; asp:=lsp; m:=PAC
	  end;
	punpack:
	  begin sp1:=asp; nextif(comma,+0172); variable(fsys); loadaddr;
	    sp2:=asp; nextif(comma,+0173); expression(fsys); load;
	    m:=UNP
	  end;
	pmark: m:=SAV;
	prelease: m:=RST;
	phalt:
	  begin m:=HLT; teststandard;
	    if lpar then lsp:=intptr else gencst(op_loc,0);
	  end;
	feof: m:=EFL;
	feoln: m:=ELN;
#ifdef  CHL
	fasptr: if sz_addr=sz_word then lsp:=intptr else lsp:=longptr;
	pmoveleft: ;
	psetstktop, fgetstktop,
#endif
	fodd, fchr: lsp:=intptr;
	fpred: b:=op_dec;
	fsucc: b:=op_inc;
	fround: m:=RND;
	fsin, fcos, fexp, fsqt, flog, fatn: lsp:=realptr;
#ifdef	CHL
	fincptr,
#endif
	fabs, fsqr, ford, ftrunc: ;
      end;
      if lpar then if lkey in [phalt,
#ifdef  CHL
			       psetstktop,
#endif
			       fabs..fatn] then
	begin expression(fsys);
#ifdef  CHL
	  if lkey in [psetstktop,fgetstktop] then
	    if ak<>cst then error(+0022)
	    else
	      begin
		if lkey=fgetstktop then gencst(op_lal,plen);
		checkasp(lsp,+0174);
		s:=pos.ad + pos.ad mod sz_word;
		nextif(comma,+0146);
		expression(fsys);
		checkasp(lsp,+0174);
		if s>sz_int then lsp:=longptr;
		if s>sz_long then lsp:=realptr;
	      end
	  else
	    begin
#endif
	     force(lsp,+0174);
	     s:=sizeof(asp,wordmult)
#ifdef  CHL
	    end
#endif
	end;
      if lkey in [ppack,punpack,fabs..fodd] then
	asp:=desub(asp);
      case lkey of
	ppage, feoln:
	  begin if asp<>textptr then error(+0175); asp:=boolptr end;
	preset, prewrite:
	  begin s:=sz_addr+sz_word;
	    if asp=textptr then gencst(op_loc,0) else
	      gencst(op_loc,sizeof(asp^.filtype,wordpart));
	  end;
	pnew, pdispose:
	  begin callnd(fsys); s:=sz_addr+sz_word end;
#ifdef  CHL
	penew, pedispose:
	  begin if find1([comma],fsys,+0146) then
	      begin insym; expression(fsys); force(intptr,+0174) end;
	    s:=sz_addr+sz_word;
	  end;
#endif
	ppack, punpack:
	  begin s:=2*sz_addr+sz_int;
	    if formof(sp1,[arrays,carray])
	       and formof(sp2,[arrays,carray]) then
	      if (spack in (sp1^.sflag - sp2^.sflag)) and
		  eqstruct(sp1^.aeltype,sp2^.aeltype) and
		  eqstruct(desub(sp1^.inxtype),asp) and
		  eqstruct(desub(sp2^.inxtype),asp) then
		begin descraddr(sp1^.arpos); descraddr(sp2^.arpos) end
	      else error(+0176)
	    else error(+0177)
	  end;
	pmark, prelease: teststandard;
#ifdef  CHL
	psetstktop:
	  if ak=cst then
	    begin int:=plen+pos.ad;
	      if find1([comma],fsys,+0146) then
		begin insym; expression(fsys); force(lsp,+0174) end;
	      pop(local,int,s);
	    end
	  else error(+0001);    {facility not actually needed yet}
	pmoveleft:
	  begin
	    for int:=1 to 3 do
	      begin
		expression(fsys);
		if int<3 then
		  begin if not formof(asp,[pointer]) then error(+0174);
		    nextif(comma,+0146) end
		else force(intptr,+0174);
		load;
	      end;
	    gencst(op_bls,sz_int);
	  end;
#endif
	feof: asp:=boolptr;
	fabs:
	  if asp=intptr then m:=ABI else
	  if asp=longptr then m:=ABL else
	  if asp=realptr then m:=ABR else errasp(+0178);
	fsqr:
	  begin
	    if (asp=intptr) or (asp=longptr) then b:=op_mli else
	    if asp=realptr then begin b:=op_mlf; fltused:=true end
	    else errasp(+0179);
	    genasp(op_dup); genasp(b)
	  end;
	ford:
#ifdef  CHL
	    if formof(asp,[pointer]) then
		if sz_word=sz_addr then asp:=intptr
		else asp:=longptr
	    else begin
		asp:=intptr;
		if not nicescalar(asp) then errasp(+0180)
	    end;
#else
	    if not nicescalar(asp) then
	      errasp(+0180); asp:=intptr;
#endif
#ifdef  CHL
	fasptr: asp:=nilptr;
	fincptr:
	  begin
	  if not formof(asp,[pointer]) then errasp(+0174);
	  nextif(comma,+0146);
	  load;
	  expression(fsys);
	  force(intptr,+0174);
	  if ak=cst then
	    gencst(op_adp,pos.ad)
	  else
	    begin load; gencst(op_ads,sz_int) end;
	  asp := nilptr;
	  ak := loaded;
	  end;
	fgetstktop:
	  begin
	    load; gencst(op_ads,sz_int); gencst(op_loi,s);
	    asp:=lsp; ak:=loaded;
	  end;
#endif
	fchr: checkbnds(charptr);
	fpred, fsucc:
	  begin genop(b);
	    if nicescalar(asp) then genrck(asp) else errasp(+0181)
	  end;
	fodd:
	  begin gencst(op_loc,1); asp:=boolptr; genasp(op_and) end;
	ftrunc, fround: if asp<>realptr then errasp(+0182);
	fsin: m:=SINX;
	fcos: m:=COSX;
	fexp: m:=EXPX;
	fsqt: m:=SQT;
	flog: m:=LOG;
	fatn: m:=ATN;
	phalt:s:=0;
	pread, preadln, pwrite, pwriteln, pput, pget: ;
      end;
      if m<>CLS then
	begin gensp(m,s);
	  if lkey>=feof then genasp(op_lfr)
	end;
      if (lkey=fround) or (lkey=ftrunc) then
	opconvert(ri);
    end;
  if lpar then nextif(rparent,+0183);
end end;

{===================================================================}

procedure convert(fsp:sp; l1:integer);
{Convert tries to make the operands of some operator of the same type.
  The operand types are given by fsp and a.asp. The resulting type
  is put in a.asp.
  l1 gives the lino of the first instruction of the right operand.
}
var l2:integer; ts:twostruct; lsp:sp;
begin with a do begin asp:=desub(asp);
  ts:=compat(asp,fsp);
  case ts of
    eq,subeq:
      ;
    il,ir,lr:
      opconvert(ts);
    es:
      expandnullset(fsp);
    li,ri,rl,se:
      begin l2:=newmark; lsp:=asp; asp:=fsp;
	convert(lsp,l1); exchange(l1,l2); relmark(l2); asp:=lsp
      end;
    noteq:
      errasp(+0184);
  end;
  if asp=realptr then fltused:=true
end end;

procedure buildset(fsys:sos);
{This is a bad construct in pascal. Two objections:
  - expr..expr very difficult to implement on most machines
  - this construct makes it hard to implement sets of different size
}
const   ncsb    = 32;   {tunable}
type    byteset = set of 0..MB1;
var     i,j,val1,val2,ncst,l1,l2,sz:integer;
	cst1,cst2,cst12,varpart:boolean;
	cstpart:array[1..ncsb] of byteset;

procedure genconstset(sz:integer);
	{level 2: <<  buildset}
var i,j:integer;

function setcode(s:byteset):byte;
	{level 3: <<  buildset}
var b,i,w:byte;
begin i:=0; w:=0; b:=1;
  for i:=0 to MB1 do
    begin if i in s then w:=w+b; b:=b+b end;
  setcode := w;
end;

begin
  i:=sz;
  repeat
    genop(op_loc); j:=i; i:=i-sz_word;
    {the bytes of the next word to be loaded on the stack}
    {are in cstpart[i+1] .. cstpart[j]}
    if sz_word=2 then put1(sp_cst2)
    else put1(sp_cst4);
    for j:=i+1 to j do put1(setcode(cstpart[j]))
  until i = 0;
end;

procedure setexpr(fsys:sos; var c:boolean; var v:integer);
	{level 2: <<  buildset}
var min:integer; lsp:sp;
begin with a do begin c:=false; v:=0; lsp:=asp;
  expression(fsys); asp:=desub(asp);
  if not eqstruct(asp,lsp^.elset) then
    begin error(+0185); lsp:=nullset end;
  if lsp=nullset then
    begin
      if bounded(asp) then bounds(asp,min,sz) else
	if asp=intptr then sz:=iopt-1 else begin errasp(+0186); sz:=0 end;
      sz:=sz div NB1 + 1; while sz mod sz_word <> 0 do sz:=sz+1;
      if sz>sz_mset then errasp(+0187);
      lsp:=newsp(power,sz); lsp^.elset:=asp
    end;
  if asp<>nil then if ak=cst then
    if (pos.ad<0) or (pos.ad div NB1>=sizeof(lsp,wordmult)) then
      error(+0188)
    else if sz<=ncsb*sz_byte then
      begin c:=true; v:=pos.ad end;
  if not c then load; asp:=lsp
end end;

begin with a do begin  {buildset}
  varpart:=false; ncst:=0; asp:=nullset;
  for i:=1 to ncsb do cstpart[i]:=[];
  if find2([notsy..lparent],fsys,+0189) then
    repeat l1:=newmark;
      setexpr(fsys+[colon2,comma],cst1,val1); cst12:=cst1;
      if find3(colon2,fsys+[comma,notsy..lparent],+0190) then
	begin setexpr(fsys+[comma,notsy..lparent],cst2,val2);
	  cst12:=cst12 and cst2;
	  if not cst12 then
	    begin
	      if cst2 then gencst(op_loc,val2);
	      if cst1 then
		begin l2:=newmark; gencst(op_loc,val1); exchange(l1,l2);
		  relmark(l2);
		end;
	      l2:=newmark; genasp(op_zer); exchange(l1,l2);
	      relmark(l2);
	      genasp(op_loc); gensp(BTS,3*sz_word)
	    end;
	end
      else
	if cst12 then val2:=val1 else genasp(op_set);
      if cst12 then
	for i:=val1 to val2 do
	  begin j:=i div NB1 + 1; ncst:=ncst+1;
	    cstpart[j]:=cstpart[j] + [i mod NB1]
	  end
      else
	if varpart then genasp(op_ior) else varpart:=true;
      relmark(l1);
    until endofloop(fsys,[notsy..lparent],comma,+0191);  {+0192}
  ak:=loaded;
  if ncst>0 then
    begin
      genconstset(sizeof(asp,wordmult));
      if varpart then genasp(op_ior);
    end
  else
    if not varpart then genasp(op_zer);  {empty set}
end end;

procedure factor(fsys: sos);
var lip:ip; lsp:sp;
begin with a do begin
  asp:=nil; packbit:=false; ak:=loaded;
  if find1([notsy..nilcst,lparent],fsys,+0193) then
    case sy of
      ident:
	begin lip:=searchid([konst,vars,field,func,carrbnd]); insym;
	  case lip^.klass of
	    func: {call moves result to top stack}
	      begin call(fsys,lip); ak:=loaded; packbit:=false end;
	    konst:
	      begin asp:=lip^.idtype;
		if nicescalar(asp) then  {including asp=nil}
		  begin ak:=cst; pos.ad:=lip^.value end
		else
		  begin ak:=ploaded; laedlb(abs(lip^.value));
		    if asp^.form=scalar then
		      begin load; if lip^.value<0 then negate end
		    else
		      if asp=zeroptr then ak:=loaded
		  end
	      end;
	    field,vars:
	      selector(fsys,lip,[used]);
	    carrbnd:
	      begin lsp:=lip^.idtype; assert formof(lsp,[carray]);
		descraddr(lsp^.arpos); lsp:=lsp^.inxtype; asp:=desub(lsp);
		if lip^.next=nil then ak:=ploaded {low bound} else
		  begin gencst(op_loi,2*sz_int); genasp(op_adi) end;
		load; checkbnds(lsp);
	      end;
	  end  {case}
	end;
      intcst:
	begin asp:=intptr; ak:=cst; pos.ad:=val; insym end;
      realcst:
	begin asp:=realptr; ak:=ploaded; laedlb(val); insym end;
      longcst:
	begin asp:=longptr; ak:=ploaded; laedlb(val); insym end;
      charcst:
	begin asp:=charptr; ak:=cst; pos.ad:=val; insym end;
      stringcst:
	begin asp:=stringstruct; laedlb(val); insym;
	  if asp<>zeroptr then ak:=ploaded;
	end;
      nilcst:
	begin insym; asp:=nilptr; genasp(op_zer); end;
      lparent:
	begin insym; expression(fsys+[rparent]); nextif(rparent,+0194) end;
      notsy:
	begin insym; factor(fsys); load; genop(op_teq); asp:=desub(asp);
	  if asp<>boolptr then errasp(+0195)
	end;
      lbrack:
	begin insym; buildset(fsys+[rbrack]); nextif(rbrack,+0196) end;
    end
end end;

procedure term(fsys:sos);
var lsy:symbol; lsp:sp; l1:integer; first:boolean;
begin with a,b do begin first:=true;
  factor(fsys+[starsy..andsy]);
  while find2([starsy..andsy],fsys,+0197) do
    begin if first then begin load; first:=false end;
      lsy:=sy; insym; l1:=newmark; lsp:=asp;
      factor(fsys+[starsy..andsy]); load; convert(lsp,l1);
      if asp<>nil then
	case lsy of
	  starsy:
	    if (asp=intptr) or (asp=longptr) then genasp(op_mli) else
	    if asp=realptr then genasp(op_mlf) else
	    if asp^.form=power then genasp(op_and) else errasp(+0198);
	  slashsy:
	    begin
	      if (asp=intptr) or (asp=longptr) then
		begin lsp:=asp;
		  convert(realptr,l1);  {make real of right operand}
		  convert(lsp,l1);  {make real of left operand}
		end;
	      if asp=realptr then genasp(op_dvf) else errasp(+0199);
	    end;
	  divsy:
	    if (asp=intptr) or (asp=longptr) then genasp(op_dvi) else
	      errasp(+0200);
	  modsy:
	    begin
	      if asp=intptr then gensp(MDI,2*sz_int) else
	      if asp=longptr then gensp(MDL,2*sz_long) else errasp(+0201);
	      genasp(op_lfr);
	    end;
	  andsy:
	    if asp=boolptr then genasp(op_and) else errasp(+0202);
	end; {case}
	relmark(l1)
    end {while}
end end;

procedure simpleexpression(fsys:sos);
var lsy:symbol; lsp:sp; l1:integer; signed,min,first:boolean;
begin with a do begin first:=true;
  signed:=(sy=plussy) or (sy=minsy);
  if signed then begin min:=sy=minsy; insym end else min:=false;
  term(fsys + [minsy,plussy,orsy]); lsp:=desub(asp);
  if signed then
    if (lsp<>intptr) and (lsp<>realptr) and (lsp<>longptr) then
      errasp(+0203)
    else if min then
      begin load; first:=false; asp:=lsp; negate end;
  while find2([plussy,minsy,orsy],fsys,+0204) do
    begin if first then begin load; first:=false end;
      lsy:=sy; insym; l1:=newmark; lsp:=asp;
      term(fsys+[minsy,plussy,orsy]); load; convert(lsp,l1);
      if asp<>nil then
	case lsy of
	  plussy:
	    if (asp=intptr) or (asp=longptr) then genasp(op_adi) else
	    if asp=realptr then genasp(op_adf) else
	    if asp^.form=power then genasp(op_ior) else errasp(+0205);
	  minsy:
	    if (asp=intptr) or (asp=longptr) then genasp(op_sbi) else
	    if asp=realptr then genasp(op_sbf) else
	    if asp^.form=power then begin genasp(op_com); genasp(op_and) end
	    else errasp(+0206);
	  orsy:
	    if asp=boolptr then genasp(op_ior) else errasp(+0207);
	end; {case}
	relmark(l1)
    end {while}
end end;

procedure expression; { fsys:sos }
var lsy:symbol; lsp:sp; l1,l2:integer;
begin with a do begin l1:=newmark;
  simpleexpression(fsys+[eqsy..insy]);
  if find2([eqsy..insy],fsys,+0208) then
    begin lsy:=sy; insym; lsp:=asp; loadcheap; l2:=newmark;
      simpleexpression(fsys); loadcheap;
      if lsy=insy then
	begin
	  if not formof(asp,[power]) then errasp(+0209) else
	    if asp=nullset then genasp(op_and) else
		  {this effectively replaces the word on top of the
		   stack by the result of the 'in' operator: false }
	    if not (compat(lsp,asp^.elset) <= subeq) then errasp(+0210) else
	      begin exchange(l1,l2); genasp(op_inn) end
	end
      else
	begin convert(lsp,l2);
	  if asp<>nil then
	    case asp^.form of
	      scalar:
		if asp=realptr then genasp(op_cmf) else genasp(op_cmi);
	      pointer:
		if (lsy=eqsy) or (lsy=nesy) then genop(op_cmp) else
		  errasp(+0211);
	      power:
		case lsy of
		  eqsy,nesy: genasp(op_cms);
		  ltsy,gtsy: errasp(+0212);
		  lesy:  {'a<=b' equivalent to 'a-b=[]'}
		    begin genasp(op_com); genasp(op_and); genasp(op_zer);
		      genasp(op_cms); lsy:=eqsy
		    end;
		  gesy:  {'a>=b' equivalent to 'a=a+b'}
		    begin gencst(op_dup,2*sizeof(asp,wordmult));
		      genasp(op_asp); genasp(op_ior);
		      genasp(op_cms); lsy:=eqsy
		    end
		end;  {case}
	      arrays:
		if string(asp) then
		  begin gencst(op_loc,asp^.size);
		    gensp(BCP,2*sz_addr+sz_word);
		    gencst(op_lfr,sz_word)
		  end
		else errasp(+0213);
	      records: errasp(+0214);
	      files: errasp(+0215)
	    end;  { case }
	  case lsy of
	    ltsy: genop(op_tlt);
	    lesy: genop(op_tle);
	    gtsy: genop(op_tgt);
	    gesy: genop(op_tge);
	    nesy: genop(op_tne);
	    eqsy: genop(op_teq)
	  end
	end;
      relmark(l2);
      asp:=boolptr; ak:=loaded
    end;
    relmark(l1)
end end;

{===================================================================}

procedure statement(fsys:sos); forward;
		{this forward declaration can be avoided}

procedure assignment(fsys:sos; fip:ip);
var la:attr; l1,l2:integer;
begin
  l1:=newmark; selector(fsys+[becomes],fip,[assigned]); l2:=newmark;
  la:=a; nextif(becomes,+0216);
  expression(fsys); loadcheap; checkasp(la.asp,+0217);
  exchange(l1,l2); a:=la;
  relmark(l1); relmark(l2);
  if not formof(la.asp,[arrays..records]) then store else
    begin loadaddr;
      if la.asp^.form<>carray then genasp(op_blm) else
	begin descraddr(la.asp^.arpos); gensp(ASZ,sz_addr);
	  gencst(op_lfr,sz_word); gencst(op_bls,sz_word)
	end;
    end;
end;

procedure gotostatement;
{jumps into structured statements can give strange results. }
label 1;
var llp:lp; lbp:bp; diff:integer;
begin
  if sy<>intcst then error(+0218) else
    begin llp:=searchlab(b.lchain,val);
      if llp<>nil then gencst(op_bra,llp^.labname) else
	begin lbp:=b.nextbp; diff:=1;
	  while lbp<>nil do
	    begin llp:=searchlab(lbp^.lchain,val);
	      if llp<>nil then goto 1;
	      lbp:=lbp^.nextbp; diff:=diff+1;
	    end;
1:        if llp=nil then errint(+0219,val) else
	    begin
	      if llp^.labdlb=0 then
		begin dlbno:=dlbno+1; llp^.labdlb:=dlbno;
		  genop(ps_ina); argdlb(dlbno);  {forward data reference}
		end;
	      laedlb(llp^.labdlb);
	      if diff=level-1 then gencst(op_zer,sz_addr) else
		gencst(op_lxl,diff);
	      gensp(GTO,2*sz_addr);
	    end;
	end;
      insym;
    end
end;

procedure compoundstatement(fsys:sos; err:integer);
begin
  repeat statement(fsys+[semicolon])
  until endofloop(fsys,[beginsy..casesy],semicolon,err)
end;

procedure ifstatement(fsys:sos);
var lb1,lb2:integer;
begin with b do begin
  expression(fsys+[thensy,elsesy]);
  force(boolptr,+0220); ilbno:=ilbno+1; lb1:=ilbno; gencst(op_zeq,lb1);
  nextif(thensy,+0221); statement(fsys+[elsesy]);
  if find3(elsesy,fsys,+0222) then
    begin ilbno:=ilbno+1; lb2:=ilbno; gencst(op_bra,lb2);
      newilb(lb1); statement(fsys); newilb(lb2)
    end
  else newilb(lb1);
end end;

procedure casestatement(fsys:sos);
label 1;
type cip=^caseinfo;
     caseinfo=record
	next: cip;
	csstart: integer;
	cslab: integer
     end;
var lsp:sp; head,p,q,r:cip; l0,l1:integer;
    ilb1,ilb2,dlb,i,n,m,min,max:integer;
begin with b do begin
  expression(fsys+[ofsy,semicolon,ident..plussy]); lsp:=a.asp; load;
  if not nicescalar(desub(lsp)) then begin error(+0223); lsp:=nil end;
  l0:=newmark; ilbno:=ilbno+1; ilb1:=ilbno;
  nextif(ofsy,+0224); head:=nil; max:=-MI; min:=MI; n:=0;
  repeat ilbno:=ilbno+1; ilb2:=ilbno;   {label of current case}
    repeat i:=cstinteger(fsys+[comma,colon1,semicolon],lsp,+0225);
      if i>max then max:=i; if i<min then min:=i; n:=n+1;
      q:=head; r:=nil; new(p);
      while q<>nil do
	begin  {chain all cases in ascending order}
	  if q^.cslab>=i then
	    begin if q^.cslab=i then error(+0226); goto 1 end;
	  r:=q; q:=q^.next
	end;
1:    p^.next:=q; p^.cslab:=i; p^.csstart:=ilb2;
      if r=nil then head:=p else r^.next:=p;
    until endofloop(fsys+[colon1,semicolon],[ident..plussy],comma,+0227);
    {+0228}
    nextif(colon1,+0229); newilb(ilb2); statement(fsys+[semicolon]);
    gencst(op_bra,ilb1);
  until lastsemicolon(fsys,[ident..plussy],+0230);  {+0231 +0232}
  assert n<>0; newilb(ilb1); l1:=newmark;
  dlb:=newdlb; genop(ps_rom); argnil;
  if (max div 3) - (min div 3) < n then
    begin argcst(min); argcst(max-min);
      m:=op_csa;
      while head<>nil do
	begin
	  while head^.cslab>min do
	    begin argnil; min:=min+1 end;
	  argilb(head^.csstart); min:=min+1; head:=head^.next
	end;
    end
  else
    begin argcst(n); m:=op_csb;
      while head<>nil do
	begin argcst(head^.cslab);argilb(head^.csstart);head:=head^.next end;
    end;
  argend; laedlb(dlb); gencst(m,sz_word); exchange(l0,l1);
  relmark(l0); relmark(l1)
end end;

procedure repeatstatement(fsys:sos);
var lb1: integer;
begin with b do begin
  ilbno:=ilbno+1; lb1:=ilbno; newilb(lb1);
  compoundstatement(fsys+[untilsy],+0233);  {+0234}
  nextif(untilsy,+0235); genlin;
  expression(fsys); force(boolptr,+0236); gencst(op_zeq,lb1);
end end;

procedure whilestatement(fsys:sos);
var lb1,lb2: integer;
begin with b do begin
  ilbno:=ilbno+1; lb1:=ilbno; newilb(lb1);
  ilbno:=ilbno+1; lb2:=ilbno;
  genlin; expression(fsys+[dosy]);
  force(boolptr,+0237); gencst(op_zeq,lb2);
  nextif(dosy,+0238); statement(fsys);
  gencst(op_bra,lb1); newilb(lb2)
end end;

procedure forstatement(fsys:sos);
var lip:ip; tosym:boolean; endlab,looplab(* ,savlb *):integer;
    av,at1,at2:attr; lsp:sp;

procedure forbound(fsys:sos; var fa:attr; fsp:sp);
begin
  expression(fsys); fa:=a; force(fsp,+0239);
  if fa.ak<>cst then
    begin temporary(fsp,reg_any);
      genasp(op_dup); fa:=a; store
    end
end;

begin with b do begin (* savlb:=reglb; *) tosym:=false;
  ilbno:=ilbno+1; looplab:=ilbno; ilbno:=ilbno+1; endlab:=ilbno;
  inita(nil,0);
  if sy<>ident then error(+0240) else
    begin lip:=searchid([vars]); insym;
      a.asp:=lip^.idtype; a.pos:=lip^.vpos;
      lip^.iflag:=lip^.iflag+[used,assigned,loopvar];
      if level>1 then
	if (a.pos.ad>=0) or (a.pos.lv<>level) then
	  error(+0241);
    end;
  lsp:=desub(a.asp);
  if not nicescalar(lsp) then begin errasp(+0242); lsp:=nil end;
  av:=a; nextif(becomes,+0243);
  forbound(fsys+[tosy,downtosy,notsy..lparent,dosy],at1,lsp);
  if find1([tosy,downtosy],fsys+[notsy..lparent,dosy],+0244) then
    begin tosym:=sy=tosy; insym end;
  forbound(fsys+[dosy],at2,lsp);
  if tosym then gencst(op_bgt,endlab) else gencst(op_blt,endlab);
  a:=at1; force(av.asp,+0245); a:=av; store; newilb(looplab);
  nextif(dosy,+0246); statement(fsys);
  a:=av; load; a:=at2; load; gencst(op_beq,endlab);
  a:=av; load; if tosym then genop(op_inc) else genop(op_dec);
  a.asp:=lsp; checkbnds(av.asp); a:=av; store;
  gencst(op_bra,looplab); newilb(endlab);
  (* reglb:=savlb *)
end end;

procedure withstatement(fsys:sos);
var lnp,savtop:np; (* savlb:integer; *) pbit:boolean;
begin with b do begin
  (* savlb:=reglb;*) savtop:=top;
  repeat variable(fsys+[comma,dosy]);
    if not formof(a.asp,[records]) then errasp(+0247) else
      begin pbit:=spack in a.asp^.sflag;
	new(lnp,wrec); lnp^.occur:=wrec; lnp^.fname:=a.asp^.fstfld;
	if a.ak<>fixed then
	  begin loadaddr; temporary(nilptr,reg_pointer); store;
	    a.ak:=pfixed;
	  end;
	a.packbit:=pbit; lnp^.wa:=a; lnp^.nlink:=top; top:=lnp;
      end;
  until endofloop(fsys+[dosy],[ident],comma,+0248);  {+0249}
  nextif(dosy,+0250); statement(fsys);
  top:=savtop; (* reglb:=savlb; *)
end end;

procedure assertion(fsys:sos);
begin teststandard;
  if opt['a']=off then
    while not (sy in fsys) do insym
  else
    begin expression(fsys); force(boolptr,+0251);
      gencst(op_loc,srcorig); gensp(ASS,2*sz_word);
    end
end;

procedure statement; {fsys: sos}
var lip:ip; llp:lp; lsy:symbol;
begin
  assert [labelsy..casesy,endsy] <= fsys;
  assert [ident,intcst] * fsys = [];
  if find2([intcst],fsys+[ident],+0252) then
    begin llp:=searchlab(b.lchain,val);
      if llp=nil then errint(+0253,val) else
	begin if llp^.seen then errint(+0254,val) else llp^.seen:=true;
	  newilb(llp^.labname)
	end;
      insym; nextif(colon1,+0255);
    end;
  if find2([ident,beginsy..casesy],fsys,+0256) then
    begin if giveline then if sy<>whilesy then genlin;
      if sy=ident then
	if id='assert  ' then
	  begin insym; assertion(fsys) end
	else
	  begin lip:=searchid([vars,field,func,proc]); insym;
	    if lip^.klass=proc then call(fsys,lip) else assignment(fsys,lip)
	  end
      else
	begin lsy:=sy; insym;
	  case lsy of
	    beginsy:
	      begin compoundstatement(fsys,+0257);  {+0258}
		nextif(endsy,+0259)
	      end;
	    gotosy:
	      gotostatement;
	    ifsy:
	      ifstatement(fsys);
	    casesy:
	      begin casestatement(fsys); nextif(endsy,+0260) end;
	    whilesy:
	      whilestatement(fsys);
	    repeatsy:
	      repeatstatement(fsys);
	    forsy:
	      forstatement(fsys);
	    withsy:
	      withstatement(fsys);
	  end
	end;
    end
end;

{===================================================================}

procedure body(fsys:sos; fip:ip);
var i,dlb,l0,l1,ssp:integer; llp:lp; spset:boolean;
begin with b do begin
{produce PRO}
  genpnam(ps_pro,fip); argend;
  gencst(ps_mes,ms_par);argcst(fip^.maxlb); argend;
  l0:=newmark; dlb:=0; trace('procentr',fip,dlb);
#ifdef  CHL
  if b.tip<>nil then
    gencst(op_zrl,-sz_word); {ensure A68BIT=0}
#endif
{global labels}
  llp:=lchain; spset:=false;
  while llp<>nil do
    begin
      if llp^.labdlb<>0 then
	begin
	  if not spset then
	    begin spset:=true;
	      gencst(ps_mes,ms_gto); argend;
	      temporary(nilptr,-1); ssp:=a.pos.ad;
	      gencst(op_lor,1); store
	    end;
	  argdlb(llp^.labdlb); lino:=lino+1; genop(ps_rom);
	  argilb(llp^.labname); argcst(ssp); argend;
#ifdef CHL
	  gengotonam(ps_exa,fip,llp^.labname);
	  genop(ps_rom);argcst(0);argend;
#endif
	end;
      llp:=llp^.nextlp
    end;
{the body itself}
  currproc:=fip;
  compoundstatement(fsys,+0261);  {+0262}
  trace('procexit',fip,dlb);
{undefined labels}
  llp:=lchain;
  while llp<>nil do
    begin if not llp^.seen then errint(+0263,llp^.labval);
      llp:=llp^.nextlp
    end;
{finish and close files}
  treewalk(top^.fname);
  if level=1 then
    begin l1:=newmark;
      genop(op_fil); argdlb(fildlb);  {temporarily}
      dlb:=newdlb; gencst(ps_con,argc+1);
      for i:=0 to argc do with argv[i] do
	begin argcst(ad);
	  if (ad=-1) and (i>1) then errid(+0264,name)
	end;
      argend; gencst(op_lxl,0); laedlb(dlb); genglb(op_lae,0);
      gencst(op_lxa,0); gensp(INI,4*sz_addr);
      exchange(l0,l1); relmark(l1); gencst(op_loc,0); gensp(HLT,0)
    end
  else
    begin inita(fip^.idtype,fip^.pfpos.ad);
      if fip^.klass=func then
	begin load;
	  if not (assigned in fip^.iflag) then
	    errid(-(+0265),fip^.name)
	end;
      genasp(op_ret)
    end;
  relmark(l0);
  gencst(ps_end,-minlb)
end end;

{===================================================================}

procedure block;  {forward declared}
begin with b do begin
  assert [labelsy..withsy] <= fsys;
  assert [ident,intcst,casesy,endsy,period] * fsys = [];
  if find3(labelsy,fsys,+0266) then labeldeclaration(fsys);
  if find3(constsy,fsys,+0267) then constdefinition(fsys);
  if find3(typesy,fsys,+0268) then typedefinition(fsys);
  if find3(varsy,fsys,+0269) then vardeclaration(fsys);
#ifndef CHL 
  if fip=progp then
    begin
      if iop[true]<>nil then
	begin
	  argv[1].ad:=posaddr(holeb,textptr,false);
	  iop[true]^.vpos.ad:=argv[1].ad
	end;
      if iop[false]<>nil then
	begin
	  argv[0].ad:=posaddr(holeb,textptr,false);
	  iop[false]^.vpos.ad:=argv[0].ad
	end;
      genhol; genpnam(ps_exp,fip);
    end;  {externals are also extern for the main body}
#else
  if not(ioaddressfixed) then {fixup adresses, only once}
  begin
    argv[1].ad:=posaddr(holeb,textptr,false);
    iop[true]^.vpos.ad:=argv[1].ad;
    argv[0].ad:=posaddr(holeb,textptr,false);
    iop[false]^.vpos.ad:=argv[0].ad;
    ioaddressfixed:=true;
  end;
  if fip=progp then begin genhol; genpnam(ps_exp,fip) end;
#endif
  fip^.pfpos.ad:=negaddr(fip^.idtype);  {function result area}
  while find2([procsy,funcsy],fsys,+0270) do pfdeclaration(fsys);
  if forwcount<>0 then error(+0271);  {forw proc not specified}
  nextif(beginsy,+0272);
  body(fsys+[casesy,endsy],fip);
  nextif(endsy,+0273);
end end;

{===================================================================}

procedure programme(fsys:sos);
var stdin,stdout:boolean; p:ip;
begin
  nextif(progsy,+0274); nextif(ident,+0275);
  if find3(lparent,fsys+[semicolon],+0276) then
    begin
      repeat
	if sy<>ident then error(+0277) else
	  begin stdin:=id='input   '; stdout:=id='output  ';
	    if stdin or stdout then
	      begin 
#ifndef CHL
		p:=newip(vars,id,textptr,nil);
		enterid(p); iop[stdout]:=p;
#endif
	      end
	    else
	      if argc<maxargc then
		begin
		  argc:=argc+1; argv[argc].name:=id; argv[argc].ad:=-1
		end;
	    insym
	  end
      until endofloop(fsys+[rparent,semicolon],[ident],comma,+0278);  {+0279}
      if argc>maxargc then
	begin error(+0280); argc:=maxargc end;
      nextif(rparent,+0281);
    end;
  nextif(semicolon,+0282);
  block(fsys,progp);
  if opt['l']<>off then
    begin gencst(ps_mes,ms_src); argcst(srcorig); argend end;
  eofexpected:=true; nextif(period,+0283);
end;

procedure compile;
var lsys:sos;
begin lsys:=[progsy,labelsy..withsy];
  repeat eofexpected:=false;
    main:=find2([progsy,labelsy,beginsy..withsy],lsys,+0284);
#ifdef CHL
	iop[false]:=newip(vars,'input   ',textptr,nil);
	enterid(iop[false]);
	iop[true]:=newip(vars,'output  ',textptr,nil);
	enterid(iop[true]);
#endif
    if main then programme(lsys) else
      begin
	if find3(constsy,lsys,+0285) then constdefinition(lsys);
	if find3(typesy,lsys,+0286) then typedefinition(lsys);
	if find3(varsy,lsys,+0287) then vardeclaration(lsys);
#ifdef CHL
        argv[1].ad:=posaddr(holeb,textptr,false);
        iop[true]^.vpos.ad:=argv[1].ad;
        argv[0].ad:=posaddr(holeb,textptr,false);
        iop[false]^.vpos.ad:=argv[0].ad;
	ioaddressfixed:=true;
#endif
	genhol;
	while find2([procsy,funcsy],lsys,+0288) do pfdeclaration(lsys);
      end;
    error(+0289);
  until false;  { the only way out is the halt in nextln on eof }
end;

{===================================================================}

procedure init1;
var c:char;
begin
{reserved words}
  rw[ 0]:='if      ';   rw[ 1]:='do      ';     rw[ 2]:='of      ';
  rw[ 3]:='to      ';   rw[ 4]:='in      ';     rw[ 5]:='or      ';
  rw[ 6]:='end     ';   rw[ 7]:='for     ';     rw[ 8]:='nil     ';
  rw[ 9]:='var     ';   rw[10]:='div     ';     rw[11]:='mod     ';
  rw[12]:='set     ';   rw[13]:='and     ';     rw[14]:='not     ';
  rw[15]:='then    ';   rw[16]:='else    ';     rw[17]:='with    ';
  rw[18]:='case    ';   rw[19]:='type    ';     rw[20]:='goto    ';
  rw[21]:='file    ';   rw[22]:='begin   ';     rw[23]:='until   ';
  rw[24]:='while   ';   rw[25]:='array   ';     rw[26]:='const   ';
  rw[27]:='label   ';   rw[28]:='repeat  ';     rw[29]:='record  ';
  rw[30]:='downto  ';   rw[31]:='packed  ';     rw[32]:='program ';
  rw[33]:='function';   rw[34]:='procedur';
{corresponding symbols}
  rsy[ 0]:=ifsy;        rsy[ 1]:=dosy;          rsy[ 2]:=ofsy;
  rsy[ 3]:=tosy;        rsy[ 4]:=insy;          rsy[ 5]:=orsy;
  rsy[ 6]:=endsy;       rsy[ 7]:=forsy;         rsy[ 8]:=nilcst;
  rsy[ 9]:=varsy;       rsy[10]:=divsy;         rsy[11]:=modsy;
  rsy[12]:=setsy;       rsy[13]:=andsy;         rsy[14]:=notsy;
  rsy[15]:=thensy;      rsy[16]:=elsesy;        rsy[17]:=withsy;
  rsy[18]:=casesy;      rsy[19]:=typesy;        rsy[20]:=gotosy;
  rsy[21]:=filesy;      rsy[22]:=beginsy;       rsy[23]:=untilsy;
  rsy[24]:=whilesy;     rsy[25]:=arraysy;       rsy[26]:=constsy;
  rsy[27]:=labelsy;     rsy[28]:=repeatsy;      rsy[29]:=recordsy;
  rsy[30]:=downtosy;    rsy[31]:=packedsy;      rsy[32]:=progsy;
  rsy[33]:=funcsy;      rsy[34]:=procsy;
{indices into rw to find reserved words fast}
  frw[0]:= 0; frw[1]:= 0; frw[2]:= 6; frw[3]:=15; frw[4]:=22;
  frw[5]:=28; frw[6]:=32; frw[7]:=33; frw[8]:=35;
{char types}
  for c:=chr(0) to chr(maxcharord) do cs[c]:=others;
  for c:='0' to '9' do cs[c]:=digit;
  for c:='A' to 'Z' do cs[c]:=upper;
  for c:='a' to 'z' do cs[c]:=lower;
  cs[chr(ascnl)]:=layout;
  cs[chr(ascvt)]:=layout;
  cs[chr(ascff)]:=layout;
  cs[chr(asccr)]:=layout;
{characters with corresponding chartype in ASCII order}
  cs[chr(ascht)]:=tabch;
  cs[' ']:=layout;      cs['"']:=dquotech;      cs['''']:=quotech;
  cs['(']:=lparentch;   cs[')']:=rparentch;     cs['*']:=star;
  cs['+']:=plusch;      cs[',']:=commach;       cs['-']:=minch;
  cs['.']:=periodch;    cs['/']:=slash;         cs[':']:=colonch;
  cs[';']:=semich;      cs['<']:=lessch;        cs['=']:=equal;
  cs['>']:=greaterch;   cs['[']:=lbrackch;      cs[']']:=rbrackch;
  cs['^']:=arrowch;     cs['{']:=lbracech;
{single character symbols in chartype order}
  csy[rparentch]:=rparent;      csy[lbrackch]:=lbrack;
  csy[rbrackch]:=rbrack;        csy[commach]:=comma;
  csy[semich]:=semicolon;       csy[arrowch]:=arrow;
  csy[plusch]:=plussy;          csy[minch]:=minsy;
  csy[slash]:=slashsy;          csy[star]:=starsy;
  csy[equal]:=eqsy;
{pascal library mnemonics}
  lmn[ELN ]:='_eln';    lmn[EFL ]:='_efl';      lmn[CLS ]:='_cls';
  lmn[WDW ]:='_wdw';
  lmn[OPN ]:='_opn';    lmn[GETX]:='_get';      lmn[RDI ]:='_rdi';
  lmn[RDC ]:='_rdc';    lmn[RDR ]:='_rdr';      lmn[RDL ]:='_rdl';
  lmn[RLN ]:='_rln';
  lmn[CRE ]:='_cre';    lmn[PUTX]:='_put';      lmn[WRI ]:='_wri';
  lmn[WSI ]:='_wsi';    lmn[WRC ]:='_wrc';      lmn[WSC ]:='_wsc';
  lmn[WRS ]:='_wrs';    lmn[WSS ]:='_wss';      lmn[WRB ]:='_wrb';
  lmn[WSB ]:='_wsb';    lmn[WRR ]:='_wrr';      lmn[WSR ]:='_wsr';
  lmn[WRL ]:='_wrl';    lmn[WSL ]:='_wsl';
  lmn[WRF ]:='_wrf';    lmn[WRZ ]:='_wrz';      lmn[WSZ ]:='_wsz';
  lmn[WLN ]:='_wln';    lmn[PAG ]:='_pag';
  lmn[ABR ]:='_abr';    lmn[RND ]:='_rnd';      lmn[SINX]:='_sin';
  lmn[COSX]:='_cos';    lmn[EXPX]:='_exp';      lmn[SQT ]:='_sqt';
  lmn[LOG ]:='_log';    lmn[ATN ]:='_atn';      lmn[ABI ]:='_abi';
  lmn[ABL ]:='_abl';
  lmn[BCP ]:='_bcp';    lmn[BTS ]:='_bts';      lmn[NEWX]:='_new';
  lmn[SAV ]:='_sav';    lmn[RST ]:='_rst';      lmn[INI ]:='_ini';
  lmn[HLT ]:='_hlt';    lmn[ASS ]:='_ass';      lmn[GTO ]:='_gto';
  lmn[PAC ]:='_pac';    lmn[UNP ]:='_unp';      lmn[DIS ]:='_dis';
  lmn[ASZ ]:='_asz';    lmn[MDI ]:='_mdi';      lmn[MDL ]:='_mdl';
{scalar variables}
  b.nextbp:=nil;
  b.reglb:=0;
  b.minlb:=0;
  b.ilbno:=0;
  b.forwcount:=0;
  b.lchain:=nil;
#ifdef  CHL
  b.tip:=nil;
#endif
  srcchno:=0;
  srclino:=1;
  srcorig:=1;
  lino:=0;
  dlbno:=0;
#ifdef CHL
  dlbno:=dlbno+1;
  glbdata:=dlbno;
  holdone := true; {until inits done}
#endif
  holeb:=0;
  argc:=1;
  lastpfno:=0;
  giveline:=true;
  including:=false;
  eofexpected:=false;
  intypedec:=false;
  fltused:=false;
  seconddot:=false;
#ifdef CHL
  ioaddressfixed:=false;
#endif
  iop[false]:=nil;
  iop[true]:=nil;
  argv[0].ad:=-1;
  argv[1].ad:=-1;
#ifdef NO_EXC
  ohead := nil;
  bcnt := 0;
  mhead := nil;
#endif NO_EXC
end;

procedure init2;
var p:ip; k:idclass; j:standpf;
    pfn:array[standpf] of idarr;
begin
{initialize the first name space}
  new(top,blck); top^.occur:=blck; top^.nlink:=nil; top^.fname:=nil;
  level:=0;
{undefined identifier pointers used by searchid}
  for k:=types to func do
    undefip[k]:=newip(k,spaces,nil,nil);
{names of standard procedures/functions}
  pfn[pread     ]:='read    ';  pfn[preadln     ]:='readln  ';
  pfn[pwrite    ]:='write   ';  pfn[pwriteln    ]:='writeln ';
  pfn[pput      ]:='put     ';  pfn[pget        ]:='get     ';
  pfn[ppage     ]:='page    ';  pfn[preset      ]:='reset   ';
  pfn[prewrite  ]:='rewrite ';  pfn[pnew        ]:='new     ';
  pfn[pdispose  ]:='dispose ';  pfn[ppack       ]:='pack    ';
  pfn[punpack   ]:='unpack  ';  pfn[pmark       ]:='mark    ';
  pfn[prelease  ]:='release ';  pfn[phalt       ]:='halt    ';
#ifdef  CHL
  pfn[penew     ]:='enew    ';  pfn[pedispose   ]:='edispose';
  pfn[psetstktop]:='setstkto';  pfn[pmoveleft   ]:='moveleft';
  pfn[fgetstktop]:='getstkto';  pfn[fasptr      ]:='asptr   ';
  pfn[fincptr   ]:='incptr  ';
#endif
  pfn[feof      ]:='eof     ';  pfn[feoln       ]:='eoln    ';
  pfn[fabs      ]:='abs     ';  pfn[fsqr        ]:='sqr     ';
  pfn[ford      ]:='ord     ';  pfn[fchr        ]:='chr     ';
  pfn[fpred     ]:='pred    ';  pfn[fsucc       ]:='succ    ';
  pfn[fodd      ]:='odd     ';  pfn[ftrunc      ]:='trunc   ';
  pfn[fround    ]:='round   ';  pfn[fsin        ]:='sin     ';
  pfn[fcos      ]:='cos     ';  pfn[fexp        ]:='exp     ';
  pfn[fsqt      ]:='sqrt    ';  pfn[flog        ]:='ln      ';
  pfn[fatn      ]:='arctan  ';
{standard procedure/function identifiers}
  for j:=pread to phalt do
    begin new(p,proc,standard); p^.klass:=proc;
      p^.name:=pfn[j]; p^.pfkind:=standard; p^.key:=j; enterid(p);
    end;
  for j:=feof to fatn do
    begin new(p,func,standard); p^.klass:=func; p^.idtype:=nil;
      p^.name:=pfn[j]; p^.pfkind:=standard; p^.key:=j; enterid(p);
    end;
{program identifier}
  progp:=newip(proc,'m_a_i_n ',nil,nil);
end;

procedure init3;
var n:np; p,q:ip; i:integer; c:char;
#if EM_WSIZE == 2
    is:packed array[1..imax] of char;
#endif
begin
  for i:=0 to sz_last do readln(errors,sizes[i]);
  if sz_int  = 2 then maxintstring  := max2bytes
  else		      maxintstring  := max4bytes;
  if sz_long = 2 then maxlongstring := max2bytes
  else		      maxlongstring := max4bytes;
  gencst(ps_mes,ms_emx); argcst(sz_word); argcst(sz_addr); argend;
  ix:=1;
  while not eoln(errors) do
    begin read(errors,c);
      if ix<smax then begin strbuf[ix]:=c; ix:=ix+1 end
    end;
  readln(errors); strbuf[ix]:=chr(0);
  for i:=1 to fnmax do
    if i<ix then source[i]:=strbuf[i] else source[i]:=' ';
  fildlb:=romstr(sp_scon,0);
{standard type pointers}
  intptr :=newsp(scalar,sz_int);
  realptr:=newsp(scalar,sz_real);
  longptr:=newsp(scalar,sz_long);
  charptr:=newsp(scalar,sz_char);
  boolptr:=newsp(scalar,sz_bool);
  nilptr :=newsp(pointer,sz_addr);
  zeroptr:=newsp(pointer,sz_addr);
  procptr:=newsp(records,sz_proc);
  nullset:=newsp(power,sz_word); nullset^.elset:=nil;
  textptr:=newsp(files,sz_head+sz_buff); textptr^.filtype:=charptr;
{standard type names}
  enterid(newip(types,'integer ',intptr,nil));
  enterid(newip(types,'real    ',realptr,nil));
  enterid(newip(types,'char    ',charptr,nil));
  enterid(newip(types,'boolean ',boolptr,nil));
  enterid(newip(types,'text    ',textptr,nil));
{standard constant names}
  q:=nil; p:=newip(konst,'false   ',boolptr,q); enterid(p);
  q:=p; p:=newip(konst,'true    ',boolptr,q); p^.value:=1; enterid(p);
  boolptr^.fconst:=p;
{maxint of the target machine}
  p:=newip(konst,'maxint  ',intptr,nil);
  if sz_int = 2 then p^.value:=MI2
  else 
#if EM_WSIZE == 4
	p^.value := MI;
#else
  {EM_WSIZE = 2, sz_int = 4}
    begin p^.idtype:=longptr; ix:=imax; is:=max4bytes;
      for i:=1 to ix do strbuf[i]:=is[i];
      p^.value:=romstr(sp_icon,sz_int);
    end;
#endif
  enterid(p);
  p:=newip(konst,spaces,charptr,nil); p^.value:=maxcharord;
  charptr^.fconst:=p;
{new name space for user externals}
  new(n,blck); n^.occur:=blck; n^.nlink:=top; n^.fname:=nil; top:=n;
{options}
  for c:='a' to 'z' do begin opt[c]:=0; forceopt[c]:=false end;
  opt['a']:=on;
  opt['i']:=NB1*sz_iset;
  opt['l']:=on;
  opt['o']:=on;
  opt['r']:=on;
#ifdef CHL
  opt['g']:=on;
  opt['w']:=on;
  holdone := false;
#endif
  sopt:=off;
end;

procedure init4;
begin
  copt:=opt['c'];
  dopt:=opt['d']; if EM_WSIZE < sz_int then dopt:=on;
  iopt:=opt['i'];
  sopt:=opt['s'];
  if sopt<>off then begin copt:=off; dopt:=off end
  else if opt['u']<>off then cs['_']:=lower;
  if copt<>off then enterid(newip(types,'string  ',zeroptr,nil));
  if dopt<>off then enterid(newip(types,'long    ',longptr,nil));
  if opt['o']=off then begin gencst(ps_mes,ms_opt); argend end;
  if dopt<>off then fltused:=true;  {temporary kludge}
end;

begin  {main body of pcompiler}
  init1;  {initialize tables and scalars}
  init2;  {initialize heap objects}
  rewrite(em); put2(sp_magic); reset(errors);
  init3;  {size dependent initialization}
  while not eof(errors) do
    begin options(false); readln(errors) end;
  rewrite(errors);
  if not eof(input) then
    begin nextch; insym;
      init4;  {option dependent initialization}
      compile
    end;
#ifdef STANDARD
9999: ;
#endif
end.  {pcompiler}
