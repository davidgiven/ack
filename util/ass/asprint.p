#
{$d+}
program asprint(prog,output);

const

  { header words }
  NTEXT   = 1;
  NDATA   = 2;
  NPROC   = 3;
  ENTRY   = 4;
  NLINE   = 5;
  SZDATA  = 6;

  escape1 = 254;        { escape to secondary opcodes }
  escape2 = 255;        { escape to tertiary opcodes }

type
  byte=   0..255;           { memory is an array of bytes }
  adr=    {0..maxadr} long; { the range of addresses }
  word=   {0..maxuint} long;{ the range of unsigned integers }
  size=   0..32766;         { the range of sizes is the positive offsets }
  sword= {-signbit..maxsint} long; { the range of signed integers }
  full=  {-maxuint..maxuint} long; { intermediate results need this range }
  double={-maxdbl..maxdbl} long;   { double precision range }
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
  { variables indicating the size of words and addresses }
  wsize: integer;	{ number of bytes in a word }
  asize: integer;	{ number of bytes in an address }
  pdsize: integer;	{ size of procedure descriptor in bytes = 2*asize }

  pc,lb,sp,hp,pd: adr;  { internal machine registers }
  i: integer;           { integer scratch variable }
  s,t :word;            { scratch variables }
  sz:size;              { scratch variables }
  ss,st: sword;         { scratch variables }
  k :double;            { scratch variables }
  j:size;               { scratch variable used as index }
  a,b:adr;              { scratch variable used for addresses }
  dt,ds:double;         { scratch variables for double precision }
  found:boolean;        { scratch }
  opcode: byte;
  iclass: insclass;
  dispat: array[insclass, byte] of dispatch ;
  insr: mnem;           { holds the instructionnumber }
  header: array[1..8] of adr;

  prog: file of byte;   { program and initialized data }

procedure getit;  { start the ball rolling }
var cset:set of char;
    f:ifset;
    insno:byte;
    iclass: insclass;
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
    begin 
	if eof(prog) then
	begin writeln('Premature EOF on EM load file') ; halt end;
	read(prog,b); if n>1 then readb:=readb(n-1)*256+b else readb:=b
    end;

    function readbyte:byte;
    begin readbyte:=readb(1) end;

    procedure skipbyte;
    var dummy: byte;
    begin dummy:=readb(1) end;

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
	1: writeln('Test flags: ',hdr);
	2: if hdr<>0 then
	   begin writeln('Unsolved references: ',hdr) end;
	3: if hdr<>3 then
	   begin writeln('Incorrect load file version') end;
	4: wsize:=hdr ; 
	5: begin asize:=hdr ; pdsize:= asize+asize end;
	6,7:
	   if hdr<>0 then
	   begin writeln('First header entry ',i,', is ',hdr) end;
	end
      end;
      writeln('word size',wsize,', pointer size',asize)
    end;

    procedure noinit;
    begin writeln('Illegal initialization'); halt end;

    procedure readint(a:adr;s:size);
    const mrange = 4;
    var i:size;
        val:double;
        cont: array[1..mrange] of byte;
    begin { construct integer out of byte sequence }
      if s<=mrange then
      begin
          for i:=1 to s do cont[i]:=readbyte ;
	  if cont[s]>=128 then val:=cont[s]-256 else val:=cont[s];
	  for i:= s-1 downto 1 do val:= val*256 + cont[i];
	  writeln(', value ',val)
      end
      else
      begin
	  write(', bytes(little endian) ');
	  for i:=1 to s do write(readbyte:4) ;
	  writeln
      end
    end;

    procedure readuns(a:adr;s:size);
    const mrange=3;
    var i:size;
        val:double;
        cont: array[1..mrange] of byte;
    begin { construct unsigned integer out of byte sequence }
      if s<=mrange then
      begin
          for i:=1 to s do cont[i]:=readbyte ;
	  val:=0;
	  for i:= s downto 1 do val:= val*256 + cont[i];
	  writeln(', value ',val)
      end
      else
      begin
	  write(', bytes(little endian) ');
	  for i:=1 to s do write(readbyte:4) ;
	  writeln
      end
    end;

    procedure readfloat(a:adr;s:size);
    var i:size; b:byte;
    begin { construct float out of string}
      i:=0;
      repeat { eat the bytes, construct the value and intialize at a }
	write(chr(readbyte)); i:=i+1;
      until b=0 ;
    end;

begin

#ifdef INSRT
  { initialize tables }
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

#endif
  { read in program text, data and procedure descriptors }
  reset(prog);
  readhdr;                               { verify first header }
  for i:=1 to 8 do header[i]:=readadr;  { read second header }
  writeln('textsize ',header[NTEXT],', datasize ',header[SZDATA]);
  writeln('data descriptors: ',header[NDATA]);
  writeln('procedure descriptors: ',header[NPROC]);
  writeln('entry procedure: ',header[ENTRY]);
  if header[7]<>0 then writeln('Second header entry 7 is ',header[7]);
  if header[8]<>0 then writeln('Second header entry 8 is ',header[8]);
  { read program text }
  for i:=0 to header[NTEXT]-1 do skipbyte;
  { read data blocks }
  writeln; writeln('Data descriptors:');
  nexta:=0;
  for i:=1 to header[NDATA] do
    begin
      n:=readbyte;
      write(nexta:5,'-  ');
      if n<>0 then
	begin
	  elem:=readbyte; firsta:=nexta;
	  case n of
	  1: { uninitialized words }
	     begin
	         writeln(elem,' uninitialised word(s)');
	         nexta:= nexta+ elem*wsize ;
	     end;
	  2: { initialized bytes }
	     begin
		 write(elem,' initialised byte(s)');
	         for j:=1 to elem do
	         begin
		     if j mod 10 = 1 then
		     begin writeln ; write(nexta:6,':') end ;
		     write(readbyte:4); nexta:=nexta+1
		 end;
		 writeln
	      end;
	  3: { initialized words }
	     begin
		 write(elem,' initialised word(s)');
	         for j:=1 to elem do
	         begin
		     if j mod 8 = 1 then
		     begin writeln ; write(nexta:6,':') end ;
		     write(readword:9); nexta:=nexta+wsize
		 end;
		 writeln
	      end;
	  4,5: { instruction and data pointers }
	     begin
		 if n=4 then
		     write(elem,' initialised data pointers')
		 else
		     write(elem,' initialised instruction pointers');
	         for j:=1 to elem do
	         begin
		     if j mod 8 = 1 then
		     begin writeln ; write(nexta:6,':') end ;
		     write(readadr:9); nexta:=nexta+asize
		 end;
		 writeln
	      end;
	  6: { signed integers }
	     begin 
		 write(elem,'-byte signed integer ');
		 readint(nexta,elem); nexta:=nexta+elem
	     end;
	  7: { unsigned integers }
	     begin
		 write(elem,'-byte unsigned integer ');
		 readuns(nexta,elem); nexta:=nexta+elem
	     end;
	  8: { floating point numbers }
	     begin
		 write(elem,'-byte floating point number ');
		 readfloat(nexta,elem); nexta:=nexta+elem
	     end;
	  end
	end
      else
	begin
	  repc:=readadr;
	  amount:=nexta-firsta;
	  writeln(repc,' copies of the data from ',firsta:2,' to ',nexta:2);
	  nexta:= nexta + repc*amount ;
	end
    end;
  if header[SZDATA]<>nexta then writeln('Data initialization error');
  { read descriptor table }
  pd:=header[NTEXT];
  for i:=1 to header[NPROC]*pdsize do skipbyte;
end;

begin getit;
#ifdef RTC
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
#endif
end.
