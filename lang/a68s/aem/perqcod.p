40000     (*  COPYRIGHT 1983 C.H.LINDSEY, UNIVERSITY OF MANCHESTER  *)
40001 THINGS NEEDING ATTENTION
40002     CHECK THE START OF THE LOCAL POPS
40003     ATTEND TO PDUP1PILE AND P2DUP2PILE
40010 (**)
40020 (**)
40040 (*+05()
40050 (*+86()
40100     (*************************)
40110     (* MEANING OF PARAMTYPES *)
40120     (*************************)
40130 (**)
40140 (* OPX - OPERAND SUPPLIED BY CODE GENERATOR
40150    ONX - NEGATIVE OF OPERAND SUPPLIED BY CODE GENERATOR
40160    LCX - LOCAL INSTRUCTION LABEL
40170    GBX - GLOBAL INSTRUCTION LABEL
40180    WOP - OPERAND SUPPLIED BY CODETABLE
40190    WNP - NEGATIVE OF OPERAND SUPPLIED BY CODETABLE
40200    NON - NO OPERAND
40210    JMP - FORWARD JUMP WITHIN CODETABLE
40220    ANP - AS WNP, BUT PROVIDES LAST OPERAND FOR AN OCODE
40230    ACP - AS WOP, BUT DITTO
40240    ACX - AS OPX, BUT DITTO
40250    ANX - AS ONX, BUT DITTO
40252    MOR - LONG OPERAND TO BE FOUND IN THE NEXT OPCOD
40260 *)
40270 (**)
46726 (**)
46728 PROCEDURE INITCODES;
46730 (*INITIALISES CODETABLE+)
46732 CONST
46734 (**)
46736  PLOADRTA(*3*)=194; PDEPROC=197;
46737 PLOADE(*3*)=198; PLOADEIM(*2*)=201; PETOSTK(*3*)=203; PS4TOS2=206; PPUSHIM2(*2*)=207;
46738 PPUSHFSTK=209; PPUSHFSTK1=210; PLOADF=211; PLOADF1=212; QPUSHIM2(*2*)=213;
46739 PLOADFIM=215; PLOADFIM1=216; PLOADFSTK=217; PLOADFSTK1=218; PSTOS4=219; PPUSHF=220; PPUSHF1=221;
46740 PF0TOF1=222; PF1TOF0=223; PPUSH2(*3*)=224; PSTOS2=227; PPUSHER0=228; PLOADER0F0=229; PLOADER0F1=230;
46741 PLOADER0STK=231;  QPUSHER0(*2*)=232; QLPINIT(*8*)=234; QDIV(*2*)=242; QCALL(*8*)=244;
46742 QABSI(*7*)=252;
46743 QRNSTART=259; QHOIST(*3*)=260; QSCOPEVAR(*6*)=263; QENVCHAIN=269;
46744 QIPUSH=270; QODD=271; QLEBT(*2*)=272; QLINE=274; QDATALIST=275; QGETPROC(*4*)=276;
46745 QNOTB=280; QPUSH1=281; QCAS(*3*)=282;
46746 QLOOPINCR(*6*)=285; QDCLSP(*4*)=291; QDCLINIT(*2*)=295;
46747  QELMBT(*5*)=297; QDUP1ST=302; QDUP2ND=303;
46748 QASGVART(*3*)=304; QRANGENT(*2*)=307;
46749 QNAKEDPTR=309; QLOADI=310; QADJSP2=311; QSTKTOE(*3+)=312; QADJSP4=315;
46750 QLOADF=316; QLOADF1=317; QLOADVAR(*6*)=318; QPUSH2(*4*)=324;
46751 QLOADRTA(*2*)=328; QCFSTRNG(*2*)=330; QRANGEXT(*3*)=332; QCALLA(*4*)=335;
46752 QSETIB=339; QSELECT(*3*)=340; (*SPARE(3)=343;*) QCOLLTOTAL(*11*)=346;
46753 QETOSTK(*6*)=357; QGETTOTAL(*5*)=363; (*SPARE(7)=368;*) QHEAVE(*6*)=375; QLOADER0STK(*6*)=381;
46754 QGETTOTCMN(*4*)=387; (*SPARE=391..400*)
46755             ST=SBTSTK; STP=SBTSTK; STS=SBTSTK;
46756             ST4=SBTSTK4; S4P=SBTSTK4; S4S=SBTSTK4;
46758             STN=SBTSTKN; SNP=SBTSTKN; SNS=SBTSTKN;
46760             PR1=SBTPR1;  PR2=SBTPR2;
46761             FP0=SBTFPR0; FP1=SBTFPR1; FP2=SBTFPR2; FP3=SBTFPR3; F0P=SBTFPR0;
46764             XN=SBTXN;
46770             O=SBTVOID; SDL=SBTDL; E=SBTE; ER0=SBTER0;
46780 (*+)
46790   PROCEDURE ICODE(OPCOD:POP;PERQCODE:MNEMONICS;TYP:PARAMTYPES;PM:INTEGER;PNXT:POP;VP1,VP2,VPR:  SBTTYP);
46800       BEGIN
46810       WITH CODETABLE[OPCOD] DO
46820         BEGIN
46830         INLINE := TRUE;
46840         PERQCOD := PERQCODE;
46850         P1 := VP1;
46860         P2 := VP2;
46870         PR := VPR;
46880         NEXT := PNXT;
46890         PARTYP := TYP;
46900         PARM := PM;
46930         END;
46950       END;
46960 (*+)
46970   PROCEDURE QCODE(OPCOD:POP;PERQCODE:MNEMONICS;TYP:PARAMTYPES;PM:INTEGER;PNXT:POP);
46980       BEGIN
46990       ICODE(OPCOD,PERQCODE,TYP,PM,PNXT,O,O,O);
47000       END;
47010 (*+)
47020   PROCEDURE OCODE(OPCOD:POP;PROUTINE:ALFA;VP1,VP2,VPR:SBTTYP);
47030     VAR I:INTEGER;
47040       BEGIN
47050       WITH CODETABLE[OPCOD] DO
47060         BEGIN
47070         INLINE := FALSE;
47080         P1 := VP1;
47090         P2 := VP2;
47100         PR := VPR;
47110         IF (P1=O) AND (P2 <> O) THEN WRITELN(OUTPUT,'FAILED OCODE-A');
47120         FOR I := 1 TO 7 DO
47124           ROUTINE[I] := PROUTINE[I];
47130         END;
47140       END;
47150   PROCEDURE FIRSTPART;
47160     VAR I:INTEGER;
47170       BEGIN
47180         FOR I := PNONE TO PLAST DO OCODE(I,'DUMMY     ',O,O,O);
47182         OCODE(PPBEGIN+1  , 'ESTART_   '                         , O , O , O );
47185         OCODE(PPBEGIN    , 'START68   '                         , O , O , O );
47190         OCODE(PPEND      , 'STOP68    '                         , O , O , O );
47210         ICODE(PABSI      ,  'DUPL    ' , NON , 0 ,QABSI         , E , O , E );
47215         QCODE(QABSI      ,  'CI      ' , WOP , 0 ,QABSI+1                   );
47220         QCODE(QABSI+1    ,  'IJGE    ' , JMP , 2 ,QABSI+2                   );
47230         QCODE(QABSI+2    ,  'INEG    ' , NON , 0 ,0                         );
47240         ICODE(PABSI-2    ,  'CI      ' , WOP , 0 ,QABSI+3       ,FP0, O ,FP0);
47250         QCODE(QABSI+3    ,  'FLOAT   ' , WOP , 3 ,QABSI+4);
47260         QCODE(QABSI+4    ,  'RGE 0,3 ' , NON , 0 ,QABSI+5);
47270         QCODE(QABSI+5    ,  'JTRUE   ' , JMP , 2 ,QABSI+6);
47272         QCODE(QABSI+6    ,  'RNEG    ' , WOP , 0 ,0);
47274         OCODE(PABSI-4     , 'CABSI     '                        ,PR1,O  ,E  );
47280         ICODE(PABSB      ,  'NULL    ' , NON , 0 ,0             , E , O , E );
47290         ICODE(PABSB-1    ,  'NULL    ' , NON , 0 ,0             , E , O , E );
47300         ICODE(PABSCH     ,  'NULL    ' , NON , 0 ,0             ,ST , O ,ST );
47310         ICODE(PADD       ,  'IADD    ' , NON , 0 ,0             , E , E , E );
47320         ICODE(PADD-2     ,  'RADD 0,1' , NON , 0 ,0             ,FP0,FP1,FP0);
47325         OCODE(PADD-4      , 'CPLUS     '                        ,PR1,PR2, E );
47330 (*+61()
47340         ICODE(PADD-3     ,  '?ADD-3  ' , WOP , 8 ,0             ,ST4,ST4,ST4);
47350 ()+61+)
47360         ICODE(PANDB      ,  'ILAND   ' , NON , 0 ,0             , E , E , E );
47370         ICODE(PANDB-1    ,  'ILAND   ' , NON , 0 ,0             , E , E , E );
47375         OCODE(PARG       ,  'CARG      '                        ,PR1, O , E );
47380         ICODE(PBIN       ,  'NULL    ' , NON , 0 ,0             , E , O , E );
47390         OCODE(PCAT       , 'CATCC     '                         ,PR1,PR2, E );
47400         OCODE(PCAT-1     , 'CATSS     '                         ,PR1,PR2, E );
47405         OCODE(PCONJ      , 'CCONJ     '                         ,PR1, O , E );
47410         ICODE(PDIV       ,  'FLOAT   ' , WOP , 3 ,QDIV          , E , E ,FP0);
47412         QCODE(QDIV       ,  'FLOAT   ' , WOP , 0 ,QDIV+1);
47414         QCODE(QDIV+1     ,  'RDIV 0,3' , NON , 0 ,0);
47420         ICODE(PDIV-2     ,  'RDIV 0,1' , NON , 0 ,0             ,FP0,FP1,FP0);
47425         OCODE(PDIV-4      , 'CDIV      '                        ,PR1,PR2, E );
47430         ICODE(PDIVAB     ,  'RDIV 0,1' , NON , 0 ,0             ,FP0,FP1,FP0);
47435         OCODE(PDIVAB-2    , 'CDIVAB    '                        ,PR1,PR2, E );
47440         ICODE(PELMBT     ,  'EXCH    ' , NON , 0 ,QELMBT        , E , E , E );
47450         QCODE(QELMBT     ,  'CI      ' , WOP , 1 ,QELMBT+1);
47452         QCODE(QELMBT+1   ,  'ISUB    ' , NON , 0 ,QELMBT+2);
47460         QCODE(QELMBT+2   ,  'ISLLT   ' , NON , 0 ,QELMBT+3);
47470         QCODE(QELMBT+3   ,  'CI      ' , WOP , 0 ,QELMBT+4);
47472         QCODE(QELMBT+4   ,  'IGT     ' , NON , 0 ,0);
47480         OCODE(PELMBY     , 'ELMBY     '                         ,PR1,PR2, E );
47490         OCODE(PENTI      , 'ENTIER    '                         ,PR1, O , E );
47500         ICODE(PEQ        ,  'IEQ     ' , NON , 0 ,0             , E , E , E );
47520         ICODE(PEQ-2      ,  'REQ 0,1 ' , NON , 0 ,0             ,FP0,FP1, E );
47525         OCODE(PEQ-4       , 'CEQ       '                        ,PR1,PR2, E );
47530         ICODE(PEQB       ,  'IEQ     ' , NON , 0 ,0             , E , E , E );
47540         ICODE(PEQB-1     ,  'IEQ     ' , NON , 0 ,0             , E , E , E );
47550         ICODE(PEQB-2     ,  'IEQ     ' , NON , 0 ,0             , E , E , E );
47560         ICODE(PEQCS      ,  'IEQ     ' , NON , 0 ,0             , E , E , E );
47570         ICODE(PEQCS-1    ,  'CI      ' , ACP , 2 ,QCFSTRNG      ,PR1,PR2, E );
47590         OCODE(PEXP       ,  'POWI      '                        ,PR1,PR2, E );
47600         OCODE(PEXP-2     ,  'POWR      '                        ,PR1,PR2,FP0);
47605         OCODE(PEXP-4     ,  'CPOW      '                        ,PR1,PR2, E );
47610         ICODE(PPASC      ,  'CALL    ' , OPX , 0 ,0             ,SDL, O , E );
47620         ICODE(PPASC+1    ,  'CALL    ' , OPX , 0 ,0             ,PR1, O , E );
47670         ICODE(PGE        ,  'ILE     ' , NON , 0 ,0             , E , E , E );
47680         ICODE(PGE-2      ,  'RGE 0,1 ' , NON , 0 ,0             ,FP0,FP1, E );
47690         ICODE(PGEBT      ,  'EXCH    ' , NON , 0 ,PLEBT         , E , E , E );
47700         ICODE(PGEBT-1    ,  'ILE     ' , NON , 0 ,0             , E , E , E );
47710         ICODE(PGECS      ,  'ILE     ' , NON , 0 ,0             , E , E , E );
47720         ICODE(PGECS-1    ,  'CI      ' , ACP , 4 ,QCFSTRNG      ,PR1,PR2, E );
47740         ICODE(PGT        ,  'ILT     ' , NON , 0 ,0             , E , E , E );
47760         ICODE(PGT-2      ,  'RGT 0,1 ' , NON , 0 ,0             ,FP0,FP1, E );
47770         ICODE(PGTBY      ,  'ILT     ' , NON , 0 ,0             , E , E , E );
47780         ICODE(PGTCS      ,  'ILT     ' , NON , 0 ,0             , E , E , E );
47790         ICODE(PGTCS-1    ,  'CI      ' , ACP , 5 ,QCFSTRNG      ,PR1,PR2, E );
47795         OCODE(PIM         , 'CIM       '                        ,PR1, O , E );
47800         ICODE(PLE        ,  'IGE     ' , NON , 0 ,0             , E , E , E );
47820         ICODE(PLE-2      ,  'RLE 0,1 ' , NON , 0 ,0             ,FP0,FP1, E );
47830         ICODE(PLEBT      ,  'ILNOT   ' , NON , 0 ,QLEBT         , E , E , E );
47832         QCODE(QLEBT      ,  'ILAND   ' , NON , 0 ,QLEBT+1);
47834         QCODE(QLEBT+1    ,  'INOT    ' , NON , 0 ,0);
47840         ICODE(PLEBT-1    ,  'ILE     ' , NON , 0 ,0             , E , E , E );
47850         ICODE(PLECS      ,  'IGE     ' , NON , 0 ,0             , E , E , E );
47860         ICODE(PLECS-1    ,  'CI      ' , ACP , 1 ,QCFSTRNG      ,PR1,PR2, E );
47920         ICODE(PLT        ,  'IGT     ' , NON , 0 ,0             , E , E , E );
47940         ICODE(PLT-2      ,  'RLT 0,1 ' , NON , 0 ,0             ,FP0,FP1, E );
47950         ICODE(PLTBY      ,  'IGT     ' , NON , 0 ,0             , E , E , E );
47960         ICODE(PLTCS      ,  'IGT     ' , NON , 0 ,0             , E , E , E );
47970         ICODE(PLTCS-1    ,  'CI      ' , ACP , 0 ,QCFSTRNG      ,PR1,PR2, E );
47980         OCODE(PLWBMSTR   , 'LWBMSTR   '                         ,PR1, O , E );
47990         OCODE(PLWBM      , 'LWBM      '                         ,PR1, O , E );
48000         OCODE(PLWB       , 'LWB       '                         ,PR1,PR2, E );
48010         ICODE(PMINUSAB   ,  'ISUB    ' , NON , 0 ,0             , E , E , E );
48020         ICODE(PMINUSAB-2 ,  'RSUB 0,1' , NON , 0 ,0             ,FP0,FP1,FP0);
48025         OCODE(PMINUSAB-4  , 'CMINAB    '                        ,PR1,PR2, E );
48030         OCODE(PMOD       , 'MOD       '                         ,PR1,PR2, E );
48040         OCODE(PMODAB     , 'MOD       '                         ,PR1,PR2, E );
48050         ICODE(PMUL       ,  'IMULT   ' , NON , 0 ,0             , E , E , E );
48060         ICODE(PMUL-2     ,  'RMULT 0,' , WOP , 1 ,0             ,FP0,FP1,FP0);
48070 (*+61()
48080         ICODE(PMUL-3     ,  '?MUL-3  ' , WOP , 8 ,0             ,ST4,ST4,ST4);
48090 ()+61+)
48095         OCODE(PMUL-4     , 'CTIMS     '                         ,PR1,PR2, E );
48100         OCODE(PMULCI     , 'MULCI     '                         ,PR1,PR2, E );
48110         OCODE(PMULCI-1   , 'MULSI     '                         ,PR1,PR2, E );
48120         OCODE(PMULIC     , 'MULIC     '                         ,PR1,PR2, E );
48130         OCODE(PMULIC-1   , 'MULIS     '                         ,PR1,PR2, E );
48140         END;
48150  PROCEDURE SECONDPART;
48160         BEGIN
48170         ICODE(PNE        ,  'INE     ' , NON , 0 ,0             , E , E , E );
48190         ICODE(PNE-2      ,  'RNE 0,1 ' , NON , 0 ,0             ,FP0,FP1, E );
48195         OCODE(PNE-4       , 'CNE       '                        ,PR1,PR2, E );
48210         ICODE(PNEB       ,  'INE     ' , NON , 0 ,0             , E , E , E );
48220         ICODE(PNEB-1     ,  'INE     ' , NON , 0 ,0             , E , E , E );
48230         ICODE(PNEB-2     ,  'INE     ' , NON , 0 ,0             , E , E , E );
48240         ICODE(PNECS      ,  'INE     ' , NON , 0 ,0             , E , E , E );
48250         ICODE(PNECS-1    ,  'CI      ' , ACP , 3 ,QCFSTRNG      ,PR1,PR2, E );
48252         ICODE(PNEGI      ,  'INEG    ' , NON , 0 ,0             , E , O , E );
48260         ICODE(PNEGI-2    ,  'RNEG    ' , WOP , 0 ,0             ,FP0, O ,FP0);
48265         OCODE(PNEGI-4     , 'CNEGI     '                        ,PR1,PR2, E );
48270         ICODE(PNOTB      ,  'INOT    ' , NON , 0 ,0             , E , O , E );
48290         ICODE(PNOTB-1    ,  'ILNOT   ' , NON , 0 ,0             , E , O , E );
48300         ICODE(PNOOP      ,  'NULL    ' , NON , 0 ,0             , E , O ,ST );
48310         ICODE(PNOOP-2    ,  'NULL    ' , NON , 0 ,0             ,FP0, O ,ST );
48320         ICODE(PNOOP-4    ,  'NULL    ' , NON , 0 ,0             , E , O ,ST );
48330         ICODE(PODD       ,  'CI      ' , WOP , 1 ,QODD          , E , O , E );
48340         QCODE(QODD       ,  'ILAND   ' , NON , 0 ,0                         );
48350         ICODE(PORB       ,  'ILOR    ' , NON , 0 ,0             , E , E , E );
48360         ICODE(PORB-1     ,  'ILOR    ' , NON , 0 ,0             , E , E , E );
48370         ICODE(POVER      ,  'IDIV    ' , NON , 0 ,0             , E , E , E );
48380         ICODE(POVERAB    ,  'IDIV    ' , NON , 0 ,0             , E , E , E );
48385         OCODE(PPLITM      , 'CRCOMPLEX '                        ,PR1,PR2, E );
48390         ICODE(PPLSAB     ,  'IADD    ' , NON , 0 ,0             , E , E , E );
48400         ICODE(PPLSAB-2   ,  'RADD 0,1' , NON , 0 ,0             ,FP0,FP1,FP0);
48410 (*+61()
48420         ICODE(PPLSAB-3   ,  '?PLSAB-3' , WOP , 8 ,0             ,ST4,ST4,ST4);
48430 ()+61+)
48435         OCODE(PPLSAB-4    , 'CPLUSAB   '                        ,PR1,PR2, E );
48440         OCODE(PPLSABS    , 'PLABSS    '                         ,PR1,PR2, E );
48450         OCODE(PPLSABS-1  , 'PLABSS    '                         ,PR1,PR2, E );
48460         OCODE(PPLSTOCS   , 'PLTOSS    '                         ,PR1,PR2, E );
48470         OCODE(PPLSTOCS-1 , 'PLTOSS    '                         ,PR1,PR2, E );
48475         OCODE(PRE        , 'CRE       '                         ,PR1,O  , E );
48480         ICODE(PREPR      ,  'NULL    ' , NON , 0 ,0             , E , E , E );
48490         ICODE(PROUN      ,  'ROUND   ' , WOP , 0 ,0             ,FP0, O , E );
48500         OCODE(PSGNI      , 'SIGNI     '                         ,PR1, O , E );
48510         OCODE(PSGNI-2    , 'SIGNR     '                         ,PR1, O , E );
48520         OCODE(PSHL       , 'SHL       '                         ,PR1,PR2, E );
48530 (*+61()
48540         ICODE(PSHRTR     ,  '?SHRTR  ' , WOP , 8 ,QSHRTR        ,ST4, O ,ST2);
48550         QCODE(QSHRTR     ,  'RUBBISH ' , WOP , 4 ,QSHRTR+1                  );
48560         QCODE(QSHRTR+1   ,  'RUBBISH ' , NON , 0 ,0                         );
48570 ()+61+)
48580         OCODE(PSHR       , 'SHR       '                         ,PR1,PR2, E );
48590         ICODE(PSUB       ,  'ISUB    ' , NON , 0 ,0             , E , E , E );
48600         ICODE(PSUB-2     ,  'RSUB 0,1' , NON , 0 ,0             ,FP0,FP1, FP0);
48605         OCODE(PSUB-4     , 'CMINUS    '                         ,PR1,PR2, E );
48610         ICODE(PTIMSAB    ,  'IMULT   ' , NON , 0 ,0             , E , E , E );
48620         ICODE(PTIMSAB-2  ,  'RMULT 0,' , WOP , 1 ,0             ,FP0,FP1,FP0);
48630 (*+61()
48640         ICODE(PTIMSAB-3  ,  '?TIMSAB-' , WOP , 8 ,0             ,ST4,ST4,ST4);
48650 ()+61+)
48655         OCODE(PTIMSAB-4  , 'CTIMSAB   '                         ,PR1,PR2, E );
48660         OCODE(PTIMSABS   , 'MULABSI   '                         ,PR1,PR2, E );
48670         OCODE(PUPBMSTR   , 'UPBMSTR   '                         ,PR1, O , E );
48680         OCODE(PUPBM      , 'UPBM      '                         ,PR1, O , E );
48690         OCODE(PUPB       , 'UPB       '                         ,PR1,PR2, E );
48696         QCODE(QCFSTRNG   ,  'IPUSH   ' , NON , 0 ,QCFSTRNG+1);
48700         OCODE(QCFSTRNG+1 , 'CFSTR     '                         , O , O , O );
48730         ICODE(PSELECT    ,  ' SR0    ' , NON , 0 ,QSELECT       , E , O ,ER0);
48731         QCODE(QSELECT    ,  'LRO0    ' , WOP , 6 ,PSELECT+2);
48734         ICODE(PSELECT+1  ,  ' SR0    ' , NON , 0 ,QSELECT+1     , E , O ,ER0);
48736         QCODE(QSELECT+1  ,  'CI      ' , OPX , 6 ,0);
48740         ICODE(PSELECT+2  ,  'CI      ' , OPX , 0 ,QSELECT+2     ,ER0, O ,ER0);
48742         QCODE(QSELECT+2  ,  'IADD    ' , NON , 0 ,0);
48760         OCODE(PSELECTROW , 'SELECTR   '                         ,PR1, O , E );
48770         OCODE(PSTRNGSLICE, 'STRSUB    '                         ,PR1,PR2, E );
48780         OCODE(PSTRNGSLICE+1, 'STRTRIM   '                       ,PR1, O , E );
48790         OCODE(PSTARTSLICE, 'STARTSL   '                         , O , O , O );
48800         OCODE(PSLICE1    , 'SLICE1    '                         , E , E ,ER0);
48810         OCODE(PSLICE2    , 'SLICE2    '                         , E , E ,ER0);
48820         OCODE(PSLICEN    , 'SLICEN    '                         ,PR1, O ,FP0);
48822         ICODE(PCASE      ,  'JUMP    ' , LCX , 0 ,0             , E , O , O );
48830         ICODE(PCASCOUNT  ,  'CI      ' , WOP , 1 ,QCAS          , O , O , O );
48840         QCODE(QCAS       ,  'ISUB    ' , NON , 0 ,QCAS+1);
48842         QCODE(QCAS+1     ,  'INDXJUMP' , NON , 0 ,QCAS+2);
48844         QCODE(QCAS+2     ,  'ARG     ' , OPX , 0 ,0);
48846         ICODE(PCASJMP    ,  'LAB     ' , LCX , 0 ,0             , O , O , O );
48848         ICODE(PCASJMP+1  ,  'JUMP    ' , LCX , 0 ,0             , O , O , O );
48850         ICODE(PJMPF      ,  'JFALSE  ' , LCX , 0 ,0             , E , O , O );
48860         ICODE(PLPINIT    ,  'LAS     ' , ANX , 0 ,QLPINIT       ,PR1, O , E );
48862         QCODE(QLPINIT    ,  'IPUSH   ' , NON , 0 ,QLPINIT+1);
48864         OCODE(QLPINIT+1  , 'LINIT1    '                         , O , O , O );
48870         ICODE(PLPINIT+1  ,  'LAS     ' , ANX , 0 ,QLPINIT+2     ,PR1, O , E );
48872         QCODE(QLPINIT+2  ,  'IPUSH   ' , NON , 0 ,QLPINIT+3);
48874         OCODE(QLPINIT+3  , 'LINIT2    '                         , O , O , O );
48880         ICODE(PLPINIT+2  ,  'LAS     ' , ANX , 0 ,QLPINIT+4     ,PR1, O , O );
48882         QCODE(QLPINIT+4  ,  'IPUSH   ' , NON , 0 ,QLPINIT+5);
48884         OCODE(QLPINIT+5  , 'LINIT3    '                         , O , O , O );
48890         ICODE(PLPINIT+3  ,  'LAS     ' , ANX , 0 ,QLPINIT+6     ,PR1, O , O );
48892         QCODE(QLPINIT+6  ,  'IPUSH   ' , NON , 0 ,QLPINIT+7);
48894         OCODE(QLPINIT+7  , 'LINIT4    '                         , O , O , O );
48900         ICODE(PLPTEST    ,  'JFALSE  ' , LCX , 0 ,0             , E , O , O );
48902         ICODE(PLPINCR    ,  'LAS     ' , ANX , 0 ,QLOOPINCR+4   , O , O , E );
48904         QCODE(QLOOPINCR+4,  'IPUSH   ' , NON , 0 ,QLOOPINCR+5);
48910         OCODE(QLOOPINCR+5, 'LOOPINC   '                         , O , O , O );
48920         ICODE(PLPINCR+1  ,  'CI      ' , WOP , 1 ,QLOOPINCR     , O , O , E );
48930         QCODE(QLOOPINCR  ,  'LAS     ' , ONX , 0 ,QLOOPINCR+1);
48940         QCODE(QLOOPINCR+1,  'OAADD   ' , NON , 0 ,QLOOPINCR+2);
48950
48960         QCODE(QLOOPINCR+2,  'IL      ' , ONX , 2 ,QLOOPINCR+3);
48964         QCODE(QLOOPINCR+3,  'IGE     ' , NON , 0 ,0);
48966         ICODE(PRANGENT   ,  'LAS     ' , ANX , 0 ,QRANGENT      , O , O , O );
48968         QCODE(QRANGENT   ,  'IPUSH   ' , NON , 0 ,QRANGENT+1);
48970         OCODE(QRANGENT+1 , 'RANGENT   '                         , O , O , O );
48980         OCODE(PRANGEXT   , 'RANGEXT   '                         , O , O , O );
48990         ICODE(PRANGEXT+1 ,  'IL      ' , WNP ,(SIZIBBASE+SIZLEBBASE-8),QRANGEXT, O , O , O );
48992         QCODE(QRANGEXT   ,  ' SR2    ' , NON , 0 ,QRANGEXT+1);
48994         QCODE(QRANGEXT+1 ,  'LRO2    ' , WOP , 8 ,QRANGEXT+2);
49000         QCODE(QRANGEXT+2 ,  ' IS     ' , WNP ,(SIZIBBASE+SIZLEBBASE-8),0   );
49020         OCODE(PRANGEXT+2 , 'RANGXTP   '                         ,STP, O , E );
49022         OCODE(PRECGEN    , 'DORECGE   '                         , O , O , O );
49030         OCODE(PACTDRSTRUCT,'CRSTRUC   '                         , O , O , E );
49040         OCODE(PACTDRMULT , 'CRMULT    '                         ,PR1, O , E );
49050         OCODE(PCHECKDESC , 'CHKDESC   '                         ,PR1,PR2, E );
49080         OCODE(PVARLISTEND, 'GARBAGE   '                         ,PR1, O , O );
49090         ICODE(PVARLISTEND+1,'ASFW    ' , WOP , 2 ,0             ,ST , O , O );
49096         ICODE(PDCLINIT   ,  'LGI     ' , MOR , 0 ,QDCLINIT      , O , O , O );
49097         QCODE(QDCLINIT   ,  '_UNINT  ' , NON , 0 ,0);
49098         ICODE(PDCLINIT+1 ,  'LGI     ' , MOR , 0 ,QDCLINIT+1    , O , O , O );
49099         QCODE(QDCLINIT+1 ,  '_UNDEFIN' , NON , 0 ,0);
49100         ICODE(PDCLINIT+2 ,  'IS      ' , ONX , 2 ,0             , O , O , O );
49106         ICODE(PPARM      ,  'IL      ' , ONX , 0 ,QDCLSP        , O , O , O );
49108
49110
49120         OCODE(PCREATEREF , 'CRREFN    '                         ,PR1, O , E );
49130         OCODE(PCREATEREF+1,'CRRECN    '                         ,PR1, O , E );
49140         OCODE(PCREATEREF+2,'CRREFR    '                         ,PR1, O , E );
49150         OCODE(PCREATEREF+3,'CRRECR    '                         ,PR1, O , E );
49160
49170         ICODE(PDCLSP     ,  ' IS     ' , ONX , 2 ,0             , E , O , O );
49180         ICODE(PDCLSP+1   ,  ' IS     ' , ONX , 2 ,QDCLSP        , E , O , O );
49190         QCODE(QDCLSP     ,  'CI      ' , MOR , 0 ,QDCLSP+1                  );
49200         QCODE(QDCLSP+1   ,  '65536   ' , NON , 0 ,QDCLSP+2                  );
49230         QCODE(QDCLSP+2   ,  'EXCH    ' , NON , 0 ,QDCLSP+3                  );
49232         QCODE(QDCLSP+3   ,  ' OAADD  ' , NON , 0 ,0);
49240         OCODE(PDCLSP+2   , 'DCLSN     '                         ,SNS, O , O );
49250         OCODE(PDCLSP+3   , 'DCLPN     '                         ,SNS, O , O );
49252         ICODE(PFIXRG     ,  'LAS     ' , ONX , 0 ,0             , O , O , O );
49254         ICODE(PFIXRG+1   ,  'IS      ' , ONX , 0 ,0             , O , O , O );
49260         END;
49270     PROCEDURE THIRDPART;
49280         BEGIN
49290         OCODE(PBOUNDS    , 'BOUND     '                         ,STS, O , E );
49300         ICODE(PLOADVAR   ,  'LAS     ' , ACP , 0 ,QLOADVAR      , O , O , E );
49304         QCODE(QLOADVAR   ,  'LAS     ' , ACX , 0 ,QLOADVAR+3);
49310         ICODE(PLOADVAR+1 ,  'LROA3   ' , ACP ,250,QLOADVAR+1    , O , O , E );
49312         QCODE(QLOADVAR+1 ,  'LROA3   ' , ACX ,250,QLOADVAR+3);
49320         ICODE(PLOADVAR+2 ,  'LROA2   ' , ACP ,192,QLOADVAR+2    , O , O , E );
49322         QCODE(QLOADVAR+2 ,  'LROA2   ' , ACX ,192,QLOADVAR+3);
49324         QCODE(QLOADVAR+3 ,  'IPUSH   ' , NON , 0 ,QLOADVAR+4);
49326         QCODE(QLOADVAR+4 ,  'IPUSH   ' , NON , 0 ,QLOADVAR+5);
49328         OCODE(QLOADVAR+5 , 'GLDVAR    '                         , O , O , O );
49330         OCODE(PLOADRT    , 'ROUTN     '                         , O , O , E );
49331         ICODE(PLOADRTA   ,  'LAS     ' , ACX , 0 ,QLOADRTA      , O , O , E );
49332         ICODE(PLOADRTA+1 ,  'LROA3   ' , ACX ,250,QLOADRTA      , O , O , E );
49333         ICODE(PLOADRTA+2 ,  'LROA2   ' , ACX ,192,QLOADRTA      , O , O , E );
49334         QCODE(QLOADRTA   ,  'IPUSH   ' , NON , 0 ,QLOADRTA+1);
49335         OCODE(QLOADRTA+1 , 'ROUTNA    '                         , O , O , O );
49336         OCODE(PLOADRTP   , 'ROUTNP    '                         ,PR1, O , E );
49340         OCODE(PSCOPETT+2 , 'TASSTPT   '                         ,PR1,PR2, E );
49350         OCODE(PSCOPETT+3 , 'SCPTTP    '                         ,PR1,PR2, E );
49360         OCODE(PSCOPETT+4 , 'SCPTTM    '                         ,PR1,PR2, E );
49370         OCODE(PASSIGTT   , 'TASSTS    '                         , E , E , E );
49372         OCODE(PASSIGTT+1 , 'TASSTS2   '                         , E ,FP0, E );
49380         OCODE(PASSIGTT+2 , 'TASSTPT   '                         ,PR1,PR2, E );
49390         OCODE(PASSIGTT+3 , 'TASSTP    '                         ,PR1,PR2, E );
49400         OCODE(PASSIGTT+4 , 'TASSTM    '                         ,PR1,PR2, E );
49410         OCODE(PSCOPETN   , 'SCPTNP    '                         ,PR1,PR2, E );
49420         OCODE(PASSIGTN   , 'TASSNP    '                         ,PR1,PR2, E );
49430         OCODE(PSCOPENT+2 , 'SCPNTPT   '                         ,PR1,PR2,FP0);
49440         OCODE(PSCOPENT+3 , 'SCPNTP    '                         ,PR1,PR2,FP0);
49480         OCODE(PASSIGNT   , 'NASSTS    '                         ,ER0, E ,ER0);
49490         OCODE(PASSIGNT+1 , 'NASSTS2   '                         ,ER0,FP0,ER0);
49500         OCODE(PASSIGNT+2 , 'NASSTPP   '                         ,ER0, E ,ER0);
49520         OCODE(PASSIGNT+3 , 'NASSTP    '                         ,PR1,PR2,FP0);
49530
49540         OCODE(PSCOPENN   , 'SCPNNP    '                         ,PR1,PR2,FP0);
49560         OCODE(PASSIGNN   , 'NASSNP    '                         ,PR1,PR2,FP0);
49580         ICODE(PSCOPEVAR  ,  'LAS     ' , ACP , 0 ,QSCOPEVAR     ,PR1, O , O );
49584         QCODE(QSCOPEVAR  ,  'LAS     ' , ACX , 0 ,QSCOPEVAR+3);
49590         ICODE(PSCOPEVAR+1,  'LROA3   ' , ACP ,250,QSCOPEVAR+1   ,PR1, O , O );
49591         QCODE(QSCOPEVAR+1,  'LROA3   ' , ACX ,250,QSCOPEVAR+3);
49592         ICODE(PSCOPEVAR+2,  'LROA2   ' , ACP ,192,QSCOPEVAR+2   ,PR1, O , O );
49594         QCODE(QSCOPEVAR+2,  'LROA2   ' , ACX ,192,QSCOPEVAR+3);
49596         QCODE(QSCOPEVAR+3,  'IPUSH   ' , NON , 0 ,QSCOPEVAR+4);
49598         QCODE(QSCOPEVAR+4,  'IPUSH   ' , NON , 0 ,QSCOPEVAR+5);
49600         OCODE(QSCOPEVAR+5, 'GVSCOPE   '                         , O , O , O );
49610         OCODE(PSCOPEEXT  , 'SCOPEXT   '                         ,PR1, O , E );
49620         ICODE(PASGVART   ,  ' IS     ' , OPX , 0 ,0             ,E , O , O );
49630         ICODE(PASGVART+1 ,  ' SRO3   ' , OPX ,250,0             ,E , O , O );
49640         ICODE(PASGVART+2 ,  ' SRO2   ' , OPX ,192,0             ,E , O , O );
49660         ICODE(PASGVART+3 ,  'LAS     ' , OPX , 0 ,QASGVART      ,FP0, O , O );
49671         QCODE(QASGVART   ,  ' ASSD   ' , WOP , 0 ,0                         );
49680         ICODE(PASGVART+4 ,  'LROA3   ' , OPX ,250,QASGVART      ,FP0, O , O );
49690         ICODE(PASGVART+5 ,  'LROA2   ' , OPX ,192,QASGVART      ,FP0, O , O );
49710         ICODE(PASGVART+6 ,  'LAS     ' , ACX , 0 ,QASGVART+1    ,PR1, O , O );
49712         QCODE(QASGVART+1 ,  'IPUSH   ' , NON , 0 ,QASGVART+2);
49714         OCODE(QASGVART+2 , 'GVASSTX   '                         , O , O , O );
49720         ICODE(PASGVART+7 ,  'LROA3   ' , ACX ,250,QASGVART+1    ,PR1, O , O );
49730         ICODE(PASGVART+8 ,  'LROA2   ' , ACX ,192,QASGVART+1    ,PR1, O , O );
49740         OCODE(PIDTYREL   , 'IS        '                         ,PR1,PR2, E );
49750         OCODE(PIDTYREL+1 , 'ISNT      '                         ,PR1,PR2, E );
49752         ICODE(PGETTOTCMN ,  'LR0     ' , NON , 0 ,QGETTOTCMN    ,ER0, O ,ER0);
49753         QCODE(QGETTOTCMN ,  'IADD    ' , NON , 0 ,0);
49754         ICODE(PGETTOTCMN+1, 'LRO0    ' , WOP , 2 ,QGETTOTCMN    ,ER0, O ,ER0);
49755         ICODE(PGETTOTCMN+2, 'LRO0    ' , WOP , 4 ,QGETTOTCMN+1  ,ER0, O ,ER0);
49756         QCODE(QGETTOTCMN+1, 'SR1     ' , NON , 0 ,QGETTOTCMN+2);
49757         QCODE(QGETTOTCMN+2, 'JFALSE  ' , JMP , 1 ,QGETTOTCMN+3);
49758         QCODE(QGETTOTCMN+3, 'LRO1    ' , WOP , 2 ,QGETTOTCMN);
49760         ICODE(PGETTOTAL  ,  'LI      ' , NON , 0 ,QGETTOTAL     ,ER0, O , E );
49761
49762
49763
49764         ICODE(PGETTOTAL+1,  'LDI     ' , WOP , 0 ,QGETTOTAL     ,ER0, O ,FP0);
49765
49766
49767
49768         QCODE(QGETTOTAL  ,  'LRO0    ' , WOP , 0 ,QGETTOTAL+1);
49769
49770         QCODE(QGETTOTAL+1,  'CI      ' , MOR , 0 ,QGETTOTAL+2);
49771         QCODE(QGETTOTAL+2,  '65536   ' , NON , 0 ,QGETTOTAL+3);
49772         QCODE(QGETTOTAL+3,  'IJGE    ' , JMP , 2 ,QGETTOTAL+4);
49773         OCODE(QGETTOTAL+4, 'SAVGARB   '                         , O , O , O );
49774         OCODE(PGETTOTAL+2, 'GTOTP     '                         ,PR1, O , E );
49775         OCODE(PGETTOTAL+3, 'GTOTN     '                         ,PR1, O , E );
49776         OCODE(PGETTOTAL+4, 'GTOTREF   '                         ,PR1, O , E );
49778         OCODE(PGETMULT   , 'GETMULT   '                         ,PR1, O , E );
49780         OCODE(PGETMULT+1 , 'GETSLN    '                         ,PR1, O , E );
49782         OCODE(PDEREF     , 'DREFS     '                         ,PR1, O , E );
49784         OCODE(PDEREF+1   , 'DREFS2    '                         ,PR1, O , E );
49786         OCODE(PDEREF+2   , 'DREFPTR   '                         ,PR1, O , E );
49788         OCODE(PDEREF+3   , 'DREFN     '                         ,PR1, O , E );
49790         OCODE(PDEREF+4   , 'DREFM     '                         ,PR1, O , E );
49800         OCODE(PSKIP      , 'SKIPS     '                         , O , O , E );
49810         OCODE(PSKIP+1    , 'SKIPPIL   '                         , O , O , E );
49812         OCODE(PSKIP+2    , 'SKIPS2    '                         , O , O ,FP0);
49820         OCODE(PSKIPSTRUCT, 'SKIPSTR   '                         , O , O , E );
49830         OCODE(PNIL       , 'NILP      '                         , O , O , E );
49840         ICODE(PVOIDNORMAL,  'SR0     ' , NON , 0 ,PVOIDNAKED    , E , O , O );
49843         ICODE(PVOIDNAKED ,  'LRO0    ' , WOP , 0 ,QGETTOTAL+1   ,ER0, O , O );
49844
49845
49846
49847
49848
49900
49910         ICODE(PWIDEN     ,  'FLOAT   ' , WOP , 0 ,0             ,E  , O ,FP0);
49940         OCODE(PWIDEN+2   , 'WIDREAL   '                         ,PR1, O , E );
49950         OCODE(PWIDEN+4   , 'WIDCHAR   '                         ,PR1, O , E );
49960         OCODE(PWIDEN+5   , 'WIDBITS   '                         ,PR1, O , E );
49970         OCODE(PWIDEN+6   , 'WIDBYTS   '                         ,PR1, O , E );
49980         OCODE(PWIDEN+7   , 'WIDSTR    '                         ,PR1, O , E );
49990         OCODE(PROWNONMULT, 'ROWNM     '                         ,PR1, O , E );
50000         OCODE(PROWMULT   , 'ROWM      '                         ,PR1, O , E );
50001         ICODE(PGETPROC   ,  'SFA     ' , NON , 0 ,QGETPROC      , O , O , O );
50002         QCODE(QGETPROC   ,  'CI      ' , ANX , 0 ,QGETPROC+1);
50003         QCODE(QGETPROC+1 ,  'IADD    ' , NON , 0 ,QGETPROC+2);
50004         QCODE(QGETPROC+2 ,  'LI      ' , NON , 0 ,QGETPROC+3);
50005         QCODE(QGETPROC+3 ,  'IPUSH   ' , NON , 0 ,PGETPROC+1);
50006         OCODE(PGETPROC+1 , 'GETPROC   '                         ,PR1, O , O );
50010         ICODE(PCALL      ,  'CI      ' , ACX , 0 ,QCALL         ,SNS, O , O );
50011         QCODE(QCALL      ,  'IPUSH   ' , NON , 0 ,QCALL+1);
50012         QCODE(QCALL+1    ,  'RPUSH   ' , WOP , 0 ,QCALL+2);
50013         QCODE(QCALL+2    ,  'SFA     ' , NON , 0 ,QCALL+3);
50014         QCODE(QCALL+3    ,  'CI      ' , WOP , 2 ,QCALL+4);
50015         QCODE(QCALL+4    ,  'IADD    ' , NON , 0 ,QCALL+5);
50016         QCODE(QCALL+5    ,  'LI      ' , NON , 0 ,QCALL+6);
50017         QCODE(QCALL+6    ,  'LI      ' , NON , 0 ,QCALL+7);
50018         QCODE(QCALL+7    ,  'CALLT   ' , NON , 0 ,0);
50019         ICODE(PCALLA     ,  'SFA     ' , NON , 0 ,QCALLA        ,SNS, O , O );
50020         QCODE(QCALLA     ,  'LAS     ' , ACX , 0 ,QCALLA+3);
50021         ICODE(PCALLA+1   ,  'SFA     ' , NON , 0 ,QCALLA+1      ,SNS, O , O );
50022         QCODE(QCALLA+1   ,  'LROA3   ' , ACX ,250,QCALLA+3);
50023         ICODE(PCALLA+2   ,  'SFA     ' , NON , 0 ,QCALLA+2      ,SNS, O , O );
50024         QCODE(QCALLA+2   ,  'LROA2   ' , ACX ,192,QCALLA+3);
50025         QCODE(QCALLA+3   ,  'IPUSH   ' , NON , 0 ,QCALL+5);
50026         ICODE(PRNSTART   ,  'ASFW    ' , OPX , 0 ,QRNSTART      , O , O , O );
50028         OCODE(QRNSTART   , 'RNSTART   '                         , O , O , O );
50029         ICODE(PRETURN    ,  'RETURN  ' , NON , 0 ,0             ,XN , O , O );
50030         OCODE(PGBSTK     , 'GBSTK     '                         , O , O , O );
50034         OCODE(POUTJUMP   , 'OUTJUMP   '                         , O , O , O );
50040         OCODE(PGETOUT    , 'GETOUT    '                         , O , O , O );
50042         ICODE(PSETIB     ,  'RPUSH   ' , WOP , 0 ,QSETIB        , O , O , O );
50044         OCODE(QSETIB     , 'SETIB     '                         , O , O , O );
50050         OCODE(PLEAPGEN   , 'GENSTR    '                         , O , O , E );
50060         OCODE(PLEAPGEN+1 , 'HEAPSTR   '                         , O , O , E );
50070         OCODE(PLEAPGEN+2 , 'GENRSTR   '                         , O , O , E );
50080         OCODE(PLEAPGEN+3 , 'GENMUL    '                         ,PR1, O , E );
50090         OCODE(PLEAPGEN+4 , 'HEAPMUL   '                         ,PR1, O , E );
50100         OCODE(PLEAPGEN+5 , 'GENRMUL   '                         ,PR1, O , E );
50110         OCODE(PPREPSTRDISP , 'PCOLLST   '                       , O , O ,FP0);
50120         OCODE(PPREPROWDISP , 'PCOLLR    '                       ,STS, O ,FP0);
50130         OCODE(PPREPROWDISP+1, 'PCOLLRM   '                      ,STS, O ,FP0);
50140         OCODE(PCOLLCHECK , 'PCOLLCK   '                         ,S4P, O ,FP0);
50150         ICODE(PCOLLTOTAL ,  'EXCH    ' , NON , 0 ,QCOLLTOTAL    ,ER0, E ,ER0);
50151         QCODE(QCOLLTOTAL  , 'SR1     ' , NON , 0 ,QCOLLTOTAL+1);
50152         QCODE(QCOLLTOTAL+1, 'EXCH    ' , NON , 0 ,QCOLLTOTAL+2);
50153
50154         QCODE(QCOLLTOTAL+2, 'SRO1    ' , OPX , 0 ,QCOLLTOTAL+3);
50155         QCODE(QCOLLTOTAL+3, 'JFALSE  ' , JMP , 1 ,0);
50156
50170         ICODE(PCOLLTOTAL+1, 'SR1     ' , NON , 0 ,QCOLLTOTAL+4  ,ER0,FP0,ER0);
50171
50172
50173         QCODE(QCOLLTOTAL+4, 'LROA1   ' , OPX , 0 ,QCOLLTOTAL+5);
50174         QCODE(QCOLLTOTAL+5, 'ASSD    ' , WOP , 0 ,0);
50175
50200         ICODE(PCOLLTOTAL+2, 'DUPL    ' , NON , 0 ,QCOLLTOTAL+6  ,ER0, E ,ER0);
50210         QCODE(QCOLLTOTAL+6, 'CI      ' , MOR , 0 ,QCOLLTOTAL+7);
50220         QCODE(QCOLLTOTAL+7, '65536   ' , NON , 0 ,QCOLLTOTAL+8);
50222         QCODE(QCOLLTOTAL+8, 'EXCH    ' , NON , 0 ,QCOLLTOTAL+9);
50230         QCODE(QCOLLTOTAL+9, 'OAADD   ' , NON , 0 ,QCOLLTOTAL+10);
50240         QCODE(QCOLLTOTAL+10,'JFALSE  ' , JMP , 1 ,PCOLLTOTAL);
50250         OCODE(PCOLLTOTAL+3,'COLLTP    '                         ,PR1,PR2,FP0);
50260         OCODE(PCOLLTOTAL+4,'COLLTM    '                         ,PR1,PR2,FP0);
50270         OCODE(PCOLLNAKED , 'COLLNP    '                         ,PR1,PR2,FP0);
50280         ICODE(PNAKEDPTR  ,  'JFALSE  ' , JMP , 1 ,QNAKEDPTR     ,ER0, O , E );
50282         QCODE(QNAKEDPTR  ,  'LR0     ' , NON , 0 ,0);
50290         ICODE(PLINE      ,  'CI      ' , OPX , 0 ,QLINE         , O , O , O );
50300         QCODE(QLINE      ,  ' IS     ' , WNP ,12 ,0);
50320         OCODE(PENDSLICE  , 'ENDSL     '                         ,PR1, O , E );
50330         OCODE(PTRIM      , 'SLICEA    '                         , O , O , O );
50340         OCODE(PTRIM+1    , 'SLICEB    '                         , O , O , O );
50350         OCODE(PTRIM+2    , 'SLICEC    '                         , O , O , O );
50360         OCODE(PTRIM+3    , 'SLICED    '                         , O , O , O );
50370         OCODE(PTRIM+4    , 'SLICEE    '                         , O , O , O );
50380         OCODE(PTRIM+5    , 'SLICEF    '                         , O , O , O );
50390         OCODE(PTRIM+6    , 'SLICEG    '                         , O , O , O );
50400         OCODE(PTRIM+7    , 'SLICEH    '                         , O , O , O );
50410         OCODE(PTRIM+8    , 'SLICEI    '                         , O , O , O );
50420         OCODE(PTRIM+9    , 'SLICEJ    '                         , O , O , O );
50430         ICODE(PJMP       ,  'JUMP    ' , LCX , 0 ,0             , O , O , O );
50432         ICODE(PENVCHAIN  ,  'IL      ' , WOP , 4 ,QENVCHAIN     , O , O , O );
50434         QCODE(QENVCHAIN  ,  ' SR2    ' , NON , 0 ,0);
50436         ICODE(PENVCHAIN+1,  'LRO2    ' , WOP ,196,QENVCHAIN     , O , O , O );
50438         ICODE(PDISCARD   ,  'JFALSE  ' , JMP , 1 ,0             , O , O , O );
50440         ICODE(PDUP1ST    ,  'SFA     ' , NON , 0 ,QDUP1ST       ,STP, O , E );
50441         ICODE(PDUP1ST+1  ,  'SFA     ' , NON , 0 ,QDUP2ND       ,ST4, O ,FP1);
50442         QCODE(QDUP1ST    ,  'LI      ' , NON , 0 ,0);
50450         ICODE(PDUP2ND    ,  'SFA     ' , NON , 0 ,QDUP1ST       ,STP, E , E );
50460         ICODE(PDUP2ND+1  ,  'SFA     ' , NON , 0 ,QDUP2ND       ,ST4, E ,FP1);
50464         ICODE(PDUP2ND+2  ,  'SFA     ' , NON , 0 ,QDUP1ST       ,STP,F0P, E );
50466         ICODE(PDUP2ND+3  ,  'SFA     ' , NON , 0 ,QDUP2ND       ,ST4,F0P,FP1);
50468         QCODE(QDUP2ND    ,  'LDI     ' , WOP , 1 ,0);
50470         ICODE(PDATALIST  ,  'CI      ' , OPX , 0 ,QDATALIST     ,SNS, O ,SDL);
50471         QCODE(QDATALIST  ,  'IPUSH   ' , NON , 0 ,PALIGN);
50472         ICODE(PASP       ,  'ASFW    ' , OPX , 0 , 0            , O , O , O );
50474         ICODE(PALIGN     ,  'ALIGN   ' , NON , 0 , 0            , O , O , O );
50476         ICODE(PHEAVE     ,  'SFA     ' , NON , 0 ,QHEAVE        , O , O , O );
50478         QCODE(QHEAVE     ,  'SFA     ' , NON , 0 ,QHEAVE+1);
50480         QCODE(QHEAVE+1   ,  'CI      ' , WOP , 2 ,QHEAVE+2);
50482         QCODE(QHEAVE+2   ,  'ISUB    ' , NON , 0 ,QHEAVE+3);
50484         QCODE(QHEAVE+3   ,  'CI      ' , OPX , 0 ,QHEAVE+4);
50486         QCODE(QHEAVE+4   ,  ' MVW    ' , NON , 0 ,QHEAVE+5);
50487         QCODE(QHEAVE+5   ,  'ASFW    ' , WNP , 2 ,0);
50490         ICODE(PHOIST     ,  'ASFW    ' , ONX , 0 ,QHOIST        , O , O , O );
50492         QCODE(QHOIST     ,  'CI      ' , ACX , 0 ,QHOIST+1);
50493         QCODE(QHOIST+1   ,  'IPUSH   ' , NON , 0 ,QHOIST+2);
50494         OCODE(QHOIST+2   , 'HOIST     '                         , O , O , O );
50496         ICODE(PPUSH      ,  'IL      ' , OPX , 0 ,QIPUSH        , O , O , O );
50498         QCODE(QIPUSH     ,  'IPUSH   ' , NON , 0 , 0                        );
50510         ICODE(PPUSH+1    ,  'LRO3    ' , OPX ,250,QIPUSH        , O , O ,ST );
50512         ICODE(PPUSH+2    ,  'LRO2    ' , OPX ,192,QIPUSH        , O , O , O );
50520         ICODE(PPUSHIM    ,  'CI      ' , OPX , 0 ,QIPUSH        , O , O ,ST );
50530         ICODE(PPUSHIM+1  ,  'LGA     ' , GBX , 0 ,QIPUSH        , O , O , O );
50531         ICODE(PLOADEIM   ,  'CI      ' , OPX , 0 ,0             , O , O , E );
50532         ICODE(PLOADEIM+1 ,  'LGA     ' , GBX , 0 ,0             , O , O , O );
50533         ICODE(PLOADE     ,  'IL      ' , OPX , 0 ,0             , O , O , O );
50534         ICODE(PLOADE+1   ,  'LRO3    ' , OPX ,250,0             , O , O , O );
50535         ICODE(PLOADE+2   ,  'LRO2    ' , OPX ,192,0             , O , O , O );
50550         ICODE(PPUSHIM2   ,  'LGA     ' , OPX , 0 ,QPUSHIM2      , O , O , O ); (*SPECIAL FOR*)
50552         QCODE(QPUSHIM2   ,  'IPUSH   ' , NON , 0 ,QPUSHIM2+1);                 (*MDCHAN AND *)
50554         QCODE(QPUSHIM2+1 ,  'IL      ' , WNP , 2 ,QIPUSH);                     (*MDCODE     *)
50560         ICODE(PPUSHIM2+1 ,  'LGA     ' , GBX , 0 ,QPUSH2        , O , O , O );
50570         ICODE(PPUSHER0   ,  'IPUSH   ' , NON , 0 ,QPUSHER0      , O , O , O );
50572         QCODE(QPUSHER0   ,  'LR0     ' , NON , 0 ,QPUSHER0+1);
50574         QCODE(QPUSHER0+1 ,  'IPUSH   ' , NON , 0 ,0);
50580         ICODE(PLOADER0F0 ,  'RPUSH   ' , WOP , 0 ,PLOADER0STK   , O , O , O );
50582         ICODE(PLOADER0F1 ,  'RPUSH   ' , WOP , 1 ,PLOADER0STK   , O , O , O );
50590         ICODE(PLOADER0STK,  'SFA     ' , NON , 0 ,QLOADER0STK   , O , O , O );
50592         QCODE(QLOADER0STK,  ' SR1    ' , NON , 0 ,QLOADER0STK+1);
50594         QCODE(QLOADER0STK+1,'LRO1    ' , WOP , 0 ,QLOADER0STK+2);
50596         QCODE(QLOADER0STK+2,' SR0    ' , NON , 0 ,QLOADER0STK+3);
50598         QCODE(QLOADER0STK+3,'LRO1    ' , WOP , 2 ,QLOADER0STK+4);
50600         QCODE(QLOADER0STK+4,'ASFW    ' , WOP , 4 ,0);
50640         ICODE(PPUSH2     ,  'LAS     ' , OPX , 0 ,QPUSH2        , O , O , O );
50650         QCODE(QPUSH2     ,  'LDI     ' , WOP , 3 ,QPUSH2+1);
50652         QCODE(QPUSH2+1   ,  'RPUSH   ' , WOP , 3 ,0);
50660         ICODE(PPUSH2+1   ,  'LRO3    ' , OPX ,252,QPUSH2+2      , O , O , O );
50662         QCODE(QPUSH2+2   ,  'IPUSH   ' , NON , 0 ,PPUSH+1);
50670         ICODE(PPUSH2+2   ,  'LRO2    ' , OPX ,194,QPUSH2+3      , O , O , O );
50672         QCODE(QPUSH2+3   ,  'IPUSH   ' , NON , 0 ,PPUSH+2);
50690         ICODE(PDECM      ,  'CI      ' , OPX , 0 ,0             , O , O , O );
50694         QCODE(PDECM+1    ,  ' IS     ' , ONX , 0 ,0);
50696         ICODE(PETOSTK    ,  'IPUSH   ' , NON , 0 ,0             , O , O , O );
50700         ICODE(PETOSTK+1  ,  'ASFW    ' , WNP , 4 ,QETOSTK        , O , O , O );
50701         QCODE(QETOSTK    ,  'SFA     ' , NON , 0 ,QETOSTK+1);
50702         QCODE(QETOSTK+1  ,  'SR1     ' , NON , 0 ,QETOSTK+2);
50703         QCODE(QETOSTK+2  ,  'JFALSE  ' , JMP , 1 ,QETOSTK+3);
50704         QCODE(QETOSTK+3  ,  'SRO1    ' , WOP , 0 ,QETOSTK+4);
50705         QCODE(QETOSTK+4  ,  'JFALSE  ' , JMP , 1 ,QETOSTK+5);
50706         QCODE(QETOSTK+5  ,  'SRO1    ' , WOP , 2 ,0);
50714         OCODE(PETOSTK+2  ,  'ETOSTK    '                        , O , O , O );
50715         ICODE(PSTKTOE    ,  'SFA     ' , NON , 0 ,QLOADI        , O , O , O );
50716         QCODE(QLOADI     ,  'LI      ' , NON , 0 ,QADJSP2                   );
50717         QCODE(QADJSP2    ,  'ASFW    ' , WOP , 2 ,0                         );
50718         ICODE(PSTKTOE+1  ,  'SFA     ' , NON , 0 ,QSTKTOE       , O , O , O );
50719         QCODE(QSTKTOE    ,  ' SR1    ' , NON , 0 ,QSTKTOE+1                 );
50720         QCODE(QSTKTOE+1  ,  'LRO1    ' , WOP , 2 ,QSTKTOE+2                 );
50721         QCODE(QSTKTOE+2  ,  'LRO1    ' , WOP , 0 ,QADJSP4                   );
50722         QCODE(QADJSP4    ,  'ASFW    ' , WOP , 4 ,0                         );
50723         OCODE(PSTKTOE+2  ,  'STKTOE    '                        , O , O , O );
50724         ICODE(PSWAP      ,  'EXCH    ' , NON , 0 ,0             , O , O , O );
50725         ICODE(PPUSHFSTK  ,  'RPUSH   ' , WOP , 0 ,0             , O , O , O );
50726         ICODE(PPUSHFSTK1 ,  'RPUSH   ' , WOP , 1 ,0             , O , O , O );
50727         ICODE(PLOADF     ,  'LAS     ' , OPX , 0 ,QLOADF        , O , O , O );
50728         QCODE(QLOADF     ,  'LDI     ' , WOP , 0 ,0);
50732         ICODE(PLOADFIM   ,  'LGA     ' , GBX , 0 ,QLOADF        , O , O , O );
50734         ICODE(PLOADF1    ,  'LAS     ' , OPX , 0 ,QLOADF1       , O , O , E );
50735         QCODE(QLOADF1    ,  'LDI     ' , WOP , 1 ,0                         );
50739         ICODE(PLOADFIM1  ,  'LGA     ' , GBX , 0 ,QLOADF1       , O , O ,FP1);
50742         ICODE(PLOADFSTK  ,  'RPOP    ' , WOP , 0 ,0             , O , O , O );
50743         ICODE(PLOADFSTK1 ,  'RPOP    ' , WOP , 1 ,0             , O , O , O );
50744         ICODE(PF0TOF1    ,  'RPUSH   ' , WOP , 0 ,PLOADFSTK1    , O , O ,FP1);
50745         ICODE(PF1TOF0    ,  'RPUSH   ' , WOP , 1 ,PLOADFSTK     , O , O ,FP0);
50750          END;
50755   PROCEDURE INITPOPARRAY;
50757     VAR I,J:SBTTYP;
50760       BEGIN
50761       FOR I := SBTSTK TO SBTFPR3 DO
50770         FOR J := SBTVOID TO SBTFPR3 DO
50780           BEGIN
50790           POPARRAY [I,J] := PNONE;
50800           POPARRAY [I,I] := PNOOP;
50810           POPARRAY [I,SBTVOID] :=PNOOP;
50820           POPARRAY [I,SBTVAR ] := PLOADVAR;
50822           POPARRAY [I,SBTPROC] := PLOADRTA;
50824           POPARRAY [I,SBTRPROC]:= PLOADRTA;
50830           END;
50910       POPARRAY[ SBTSTK  , SBTSTK4 ] := PVARLISTEND+1;
50920       POPARRAY[ SBTSTK  , SBTID   ] := PPUSH;
50930       POPARRAY[ SBTSTK  , SBTIDV  ] := PPUSH;
50940       POPARRAY[ SBTSTK  , SBTLIT  ] := PPUSHIM;
50950       POPARRAY[ SBTSTK  , SBTDEN  ] := PPUSHIM;
50951       POPARRAY[ SBTE    , SBTID   ] := PLOADE;
50952       POPARRAY[ SBTE    , SBTIDV  ] := PLOADE;
50954       POPARRAY[ SBTE    , SBTVAR  ] := PLOADVAR;
50956       POPARRAY[ SBTE    , SBTLIT  ] := PLOADEIM;
50958       POPARRAY[ SBTE    , SBTDEN  ] := PLOADEIM;
50960       POPARRAY[ SBTSTK  , SBTDL   ] := PNOOP;
50970       POPARRAY[ SBTSTK4 , SBTID   ] := PPUSH2;
50980       POPARRAY[ SBTSTK4 , SBTIDV  ] := PPUSH2;
51000       POPARRAY[ SBTSTK4 , SBTDEN  ] := PPUSHIM2;
51030       POPARRAY[ SBTSTK  , SBTPR1  ] := PNOOP;
51040       POPARRAY[ SBTSTK  , SBTPR2  ] := PNOOP;
51050       POPARRAY[ SBTSTK4 , SBTPR1  ] := PNOOP;
51060       POPARRAY[ SBTSTK4 , SBTPR2  ] := PNOOP;
51070       POPARRAY[ SBTSTK4 , SBTSTK  ] := PSTOS4;
51071       POPARRAY[ SBTSTK  , SBTE    ] := PETOSTK;
51072       POPARRAY[ SBTE    , SBTSTK  ] := PSTKTOE;
51073       POPARRAY[ SBTSTK4 , SBTFPR0 ] := PPUSHFSTK;
51074       POPARRAY[ SBTSTK4 , SBTFPR1 ] := PPUSHFSTK1;
51075       POPARRAY[ SBTFPR0 , SBTID   ] := PLOADF;
51076       POPARRAY[ SBTFPR0 , SBTIDV  ] := PLOADF;
51077       POPARRAY[ SBTFPR0 , SBTLIT  ] := PLOADFIM;
51078       POPARRAY[ SBTFPR0 , SBTDEN  ] := PLOADFIM-1;
51079       POPARRAY[ SBTFPR1 , SBTID   ] := PLOADF1;
51080       POPARRAY[ SBTFPR1 , SBTIDV  ] := PLOADF1;
51082       POPARRAY[ SBTFPR1 , SBTLIT  ] := PLOADFIM1;
51083       POPARRAY[ SBTFPR1 , SBTDEN  ] := PLOADFIM1-1;
51084       POPARRAY[ SBTFPR0 , SBTSTK4 ] := PLOADFSTK;
51085       POPARRAY[ SBTFPR1 , SBTSTK4 ] := PLOADFSTK1;
51086       POPARRAY[ SBTFPR1 , SBTFPR0 ] := PF0TOF1;
51087       POPARRAY[ SBTFPR0 , SBTFPR1 ] := PF1TOF0;
51090       POPARRAY[ SBTSTK4 , SBTER0  ] := PPUSHER0;
51092       POPARRAY[ SBTSTK4 , SBTSTKR0] := PPUSHER0; (*ACTUALLY, LOAD PUTS IT INTO SBTER0 FIRST*)
51094       POPARRAY[ SBTER0  , SBTSTKR0] := PNOOP; (*ACTUALLY, LOAD PUTS IT INTO SBTER0 FIRST*)
51100       POPARRAY[ SBTER0  , SBTSTK4 ] := PLOADER0STK;
51110       POPARRAY[ SBTER0  , SBTFPR0 ] := PLOADER0F0;
51120       POPARRAY[ SBTER0  , SBTFPR1 ] := PLOADER0F1;
51130       END;
51140   PROCEDURE INITLENARRAY;
51150     VAR I:SBTTYP;
51160       BEGIN
51170       FOR I := SBTSTK TO SBTXN DO LENARRAY[I] := 0;
51180       LENARRAY[SBTSTK ] := SZWORD;
51184       LENARRAY[SBTSTK4] := 2*SZWORD;
51186       LENARRAY[SBTSTKR0]:= SZWORD; (*FOR NAKES VALUES*)
51190       LENARRAY[SBTE   ] := SZWORD;
51191       LENARRAY[SBTER0 ] := 2*SZWORD; (*FOR NAKED VALUES*)
51192       LENARRAY[SBTFPR0] := 2*SZWORD;
51193       LENARRAY[SBTFPR1] := 2*SZWORD;
51194       LENARRAY[SBTFPR2] := 2*SZWORD;
51195       LENARRAY[SBTFPR3] := 2*SZWORD;
51210       END;
51220     BEGIN  (* INITCODES +)
51230     FIRSTPART; SECONDPART; THIRDPART;  INITPOPARRAY; INITLENARRAY;
51240     END;
51250 (*+)
51260 ()+86+)
51270 ()+05*)
59771
59772
59773
60280 (**)
60290 (**)
60300 (**)
60310 (**)
60320 (**)
60340 BEGIN
60360 (*+25() LINELIMIT(OUTPUT,10000); LINELIMIT(LSTFILE,10000); ()+25*)
60375         DUMP(FIRSTSTACK,LASTSTACK);
60380 END  (*+25()     (*$G-+)    ()+25*).
####S
