60000 (*  COPYRIGHT 1983 C.H.LINDSEY, UNIVERSITY OF MANCHESTER  *)
60010  (**)
60020  (**)
60030  (*+86()
60031            (************************************)
60032            (*          EM-1 VERSION            *)
60033            (************************************)
60034  (**)
60035            (************************************)
60036            (*      MEANING OF PARAMTYPES       *)
60037            (************************************)
60038  (**)
60040  (*  WOP - OPERAND SUPPLIED BY CODETABLE
60042      WNP - NEGATIVE OF OPERAND SUPPLIED BY CODETABLE
60043      WLB - OPERAND SUPPLIED BY CODETABLE IS A GLOBAL LABEL OFFSET
60044      OPX - OPERAND SUPPLIED BY CODE GENERATOR
60046      ONX - NEGATIVE OF OPERAND SUPPLIED BY CODE GENERATOR
60048      OPL - OPERAND SUPPLIED BY CODE GENERATOR IS A GLOBAL LABEL OFFSET
60049      ONL - NEGATIVE OF ABOVE
60050      LCX - LOCAL INSTRUCTION LABEL
60052      GBX - GLOBAL DATA LABEL
60054      NON - NO OPERAND
60056      JMP - FORWARD JUMP WITHIN CODETABLE
60058      ACP - AS WOP, BUT PROVIDES LAST OPERAND FOR AN OCODE
60059      ACB - AS WLB, BUT DITTO
60060      ANP - AS WNP, BUT DITTO
60062      ACX - AS OPX, BUT DITTO
60064      ANX - AS ONX, BUT DITTO
60066      ACL - AS OPL, BUT DITTO
60067      ANL - AS ONL, BUT DITTO
60068      MOR - LONG OPERAND IN PARAM SUPPLIED BY CODETABLE
60072  *)
60080  PROCEDURE INITCODES;
60090  (* INITIALISES CODETABLE *)
60100  CONST
60310                          (* EXTRA P-OPS AND CODETABLE ENTRIES *)
60320  PPUSH1(*3*)=200; PPUSHFTN(*3*)=203; PS4TOS2=206; PPUSHIM2(*2*)=207; PPUSH2(*3*)=209; PSTOS2=212; PPUSHIM4(*2*)=213;
60328  PLOADRTA(*3*)=215; PPUSHI2A=218;
60330  QDCLINIT(*5*)=219; QCOLLTOTAL(*8*)=224; QSELECT=232; QEQ=233; QGE=234; QGT=235;
60340  QRANGENT=238; QLOADRTA=239; QLE=240; QLT=241; QNE=242;
60350  QNOTB=243; QODD=244; QCAS=245; QSCOPENT=246; QLEBT=247;
60360  QCFSTRNG=248; QVOIDNM(*5*)=249; QDCLSP(*6*)=254; QOUTJUMP(*4*)=260; QWIDEN(*2*)=264;
60370  QELMBT=266; QRANGEXT(*2*)=267; QASGVART(*2*)=269; QASSIGNT(*2*)=271; QPASC(*2*)=273;
60376  QLOOPINCR(*5*)=275; QPUSH2=280; QPUSHIM4(*2*)=281; QGETPROC(*2*)=283; QABSI(*6*)=285;
60380  QDUP2ND(*2*)=291; QDUP2PILE=293; (*SPARE(2)=294;*) QCALLA(*4*)=296; QHOIST(*2*)=300; QLOADVAR(*5*)=302;
60382  QLPINIT(*4*)=307; QSCOPEVAR(*5*)=311; QSETIB(*3*)=316; QRNSTART(*2*)=319;
60384  (*321-350 SPARE*)
60390              ST=SBTSTK; STP=SBTSTK; STS=SBTSTK;
60400              ST2=SBTSTK2; S2P=SBTSTK2; S2S=SBTSTK2;
60410              ST4=SBTSTK4; S4P=SBTSTK4; S4S=SBTSTK4;
60420              STN=SBTSTKN; SNP=SBTSTKN; SNS=SBTSTKN;
60430              PR1=SBTPR1;  PR2=SBTPR2;  PRR=SBTPRR;
60440              O=SBTVOID; DLS=SBTDL;
60445              SRE=SBTSTK4;SP=(*+19()SBTSTK2()+19*)(*-19()SBTSTK()-19*);
60450  (**)
60460    PROCEDURE ICODE(OPCOD:POP;EMCODE:COMPACT;TYP:PARAMTYPES;PM:INTEGER;PNXT:POP;VP1,VP2,VPR:SBTTYP);
60470        BEGIN
60480        WITH CODETABLE[OPCOD] DO
60490          BEGIN
60500          INLINE := TRUE;
60510          EMCOD := EMCODE;
60520          P1 := VP1;
60530          P2 := VP2;
60540          PR := VPR;
60550          NEXT := PNXT;
60560          PARTYP := TYP;
60570          CASE TYP OF
60580            LCX,GBX,WLB,ACB,OPX,ONX,ACX,ANX,OPL,ONL,ACL,ANL,NON:PARM := PM;
60590            WOP,ACP,JMP: PARM := PM;
60595            WNP,ANP: PARM := -PM;
60596            MOR: PARM:=PM;
60600            END;
60610          END;
60620        END;
60630  (*+)
60640    PROCEDURE QCODE(OPCOD:POP;EMCODE:COMPACT;TYP:PARAMTYPES;PM:INTEGER;PNXT:POP);
60650        BEGIN
60660        ICODE(OPCOD,EMCODE,TYP,PM,PNXT,O,O,O);
60670        END;
60680  (*+)
60690    PROCEDURE OCODE(OPCOD:POP;PROUTINE:ALFA;VP1,VP2,VPR:SBTTYP);
60700      VAR I:INTEGER;
60710        BEGIN
60720        WITH CODETABLE[OPCOD] DO
60730          BEGIN
60740          INLINE := FALSE;
60750          P1 := VP1;
60760          P2 := VP2;
60770          PR := VPR;
60780          IF (P1=O) AND (P2 <> O) THEN WRITELN(LSTFILE,'FAILED OCODE-A');
60790          FOR I := 1 TO RTNLENGTH DO ROUTINE[I] := PROUTINE[I];
60800          END;
60810        END;
60820    PROCEDURE FIRSTPART;
60830      VAR I:INTEGER;
60840        BEGIN
60850          FOR I := PNONE TO PLAST DO OCODE(I,'DUMMY     ',O,O,O);
60860          OCODE(PPEND      , 'STOP68    '                , O , O , O );
60864          OCODE(PPBEGIN    , 'ESTART0   '                , O , O , O );
60870          OCODE(PPBEGIN+1  , 'START68   '                , O , O , O );
60880          ICODE(PABSI      ,  DUP , WOP , SZINT ,QABSI   ,ST , O ,ST );
60890          QCODE(QABSI      ,  ZGE , JMP , 2 ,QABSI+1                 );
60900          QCODE(QABSI+1    ,  NGI , WOP , SZINT ,0                   );
60910          ICODE(PABSI-2    ,  DUP , WOP , SZREAL ,QABSI+2,SRE, O ,SRE);
60920          OCODE(PABSI-4    , 'CABSI     '                ,PR1, O ,PRR);
60924          QCODE(QABSI+2    ,  ZRF , WOP , SZREAL ,QABSI+3            );
60930          QCODE(QABSI+3    ,  CMF , WOP , SZREAL ,QABSI+4            );
60940          QCODE(QABSI+4    ,  ZGE , JMP , 2 ,QABSI+5                 );
60950          QCODE(QABSI+5    ,  NGF , WOP , SZREAL ,0                  );
60960          ICODE(PABSB      ,  NOP , NON , 0 ,0           ,ST , O ,ST );
60970          ICODE(PABSB-1    ,  NOP , NON , 0 ,0           ,ST , O ,ST );
60980          ICODE(PABSCH     ,  NOP , NON , 0 ,0           ,ST , O ,ST );
60990          ICODE(PADD       ,  ADI , WOP , SZINT ,0       ,ST ,ST ,ST );
61020          ICODE(PADD-2     ,  ADF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
61040          OCODE(PADD-4     , 'CPLUS     '                ,PR1,PR2,PRR);
61050          ICODE(PANDB      , CAND , WOP , SZWORD ,0      ,ST ,ST ,ST );
61060          ICODE(PANDB-1    , CAND , WOP , SZINT ,0       ,ST ,ST ,ST );
61070          OCODE(PARG       , 'CARG      '                ,PR1,O  ,PRR);
61080          ICODE(PBIN       ,  NOP , NON , 0 ,0           ,ST , O ,ST );
61090          OCODE(PCAT       , 'CATCC     '                ,PR1,PR2,PRR);
61100          OCODE(PCAT-1     , 'CATSS     '                ,PR1,PR2,PRR);
61110          OCODE(PCONJ      , 'CCONJ     '                ,PR1,O  ,PRR);
61120          OCODE(PDIV       , 'DIV       '                ,PR1,PR2,PRR);
61130          ICODE(PDIV-2     ,  DVF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
61140          OCODE(PDIV-4     , 'CDIV      '                ,PR1,PR2,PRR);
61150          ICODE(PDIVAB     ,  DVF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
61160          OCODE(PDIVAB-2   , 'CDIVAB    '                ,PR1,PR2,PRR);
61170          ICODE(PELMBT     ,  EXG , WOP , SZINT ,QELMBT  , ST, ST, ST);
61180          QCODE(QELMBT     ,  ROL , WOP , SZINT ,PODD                );
61210          OCODE(PELMBY     , 'ELMBY     '                ,PR1,PR2,PRR);
61220          OCODE(PENTI      , 'ENTIER    '                ,PR1, O ,PRR);
61230          ICODE(PEQ        ,  CMI , WOP , SZINT ,QEQ     ,ST ,ST ,ST );
61240          QCODE(QEQ        ,  TEQ , NON , 0 ,0                       );
61250          ICODE(PEQ-2      ,  CMF , WOP , SZREAL ,QEQ    ,SRE,SRE,ST );
61260          OCODE(PEQ-4      , 'CEQ       '                ,PR1,PR2,PRR);
61270          ICODE(PEQB       ,  CMI , WOP , SZINT ,QEQ     ,ST ,ST ,ST );
61280          ICODE(PEQB-1     ,  CMI , WOP , SZWORD ,QEQ    ,ST ,ST ,ST );
61290          ICODE(PEQB-2     ,  CMI , WOP , SZWORD ,QEQ    ,ST ,ST ,ST );
61300          ICODE(PEQCS      ,  CMI , WOP , SZINT ,QEQ     ,ST ,ST ,ST );
61310          ICODE(PEQCS-1    ,  LOC , ACP , 2 ,QCFSTRNG    ,PR1,PR2,PRR);
61320          OCODE(QCFSTRNG   , 'CFSTR     '                , O , O , O );
61330          OCODE(PEXP       , 'POWI      '                ,PR1,PR2,PRR);
61340          OCODE(PEXP-2     , 'POWR      '                ,PR1,PR2,PRR);
61350          OCODE(PEXP-4     , 'CPOW      '                ,PR1,PR2,PRR);
61355          ICODE(PPASC      ,  LFC , WOP , 0 ,QPASC       ,DLS, O ,PRR);
61357          QCODE(QPASC      ,  LXL , WOP , 0 ,QPASC+1                 );
61360          QCODE(QPASC+1    ,  CAL , OPX , 0 ,0                       );
61370          ICODE(PPASC+1    ,  LXL , WOP , 0 ,QPASC+1     ,PR1, O ,PRR);
61380          ICODE(PPASC+2    ,  CAL , OPX , 0 ,0           ,PR1,PR2,PRR);
61390          OCODE(PPASC+3    , 'PASC      '                ,PR1,PR2,PRR);
61395          ICODE(PASP       ,  ASP , OPX , 0 ,0           , O , O , O );
61400          ICODE(PENVCHAIN  ,  LXL , OPX , 0 ,0           , O , O , O );
61410          ICODE(PENVCHAIN+1,  LXA , OPX , 0 ,0           , O , O , O );
61420          ICODE(PGE        ,  CMI , WOP , SZINT ,QGE     ,ST ,ST ,ST );
61430          ICODE(PGE-2      ,  CMF , WOP , SZREAL ,QGE    ,SRE,SRE,ST );
61440          ICODE(PGEBT      ,  EXG , WOP , SZINT ,PLEBT   ,ST ,ST ,ST );
61450          ICODE(PGEBT-1    ,  CMU , WOP , SZINT ,QGE     ,ST ,ST ,ST );
61460          ICODE(PGECS      ,  CMI , WOP , SZINT ,QGE     ,ST ,ST ,ST );
61470          ICODE(PGECS-1    ,  LOC , ACP , 4 ,QCFSTRNG    ,PR1,PR2,PRR);
61480          QCODE(QGE        ,  TGE , NON , 0 ,0                       );
61490          ICODE(PGT        ,  CMI , WOP , SZINT ,QGT     ,ST ,ST ,ST );
61500          QCODE(QGT        ,  TGT , NON , 0 ,0                       );
61510          ICODE(PGT-2      ,  CMF , WOP , SZREAL ,QGT    ,SRE,SRE,ST );
61520          ICODE(PGTBY      ,  CMU , WOP , SZINT ,QGT     ,ST ,ST ,ST );
61530          ICODE(PGTCS      ,  CMI , WOP , SZINT ,QGT     ,ST ,ST ,ST );
61540          ICODE(PGTCS-1    ,  LOC , ACP , 5 ,QCFSTRNG    ,PR1,PR2,PRR);
61550          OCODE(PIM        , 'CIM       '                ,PR1, O ,PRR);
61560          ICODE(PLE        ,  CMI , WOP , SZINT ,QLE     ,ST ,ST ,ST );
61570          QCODE(QLE        ,  TLE , NON , 0 ,0                       );
61580          ICODE(PLE-2      ,  CMF , WOP , SZREAL ,QLE    ,SRE,SRE,ST );
61590          ICODE(PLEBT      ,  COM , WOP , SZINT ,QLEBT   ,ST ,ST ,ST );
61592          QCODE(QLEBT      , CAND , WOP , SZINT ,QEQ                 );
61600          ICODE(PLEBT-1    ,  CMU , WOP , SZINT ,QLE     ,ST ,ST ,ST );
61610          ICODE(PLECS      ,  CMI , WOP , SZINT ,QLE     ,ST ,ST ,ST );
61620          ICODE(PLECS-1    ,  LOC , ACP , 1 ,QCFSTRNG    ,PR1,PR2,PRR);
61680          ICODE(PLT        ,  CMI , WOP , SZINT ,QLT     ,ST ,ST ,ST );
61690          QCODE(QLT        ,  TLT , NON , 0 ,0                       );
61700          ICODE(PLT-2      ,  CMF , WOP , SZREAL ,QLT    ,SRE,SRE,ST );
61710          ICODE(PLTBY      ,  CMU , WOP , SZINT ,QLT     ,ST ,ST ,ST );
61720          ICODE(PLTCS      ,  CMI , WOP , SZINT ,QLT     ,ST ,ST ,ST );
61730          ICODE(PLTCS-1    ,  LOC , ACP , 0 ,QCFSTRNG    ,PR1,PR2,PRR);
61740          OCODE(PLWBMSTR   , 'LWBMSTR   '                ,PR1, O ,PRR);
61750          OCODE(PLWBM      , 'LWBM      '                ,PR1, O ,PRR);
61760          OCODE(PLWB       , 'LWB       '                ,PR1,PR2,PRR);
61770          ICODE(PMINUSAB   ,  SBI , WOP , SZINT ,0       ,ST ,ST ,ST );
61780          ICODE(PMINUSAB-2 ,  SBF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
61790          OCODE(PMINUSAB-4 , 'CMINAB    '                ,PR1,PR2,PRR);
61800          OCODE(PMOD       , 'MOD       '                ,PR1,PR2,PRR);
61810          OCODE(PMODAB     , 'MOD       '                ,PR1,PR2,PRR);
61820          ICODE(PMUL       ,  MLI , WOP , SZINT ,0       ,ST ,ST ,ST );
61830          ICODE(PMUL-2     ,  MLF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
61870          OCODE(PMUL-4     , 'CTIMS     '                ,PR1,PR2,PRR);
61880          OCODE(PMULCI     , 'MULCI     '                ,PR1,PR2,PRR);
61890          OCODE(PMULCI-1   , 'MULSI     '                ,PR1,PR2,PRR);
61900          OCODE(PMULIC     , 'MULIC     '                ,PR1,PR2,PRR);
61910          OCODE(PMULIC-1   , 'MULIS     '                ,PR1,PR2,PRR);
61920          END;
61930   PROCEDURE SECONDPART;
61940          BEGIN
61950          ICODE(PNE        ,  CMI , WOP , SZINT ,QNE     ,ST ,ST ,ST );
61960          QCODE(QNE        ,  TNE , NON , 0 ,0                       );
61970          ICODE(PNE-2      ,  CMF , WOP , SZREAL ,QNE    ,SRE,SRE,ST );
61980          OCODE(PNE-4      , 'CNE       '                ,PR1,PR2,PRR);
61990          ICODE(PNEGI      ,  NGI , WOP , SZINT ,0       ,ST , O ,ST );
62000          ICODE(PNEB       ,  CMI , WOP , SZINT ,QNE     ,ST ,ST ,ST );
62010          ICODE(PNEB-1     ,  CMI , WOP , SZINT ,QNE     ,ST ,ST ,ST );
62020          ICODE(PNEB-2     ,  CMI , WOP , SZINT ,QNE     ,ST ,ST ,ST );
62030          ICODE(PNECS      ,  CMI , WOP , SZINT ,QNE     ,ST ,ST ,ST );
62040          ICODE(PNECS-1    ,  LOC , ACP , 3 ,QCFSTRNG    ,PR1,PR2,PRR);
62050          ICODE(PNEGI-2    ,  NGF , WOP , SZREAL ,0      ,SRE, O ,SRE);
62060          OCODE(PNEGI-4    , 'CNEGI     '                ,PR1,PR2,PRR);
62070          ICODE(PNOTB      ,  LOC , WOP , 1 ,QNOTB       ,ST , O ,ST );
62080          QCODE(QNOTB      ,  XOR , WOP , SZWORD ,0                  );
62090          ICODE(PNOTB-1    ,  COM , WOP , SZWORD ,0      ,ST , O ,ST );
62100          ICODE(PNOOP      ,  NOP , NON , 0 ,0           ,ST , O ,ST );
62110          ICODE(PNOOP-2    ,  NOP , NON , 0 ,0           ,SRE, O ,SRE);
62120          ICODE(PNOOP-4    ,  NOP , NON , 0 ,0           ,SP, O ,SP);
62130          ICODE(PODD       ,  LOC , WOP , 1 ,QODD        ,ST , O ,ST );
62140          QCODE(QODD       , CAND , WOP , SZINT ,0                   );
62150          ICODE(PORB       ,  IOR , WOP , SZWORD ,0      ,ST ,ST ,ST );
62160          ICODE(PORB-1     ,  IOR , WOP , SZWORD ,0      ,ST ,ST ,ST );
62170          ICODE(POVER      ,  DVI , WOP , SZINT,0        ,ST ,ST ,ST );
62180          ICODE(POVERAB    ,  DVI , WOP , SZINT,0        ,ST ,ST ,ST );
62190          OCODE(PPLITM     , 'CRCOMPLEX '                ,PR1,PR2,PRR);
62200          ICODE(PPLSAB     ,  ADI , WOP , SZINT ,0       ,ST ,ST ,ST );
62210          ICODE(PPLSAB-2   ,  ADF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
62250          OCODE(PPLSAB-4   , 'CPLUSAB   '                ,PR1,PR2,PRR);
62260          OCODE(PPLSABS    , 'PLABSS    '                ,PR1,PR2,PRR);
62270          OCODE(PPLSABS-1  , 'PLABSS    '                ,PR1,PR2,PRR);
62280          OCODE(PPLSTOCS   , 'PLTOSS    '                ,PR1,PR2,PRR);
62290          OCODE(PPLSTOCS-1 , 'PLTOSS    '                ,PR1,PR2,PRR);
62300          OCODE(PRE        , 'CRE       '                ,PR1,O  ,PRR);
62310          ICODE(PREPR      ,  NOP , NON , 0 ,0           ,ST ,ST ,ST );
62320          OCODE(PROUN      , 'ROUN      '                ,PR1, O ,PRR);
62330          OCODE(PSGNI      , 'SIGNI     '                ,PR1, O ,PRR);
62340          OCODE(PSGNI-2    , 'SIGNR     '                ,PR1, O ,PRR);
62350          OCODE(PSHL       , 'SHL       '                ,PR1,PR2,PRR);
62410          OCODE(PSHR       , 'SHR       '                ,PR1,PR2,PRR);
62420          ICODE(PSUB       ,  SBI , WOP , SZINT ,0       ,ST ,ST ,ST );
62430          ICODE(PSUB-2     ,  SBF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
62440          OCODE(PSUB-4     , 'CMINUS    '                ,PR1,PR2,PRR);
62450          ICODE(PTIMSAB    ,  MLI , WOP , SZINT ,0       ,ST ,ST ,ST );
62460          ICODE(PTIMSAB-2  ,  MLF , WOP , SZREAL ,0      ,SRE,SRE,SRE);
62500          OCODE(PTIMSAB-4  , 'CTIMSAB   '                ,PR1,PR2,PRR);
62510          OCODE(PTIMSABS   , 'MULABSI   '                ,PR1,PR2,PRR);
62520          OCODE(PUPBMSTR   , 'UPBMSTR   '                ,PR1, O ,PRR);
62530          OCODE(PUPBM      , 'UPBM      '                ,PR1, O ,PRR);
62540          OCODE(PUPB       , 'UPB       '                ,PR1,PR2,PRR);
62570          OCODE(PSELECT    , 'SELECTT   '                ,PR1, O ,PRR);
62575          OCODE(PSELECT+1  , 'SELECTS   '                ,PR1, O ,PRR);
62576          OCODE(PSELECT+2  , 'SELECTN   '                ,PR1, O ,PRR);
62580 (*       ICODE(PSELECT+1  ,  LOC , OPX , 0 ,QSELECT     ,ST , O ,ST );
62590          QCODE(QSELECT    ,  ADI , WOP , SZINT ,0                   );*)
62600          OCODE(PSELECTROW , 'SELECTR   '                ,PR1, O ,PRR);
62610          OCODE(PSTRNGSLICE, 'STRSUB    '                ,PR1,PR2,PRR);
62620          OCODE(PSTRNGSLICE+1, 'STRTRIM   '              ,PR1, O ,PRR);
62630          OCODE(PSTARTSLICE, 'STARTSL   '                , O , O , O );
62640          OCODE(PSLICE1    , 'SLICE1    '                ,PR1,PR2,PRR);
62650          OCODE(PSLICE2    , 'SLICE2    '                ,PR1,PR2,PRR);
62660          OCODE(PSLICEN    , 'SLICEN    '                ,PR1, O ,PRR);
62670          ICODE(PCASE      ,  LAE , GBX , 0 ,QCAS        ,ST , O , O );
62680          QCODE(QCAS       ,  CSA , WOP , SZWORD ,0                  );
62690          ICODE(PJMPF      ,  ZEQ , LCX , 0 ,0           ,ST , O , O );
62700          ICODE(PLPINIT    ,  LAL , ANX , 0 ,QLPINIT     ,PR1, O ,PRR);
62704          OCODE(QLPINIT    , 'LINIT1    '                , O , O , O );
62710          ICODE(PLPINIT+1  ,  LAL , ANX , 0 ,QLPINIT+1   ,PR1, O ,PRR);
62714          OCODE(QLPINIT+1  , 'LINIT2    '                , O , O , O );
62720          ICODE(PLPINIT+2  ,  LAL , ANX , 0 ,QLPINIT+2   ,PR1, O , O );
62724          OCODE(QLPINIT+2  , 'LINIT3    '                , O , O , O );
62730          ICODE(PLPINIT+3  ,  LAL , ANX , 0 ,QLPINIT+3   ,PR1, O , O );
62734          OCODE(QLPINIT+3  , 'LINIT4    '                , O , O , O );
62740          ICODE(PLPTEST    ,  ZEQ , LCX , 0 ,0           ,ST , O , O );
62750          ICODE(PLPINCR    ,  LAL , ANX , 0 ,QLOOPINCR+4 , O , O ,PRR);
62760          ICODE(PLPINCR+1  ,  INL , ONX , 0 ,QLOOPINCR   , O , O ,ST );
62770          QCODE(QLOOPINCR  ,  LOL , ONX , 0 ,QLOOPINCR+1             );
62780          QCODE(QLOOPINCR+1,  LOL , ONX , SZINT, QLOOPINCR+2         );
62790          QCODE(QLOOPINCR+2,  CMI , WOP , SZINT, QLOOPINCR+3         );
62792          QCODE(QLOOPINCR+3,  TLE , NON , 0 ,0                       );
62793          OCODE(QLOOPINCR+4, 'LOOPINC   '                , O , O , O );
62805          ICODE(PRANGENT   ,  LAL , ANX , 0 ,QRANGENT    , O , O , O );
62810          OCODE(QRANGENT   , 'RANGENT   '                , O , O , O );
62820          OCODE(PRANGEXT   , 'RANGEXT   '                , O , O , O );
62830          ICODE(PRANGEXT+1 ,  LFL , WNP ,
62835   SIZIBBASE+SIZLEBBASE-(2*SZWORD+2*SZADDR) ,QRANGEXT    , O , O , O );
62840          QCODE(QRANGEXT   ,  LFF , WOP ,
62845                         2*SZWORD+2*SZADDR  ,QRANGEXT+1              );
62850          QCODE(QRANGEXT+1 ,  SFL , WNP ,
62855   SIZIBBASE+SIZLEBBASE-(2*SZWORD+2*SZADDR) ,0                       );
62860          OCODE(PRANGEXT+2 , 'RANGXTP   '                ,PR1, O ,PRR);
62865          OCODE(PRECGEN    , 'DORECGEN  '                , O , O , O );
62870          OCODE(PACTDRSTRUCT,'CRSTRUCT  '                ,PR1, O ,PRR);
62880          OCODE(PACTDRMULT , 'CRMULT    '                ,PR1, O ,PRR);
62910          OCODE(PCHECKDESC , 'CHKDESC   '                ,PR1,PR2,PRR);
62920          OCODE(PVARLISTEND, 'GARBAGE   '                ,PR1, O , O );
62930          ICODE(PVARLISTEND+1,ASP , WOP , SZINT ,0       , O , O ,ST );
62940          ICODE(PDCLINIT   ,  LOC , MOR , -32000-768,0   , O , O , O );
62944          ICODE(PDCLINIT+1 ,  LAE , WLB ,-FIRSTIBOFFSET,QDCLINIT, O , O , O );
62946          QCODE(QDCLINIT   ,  LPB , NON , 0 ,QDCLINIT+1              );
62948          QCODE(QDCLINIT+1 ,  ADP , WOP , (2*SZADDR)+(SZINT+SZLONG),QDCLINIT+2);
62950          QCODE(QDCLINIT+2 ,  LOI , WOP , SZADDR,0                   );
62952          ICODE(PDCLINIT+2 ,  DUP , WOP , SZINT,QDCLINIT+3 , O , O , O );
62953          QCODE(QDCLINIT+3 ,  STL , ONX , SZINT ,0                   );
62954          ICODE(PDCLINIT+3 ,  DUP , WOP , SZADDR,QDCLINIT+4 , O , O , O );
62955          QCODE(QDCLINIT+4 ,  SFL , ONX , SZADDR ,0                  );
62960          OCODE(PCREATEREF , 'CRREFN    '                ,PR1, O ,PRR);
62970          OCODE(PCREATEREF+1, 'CRRECN    '               ,PR1, O ,PRR);
62980          OCODE(PCREATEREF+2, 'CRREFR    '               ,PR1, O ,PRR);
62990          OCODE(PCREATEREF+3, 'CRRECR    '               ,PR1, O ,PRR);
63000          OCODE(PCREATEREF+4, 'SETCC     '               ,PR1, O ,PRR);
63010          ICODE(PDCLSP     ,  STL , ONX , SZWORD , 0     ,ST , O , O );
63012 (*+12()  ICODE(PDCLSP+1   ,  SFL , ONX , SZADDR ,QDCLSP ,SP , O , O );
63040          QCODE(QDCLSP     ,  LIL , ONX , SZADDR,QDCLSP+1            );
63050          QCODE(QDCLSP+1   ,  INC , NON , 0     ,QDCLSP+2            );
63060          QCODE(QDCLSP+2   ,  SIL , ONX , SZADDR, 0                  ); ()+12*)
63072 (*+13()  ICODE(PDCLSP+1   ,  DUP , WOP , SZADDR,QDCLSP  ,SP , O , O );
63073          QCODE(QDCLSP     ,  STL , ONX , SZADDR,QDCLSP+1            );
63074          QCODE(QDCLSP+1   ,  DUP , WOP , SZADDR,QDCLSP+2            );
63075          QCODE(QDCLSP+2   ,  LOI , WOP , 2     ,QDCLSP+3            );
63076          QCODE(QDCLSP+3   ,  INC , NON , 0     ,QDCLSP+4            );
63077          QCODE(QDCLSP+4   ,  EXG , WOP , SZADDR,QDCLSP+5            );
63078          QCODE(QDCLSP+5   ,  STI , WOP , 2     , 0                  ); ()+13*)
63080          OCODE(PDCLSP+2   , 'DCLSN     '                ,SNS, O , O );
63090          OCODE(PDCLSP+3   , 'DCLPN     '                ,SNS, O , O ); 
63099          ICODE(PFIXRG     ,  LAL , ONX , 0 ,0           , O , O , O );
63100          ICODE(PFIXRG+1   ,  SFL , ONX , 0 ,0           , O , O , O );
63101          END;
63110      PROCEDURE THIRDPART;
63120          BEGIN
63130          OCODE(PBOUNDS    , 'BOUND     '                ,STS, O ,PRR);
63140          ICODE(PLOADVAR   ,  LAL , ACX , 0 ,QLOADVAR    , O , O ,PRR);
63145          QCODE(QLOADVAR   ,  LXL , ACP , 0 ,QLOADVAR+4              );
63150          ICODE(PLOADVAR+1 ,  LAE , ACL , 0 ,QLOADVAR+1  , O , O ,PRR);
63155          QCODE(QLOADVAR+1 ,  LAE , ACB ,-FIRSTIBOFFSET,QLOADVAR+4   );
63156          ICODE(PLOADVAR+2 ,  DUP , ACP ,SZADDR,QLOADVAR+2,O , O ,PRR);
63157          QCODE(QLOADVAR+2 ,  ADP , ACX , 0 ,QLOADVAR+3              );
63158          QCODE(QLOADVAR+3 ,  EXG , WOP , SZADDR,QLOADVAR+4          );
63160          OCODE(QLOADVAR+4 , 'GLDVAR    '                , O , O , O );
63170          OCODE(PLOADRT    , 'ROUTN     '                , O , O ,PRR);
63172          ICODE(PLOADRTA   ,  LXL , ACP , 0 ,QLOADRTA    , O , O ,SP);
63174          ICODE(PLOADRTA+1 ,  LAE , ACB ,-FIRSTIBOFFSET,QLOADRTA, O , O ,SP);
63176          ICODE(PLOADRTA+2 ,  ADP , ACP , 0 ,QLOADRTA    , O , O ,SP);
63178          OCODE(QLOADRTA   , 'ROUTNA    '                , O , O , O );
63180          OCODE(PLOADRTP   , 'ROUTNP    '                ,PR1, O ,PRR);
63190          OCODE(PSCOPETT+2 , 'TASSTPT   '                ,PR1,PR2,PRR);
63200          OCODE(PSCOPETT+3 , 'SCPTTP    '                ,PR1,PR2,PRR);
63210          OCODE(PSCOPETT+4 , 'SCPTTM    '                ,PR1,PR2,PRR);
63220          OCODE(PASSIGTT   , 'TASSTS    '                ,PR1,PR2,PRR);
63225          OCODE(PASSIGTT+1 , 'TASSTS2   '                ,PR1,PR2,PRR);
63230          OCODE(PASSIGTT+2 , 'TASSTPT   '                ,PR1,PR2,PRR);
63240          OCODE(PASSIGTT+3 , 'TASSTP    '                ,PR1,PR2,PRR);
63250          OCODE(PASSIGTT+4 , 'TASSTM    '                ,PR1,PR2,PRR);
63260          OCODE(PSCOPETN   , 'SCPTNP    '                ,PR1,PR2,PRR);
63270          OCODE(PASSIGTN   , 'TASSNP    '                ,PR1,PR2,PRR);
63300          OCODE(PSCOPENT+2 , 'SCPNTPT   '                ,PR1,PR2,PRR);
63310          OCODE(PSCOPENT+3 , 'SCPNTP    '                ,PR1,PR2,PRR);
63330          OCODE(PASSIGNT   , 'NASSTS    '                ,PR1,PR2,PRR);
63340          OCODE(PASSIGNT+1 , 'NASSTS2   '                ,PR1,PR2,PRR);
63350          OCODE(PASSIGNT+2 , 'NASSTPT   '                ,PR1,PR2,PRR);
63360          OCODE(PASSIGNT+3 , 'NASSTP    '                ,PR1,PR2,PRR);
63390          OCODE(PSCOPENN   , 'SCPNNP    '                ,PR1,PR2,PRR);
63410          OCODE(PASSIGNN   , 'NASSNP    '                ,PR1,PR2,PRR);
63430          ICODE(PSCOPEVAR  ,  LAL , ACX , 0 ,QSCOPEVAR   ,PR1, O , O );
63435          QCODE(QSCOPEVAR  ,  LXL , ACP , 0 ,QSCOPEVAR+4             );
63440          ICODE(PSCOPEVAR+1,  LAE , ACL , 0 ,QSCOPEVAR+1 ,PR1, O , O );
63445          QCODE(QSCOPEVAR+1,  LAE , ACB ,-FIRSTIBOFFSET,QSCOPEVAR+4  );
63446          ICODE(PSCOPEVAR+2,  DUP , ACP,SZADDR,QSCOPEVAR+2,PR1,O , O );
63447          QCODE(QSCOPEVAR+2,  ADP , ACX , 0 ,QSCOPEVAR+3             );
63448          QCODE(QSCOPEVAR+3,  EXG , WOP , SZADDR ,QSCOPEVAR+4        );
63450          OCODE(QSCOPEVAR+4, 'GVSCOPE   '                , O , O , O );
63460          OCODE(PSCOPEEXT  , 'SCOPEXT   '                ,PR1, O ,PRR);
63470          ICODE(PASGVART   ,  STL , OPX , 0 ,0           ,ST , O , O );
63480          ICODE(PASGVART+1 ,  STE , OPL , 0 ,0           ,ST , O , O );
63490          ICODE(PASGVART+2 ,  STF , OPX , 0 ,0           ,ST , O , O );
63510          ICODE(PASGVART+3 ,  LAL , OPX , 0 ,QASGVART    ,SRE, O , O );
63520          QCODE(QASGVART   ,  STI , WOP , SZREAL ,0                  );
63530          ICODE(PASGVART+4 ,  LAE , OPL , 0 ,QASGVART    ,SRE, O , O );
63540          ICODE(PASGVART+5 ,  ADP , OPX , 0 ,QASGVART    ,SRE, O , O );
63560          ICODE(PASGVART+6 ,  LAL , ACX , 0 ,QASGVART+1  ,ST , O , O );
63570          ICODE(PASGVART+7 ,  LAE , ACL , 0 ,QASGVART+1  ,ST , O , O );
63572          ICODE(PASGVART+8 ,  ADP , ACX , 0 ,QASGVART+1  ,PR1, O , O );
63580          OCODE(QASGVART+1 , 'GVASSTX   '                , O , O , O );
63590          OCODE(PIDTYREL   , 'IS        '                ,PR1,PR2,PRR);
63600          OCODE(PIDTYREL+1 , 'ISNT      '                ,PR1,PR2,PRR);
63602          OCODE(PGETTOTCMN , 'GTOTSTR   '                ,PR1, O ,PRR);
63604          OCODE(PGETTOTCMN+1,'GTOTMUL   '                ,PR1, O ,PRR);
63606          OCODE(PGETTOTCMN+2,'GTOTRFR   '                ,PR1, O ,PRR);
63608          OCODE(PGETTOTAL  , 'GTOTS     '                ,PR1, O ,PRR);
63610          OCODE(PGETTOTAL+1, 'GTOTS2    '                ,PR1, O ,PRR);
63612          OCODE(PGETTOTAL+2, 'GTOTP     '                ,PR1, O ,PRR);
63614          OCODE(PGETTOTAL+3, 'GTOTN     '                ,PR1, O ,PRR);
63616          OCODE(PGETTOTAL+4, 'GTOTREF   '                ,PR1, O ,PRR);
63618          OCODE(PGETMULT   , 'GETMULT   '                ,PR1, O ,PRR);
63620          OCODE(PGETMULT+1 , 'GETSLN    '                ,PR1, O ,PRR);
63630          OCODE(PDEREF     , 'DREFS     '                ,PR1, O ,PRR);
63631          OCODE(PDEREF+1   , 'DREFS2    '                ,PR1, O ,PRR);
63632          OCODE(PDEREF+2   , 'DREFPTR   '                ,PR1, O ,PRR);
63634          OCODE(PDEREF+3   , 'DREFN     '                ,PR1, O ,PRR);
63640          OCODE(PDEREF+4   , 'DREFM     '                ,PR1, O ,PRR);
63650          OCODE(PSKIP      , 'SKIPS     '                , O , O ,PRR);
63660          OCODE(PSKIP+1    , 'SKIPPIL   '                , O , O ,PRR);
63665          OCODE(PSKIP+2    , 'SKIPS2    '                , O , O ,PRR);
63670          OCODE(PSKIPSTRUCT, 'SKIPSTR   '                , O , O ,PRR);
63680          OCODE(PNIL       , 'NILP      '                , O , O ,PRR);
63690          ICODE(PVOIDNORMAL,  DUP , WOP , SZADDR ,QVOIDNM, SP, O , O );
63700          QCODE(QVOIDNM    ,  LOI , WOP , SZWORD ,QVOIDNM+1          );
63710          QCODE(QVOIDNM+1  ,  ZEQ , JMP , 3 ,QVOIDNM+2               );
63720          QCODE(QVOIDNM+2  ,  ASP , WOP , SZADDR ,QVOIDNM+3          );
63730          QCODE(QVOIDNM+3  ,  BRA , JMP , 2 ,QVOIDNM+4               );
63740          OCODE(QVOIDNM+4  , 'GARBAGE   '                , O , O , O );
63750          OCODE(PVOIDNAKED , 'VOIDN     '                ,PR1, O , O );
63760          ICODE(PWIDEN     ,  LOC , WOP , SZINT ,QWIDEN  , ST, O ,SRE);
63770          QCODE(QWIDEN     ,  LOC , WOP , SZREAL ,QWIDEN+1           );
63780          QCODE(QWIDEN+1   ,  CIF , NON , 0 ,0                       );
63790          OCODE(PWIDEN+2   , 'WIDREAL   '                ,PR1, O ,PRR);
63800          OCODE(PWIDEN+4   , 'WIDCHAR   '                ,PR1, O ,PRR);
63810          OCODE(PWIDEN+5   , 'WIDBITS   '                ,PR1, O ,PRR);
63820          OCODE(PWIDEN+6   , 'WIDBYTS   '                ,PR1, O ,PRR);
63830          OCODE(PWIDEN+7   , 'WIDSTR    '                ,PR1, O ,PRR);
63840          OCODE(PROWNONMULT, 'ROWNM     '                ,PR1, O ,PRR);
63850          OCODE(PROWMULT   , 'ROWM      '                ,PR1, O ,PRR);
63855          ICODE(PGETPROC   ,  LOR , WOP , 1 ,QGETPROC    ,PR1, O ,PRR);
63856          QCODE(QGETPROC   ,  ADP , ANX , SZADDR ,QGETPROC+1         );
63857          QCODE(QGETPROC+1 ,  LOI , WOP , SZADDR,PGETPROC+1          );
63859          OCODE(PGETPROC+1 , 'GETPROC   '                ,PR1, O ,PRR);
63860          ICODE(PCALL      ,  LFR , WOP , 2*SZADDR,QCALLA,SNS, O , O );
63862          ICODE(PCALLA     ,  LXL , ACP , 0 ,QCALLA      ,SNS, O , O );
63865          ICODE(PCALLA+1   ,  LAE , ACB ,-FIRSTIBOFFSET,QCALLA,SNS, O , O );
63867          ICODE(PCALLA+2   ,  ADP , ACP , 0 ,QCALLA      ,SNS, O , O );
63869          QCODE(QCALLA     ,  DUP , WOP , 2*SZADDR,QCALLA+1          );
63870          QCODE(QCALLA+1   ,  ASP , WOP , SZADDR,QCALLA+2            );
63872          QCODE(QCALLA+2   ,  LOI , WOP , SZADDR,QCALLA+3            );
63874          QCODE(QCALLA+3   ,  CAI , NON , 0 ,0                       );
63875          ICODE(PRNSTART   ,  LOC , WOP ,A68STAMP,QRNSTART, O , O , O );
63876          QCODE(QRNSTART   ,  STL , WOP , -SZWORD ,QRNSTART+1        );
63877          OCODE(QRNSTART+1 , 'RNSTART   '                , O , O , O );
63878          ICODE(PRETURN    ,  RET , OPX , 0 ,0           ,STN, O , O );
63880          OCODE(PGBSTK     , 'GBSTK     '                , O , O , O );
63884          ICODE(POUTJUMP   ,  LOR , WOP , 1 ,QOUTJUMP    , O , O , O );
63885          QCODE(QOUTJUMP   ,  SFE , GBX , SZADDR ,QOUTJUMP+1         );
63886          QCODE(QOUTJUMP+1 ,  LOR , WOP , 0 ,QOUTJUMP+2              );
63887          QCODE(QOUTJUMP+2 ,  SFE , GBX , 2*SZADDR ,QOUTJUMP+3       );
63888          QCODE(QOUTJUMP+3 ,  GTO , GBX , 0 ,0                       );
63890          OCODE(PGETOUT    , 'GETOUT    '                , O , O , O );
63892          ICODE(PSETIB     ,  LFR , WOP , 2*SZADDR,QSETIB, O , O , O );
63895          QCODE(QSETIB     ,  EXG , WOP , SZADDR ,QSETIB+1           );
63896          QCODE(QSETIB+1   ,  STR , WOP , 0 ,QSETIB+2                );
63897          QCODE(QSETIB+2   ,  STR , WOP , 1 ,0                       );
63900          OCODE(PLEAPGEN   , 'GENSTR    '                , O , O ,PRR);
63910          OCODE(PLEAPGEN+1 , 'HEAPSTR   '                , O , O ,PRR);
63920          OCODE(PLEAPGEN+2 , 'GENRSTR   '                , O , O ,PRR);
63930          OCODE(PLEAPGEN+3 , 'GENMUL    '                ,PR1, O ,PRR);
63940          OCODE(PLEAPGEN+4 , 'HEAPMUL   '                ,PR1, O ,PRR);
63950          OCODE(PLEAPGEN+5 , 'GENRMUL   '                ,PR1, O ,PRR);
63960          OCODE(PPREPSTRDISP , 'PCOLLST   '              , O , O ,PRR);
63970          OCODE(PPREPROWDISP , 'PCOLLR    '              ,STS, O ,PRR);
63980          OCODE(PPREPROWDISP+1, 'PCOLLRM   '             ,STS, O ,PRR);
63990          OCODE(PCOLLCHECK , 'PCOLLCK   '                ,STP, O , O );
64000 (*       ICODE(PCOLLTOTAL ,  DUP , WOP , SZINT ,QCOLLTOTAL,STP,ST ,O);
64010          QCODE(QCOLLTOTAL ,  LOC , OPX , 0 ,QCOLLTOTAL+1            );
64020          QCODE(QCOLLTOTAL+1, ADI , WOP , SZINT ,QCOLLTOTAL+2        );
64030          QCODE(QCOLLTOTAL+2, EXG , WOP , SZINT ,QCOLLTOTAL+3        );
64040          QCODE(QCOLLTOTAL+3, STI , WOP , SZINT ,0                   );*)
64050 (*       ICODE(PCOLLTOTAL+2, DUP , WOP , SZINT,QCOLLTOTAL+4,STP,ST,O);
64060          QCODE(QCOLLTOTAL+4, LOC , WOP ,15 ,QCOLLTOTAL+5            );
64070          QCODE(QCOLLTOTAL+5,CSET , WOP , SZINT ,QCOLLTOTAL+6        );
64080          QCODE(QCOLLTOTAL+6, EXG , WOP , SZINT ,QCOLLTOTAL+7        );
64090          QCODE(QCOLLTOTAL+7, STI , WOP , SZINT ,PCOLLTOTAL          );*)
64092          OCODE(PCOLLTOTAL   , 'COLLTS    '              ,PR1,PR2,PRR);
64093          OCODE(PCOLLTOTAL+1 , 'COLLTS2   '              ,PR1,PR2,PRR);
64095          OCODE(PCOLLTOTAL+2 , 'COLLTPT   '              ,PR1,PR2,PRR);
64100          OCODE(PCOLLTOTAL+3 , 'COLLTP    '              ,PR1,PR2,PRR);
64110          OCODE(PCOLLTOTAL+4 , 'COLLTM    '              ,PR1,PR2,PRR);
64120          OCODE(PCOLLNAKED , 'COLLNP    '                ,PR1,PR2,PRR);
64130          OCODE(PNAKEDPTR  , 'NAKPTR    '                ,PR1, O ,PRR);
64140          ICODE(PLINE      ,  LIN , OPX , 0 ,0           , O , O , O );
64170          OCODE(PENDSLICE  , 'ENDSL     '                ,PR1, O ,PRR);
64180          OCODE(PTRIM      , 'SLICEA    '                , O , O , O );
64190          OCODE(PTRIM+1    , 'SLICEB    '                , O , O , O );
64200          OCODE(PTRIM+2    , 'SLICEC    '                , O , O , O );
64210          OCODE(PTRIM+3    , 'SLICED    '                , O , O , O );
64220          OCODE(PTRIM+4    , 'SLICEE    '                , O , O , O );
64230          OCODE(PTRIM+5    , 'SLICEF    '                , O , O , O );
64240          OCODE(PTRIM+6    , 'SLICEG    '                , O , O , O );
64250          OCODE(PTRIM+7    , 'SLICEH    '                , O , O , O );
64260          OCODE(PTRIM+8    , 'SLICEI    '                , O , O , O );
64270          OCODE(PTRIM+9    , 'SLICEJ    '                , O , O , O );
64280          ICODE(PJMP       ,  BRA , LCX , 0 ,0           , O , O , O );
64282          ICODE(PDUP1PILE  ,  DUP , WOP , SZADDR,0       ,SP , O , SP);
64284          ICODE(PDUP2PILE  ,  DUP , WOP , SZADDR*2,QDUP2PILE,SP,SP,SP);
64286          QCODE(QDUP2PILE  ,  ASP , WOP , SZADDR,0                   );
64290          ICODE(PDUP1ST    ,  DUP , WOP , SZINT,0        , ST, O , ST);
64294          ICODE(PDUP1ST+1  ,  DUP , WOP , SZREAL,0       ,SRE, O ,SRE);
64300          ICODE(PDUP2ND    ,  DUP , WOP , SZINT*2,QDUP2ND,ST ,ST , ST);
64310          ICODE(PDUP2ND+1  ,  DUP , WOP ,SZREAL+SZINT,QDUP2ND,SRE, ST,SRE);
64312          ICODE(PDUP2ND+2  ,  DUP , WOP , SZINT+SZREAL,QDUP2ND+1, ST,SRE, ST);
64314          ICODE(PDUP2ND+3  ,  DUP , WOP , SZREAL*2,QDUP2ND+1,SRE,SRE,SRE);
64318          QCODE(QDUP2ND    ,  ASP , WOP , SZINT ,0                   );
64319          QCODE(QDUP2ND+1  ,  ASP , WOP , SZREAL ,0                  );
64320          ICODE(PDATALIST  ,  LOC , OPX , 0 ,0           ,SNS, O ,DLS);
64322          ICODE(PHOIST     ,  ASP , ONX , 0 ,QHOIST      , O , O , O );
64324          QCODE(QHOIST     ,  LOC , ACX , 0 ,QHOIST+1                );
64326          OCODE(QHOIST+1   , 'HOIST     '                , O , O , O );
64330          ICODE(PPUSH      ,  LOL , OPX , 0 ,0           , O , O , O );
64340          ICODE(PPUSH+1    ,  LOE , OPL , 0 ,0           , O , O , O );
64350          ICODE(PPUSH+2    ,  LOF , OPX , 0 ,0           , O , O , O );
64360          ICODE(PPUSH1     ,  LFL , OPX , 0 ,0           , O , O , O );
64375          ICODE(PPUSH1+1   ,  LFE , OPL , 0 ,0           , O , O , O );
64376          ICODE(PPUSH1+2   ,  LFF , OPX , 0 ,0           , O , O , O );
64377          ICODE(PPUSH2     ,  LAL , OPX , 0 ,QPUSH2      , O , O , O );
64378          ICODE(PPUSH2+1   ,  LAE , OPL , 0 ,QPUSH2      , O , O , O );
64379          ICODE(PPUSH2+2   ,  ADP , OPX , 0 ,QPUSH2      , O , O , O );
64380          QCODE(QPUSH2     ,  LOI , WOP , SZREAL ,0                  );
64385          ICODE(PPUSHIM    ,  LOC , OPX , 0 ,0           , O , O ,ST );
64390          ICODE(PPUSHIM+1  ,  LAE , GBX , 0 ,0           , O , O ,SP );
64395          ICODE(PPUSHIM+2  ,  LFC , OPX , 0 ,0           , O , O ,SP );
64397          ICODE(PPUSHIM+3  ,  LOE , GBX , 0 ,0           , O , O ,ST );
64400          ICODE(PPUSHIM2   ,  LFC , OPX , 0 ,0           , O , O , O );
64410          ICODE(PPUSHIM2+1 ,  LAE , GBX , 0 ,0           , O , O , O );
64411          ICODE(PPUSHI2A   ,  ASP , WOP , SZINT-SZREAL,0 , O , O , O );
64412          ICODE(PPUSHIM4   ,  LXL , WOP , 0 ,QPUSHIM4    , O , O , O );
64414          QCODE(QPUSHIM4   ,  LPI , OPX , 0 ,0                       );
64415          ICODE(PPUSHIM4+1 ,  LAE , GBX , 0 ,QPUSHIM4+1  , O , O , O );
64416          QCODE(QPUSHIM4+1 ,  LOI , WOP , SZREAL ,0                  );
64420          ICODE(PPUSHFTN   ,  LFR , WOP , SZINT ,0       , O , O , O );
64430          ICODE(PPUSHFTN+1 ,  LFR , WOP , SZADDR ,0      , O , O , O );
64435          ICODE(PPUSHFTN+2 ,  LFR , WOP , SZREAL ,0      , O , O , O );
64440          ICODE(PSWAP      ,  EXG , WOP , SZWORD ,0      , O , O , O );
64442          ICODE(PSWAP+1    ,  EXG , WOP , SZADDR ,0      , O , O , O );
64444          ICODE(PSWAP+2    ,  EXG , WOP , SZREAL ,0      , O , O , O );
64446          OCODE(PSWAP+3    , 'SWAP      '                , O , O , O );
64448 (*+13()  ICODE(PPARM      ,  LOL , ONX , 0 ,QDCLSP+1    , O , O , O ); ()+13*)
64450 (*+12()  ICODE(PPARM      ,  LIL , ONX , 0 ,QDCLSP+4    , O , O , O ); 
64460          QCODE(QDCLSP+4   ,  INC , NON , 0 ,QDCLSP+5                );
64465          QCODE(QDCLSP+5   ,  SIL , ONX , 0 ,0                       ); ()+12*)
64470 (*       ICODE(PSTOS2     ,  ASP , WOP ,-SZINT ,0       , O , O , O );
64530          ICODE(PS4TOS2    ,  ASP , WOP , SZADDR ,0      , O , O , ST); *)
64560          ICODE(PDECM      ,  LOC , OPX , 0 ,0           , O , O , O );
64565          ICODE(PDECM+1    ,  STL , ONX , 0 ,0           , O , O , O );
64570          END;
64580    PROCEDURE INITPOPARRAY;
64590      VAR I,J:SBTTYP;
64600        BEGIN
64610        FOR I := SBTSTK TO SBTDL DO
64620          FOR J := SBTVOID TO SBTPRR DO
64630            BEGIN
64640            POPARRAY [I,J] := PNONE;
64650            POPARRAY [I,I] := PNOOP;
64660            POPARRAY [I,SBTVOID] :=PNOOP;
64670            POPARRAY [I,SBTVAR ] := PLOADVAR;
64672            POPARRAY [I,SBTPROC] := PLOADRTA;
64674            POPARRAY [I,SBTRPROC]:= PLOADRTA;
64680            END;
64700        POPARRAY[ SBTSTK2 , SBTSTK4 ] := PS4TOS2;
64710        POPARRAY[ SBTSTK4 , SBTPRR  ] := PPUSHFTN+2;
64720        POPARRAY[ SBTSTK4 , SBTID   ] := PPUSH2;
64730        POPARRAY[ SBTSTK4 , SBTIDV  ] := PPUSH2;
64735        POPARRAY[ SBTSTK4 , SBTDEN  ] := PPUSHIM4;
64740        POPARRAY[ SBTSTK4 , SBTPR1  ] := PNOOP;
64750        POPARRAY[ SBTSTK4 , SBTPR2  ] := PNOOP;
64760        POPARRAY[ SBTSTK  , SBTSTK2 ] := PVARLISTEND+1;
64770        POPARRAY[ SBTSTK  , SBTID   ] := PPUSH;
64780        POPARRAY[ SBTSTK  , SBTIDV  ] := PPUSH;
64790        POPARRAY[ SBTSTK  , SBTLIT  ] := PPUSHIM;
64800        POPARRAY[ SBTSTK  , SBTDEN  ] := PPUSHIM;
64810        POPARRAY[ SBTSTK  , SBTDL   ] := PNOOP;
64820        POPARRAY[ SBTSTK2 , SBTID   ] := PPUSH1;
64830        POPARRAY[ SBTSTK2 , SBTIDV  ] := PPUSH1;
64840        POPARRAY[ SBTSTK2 , SBTLIT  ] := PPUSHIM2;
64850        POPARRAY[ SBTSTK2 , SBTDEN  ] := PPUSHIM2;
64852        POPARRAY[ SBTSTK2A, SBTLIT  ] := PPUSHI2A;
64860        POPARRAY[ SBTSTK  , SBTPRR  ] := PPUSHFTN;
64870        POPARRAY[ SBTSTK2 , SBTPRR  ] := PPUSHFTN+1;
64880        POPARRAY[ SBTSTK  , SBTPR1  ] := PNOOP;
64890        POPARRAY[ SBTSTK  , SBTPR2  ] := PNOOP;
64900        POPARRAY[ SBTSTK2 , SBTPR1  ] := PNOOP;
64910        POPARRAY[ SBTSTK2 , SBTPR2  ] := PNOOP;
64920        POPARRAY[ SBTSTK2 , SBTSTK  ] := PSTOS2;
64980        END;
64990    PROCEDURE INITLENARRAY;
65000      VAR I:SBTTYP;
65010          BEGIN
65020          FOR I := SBTSTK TO SBTPRR DO LENARRAY[I] := 0;
65030          LENARRAY[SBTSTK ] := SZWORD;
65040  (*+19() LENARRAY[SBTSTK2] := SZADDR;
65042          LENARRAY[SBTSTK2A]:= 3*SZWORD; ()+19*)
65050          LENARRAY[SBTSTK4] := SZREAL;
65060          END;
65070      BEGIN  (* INITCODES +)
65080      FIRSTPART; SECONDPART; THIRDPART;  INITPOPARRAY; INITLENARRAY;
65090      END;
65100  (*+)
65110  ()+86*)
65120  (**)
65130  (**)
65140  (**)
65150  (**)
65160  (**)
65170  (*+71()  BEGIN
65180          DUMP(FIRSTSTACK,LASTSTACK);
65190   END  . ()+71*)
