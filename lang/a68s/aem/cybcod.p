60000 (*  COPYRIGHT 1983 C.H.LINDSEY, UNIVERSITY OF MANCHESTER  *)
60010  (**)
60020  (**)
60030            (*$T-*)
60040  (*+23()
60050  PROCEDURE OCODE(OPCOD:POP;ROUTINE:ALFA);
60060      BEGIN
60070      CODETABLE[OPCOD].ROUTINE:=ROUTINE;
60080      END;
60090  PROCEDURE INITCODES;
60100    VAR I:  INTEGER;
60110      BEGIN
60120      FOR I := PNONE TO PLAST DO
60130        BEGIN CODETABLE[I].ROUTINE := '          '; CODETABLE[I].PR := SBTSTK END;
60140      OCODE (PNONE     , 'PNONE     ');
60150      OCODE (PIM       , 'PIM       ');
60160      OCODE (PRE       , 'PRE       ');
60170      OCODE (PBIN      , 'PBIN      ');
60180      OCODE (PREPR     , 'PREPR     ');
60190      OCODE (PSGNI     , 'PSGNI     ');
60200      OCODE (PSHRTC    , 'PSHRTC    ');
60210      OCODE (PSHRTR    , 'PSHRTR    ');
60220      OCODE (PSHRTI    , 'PSHRTI    ');
60230      OCODE (PLENGC    , 'PLENGC    ');
60240      OCODE (PLENGR    , 'PLENGR    ');
60250      OCODE (PLENGI    , 'PLENGI    ');
60260      OCODE (PODD      , 'PODD      ');
60270      OCODE (PROUNL    , 'PROUNL    ');
60280      OCODE (PROUN     , 'PROUN     ');
60290      OCODE (PENTIL    , 'PENTIL    ');
60300      OCODE (PENTI     , 'PENTI     ');
60310      OCODE (PCONJ     , 'PCONJ     ');
60320      OCODE (PARGL     , 'PARGL     ');
60330      OCODE (PARG      , 'PARG      ');
60340      OCODE (PNOTB     , 'PNOTB     ');
60350      OCODE (PABSCH    , 'PABSCH    ');
60360      OCODE (PABSB     , 'PABSB     ');
60370      OCODE (PABSC     , 'PABSC     ');
60380      OCODE (PABSI     , 'PABSI     ');
60390      OCODE (PNEGI     , 'PNEGI     ');
60400      OCODE (PPLITM    , 'PPLITM    ');
60410      OCODE (PMULIC    , 'PMULIC    ');
60420      OCODE (PMULCI    , 'PMULCI    ');
60430      OCODE (PELMBY    , 'PELMBY    ');
60440      OCODE (PELMBT    , 'PELMBT    ');
60450      OCODE (PSHR      , 'PSHR      ');
60460      OCODE (PSHL      , 'PSHL      ');
60470      OCODE (PLWBMSTR  , 'PLWBMSTR  ');
60480      OCODE (PLWBM     , 'PLWBM     ');
60490      OCODE (PLWB      , 'PLWB      ');
60500      OCODE (PUPBMSTR  , 'PUPBMSTR  ');
60510      OCODE (PUPBM     , 'PUPBM     ');
60520      OCODE (PUPB      , 'PUPB      ');
60530      OCODE (PORB      , 'PORB      ');
60540      OCODE (PANDB     , 'PANDB     ');
60550      OCODE (PMODAB    , 'PMODAB    ');
60560      OCODE (POVERAB   , 'POVERAB   ');
60570      OCODE (PDIVAB    , 'PDIVAB    ');
60580      OCODE (PTIMSABS  , 'PTIMSABS  ');
60590      OCODE (PTIMSAB   , 'PTIMSAB   ');
60600      OCODE (PMINUSAB  , 'PMINUSAB  ');
60610      OCODE (PPLSTOCS  , 'PPLSTOCS  ');
60620      OCODE (PPLSABCH  , 'PPLSABCH  ');
60630      OCODE (PPLSABS   , 'PPLSABS   ');
60640      OCODE (PPLSAB    , 'PPLSAB    ');
60650      OCODE (PCAT      , 'PCAT      ');
60660      OCODE (PGEBT     , 'PGEBT     ');
60670      OCODE (PGECS     , 'PGECS     ');
60680      OCODE (PGE       , 'PGE       ');
60690      OCODE (PGTBY     , 'PGTBY     ');
60700      OCODE (PGTCS     , 'PGTCS     ');
60710      OCODE (PGT       , 'PGT       ');
60720      OCODE (PLEBT     , 'PLABT     ');
60730      OCODE (PLECS     , 'PLECS     ');
60740      OCODE (PLE       , 'PLE       ');
60750      OCODE (PLTBY     , 'PLTBY     ');
60760      OCODE (PLTCS     , 'PLTCS     ');
60770      OCODE (PLT       , 'PLT       ');
60780      OCODE (PNEB      , 'PNEB      ');
60790      OCODE (PNECS     , 'PNECS     ');
60800      OCODE (PNE       , 'PNE       ');
60810      OCODE (PEQB      , 'PEQB      ');
60820      OCODE (PEQCS     , 'PEQCS     ');
60830      OCODE (PEQ       , 'PEQ       ');
60840      OCODE (PEXP      , 'PEXP      ');
60850      OCODE (PMOD      , 'PMOD      ');
60860      OCODE (POVER     , 'POVER     ');
60870      OCODE (PDIV      , 'PDIV      ');
60880      OCODE (PMUL      , 'PMUL      ');
60890      OCODE (PSUB      , 'PSUB      ');
60900      OCODE (PADD      , 'PADD      ');
60910      OCODE (PNOOP     , 'PNOOP     ');
60920      OCODE (PASP      , 'PASP      ');
60930      OCODE (PHOIST    , 'PHOIST    ');
60940      OCODE (PSELECT   , 'PSELECT   ');
60950      OCODE (PSELECTROW, 'PSELECTROW');
60960      OCODE (PSTRNGSLICE , 'PSTRNGSLIC');
60970      OCODE (PSTARTSLICE , 'PSTARTSLIC');
60980      OCODE (PSLICE1   , 'PSLICE1   ');
60990      OCODE (PSLICE2   , 'PSLICE2   ');
61000      OCODE (PSLICEN   , 'PSLICEN   ');
61010      OCODE (PCASE     , 'PCASE     ');
61020      OCODE (PJMPF     , 'PJMPF     ');
61030      OCODE (PLPINIT   , 'PLPINIT   ');
61040      OCODE (PRANGENT  , 'PRANGENT  ');
61050      OCODE (PRANGEXT  , 'PRANGEXT  ');
61060      OCODE (PROUTNENT , 'PROUTNENT ');
61070      OCODE (PACTDRMULT, 'PACTDRMULT');
61080      OCODE (PACTDRSTRUCT, 'PACTDRSTRU');
61090      OCODE (PVARLISTEND , 'PVARLISTEN');
61100      OCODE (PDCLINIT  , 'PDCLINIT  ');
61110      OCODE (PCREATEREF, 'PCREATEREF');
61120      OCODE (PCHECKDESC, 'PCHECKDESC');
61130      OCODE (PDCLSP    , 'PDCLSP    ');
61140      OCODE (PDECM     , 'PDECM     ');
61150      OCODE (PBOUNDS   , 'PBOUNDS   ');
61160      OCODE (PLOADRT   , 'PLOADRT   ');
61170      OCODE (PLOADRTP  , 'PLOADRTP  ');
61180      OCODE (PSCOPETT  , 'PSCOPETT  ');
61190      OCODE (PASSIGTT  , 'PASSIGTT  ');
61200      OCODE (PSCOPETN  , 'PSCOPETN  ');
61210      OCODE (PASSIGTN  , 'PASSIGTN  ');
61220      OCODE (PSCOPENT  , 'PSCOPENT  ');
61230      OCODE (PASSIGNT  , 'PASSIGNT  ');
61240      OCODE (PSCOPENN  , 'PSCOPENN  ');
61250      OCODE (PASSIGNN  , 'PASSIGNN  ');
61260      OCODE (PSCOPEVAR , 'PSCOPEVAR ');
61270      OCODE (PSCOPEEXT , 'PSCOPEEXT ');
61280      OCODE (PLOADVAR  , 'PLOADVAR  ');
61290      OCODE (PASGVART  , 'PASGVART  ');
61300      OCODE (PGETPROC  , 'PGETPROC  ');
61310      OCODE (PIDTYREL  , 'PIDTYREL  ');
61320      OCODE (PDEREF    , 'PDEREF    ');
61330      OCODE (PGETTOTAL , 'PGETTOTAL ');
61332      OCODE (PGETMULT  , 'PGETMULT  ');
61340      OCODE (PGETTOTCMN, 'PGETTOTCMN');
61350      OCODE (PVOIDNAKED, 'PVOIDNAKED');
61360      OCODE (PSKIP     , 'PSKIP     ');
61370      OCODE (PSKIPSTRUCT , 'PSKIPSTRUC');
61380      OCODE (PNIL      , 'PNIL      ');
61390      OCODE (PVOIDNORMAL , 'PVOIDNORMA');
61400      OCODE (PVOIDSPECIAL, 'PVOIDSPECI');
61410      OCODE (PWIDEN    , 'PWIDEN    ');
61420      OCODE (PROWNONMULT , 'PROWNONMUL');
61430      OCODE (PROWMULT  , 'PROWMULT  ');
61440      OCODE (PCALL     , 'PCALL     ');
61450      OCODE (PRETURN   , 'PRETURN   ');
61460      OCODE (PPARBEGIN , 'PPARBEGIN ');
61470      OCODE (PLPINCR   , 'PLPINCR   ');
61480      OCODE (PLPTEST   , 'PLPTEST   ');
61490      OCODE (PGBSTK    , 'PGBSTK    ');
61500      OCODE (PLEAPGEN  , 'PLEAPGEN  ');
61510      OCODE (PSWAP     , 'PSWAP     ');
61520      OCODE (PPREPSTRDISP, 'PPREPSTRDI');
61530      OCODE (PPREPROWDISP, 'PPREPROWDI');
61540      OCODE (PCOLLTOTAL, 'PCOLLTOTAL');
61550      OCODE (PCOLLNAKED, 'PCOLLNAKED');
61560      OCODE (PCOLLCHECK, 'PCOLLCHECK');
61570      OCODE (PLINE     , 'PLINE     ');
61580      OCODE (PENDSLICE , 'PENDSLICE ');
61590      OCODE (PTRIM     , 'PTRIM     ');
61600      OCODE (PJMP      , 'PJMP      ');
61610      OCODE (PPUSH     , 'PPUSH     ');
61620      OCODE (PPUSHIM   , 'PPUSHIM   ');
61630      OCODE (PGETOUT   , 'PGETOUT   ');
61640      OCODE (PSETIB    , 'PSETIB    ');
61650      OCODE (PRNSTART  , 'PRNSTART  ');
61660      OCODE (PPARM     , 'PPARM     ');
61670      OCODE (PNAKEDPTR , 'PNAKEDPTR ');
61680      OCODE (PPBEGIN   , 'PPBEGIN   ');
61690      OCODE (PPEND     , 'PPEND     ');
61710      OCODE (PLAST     , 'PLAST     ');
61720      OCODE (PPASC     , 'PPASC     ');
61730      OCODE (PENVCHAIN , 'PENVCHAIN ');
61740      OCODE (PDUP1ST   , 'PDUP1ST   ');
61750      OCODE (PDUP2ND   , 'PDUP2ND   ');
61760      OCODE (PDATALIST , 'PDATALIST ');
61770      END;
61780  ()+23*)
61790            (************************************)
61800            (*        CYBER  VERSION            *)
61810            (************************************)
61820  PROCEDURE INITCODES;
61830  (*INITIALIZES CODETABLE*)
61840    CONST
61850  (*+61() X12 = SBTX12; X45 = SBTX45; ST2 = SBTSTK2; ()+61*)
61860        X5 = SBTX5; X5S = SBTX5; ST = SBTSTK; O = SBTVOID;
61870        SN = SBTSTKN; SNS = SBTSTKN; SNP = SBTSTKN; SDL = SBTDL; XN = SBTXN;
61880        X5P = SBTX5; STP = SBTSTK; X0S = SBTX0; STS = SBTSTK; X6 = SBTX6;
61890        X0 = SBTX0; X0P = SBTX0; X1 = SBTX1; X1S = SBTX1; X1P = SBTX1;
61910        PPOPTOX0(*2*)=203; PPOPTOX1(*2*)=205; PPUSHX6=207;
61920        PLOADX5IM(*2*)=208; PX5TOX0=210; PX5TOX1=211;
61930        PX6TOX5=212; PX6TOX0=213; PX6TOX1=214; PX1TOX0=215; PLOADX6(*3*)=216;
61940        QELMBT=219; QNORM=220; QNEGI=221; QABSB=222; QABSB1=223;
61950        QNAKEDPTR=224; QDIV=225; QDIV1=226; QDIV2=227; QSWAP=228; QSWAP1=229; QCFSTRNG=230;
61960        QGEBT=231; QRANGENT=232; QWIDEN=233; QLPINIT(*4*)=234; QDCLINIT(*1*)=238;
61970        PPUSHX0=239; QGETPROC=240; QPARAM1A=241;
61980        QPBEGIN(*5*)=242;
61990        QLOADRTA=247; QCHECKDESC=248; QABSI=249; QABSI1=250; QLOADX6=251; QPOPTOX6=252;
62000        QMUL=253; QMUL1=254; QCAS=255; QVOIDNM=256; QVOIDNM1=257; QVOIDNM2=258; QVOIDNM3=259;
62010        QASSIGNT(*2*)=260;QCOLLTOTAL(*5*)=262;QSCOPEVAR(*3*)=267;QLOADVAR(*4*)=270;
62020        QLOADX0=274;QPOPX0=275;QPOPX1=276;
62030        QDCLSP(*4*)=277; QLOOPINCR(*6*)=281;
62040        QSETIB(*2*)=287; QPOP1=289; QPASC(*2*)=290;
62050        QPUSH(*3*)=292; QVASSTX(*4*)=295;
62060        QRANGEXT(*3*)=299; QLINK=302; QENTER=303; QLINE=304;
62070        QNOTB=305; QEQ=306; QEQ1=307; QNE=308; QNE1=309; QCALL(*5*)=310; PPOPTOX5=315; QRNSTART(*3*)=316;
62080        PPUSHX5=319; PLOADX6IM(*2*)=320;PPOPTOX6=322;PX5TOX6=323;
62090        PLOADX5(*3*)=324;
62100        PLOADX0(*4*)=327; PLOADX1(*3*)=331;
62110        QGETTOTCMN(*2*)=334; QGETTOTAL(*6*)=336; QPARM(*5*)=342; QCALLA(*4*)=347;
62120        QSELECT(*5*)=351; QDECM(*5*)=356; PLOADRTA(*3*)=361;
62124        PPUSHX1=364; PX1TOX5=365; PX1TOX6=366;
62130  (*+61()
62140          QASGVART(*5+)=367; PPUSH2(*3+)=372; QPUSH2(*6+)=375;
62150          PLOADX12(*3+)=381; QLOADX12=384; PLOADX45(*3+)=385; QLOADX45=388;
62160          PPUSHX12=389; QPUSHX12(*2+)=390; PPUSHX45=392; QPUSHX45(*2+)=393;
62170          PPOPTOX12=395; QPOPTOX12(*2+)=396; PPOPTOX45=398; QPOPTOX45(*2+)=399;
62180          PX12TOX45=401; QX12TOX45=402; PX45TOX12=400;5QX45TOX12=401; 6               3
62190          QLENGR=402; QMULL(*7+)=403; QADD(*10+)=410;
62200  ()+61*)
62210    VAR I: INTEGER;
62220    PROCEDURE ICODE(OPCOD: POP; COMPASS: ALFA; PNEXT: POP;VP1,VP2,VPR:SBTTYP);
62230    (*WARNING: THIS PROCEDURE WILL NOT COPE WITH ERRONEOUS COMPASS*)
62240        LABEL 99;
62250        CONST SHIFT1=100000B;
62260        VAR CHA: CHAR;
62270            II: INTEGER;
62280            L: PACKED RECORD
62290                CASE INTEGER OF
62300                  1: (FM: PACKED ARRAY [1..2] OF CHAR;
62310                      LJT, LKT: (B, A, X, KK, STAR);
62320                      LI, LJ, LK: 0..7;
62330                      LOP1, LOP2: (PLUS, MINUS, TIMES, OVER, COMMA, MISSING);
62340                      LKP: BOOLEAN;
62350                      LKK: -400000B..377777B);
62360                  2: (LW: INTEGER)
62370                      (*NOTE THAT LW:=0 SETS LI,LJ,LK TO 0, LOP1,LOP2 TO PLUS AND LJT,LKT TO B*)
62380                END;
62390            M: PACKED RECORD
62400                CASE INTEGER OF
62410                  1: (F: 0..7; M: 0..7; I: 0..7; J: 0..7; K: 0..7);
62420                  2: (MW: 0..77777B)
62430                END;
62440          BEGIN WITH L, M DO
62450            BEGIN
62460            LW := 0; MW := 0; LKP := FALSE; LKK := 0;
62470            FM[1] := COMPASS[1]; FM[2] := COMPASS[2];
62480            CHA := COMPASS[3];
62490            IF CHA IN ['0'..'9'] THEN BEGIN LI := ORD(CHA)-ORD('0'); II := 4 END
62500            ELSE II := 3;
62510            WHILE COMPASS[II]=' ' DO
62520              IF II=10 THEN GOTO 99 ELSE II := II+1;
62530            CHA := COMPASS[II];
62540            IF (CHA='-') AND NOT(COMPASS[II+1] IN ['0'..'9']) THEN
62550              BEGIN LOP1 := MINUS; II := II+1; CHA := COMPASS[II] END;
62560            IF CHA IN ['B', 'A', 'X'] THEN
62570              BEGIN
62580              IF CHA='B' THEN LJT := B
62590              ELSE IF CHA='A' THEN LJT := A
62600              ELSE IF CHA='X' THEN LJT := X;
62610              LJ := ORD(COMPASS[II+1])-ORD('0');
62620              II := II+2; CHA := COMPASS[II]
62630              END
62640            ELSE LJT := KK;
62650            LKT := KK;
62660            IF CHA='+' THEN LOP2 := PLUS
62670            ELSE IF CHA='-' THEN LOP2 := MINUS
62680            ELSE IF CHA='*' THEN LOP2 := TIMES
62690            ELSE IF CHA='/' THEN LOP2 := OVER
62700            ELSE IF CHA=',' THEN LOP2 := COMMA
62710            ELSE IF CHA IN ['0'..'9'] THEN
62720              BEGIN LKK := ORD(CHA)-ORD('0'); LKP := TRUE END
62730            ELSE BEGIN LKT := B; LOP2 := MISSING END;
62740              II := II+1; CHA := COMPASS[II];
62750            IF CHA IN ['B', 'A', 'X'] THEN
62760              BEGIN
62770              IF CHA='B' THEN LKT := B
62780              ELSE IF CHA='A' THEN LKT := A
62790              ELSE IF CHA='X' THEN LKT := X;
62800              LK := ORD(COMPASS[II+1])-ORD('0');
62810              II := II+2
62820              END
62830            ELSE LK := 0;
62840            (*READ K*)
62850            WHILE II<=10 DO
62860              BEGIN CHA := COMPASS[II];
62870              IF CHA IN ['0'..'9'] THEN
62880                BEGIN LKK := LKK*10+ORD(CHA)-ORD('0'); LKP := TRUE END
62890              ELSE IF CHA='*' THEN LKT := STAR;
62900              II := II+1
62910              END;
62920            IF LOP2=MINUS THEN
62930              IF LKP THEN
62940              BEGIN LKK := -LKK; LOP2 := PLUS END
62950            ELSE LKK := 1; (*OR ANY ODD NUMBER*)
62960        99: WITH CODETABLE[OPCOD] DO
62970              BEGIN
62980              P1 := VP1;
62990              P2 := VP2;
63000              PR := VPR;
63010              IF (P1=O)AND(P2<>O) THEN WRITELN(OUTPUT,'FAILED ICODE-A');
63020              IF (P2=ST) THEN WRITELN(OUTPUT,'FAILED ICODE-B');
63030              IF FM='LB' THEN
63040                LEN := F0
63050              ELSE IF FM[1]='S' THEN
63060                BEGIN
63070                CASE FM[2] OF
63080                  'A': F := 5;
63090                  'B': F := 6;
63100                  'X': F := 7
63110                  END;
63120                I := LI; J := LJ;
63130                CASE LKT OF
63140             STAR,KK: BEGIN
63150                      LEN := F30;
63160                      CASE LJT OF
63170                        A: M := 0;
63180                     KK,B: M := 1;
63190                        X: M := 2
63200                        END
63210                      END;
63220                  B:  BEGIN
63230                      LEN := F15;
63240                      CASE LJT OF
63250                        X: M := 3;
63260                        A: M := 4;
63270                        B: M := 6;
63280                        END;
63290                      IF LOP2=MINUS THEN M := M+1;
63300                      K := LK
63310                      END
63320                  END
63330                END
63340              ELSE IF FM='BX' THEN
63350                BEGIN
63360                F := 1; LEN := F15;
63370                I := LI; K := LJ;
63380                IF LKT=B (*I.E. ABSENT*) THEN
63390                  BEGIN M := 0; J := LJ END
63400                ELSE
63410                  BEGIN
63420                  CASE LOP2 OF
63430                    TIMES: M := 1;
63440                    PLUS: M := 2;
63450                    MINUS: M := 3
63460                    END;
63470                  J := LK
63480                  END;
63490                IF LOP1=MINUS THEN M := M+4
63500                END
63510              ELSE IF (FM[1] IN ['F', 'D', 'R', 'I', 'C']) AND (FM[2]='X') THEN
63520                BEGIN
63530                LEN := F15;
63540                I := LI; J := LJ; K := LK;
63550                IF LOP2 IN [PLUS, MINUS] THEN
63560                  BEGIN
63570                  F := 3;
63580                  CASE FM[1] OF
63590                    'F': M := 0;
63600                    'D': M := 2;
63610                    'R': M := 4;
63620                    'I': M := 6
63630                    END;
63640                  IF LOP2=MINUS THEN M := M+1
63650                  END
63660                ELSE
63670                  BEGIN F := 4;
63680                  CASE FM[1] OF
63690                    'F': M := 0;
63700                    'R': M := 1;
63710                    'D': M := 2;
63720                    'C': BEGIN M := 7; K := LJ END
63730                    END;
63740                  IF LOP2=OVER THEN M := M+4
63750                  END
63760                END
63770              ELSE IF (FM[1] IN ['M','L','A','N','Z','U','P']) AND (FM[2]='X') THEN
63780                BEGIN
63790              IF LKP THEN
63800                  BEGIN
63810                  MW := LKK; (*SET JK*)
63820                  CASE FM[1] OF
63830                    'M': BEGIN F := 4; M := 3 END;
63840                    'L': BEGIN F := 2; M := 0 END;
63850                    'A': BEGIN F := 2; M := 1 END
63860                    END
63870                  END
63880                ELSE
63890                  BEGIN F := 2;
63900                IF LKT=X THEN BEGIN J := LJ; K := LK END
63910                ELSE BEGIN J := LK; K := LJ END;
63920                  CASE FM[1] OF
63930                    'L': M := 2;
63940                    'A': M := 3;
63950                    'N': M := 4;
63960                    'Z': M := 5;
63970                    'U': M := 6;
63980                    'P': M := 7
63990                    END
64000                  END;
64010                LEN := F15; I := LI
64020                END
64030              ELSE (*JUMP*)
64040                BEGIN F := 0;
64050                LEN := F30;
64060                IF LJT=X THEN
64070                  BEGIN M := 3; J := LJ;
64080                  IF FM='ZR' THEN I := 0
64090                  ELSE IF FM='NZ' THEN I := 1
64100                  ELSE IF FM='PL' THEN I := 2
64110                  ELSE IF FM='NG' THEN I := 3
64120                  ELSE IF FM='IR' THEN I := 4
64130                  ELSE IF FM='OR' THEN I := 5
64140                  ELSE IF FM='DF' THEN I := 6
64150                  ELSE IF FM='ID' THEN I := 7
64160                  ELSE HALT
64170                  END
64180                ELSE
64190                  BEGIN I := LJ; J := LK;
64200                  IF FM='PS' THEN M := 0
64210                  ELSE IF FM='RJ' THEN M := 1
64220                  ELSE IF FM='JP' THEN M := 2
64230                  ELSE IF FM='EQ' THEN M := 4
64240                  ELSE IF FM='NE' THEN M := 5
64250                  ELSE IF FM='GE' THEN M := 6
64260                  ELSE IF FM='LE' THEN BEGIN M := 6; I := LJ; J := LI END
64270                  ELSE IF FM='LT' THEN M := 7
64280                  ELSE IF FM='GT' THEN BEGIN M := 7; I := LJ; J := LI END
64290                  ELSE IF FM='NO' THEN BEGIN F := 4; M := 6; LEN := F15 END
64300                  ELSE HALT
64310                  END
64320                END;
64330              REL := 0;
64340              IF LEN=F15 THEN FMIJK := MW
64350              ELSE IF (LKP) AND (LKT<>STAR) THEN
64360                IF LKK>=0 THEN FMIJK := MW*SHIFT1+LKK
64370                ELSE FMIJK := MW*SHIFT1+LKK+1000000B
64380              ELSE IF LEN=F30 THEN
64390                IF (LKT <> STAR) AND (LOP2<>MISSING) THEN
64400                  BEGIN LEN := F30K; FMIJK := MW*SHIFT1+LKK END
64410                ELSE BEGIN FMIJK := MW*SHIFT1; REL := LKK END;
64420              INLINE := TRUE;
64430              NEXT := PNEXT
64440              END
64450            END
64460          END;
64470    PROCEDURE OCODE(OPCOD: POP; PROUTINE: ALFA;VP1,VP2,VPR:SBTTYP);
64480        VAR I: INTEGER;
64490          BEGIN
64500          WITH CODETABLE[OPCOD] DO
64510            BEGIN
64520            P1 := VP1;
64530            P2 := VP2;
64540            PR := VPR;
64550            IF (P1=O)AND(P2<>O) THEN WRITELN(OUTPUT,'FAILED OCODE-A');
64560            IF P2=ST THEN WRITELN(OUTPUT,'FAILED OCODE-B');
64570            INLINE := FALSE;
64580            LINKINS := NIL;
64590            FOR I := 1 TO 7 DO ROUTINE[I] := PROUTINE[I]
64600            END
64610          END;
64620  (**)
64630    PROCEDURE QCODE (OPCOD:POP; COMPASS:ALFA; PNEXT:POP );
64640        BEGIN ICODE(OPCOD, COMPASS, PNEXT, O, O ,O )  END;
64650  (**)
64660    PROCEDURE FIRSTPART;
64670      VAR I: INTEGER;
64680        BEGIN FOR I := PNONE TO PLAST DO OCODE(I, 'DUMMY     ', O , O , O );
64690  (**)
64700        ICODE(PPBEGIN     , 'SB7 2     ', QPBEGIN      ,O  ,O  ,O  );
64710        QCODE(QPBEGIN     , 'RJ  B0+   ', 0);
64720        ICODE(PPBEGIN+1   , 'SB6 B2+   ', QPBEGIN+1    ,O  ,O  ,O  );
64730        QCODE(QPBEGIN+1   , 'SB7 B6+100', QPBEGIN+2);
64740        QCODE(QPBEGIN+2   , 'SA0 5     ', QPBEGIN+3);
64750        QCODE(QPBEGIN+3   , 'GEB7,B4,41', QPBEGIN+4);
64760        OCODE(QPBEGIN+4   , 'START68   '               ,O  ,O  ,O  );
64770        OCODE(PPEND       , 'STOP68    '               ,O  ,O  ,O  );
64780        OCODE(PPOP        , '          '               ,O  ,O  ,O  );
64790        ICODE(PABSI       , 'BX3 X1    ', QABSI        ,X1 ,O  ,X1 );
64800        QCODE(QABSI       , 'AX3 59    ', QABSI1);
64810        QCODE(QABSI1      , 'BX1 X1-X3 ', 0);
64820        ICODE(PABSI-2     , 'BX3 X1    ', QABSI        ,X1 ,O  ,X1 );
64830        OCODE(PABSI-4     , 'CABSI     '               ,X0 ,O  ,X6 );
64840        ICODE(PABSB       , 'MX3 1     ', QABSB        ,X1 ,O  ,X1 );
64850        QCODE(QABSB       , 'BX1 X1*X3 ', QABSB1);
64860        QCODE(QABSB1      , 'LX1 1     ', 0);
64870        ICODE(PABSB-1     , 'NO        ', 0            ,X1 ,O  ,X1 );
64880        ICODE(PABSCH      , 'NO        ', 0            ,X1 ,O  ,X1 );
64890        ICODE(PADD        , 'IX1 X5+X1 ', 0            ,X5 ,X1 ,X1 );
64900        ICODE(PADD-2      , 'RX1 X5+X1 ', QNORM        ,X5 ,X1 ,X1 );
64910  (*+61()
64920        ICODE(PADD-3      , 'FX3 X1+X4 ', QADD         ,X45,X12,X12);
64930        QCODE(QADD        , 'DX4 X1+X4 ', QADD+1);
64940        QCODE(QADD+1      , 'NX3 X3    ', QADD+2);
64950        QCODE(QADD+2      , 'RX5 X2+X5 ', QADD+3);
64960        QCODE(QADD+3      , 'RX5 X4+X5 ', QADD+4);
64970        QCODE(QADD+4      , 'FX4 X3+X5 ', QADD+5);
64980        QCODE(QADD+5      , 'NX4 X4    ', QADD+6);
64990        QCODE(QADD+6      , 'DX5 X3+X5 ', QADD+7);
65000        QCODE(QADD+7      , 'NX5 X5    ', QADD+8);
65010        QCODE(QADD+8      , 'FX1 X4+X5 ', QADD+9);
65020        QCODE(QADD+9      , 'DX2 X4+X5 ', 0);
65030  ()+61*)
65040        OCODE(PADD-4      , 'CPLUS     '               ,X0 ,X1 ,X6 );
65050        ICODE(PANDB       , 'BX1 X1*X5 ', 0            ,X5 ,X1 ,X1 );
65060        ICODE(PANDB-1     , 'BX1 X1*X5 ', 0            ,X5 ,X1 ,X1 );
65070        OCODE(PARG        , 'CARG      '               ,X0 ,O  ,X6 );
65080        ICODE(PBIN        , 'NO        ', 0            ,X1 ,O  ,X1 );
65090        OCODE(PCAT        , 'CATCC     '               ,X0 ,X1 ,X6 );
65100        OCODE(PCAT-1      , 'CATSS     '               ,X0 ,X1 ,X6 );
65110        OCODE(PCONJ       , 'CCONJ     '               ,X0 ,O  ,X6 );
65120        ICODE(PDIV        , 'PX5 X5    ', QDIV         ,X5 ,X1 ,X1 );
65130        QCODE(QDIV        , 'NX5 X5    ', QDIV1);
65140        QCODE(QDIV1       , 'PX1 X1    ', QDIV2);
65150        QCODE(QDIV2       , 'NX1 X1    ', PDIV-2);
65160        ICODE(PDIV-2      , 'RX1 X5/X1 ', 0            ,X5 ,X1 ,X1 );
65170        OCODE(PDIV-4      , 'CDIV      '               ,X0 ,X1 ,X6 );
65180        ICODE(PDIVAB      , 'RX1 X5/X1 ', 0            ,X5 ,X1 ,X1 );
65190        OCODE(PDIVAB-2    , 'CDIVAB    '               ,X0 ,X1 ,X6 );
65200        ICODE(PELMBT      , 'SB3 X5-1  ', QELMBT       ,X5 ,X1 ,X1 );
65210        QCODE(QELMBT      , 'LX1 B3,X1 ', 0);
65220        OCODE(PELMBY      , 'ELEMBY    '               ,X5 ,X1 ,X1 );
65230        OCODE(PENTI       , 'ENTIER    '               ,X1 ,O  ,X1 );
65240        ICODE(PEQ         , 'IX3 X1-X5 ', QEQ          ,X5 ,X1 ,X1 );
65250        QCODE(QEQ         , 'IX1 X5-X1 ', QEQ1);
65260        QCODE(QEQ1        , 'BX1 -X1-X3', 0);
65270        ICODE(PEQ-2       , 'IX3 X1-X5 ', QEQ          ,X5 ,X1 ,X1 );
65280        OCODE(PEQ-4       , 'CEQ       '               ,X0 ,X1 ,X6 );
65290        ICODE(PEQB        , 'BX1 -X1-X5', 0            ,X5 ,X1 ,X1 );
65300        ICODE(PEQB-1      , 'IX3 X1-X5 ', QEQ          ,X5 ,X1 ,X1 );
65310        ICODE(PEQB-2      , 'IX3 X1-X5 ', QEQ          ,X5 ,X1 ,X1 );
65320        ICODE(PEQCS       , 'IX3 X1-X5 ', QEQ          ,X5 ,X1 ,X1 );
65330        ICODE(PEQCS-1     , 'SX2 2     ', QCFSTRNG     ,X0 ,X1 ,X6 );
65340        OCODE(PEXP        , 'POWI      '               ,X5 ,X1 ,X1 );
65350        OCODE(PEXP-2      , 'POWR      '               ,X5 ,X1 ,X1 );
65360        OCODE(PEXP-4      , 'CPOW      '               ,X0 ,X1 ,X6 );
65370        ICODE(PPASC       , 'SX6 B5    ', QPASC        ,SDL,O  ,X6 );
65380        ICODE(PPASC+1     , 'SX6 B5    ', QPASC        ,X0S,O  ,X6 );
65390        ICODE(PPASC+2     , 'SX6 B5    ', QPASC        ,X0S,X1 ,X6 );
65400        OCODE(PPASC+3     , 'PASC      '               ,STS,O  ,X6 );
65410        QCODE(QPASC       , 'SX7 2*    ', QPASC+1);
65420        QCODE(QPASC+1     , 'EQ  B0+   ', 0);
65430        ICODE(PENVCHAIN   , 'SA3 B5    ', 0            ,O  ,O  ,O  );
65440        ICODE(PENVCHAIN+1 , 'SA3 X3    ', 0            ,O  ,O  ,O  );
65450        ICODE(PGE         , 'IX1 X5-X1 ', PNOTB        ,X5 ,X1 ,X1 );
65460        ICODE(PGE-2       , 'IX1 X5-X1 ', PNOTB        ,X5 ,X1 ,X1 );
65470        ICODE(PGEBT       , 'BX1 -X5*X1', QGEBT        ,X5 ,X1 ,X1 );
65480        QCODE(QGEBT       , 'BX5 X5-X5 ', PEQ);
65490        ICODE(PGEBT-1     , 'IX1 X5-X1 ', PNOTB        ,X5 ,X1 ,X1 );
65500        ICODE(PGECS       , 'IX1 X5-X1 ', PNOTB        ,X5 ,X1 ,X1 );
65510        ICODE(PGECS-1     , 'SX2 4     ', QCFSTRNG     ,X0 ,X1 ,X6 );
65520        ICODE(PGT         , 'IX1 X1-X5 ', 0            ,X5 ,X1 ,X1 );
65530        ICODE(PGT-2       , 'IX1 X1-X5 ', 0            ,X5 ,X1 ,X1 );
65540        ICODE(PGTBY       , 'IX1 X1-X5 ', 0            ,X5 ,X1 ,X1 );
65550        ICODE(PGTCS       , 'IX1 X1-X5 ', 0            ,X5 ,X1 ,X1 );
65560        ICODE(PGTCS-1     , 'SX2 5     ', QCFSTRNG     ,X0 ,X1 ,X6 );
65570        OCODE(PIM         , 'CIM       '               ,X0 ,O  ,X6 );
65580        ICODE(PLE         , 'IX1 X1-X5 ', PNOTB        ,X5 ,X1 ,X1 );
65590        ICODE(PLE-2       , 'IX1 X1-X5 ', PNOTB        ,X5 ,X1 ,X1 );
65600        ICODE(PLEBT       , 'BX1 -X1*X5', QGEBT        ,X5 ,X1 ,X1 );
65610        ICODE(PLEBT-1     , 'IX1 X1-X5 ', PNOTB        ,X5 ,X1 ,X1 );
65620        ICODE(PLECS       , 'IX1 X1-X5 ', PNOTB        ,X5 ,X1 ,X1 );
65630        ICODE(PLECS-1     , 'SX2 B1    ', QCFSTRNG     ,X0 ,X1 ,X6 );
65640  (*+61()
65650        ICODE(PLENGR      , 'BX2 X2-X2 ', QLENGR       ,X1 ,O  ,X12);
65660        QCODE(QLENGR      , 'DX2 X1+X2 ', 0);
65670  ()+61*)
65680        ICODE(PLT         , 'IX1 X5-X1 ', 0            ,X5 ,X1 ,X1 );
65690        ICODE(PLT-2       , 'IX1 X5-X1 ', 0            ,X5 ,X1 ,X1 );
65700        ICODE(PLTBY       , 'IX1 X5-X1 ', 0            ,X5 ,X1 ,X1 );
65710        ICODE(PLTCS       , 'IX1 X5-X1 ', 0            ,X5 ,X1 ,X1 );
65720        ICODE(PLTCS-1     , 'SX2 B0    ', QCFSTRNG     ,X0 ,X1 ,X6 );
65730        OCODE(PLWBMSTR    , 'LWBMSTR   '               ,X0 ,O  ,X6 );
65740        OCODE(PLWBM       , 'LWBM      '               ,X0 ,O  ,X6 );
65750        OCODE(PLWB        , 'LWB       '               ,X0 ,X1 ,X6 );
65760        ICODE(PMINUSAB    , 'IX1 X5-X1 ', 0            ,X5 ,X1 ,X1 );
65770        ICODE(PMINUSAB-2  , 'RX1 X5-X1 ', QNORM        ,X5 ,X1 ,X1 );
65780        OCODE(PMINUSAB-4  , 'CMINAB    '               ,X0 ,X1 ,X6 );
65790        OCODE(PMOD        , 'MOD       '               ,X5 ,X1 ,X1 );
65800        OCODE(PMODAB      , 'MOD       '               ,X5 ,X1 ,X1 );
65810        ICODE(PMUL        , 'DX1 X1*X5 ', QMUL         ,X5 ,X1 ,X1 );
65820        QCODE(QMUL        , 'BX3 X3-X3 ', QMUL1);
65830        QCODE(QMUL1       , 'IX1 X1+X3 ', 0);
65840        ICODE(PMUL-2      , 'RX1 X1*X5 ', 0            ,X5 ,X1 ,X1 );
65850  (*+61()
65860        ICODE(PMUL-3      , 'RX2 X2*X4 ', QMULL        ,X45,X12,X12);
65870        QCODE(QMULL       , 'RX5 X1*X5 ', QMULL+1);
65880        QCODE(QMULL+1     , 'RX2 X2+X5 ', QMULL+2);
65890        QCODE(QMULL+2     , 'FX3 X1*X4 ', QMULL+3);
65900        QCODE(QMULL+3     , 'DX4 X1*X4 ', QMULL+4);
65910        QCODE(QMULL+4     , 'RX4 X4+X2 ', QMULL+5);
65920        QCODE(QMULL+5     , 'FX1 X3+X4 ', QMULL+6);
65930        QCODE(QMULL+6     , 'DX2 X3+X4 ', 0);
65940  ()+61*)
65950        OCODE(PMUL-4      , 'CTIMS     '               ,X0 ,X1 ,X6 );
65960        OCODE(PMULCI      , 'MULCI     '               ,X0 ,X1 ,X6 );
65970        OCODE(PMULCI-1    , 'MULSI     '               ,X0 ,X1 ,X6 );
65980        OCODE(PMULIC      , 'MULIC     '               ,X0 ,X1 ,X6 );
65990        OCODE(PMULIC-1    , 'MULIS     '               ,X0 ,X1 ,X6 );
66000        ICODE(PNE         , 'IX3 X1-X5 ', QNE          ,X5 ,X1 ,X1 );
66010        QCODE(QNE         , 'IX1 X5-X1 ', QNE1);
66020        QCODE(QNE1        , 'BX1 X1-X3 ', 0);
66030        ICODE(PNE-2       , 'IX3 X1-X5 ', QNE          ,X5 ,X1 ,X1 );
66040        OCODE(PNE-4       , 'CNE       '               ,X0 ,X1 ,X6 );
66050        ICODE(PNEB        , 'BX1 X1-X5 ', 0            ,X5 ,X1 ,X1 );
66060        ICODE(PNEB-1      , 'IX3 X1-X5 ', QNE          ,X5 ,X1 ,X1 );
66070        ICODE(PNEB-2      , 'IX3 X1-X5 ', QNE          ,X5 ,X1 ,X1 );
66080        ICODE(PNECS       , 'IX3 X1-X5 ', QNE          ,X5 ,X1 ,X1 );
66090        ICODE(PNECS-1     , 'SX2 3     ', QCFSTRNG     ,X0 ,X1 ,X6 );
66100        ICODE(PNEGI       , 'BX3 X3-X3 ', QNEGI        ,X1 ,O  ,X1 );
66110        QCODE(QNEGI       , 'IX1 X3-X1 ', 0);
66120        ICODE(PNEGI-2     , 'BX3 X3-X3 ', QNEGI        ,X1 ,O  ,X1 );
66130        OCODE(PNEGI-4     , 'CNEGI     '               ,X0 ,O  ,X6 );
66140        ICODE(PNOTB       , 'MX3   1   ', QNOTB        ,X1 ,O  ,X1 );
66150        QCODE(QNOTB       , 'BX1 X3-X1 ', 0);
66160        ICODE(PNOTB-1     , 'BX3 X3-X3 ', QEQ1         ,X1 ,O  ,X1 );
66170        ICODE(PNOOP       , 'NO        ', 0            ,X1 ,O  ,X1 );
66180        ICODE(PNOOP-2     , 'NO        ', 0            ,X1 ,O  ,X1 );
66190        ICODE(PNOOP-4     , 'NO        ', 0            ,X1 ,O  ,X1 );
66200        ICODE(PODD        , 'LX1   59  ', 0            ,X1 ,O  ,X1 );
66210        ICODE(PORB        , 'BX1 X1+X5 ', 0            ,X5 ,X1 ,X1 );
66220        ICODE(PORB-1      , 'BX1 X1+X5 ', 0            ,X5 ,X1 ,X1 );
66230        OCODE(POVER       , 'OVER      '               ,X5 ,X1 ,X1 );
66240        OCODE(POVERAB     , 'OVER      '               ,X5 ,X1 ,X1 );
66250        OCODE(PPLITM      , 'CRCOMPLEX '               ,X0 ,X1 ,X6 );
66260        ICODE(PPLSAB      , 'IX1 X5+X1 ', 0            ,X5 ,X1 ,X1 );
66270        ICODE(PPLSAB-2    , 'RX1 X5+X1 ', QNORM        ,X5 ,X1 ,X1 );
66280  (*+61()
66290        ICODE(PPLSAB-3    , 'FX3 X1+X4 ', QADD         ,X45,X12,X12);
66300  ()+61*)
66310        OCODE(PPLSAB-4    , 'CPLUSAB   '               ,X0 ,X1 ,X6 );
66320        OCODE(PPLSABS     , 'PLABSS    '               ,X0 ,X1 ,X6 );
66330        OCODE(PPLSABS-1   , 'PLABSS    '               ,X0 ,X1 ,X6 );
66340        OCODE(PPLSTOCS    , 'PLTOSS    '               ,X0 ,X1 ,X6 );
66350        OCODE(PPLSTOCS-1  , 'PLTOSS    '               ,X0 ,X1 ,X6 );
66360        OCODE(PRE         , 'CRE       '               ,X0 ,O  ,X6 );
66370        ICODE(PREPR       , 'NO        ', 0            ,X1 ,O  ,X1 );
66380        OCODE(PROUN       , 'ROUN      '               ,X1 ,O  ,X1 );
66390        OCODE(PSGNI       , 'SIGN      '               ,X1 ,O  ,X1 );
66400        OCODE(PSGNI-2     , 'SIGN      '               ,X1 ,O  ,X1 );
66410        OCODE(PSHL        , 'SHL       '               ,X5 ,X1 ,X1 );
66420  (*+61()
66430        ICODE(PSHRTR      , 'RX1 X1+X2 ', QNORM        ,X12,O  ,X1 );
66440  ()+61*)
66450        OCODE(PSHR        , 'SHR       '               ,X5 ,X1 ,X1 );
66460        ICODE(PSUB        , 'IX1 X5-X1 ', 0            ,X5 ,X1 ,X1 );
66470        ICODE(PSUB-2      , 'RX1 X5-X1 ', QNORM        ,X5 ,X1 ,X1 );
66480        OCODE(PSUB-4      , 'CMINUS    '               ,X0 ,X1 ,X6 );
66490        ICODE(PTIMSAB     , 'DX1 X5*X1 ', 0            ,X5 ,X1 ,X1 );
66500        ICODE(PTIMSAB-2   , 'RX1 X1*X5 ', 0            ,X5 ,X1 ,X1 );
66510  (*+61()
66520        ICODE(PTIMSAB-3   , 'RX2 X2*X4 ', QMULL        ,X45,X12,X12);
66530  ()+61*)
66540        OCODE(PTIMSAB-4   , 'CTIMSAB   '               ,X0 ,X1 ,X6 );
66550        OCODE(PTIMSABS    , 'MULABSI   '               ,X0 ,X1 ,X6 );
66560        OCODE(PUPBMSTR    , 'UPBMSTR   '               ,X0 ,O  ,X6 );
66570        OCODE(PUPBM       , 'UPBM      '               ,X0 ,O  ,X6 );
66580        OCODE(PUPB        , 'UPB       '               ,X0 ,X1 ,X6 );
66590        OCODE(QCFSTRNG    , 'CFSTR     '               ,O  ,O  ,O  );
66600        QCODE(QNORM       , 'NX1 B0,X1 ', 0);
66610        END;
66620    PROCEDURE SECONDPART;
66630        BEGIN
66640        ICODE(PGETPROC    , 'SA3 B6+   ', QGETPROC     ,O  ,O  ,O  );
66650        QCODE(QGETPROC    , 'BX0 X3    ', PGETPROC+1);
66660        OCODE(PGETPROC+1  , 'GETPROC   '               ,X0 ,O  ,O  );
66670        ICODE(PSELECT     , 'SA3 X1+B1 ', QSELECT+1    ,X1 ,O  ,X1 );
66672        QCODE(QSELECT+1   , 'AX3 25    ', QSELECT+2);
66673        QCODE(QSELECT+2   , 'SX3 X3+   ', QSELECT+3);
66674        QCODE(QSELECT+3   , 'LX1 42    ', QSELECT+4);
66676        QCODE(QSELECT+4   , 'BX1 X1+X3 ', 0);
66678        ICODE(PSELECT+1   , 'SX3 B1+   ', QSELECT+3    ,X1 ,O  ,X1 );
66680        ICODE(PSELECT+2   , 'SX3   B0+ ', QSELECT      ,X0 ,O  ,X0 );
66690        QCODE(QSELECT     , 'IX0 X0+X3 ', 0);
66700        OCODE(PSELECTROW  , 'SELECTR   '               ,X0 ,O  ,X6 );
66710        OCODE(PSTRNGSLICE , 'STRSUB    '               ,X0 ,X1 ,X6 );
66720        OCODE(PSTRNGSLICE+1,'STRTRIM   '               ,X0S,O  ,X6 );
66730        OCODE(PSTARTSLICE , 'STARTSL   '               ,STP,O  ,O  );
66740        OCODE(PSLICE1     , 'SLICE1    '               ,X0 ,X1 ,X0 );
66750        OCODE(PSLICE2     , 'SLICE2    '               ,X0S,X1 ,X0 );
66760        OCODE(PSLICEN     , 'SLICEN    '               ,X0S,O  ,X6 );
66770        ICODE(PCASE       , 'SA3 +     ', QCAS         ,X1 ,O  ,O  );
66780        OCODE(QCAS        , 'CASE      '               ,O  ,O  ,O  );
66785        ICODE(PCASJMP     , 'EQ    B0, ', 0            ,O  ,O  ,O  );
66787        ICODE(PCASJMP+1   , 'EQ    B0, ', 0            ,O  ,O  ,O  );
66790        ICODE(PJMPF       , 'PL    X1, ', 0            ,X1 ,O  ,O  );
66800        ICODE(PLPINIT     , 'SX1 B5+   ', QLPINIT      ,X0S,O  ,X6 );
66810        OCODE(QLPINIT     , 'LINIT1    '               ,O  ,O  ,O  );
66820        ICODE(PLPINIT+1   , 'SX1 B5+   ', QLPINIT+1    ,X0S,O  ,X6 );
66830        OCODE(QLPINIT+1   , 'LINIT2    '               ,O  ,O  ,O  );
66840        ICODE(PLPINIT+2   , 'SX1 B5+   ', QLPINIT+2    ,X0S,O  ,O  );
66850        OCODE(QLPINIT+2   , 'LINIT3    '               ,O  ,O  ,O  );
66860        ICODE(PLPINIT+3   , 'SX1 B5+   ', QLPINIT+3    ,X0S,O  ,O  );
66870        OCODE(QLPINIT+3   , 'LINIT4    '               ,O  ,O  ,O  );
66880        ICODE(PLPTEST     , 'ZR    X6, ', 0            ,X6 ,O  ,O  );
66888        ICODE(PLPINCR     , 'SX0 B5+   ', QLOOPINCR+5  ,O  ,O  ,X6 );
66890        OCODE(QLOOPINCR+5 , 'LOOPINC   '               ,O  ,O  ,O  );
66900        ICODE(PLPINCR+1   , 'SA4 B5+   ', QLOOPINCR    ,O  ,O  ,X6 );
66910        QCODE(QLOOPINCR   , 'SX3 B1    ', QLOOPINCR+1);
66920        QCODE(QLOOPINCR+1 , 'IX7 X4+X3 ', QLOOPINCR+2);
66930        QCODE(QLOOPINCR+2 , 'SA7 A4    ', QLOOPINCR+3);
66940        QCODE(QLOOPINCR+3 , 'SA3 A4+B1 ', QLOOPINCR+4);
66950        QCODE(QLOOPINCR+4 , 'IX6 X3-X4 ', 0);
66960        ICODE(PRANGENT    , 'SX2 B5+   ', QRANGENT     ,O  ,O  ,O  );
66970        OCODE(QRANGENT    , 'RANGENT   '               ,O  ,O  ,O  );
66980        OCODE(PRANGEXT    , 'RANGEXT   '               ,O  ,O  ,O  );
66990        ICODE(PRANGEXT+1  , 'SA3 B5+12 ', QRANGEXT     ,O  ,O  ,O  );
67000        QCODE(QRANGEXT    , 'SA2 X3+2  ', QRANGEXT+1);
67010        QCODE(QRANGEXT+1  , 'BX7 X2    ', QRANGEXT+2);
67020        QCODE(QRANGEXT+2  , 'SA7 A3    ', 0);
67030        OCODE(PRANGEXT+2  , 'RANGXTP   '               ,X0 ,O  ,X6 );
67032        OCODE(PRECGEN     , 'DORECGE   '               ,O  ,O  ,O  );
67040        OCODE(PACTDRMULT  , 'CRMULT    '               ,X0 ,O  ,X6 );
67050        OCODE(PACTDRSTRUCT, 'CRSTRUC   '               ,O  ,O  ,X6 );
67060        OCODE(PCHECKDESC  , 'CHKDESC   '               ,X0 ,X1 ,X6 );
67070        OCODE(PVARLISTEND , 'GARBAGE   '               ,X0 ,O  ,O  );
67080        ICODE(PVARLISTEND+1,'SB6 B6-B1 ', 0            ,ST ,O  ,O  );
67090        ICODE(PDCLINIT    , 'SA3 B2+328', QDCLINIT     ,O  ,O  ,O  ); (*FIRSTVAR*)
67100        QCODE(QDCLINIT    , 'BX7 X3    ',0);
67110        ICODE(PDCLINIT+1  , 'SA3 B2+329', QDCLINIT     ,O  ,O  ,O  ); (*FIRSTVAR+1*)
67120        ICODE(PDCLINIT+2  , 'SA7 B5+   ',0             ,O  ,O  ,O  );
67130        ICODE(PPARM       , 'SA3 B5+   ', QPARM        ,O  ,O  ,O  );
67140        QCODE(QPARM       , 'SA2 X3    ', QPARM+1);
67150        QCODE(QPARM+1     , 'SX7 B1    ', QPARM+2);
67160        QCODE(QPARM+2     , 'LX7 47    ', QPARM+3);
67170        QCODE(QPARM+3     , 'IX7 X2+X7 ', QPARM+4);
67180        QCODE(QPARM+4     , 'SA7 A2    ', 0);
67210        OCODE(PCREATEREF  , 'CRREFN    '               ,X0 ,O  ,X6 );
67220        OCODE(PCREATEREF+1, 'CRRECN    '               ,X0 ,O  ,X6 );
67230        OCODE(PCREATEREF+2, 'CRREFR    '               ,X0 ,O  ,X6 );
67240        OCODE(PCREATEREF+3, 'CRRECR    '               ,X0 ,O  ,X6 );
67260        ICODE(PDCLSP      , 'SA6 B5+   ', 0            ,X6 ,O  ,O  );
67270        ICODE(PDCLSP+1    , 'SA3 X6    ', QDCLSP       ,X6 ,O  ,O  );
67280        QCODE(QDCLSP      , 'SX7 B1    ', QDCLSP+1);
67290        QCODE(QDCLSP+1    , 'LX7 47    ', QDCLSP+2);
67300        QCODE(QDCLSP+2    , 'IX7 X3+X7 ', QDCLSP+3);
67310        QCODE(QDCLSP+3    , 'SA7 A3    ', PDCLSP);
67320        OCODE(PDCLSP+2    , 'DCLSN     '               ,SNS,O  ,O  );
67330        OCODE(PDCLSP+3    , 'DCLPN     '               ,SNS,O  ,O  );
67340        ICODE(PFIXRG      , 'SX7 B5+   ', 0            , O ,O  ,O  );
67350        ICODE(PFIXRG+1    , 'SA7 B5+   ', 0            , O ,O  ,O  );
67360        OCODE(PBOUNDS     , 'BOUND     '               ,STS,O  ,X6 );
67370        ICODE(PLOADVAR    , 'SX1 B5+   ', QLOADVAR     ,O  ,O  ,X6 );
67380        QCODE(QLOADVAR    , 'SX2 B5    ', QLOADVAR+1);
67390        OCODE(QLOADVAR+1  , 'GLDVAR    '               ,O  ,O  ,O  );
67400        ICODE(PLOADVAR+1  , 'SX1 B2+   ', QLOADVAR+2   ,O  ,O  ,X6 );
67410        QCODE(QLOADVAR+2  , 'SX2 B2+345', QLOADVAR+1);                (*FIRSTIBOFFSET*)
67420        ICODE(PLOADVAR+2  , 'SX1 X3+   ', QLOADVAR+3   ,O  ,O  ,X6 );
67430        QCODE(QLOADVAR+3  , 'SX2 X3    ', QLOADVAR+1);
67440        OCODE(PLOADRT     , 'ROUTN     '               ,O  ,O  ,X6 );
67450        ICODE(PLOADRTA    , 'SX1 B5+   ', QLOADRTA     ,O  ,O  ,X6 );
67460        ICODE(PLOADRTA+1  , 'SX1 B2+   ', QLOADRTA     ,O  ,O  ,X6 );
67470        ICODE(PLOADRTA+2  , 'SX1 X3+   ', QLOADRTA     ,O  ,O  ,X6 );
67480        OCODE(QLOADRTA    , 'ROUTNA    '               ,O  ,O  ,O  );
67490        OCODE(PLOADRTP    , 'ROUTNP    '               ,X0 ,O  ,X6 );
67500        OCODE(PSCOPETT+2  , 'TASSTPT   '               ,X0 ,X1 ,X6 );
67510        OCODE(PSCOPETT+3  , 'SCPTTP    '               ,X0 ,X1 ,X6 );
67520        OCODE(PSCOPETT+4  , 'SCPTTM    '               ,X0 ,X1 ,X6 );
67530        OCODE(PASSIGTT    , 'TASSTS    '               ,X0 ,X1 ,X6 );
67540  (*+61()
67550        OCODE(PASSIGTT+1  , 'TASSTS2   '               ,X0 ,X12,X6 );
67560  ()+61*)
67570        OCODE(PASSIGTT+2  , 'TASSTPT   '               ,X0 ,X1 ,X6 );
67580        OCODE(PASSIGTT+3  , 'TASSTP    '               ,X0 ,X1 ,X6 );
67590        OCODE(PASSIGTT+4  , 'TASSTM    '               ,X0 ,X1 ,X6 );
67600        OCODE(PSCOPETN    , 'SCPTNP    '               ,X0 ,X1 ,X6 );
67610        OCODE(PASSIGTN    , 'TASSNP    '               ,X0 ,X1 ,X6 );
67620        OCODE(PSCOPENT+2  , 'SCPNTPT   '               ,X0 ,X1 ,X6 );
67630        OCODE(PSCOPENT+3  , 'SCPNTP    '               ,X0 ,X1 ,X6 );
67640        OCODE(PASSIGNT    , 'NASSTS    '               ,X0 ,X1 ,X6 );
67650        OCODE(PASSIGNT+1  , 'NASSTS2   '               ,X0 ,X1 ,X6 );
67660        OCODE(PASSIGNT+2  , 'NASSTPT   '               ,X0 ,X1 ,X6 );
67670        OCODE(PASSIGNT+3  , 'NASSTP    '               ,X0 ,X1 ,X6 );
67690        OCODE(PSCOPENN    , 'SCPNNP    '               ,X0 ,X1 ,X6 );
67700        OCODE(PASSIGNN    , 'NASSNP    '               ,X0 ,X1 ,X6 );
67710        ICODE(PSCOPEVAR   , 'SX2 B5+   ', QSCOPEVAR    ,X0 ,O  ,O  );
67720        QCODE(QSCOPEVAR   , 'SX3 B5    ', QSCOPEVAR+1);
67730        OCODE(QSCOPEVAR+1 , 'GVSCOPE   '               ,O  ,O  ,O  );
67740        ICODE(PSCOPEVAR+1 , 'SX2 B2+   ', QSCOPEVAR+2  ,X0 ,O  ,O  );
67750        QCODE(QSCOPEVAR+2 , 'SX3 B2+345', QSCOPEVAR+1);
67760        ICODE(PSCOPEVAR+2 , 'SX2 X3+   ', QSCOPEVAR+1  ,X0 ,O  ,O  );
67770        OCODE(PSCOPEEXT   , 'SCOPEXT   '               ,X0 ,O  ,X6 );
67780        ICODE(PASGVART    , 'SA6   B5+ ', 0            ,X6 ,O  ,O  );
67790        ICODE(PASGVART+1  , 'SA6   B2+ ', 0            ,X6 ,O  ,O  );
67800        ICODE(PASGVART+2  , 'SA6   X3+ ', 0            ,X6 ,O  ,O  );
67810  (*+61()
67820        ICODE(PASGVART+3  , 'BX7 X1    ', QASGVART     ,X12,O  ,O  );
67830        QCODE(QASGVART    , 'SA7 B5+   ', QASGVART+1);
67840        QCODE(QASGVART+1  , 'BX7 X2    ', QASGVART+2);
67850        QCODE(QASGVART+2  , 'SA7 A7+B1 ', 0);
67860        ICODE(PASGVART+4  , 'BX7 X1    ', QASGVART+3   ,X12,O  ,O  );
67870        QCODE(QASGVART+3  , 'SA7 B2+   ', QASGVART+1);
67880        ICODE(PASGVART+5  , 'BX7 X1    ', QASGVART+4   ,X12,O  ,O  );
67890        QCODE(QASGVART+4  , 'SA7 X3+   ', QASGVART+1);
67900  ()+61*)
67910        ICODE(PASGVART+6  , 'SX1 B5+   ', QVASSTX      ,X0 ,O  ,O  );
67920        OCODE(QVASSTX     , 'GVASSTX   '               ,O  ,O  ,O  );
67930        ICODE(PASGVART+7  , 'SX1 B2+   ', QVASSTX      ,X0 ,O  ,O  );
67940        ICODE(PASGVART+8  , 'SX1 X3+   ', QVASSTX      ,X0 ,O  ,O  );
67950        OCODE(PIDTYREL    , 'IS        '               ,X0 ,X1 ,X6 );
67960        OCODE(PIDTYREL+1  , 'ISNT      '               ,X0 ,X1 ,X6 );
67980        ICODE(PGETTOTCMN  , 'BX1 X0    ', QGETTOTCMN   ,X0 ,O  ,X1 );
67990        QCODE(QGETTOTCMN  , 'AX0 42    ', QGETTOTCMN+1);
68000        QCODE(QGETTOTCMN+1, 'IX1 X1+X0 ', 0);
68005        OCODE(PGETTOTCMN+1, 'GTOTMUL   '               ,X0 ,O  ,X1 );
68010        OCODE(PGETTOTCMN+2, 'GTOTRFR   '               ,X0 ,O  ,X1 );
68030        ICODE(PGETTOTAL   , 'SA5 X1    ', QGETTOTAL    ,X1 ,O  ,X5 );
68040        QCODE(QGETTOTAL   , 'AX1 42    ', QGETTOTAL+1);
68050        QCODE(QGETTOTAL+1 , 'SA3 X1    ', QGETTOTAL+2);
68060        QCODE(QGETTOTAL+2 , 'AX3 47    ', QGETTOTAL+3);
68070        QCODE(QGETTOTAL+3 , 'NZ  X3,2* ', QGETTOTAL+4);
68080        OCODE(QGETTOTAL+4 , 'SAVGARB   '               ,O  ,O  ,O  );
68090  (*+61()
68100        ICODE(PGETTOTAL+1 , 'SA4 X1    ', QGETTOTAL+5  ,X1 ,O  ,X45);
68110        QCODE(QGETTOTAL+5 , 'SA5 A4+B1 ', QGETTOTAL+1);
68120  ()+61*)
68130        OCODE(PGETTOTAL+2 , 'GTOTP     '               ,X0 ,O  ,X6 );
68140        OCODE(PGETTOTAL+3 , 'GTOTN     '               ,X0 ,O  ,X6 );
68150        OCODE(PGETTOTAL+4 , 'GTOTREF   '               ,X0 ,O  ,X6 );
68152        OCODE(PGETMULT    , 'GETMULT   '               ,X0 ,O  ,X6 );
68154        OCODE(PGETMULT+1  , 'GETSLN    '               ,X0 ,O  ,X6 );
68160        OCODE(PDEREF      , 'DREFS     '               ,X0 ,O  ,X6 );
68170        OCODE(PDEREF+2    , 'DREFPTR   '               ,X0 ,O  ,X6 );
68180        OCODE(PDEREF+3    , 'DREFN     '               ,X0 ,O  ,X6 );
68190        OCODE(PDEREF+4    , 'DREFM     '               ,X0 ,O  ,X6 );
68200        OCODE(PSKIP       , 'SKIPS     '               ,O  ,O  ,X6 );
68210        OCODE(PSKIP+1     , 'SKIPPIL   '               ,O  ,O  ,X6 );
68220        OCODE(PSKIPSTRUCT , 'SKIPSTR   '               ,O  ,O  ,X6 );
68230        OCODE(PNIL        , 'NILP      '               ,O  ,O  ,X6 );
68240        ICODE(PVOIDNORMAL , 'SA3 X1    ', QVOIDNM      ,X1 ,O  ,O  );
68250        QCODE(QVOIDNM     , 'AX3 47    ', QVOIDNM1);
68260        QCODE(QVOIDNM1    , 'NZ  X3,3* ', QVOIDNM2);
68270        QCODE(QVOIDNM2    , 'SX0 A3    ', QVOIDNM3);
68280        OCODE(QVOIDNM3    , 'GARBAGE   '               ,O  ,O  ,O  );
68290        ICODE(PVOIDNAKED  , 'LX1 18    ', PVOIDNORMAL  ,X1 ,O  ,O  );
68300        ICODE(PWIDEN      , 'PX1 X1    ', QWIDEN       ,X1 ,O  ,X1 );
68310        QCODE(QWIDEN      , 'NX1 X1    ', 0);
68320        OCODE(PWIDEN+2    , 'WIDREAL   '               ,X0 ,O  ,X6 );
68330        OCODE(PWIDEN+4    , 'WIDCHAR   '               ,X0 ,O  ,X6 );
68340        OCODE(PWIDEN+5    , 'WIDBITS   '               ,X0 ,O  ,X6 );
68350        OCODE(PWIDEN+6    , 'WIDBYTS   '               ,X0 ,O  ,X6 );
68360        OCODE(PWIDEN+7    , 'WIDSTR    '               ,X0 ,O  ,X6 );
68370        OCODE(PROWNONMULT , 'ROWNM     '               ,X0 ,O  ,X6 );
68380        OCODE(PROWMULT    , 'ROWM      '               ,X0 ,O  ,X6 );
68390        ICODE(PCALL       , 'SX1 B0+   ', QCALL        ,SNS,O  ,O  );
68400        QCODE(QCALL       , 'SA5 X6    ', QCALL+1);
68410        QCODE(QCALL+1     , 'AX6 42    ', QCALL+2);
68420        QCODE(QCALL+2     , 'SB7 X5    ', QCALL+3);
68430        QCODE(QCALL+3     , 'SX7 2*    ', QCALL+4);
68440        QCODE(QCALL+4     , 'JP  B7    ', 0);
68450        ICODE(PCALLA      , 'SX6 B5+   ', QCALLA       ,SNS,O  ,O  );
68460        ICODE(PCALLA+1    , 'SX6 B2+   ', QCALLA       ,SNS,O  ,O  );
68470        ICODE(PCALLA+2    , 'SX6 X3+   ', QCALLA       ,SNS,O  ,O  );
68480        QCODE(QCALLA      , 'SA5 X2    ', QCALLA+1);
68490        QCODE(QCALLA+1    , 'SB7 X5    ', QCALLA+2);
68500        QCODE(QCALLA+2    , 'SX7 2*    ', QCALLA+3);
68510        QCODE(QCALLA+3    , 'JP  B7    ', 0);
68520        ICODE(PRNSTART    , 'SA6 B6    ', QRNSTART     ,O  ,O  ,O  );
68530        QCODE(QRNSTART    , 'BX3 X7    ', QRNSTART+1);
68540        QCODE(QRNSTART+1  , 'SX4 B0+   ', QRNSTART+2);
68550        OCODE(QRNSTART+2  , 'RNSTART   '               ,O  ,O  ,O  );
68560        OCODE(PRETURN     , 'RETURN    '               ,XN ,O  ,O  );
68570        OCODE(PGBSTK      , 'GBSTK     '               ,O  ,O  ,O  );
68580        OCODE(PGETOUT     , 'GETOUT    '               ,O  ,O  ,O  );
68590        ICODE(PSETIB      , 'SB5 X6    ', QSETIB       ,O  ,O  ,O  );
68600        QCODE(QSETIB      , 'LX6 18    ', QSETIB+1);
68610        QCODE(QSETIB+1    , 'SB6 X6    ', 0);
68620        OCODE(PLEAPGEN    , 'GENSTR    '               ,O  ,O  ,X6 );
68630        OCODE(PLEAPGEN+1  , 'HEAPSTR   '               ,O  ,O  ,X6 );
68640        OCODE(PLEAPGEN+2  , 'GENRSTR   '               ,O  ,O  ,X6 );
68650        OCODE(PLEAPGEN+3  , 'GENMUL    '               ,X0 ,O  ,X6 );
68660        OCODE(PLEAPGEN+4  , 'HEAPMUL   '               ,X0 ,O  ,X6 );
68670        OCODE(PLEAPGEN+5  , 'GENRMUL   '               ,X0 ,O  ,X6 );
68680        OCODE(PPREPSTRDISP, 'PCOLLST   '               ,O  ,O  ,X6 );
68690        OCODE(PPREPROWDISP, 'PCOLLR    '               ,STS,O  ,X6 );
68700        OCODE(PPREPROWDISP+1,'PCOLLRM   '              ,STS,O  ,X6 );
68710        OCODE(PCOLLCHECK  , 'PCOLLCK   '               ,X0 ,O  ,X6 );
68720  (**)
68730        END;
68740    PROCEDURE THIRDPART;
68750        BEGIN
68760        ICODE(PCOLLTOTAL  , 'SA4 B6-B1 ', QCOLLTOTAL   ,ST ,X6 ,ST );
68770        QCODE(QCOLLTOTAL  , 'SA6 X4+   ', 0);
68780        ICODE(PCOLLTOTAL+2, 'SA4 X6    ', QCOLLTOTAL+1 ,ST ,X6 ,ST );
68790        QCODE(QCOLLTOTAL+1, 'SX7 B1    ', QCOLLTOTAL+2);
68800        QCODE(QCOLLTOTAL+2, 'LX7 47    ', QCOLLTOTAL+3);
68810        QCODE(QCOLLTOTAL+3, 'IX7 X4+X7 ', QCOLLTOTAL+4);
68820        QCODE(QCOLLTOTAL+4, 'SA7 A4    ', PCOLLTOTAL);
68830        OCODE(PCOLLTOTAL+3, 'COLLTP    '               ,X0 ,X1 ,X6 );
68840        OCODE(PCOLLTOTAL+4, 'COLLTM    '               ,X0 ,X1 ,X6 );
68850        OCODE(PCOLLNAKED  , 'COLLNP    '               ,X0 ,X1 ,X6 );
68860        ICODE(PNAKEDPTR   , 'LX1 18    ', QNAKEDPTR    ,X1 ,O  ,X6 );
68862        QCODE(QNAKEDPTR   , 'SX6 X1    ', 0);
68870        ICODE(PLINE       , 'SX7   B0+ ', QLINE        ,O  ,O  ,O  );
68880        QCODE(QLINE       , 'SA7  B5+9 ', 0);
68890        OCODE(PENDSLICE   , 'ENDSL     '               ,X0 ,O  ,X0 );
68900        OCODE(PTRIM       , 'SLICEA    '               ,STP,O  ,O  );
68910        OCODE(PTRIM+1     , 'SLICEB    '               ,STP,O  ,O  );
68920        OCODE(PTRIM+2     , 'SLICEC    '               ,STP,O  ,O  );
68930        OCODE(PTRIM+3     , 'SLICED    '               ,STP,O  ,O  );
68940        OCODE(PTRIM+4     , 'SLICEE    '               ,STP,O  ,O  );
68950        OCODE(PTRIM+5     , 'SLICEF    '               ,STP,O  ,O  );
68960        OCODE(PTRIM+6     , 'SLICEG    '               ,STP,O  ,O  );
68970        OCODE(PTRIM+7     , 'SLICEH    '               ,STP,O  ,O  );
68980        OCODE(PTRIM+8     , 'SLICEI    '               ,STP,O  ,O  );
68990        OCODE(PTRIM+9     , 'SLICEJ    '               ,STP,O  ,O  );
69000        ICODE(PJMP        , 'EQ    B0, ', 0            ,O  ,O  ,O  );
69010        ICODE(PDUP1ST     , 'SA1 B6-B1 ', 0            ,STP,O  ,X1 );
69010        ICODE(PDUP1PILE   , 'SA1 B6-B1 ', 0            ,STP,O  ,X1 );
69020        ICODE(PDUP2ND     , 'SA1 B6-B1 ', 0            ,STP,X5P,X1 );
69020        ICODE(PDUP2PILE   , 'SA1 B6-B1 ', 0            ,STP,X5P,X1 );
69030  (*+61() ICODE(PDUP2ND+1   , 'SA1 B6-B1 ', 0            ,STP,X45,X1 ); ()+61*)
69040        ICODE(PDATALIST   , 'SX7 B0+   ', QPUSH+1      ,SNS,O  ,SDL);
69050        OCODE(PHOIST      , 'HOIST     '               ,O  ,O  ,O  );
69060        ICODE(PSTATICLINK , 'SX6 B5    ', 0            ,O  ,O  ,O  );
69070        ICODE(PASP        , 'SB6 B6-   ', 0            ,O  ,O  ,O  );
69080        ICODE(PLOADX5     , 'SA5   B5+ ', 0            ,O  ,O  ,O  );
69090        ICODE(PLOADX5+1   , 'SA5   B2+ ', 0            ,O  ,O  ,O  );
69100        ICODE(PLOADX5+2   , 'SA5   X3+ ', 0            ,O  ,O  ,O  );
69110        ICODE(PLOADX5IM   , 'SX5   B0+ ', 0            ,O  ,O  ,X5 );
69120        ICODE(PLOADX5IM+1 , 'SA5   B0+ ', 0            ,O  ,O  ,O  );
69130        ICODE(PPUSH       , 'SA4   B5+ ', QPUSH        ,O  ,O  ,O  );
69140        QCODE(QPUSH       , 'BX7 X4    ', QPUSH+1);
69150        QCODE(QPUSH+1     , 'SA7 B6    ', QPUSH+2);
69160        QCODE(QPUSH+2     , 'SB6 B6+B1 ', 0);
69170        ICODE(PPUSH+1     , 'SA4   B2+ ', QPUSH        ,O  ,O  ,O  );
69180        ICODE(PPUSH+2     , 'SA4   X3+ ', QPUSH        ,O  ,O  ,O  );
69190        ICODE(PPUSHIM     , 'SX7   B0+ ', QPUSH+1      ,O  ,O  ,ST );
69200        ICODE(PPUSHIM+1   , 'SA4   B0+ ', QPUSH        ,O  ,O  ,O  );
69210        ICODE(PLOADX0     , 'SA4 B5+   ', QLOADX0      ,O  ,O  ,O  );
69220        QCODE(QLOADX0     , 'BX0 X4    ', 0);
69230        ICODE(PLOADX0+1   , 'SA4 B2+   ', QLOADX0      ,O  ,O  ,O  );
69240        ICODE(PLOADX0+2   , 'SA4 X3+   ', QLOADX0      ,O  ,O  ,O  );
69250        ICODE(PLOADX1     , 'SA1 B5+   ', 0            ,O  ,O  ,O  );
69260        ICODE(PLOADX1+1   , 'SA1 B2+   ', 0            ,O  ,O  ,O  );
69270        ICODE(PLOADX1+2   , 'SA1 X3+   ', 0            ,O  ,O  ,O  );
69280        ICODE(PLOADX6     , 'SA4 B5+   ', QLOADX6      ,O  ,O  ,O  );
69290        QCODE(QLOADX6     , 'BX6 X4    ', 0);
69300        ICODE(PLOADX6+1   , 'SA4 B2+   ', QLOADX6      ,O  ,O  ,O  );
69310        ICODE(PLOADX6+2   , 'SA4 X3+   ', QLOADX6      ,O  ,O  ,O  );
69320        ICODE(PLOADX0IM   , 'SX0 B0+   ', 0            ,O  ,O  ,X0 );
69330        ICODE(PLOADX0IM+1 , 'SA4 B0+   ', QLOADX0      ,O  ,O  ,O  );
69340        ICODE(PLOADX1IM   , 'SX1 B0+   ', 0            ,O  ,O  ,X1 );
69350        ICODE(PLOADX1IM+1 , 'SA1 B0+   ', 0            ,O  ,O  ,O  );
69360        ICODE(PLOADX2IM   , 'SX2 B0+   ', 0            ,O  ,O  ,O  );
69370        ICODE(PLOADX2IM+1 , 'SA2 B0+   ', 0            ,O  ,O  ,O  );
69380        ICODE(PLOADX3IM   , 'SX3 B0+   ', 0            ,O  ,O  ,O  );
69390        ICODE(PLOADX3IM+1 , 'SA3 B0+   ', 0            ,O  ,O  ,O  );
69400        ICODE(PLOADX4IM   , 'SX4 B0+   ', 0            ,O  ,O  ,O  );
69410        ICODE(PLOADX4IM+1 , 'SA4 B0+   ', 0            ,O  ,O  ,O  );
69420        ICODE(PLOADX6IM   , 'SX6 B0+   ', 0            ,O  ,O  ,X6 );
69430        ICODE(PLOADX6IM+1 , 'SA4 B0+   ', QLOADX6      ,O  ,O  ,O  );
69440        ICODE(PPOPTOX0    , 'SB6 B6-B1 ', QPOPX0       ,O  ,O  ,O  );
69450        QCODE(QPOPX0      , 'SA4 B6    ', QLOADX0);
69460        ICODE(PPOPTOX1    , 'SB6 B6-B1 ', QPOPX1       ,O  ,O  ,O  );
69470        QCODE(QPOPX1      , 'SA1 B6    ', 0);
69480        ICODE(PPOPTOX6    , 'SB6 B6-B1 ', QPOPTOX6   ,O  ,O  ,O  );
69490        QCODE(QPOPTOX6    , 'SA4 B6    ', QLOADX6);
69500        ICODE(PX5TOX0     , 'BX0 X5    ', 0            ,X5 ,O  ,X0 );
69510        ICODE(PX5TOX1     , 'BX1 X5    ', 0            ,X5 ,O  ,X1 );
69520        ICODE(PX5TOX6     , 'BX6 X5    ', 0            ,X5 ,O  ,X6 );
69530        ICODE(PPUSHX6     , 'SA6 B6    ', QPUSH+2      ,X6 ,O  ,O  );
69540        ICODE(PX6TOX5     , 'BX5 X6    ', 0            ,X6 ,O  ,X5 );
69550        ICODE(PX6TOX0     , 'BX0 X6    ', 0            ,X6 ,O  ,X0 );
69560        ICODE(PX6TOX1     , 'BX1 X6    ', 0            ,X6 ,O  ,X1 );
69570        ICODE(PPUSHX5     , 'BX7 X5    ', QPUSH+1     ,X5 ,O  ,O  );
69580        ICODE(PPOPTOX5    , 'SB6 B6-B1 ', QPOP1        ,O  ,O  ,O  );
69590        QCODE(QPOP1       , 'SA5 B6    ', 0);
69600        ICODE(PPUSHX0     , 'BX7 X0    ', QPUSH+1     ,X0 ,O  ,O  );
69610        ICODE(PPUSHX1     , 'BX7 X1    ', QPUSH+1     ,X1 ,O  ,O  );
69620        ICODE(PX1TOX5     , 'BX5 X1    ', 0            ,X1 ,O  ,X5 );
69630        ICODE(PX1TOX6     , 'BX6 X1    ', 0            ,X1 ,O  ,X6 );
69640        ICODE(PX1TOX0     , 'BX0 X1    ', 0            ,X1 ,O  ,X0 );
69650        ICODE(PSWAP       , 'BX3 X1    ', QSWAP        ,O  ,O  ,O  );
69660        QCODE(QSWAP       , 'BX1 X5    ', QSWAP1);
69670        QCODE(QSWAP1      , 'BX5 X3    ', 0);
69680  (*+61()
69690        ICODE(PPUSH2      , 'SA3 B5+   ', QPUSH2       ,O  ,O  ,O  );
69700        QCODE(QPUSH2      , 'BX7 X3    ', QPUSH2+1);
69710        QCODE(QPUSH2+1    , 'SA7 B6    ', QPUSH2+2);
69720        QCODE(QPUSH2+2    , 'SA3 A3+B1 ', QPUSH2+3);
69730        QCODE(QPUSH2+3    , 'BX7 X3    ', QPUSH2+4);
69740        QCODE(QPUSH2+4    , 'SA7 A7+B1 ', QPUSH2+5);
69750        QCODE(QPUSH2+5    , 'SB6 A7+B1 ', 0);
69760        ICODE(PPUSH2+1    , 'SA3 B2+   ', QPUSH2       ,O  ,O  ,O  );
69770        ICODE(PPUSH2+2    , 'SA3 X3+   ', QPUSH2       ,O  ,O  ,O  );
69780        ICODE(PLOADX12    , 'SA1 B5+   ', QLOADX12     ,O  ,O  ,O  );
69790        QCODE(QLOADX12    , 'SA2 A1+B1 ', 0);
69800        ICODE(PLOADX12+1  , 'SA1 B2+   ', QLOADX12     ,O  ,O  ,O  );
69810        ICODE(PLOADX12+2  , 'SA1 X3+   ', QLOADX12     ,O  ,O  ,O  );
69820        ICODE(PLOADX45    , 'SA4 B5+   ', QLOADX45     ,O  ,O  ,O  );
69830        QCODE(QLOADX45    , 'SA5 A4+B1 ', 0);
69840        ICODE(PLOADX45+1  , 'SA4 B2+   ', QLOADX45     ,O  ,O  ,O  );
69850        ICODE(PLOADX45+2  , 'SA4 X3+   ', QLOADX45     ,O  ,O  ,O  );
69860        ICODE(PPUSHX12    , 'BX7 X1    ', QPUSHX12     ,O  ,O  ,O  );
69870        QCODE(QPUSHX12    , 'SA7 B6    ', QPUSHX12+1);
69880        QCODE(QPUSHX12+1  , 'BX7 X2    ', QPUSH2+4);
69890        ICODE(PPUSHX45    , 'BX7 X4    ', QPUSHX45     ,O  ,O  ,O  );
69900        QCODE(QPUSHX45    , 'SA7 B6    ', QPUSHX45+1);
69910        QCODE(QPUSHX45+1  , 'BX7 X5    ', QPUSH2+4);
69920        ICODE(PPOPTOX12   , 'SA2 B6-B1 ', QPOPTOX12    ,O  ,O  ,O  );
69930        QCODE(QPOPTOX12   , 'SA1 A2-B1 ', QPOPTOX12+1);
69940        QCODE(QPOPTOX12+1 , 'SB6 A1    ', 0);
69950        ICODE(PPOPTOX45   , 'SA5 B6-B1 ', QPOPTOX45    ,O  ,O  ,O  );
69960        QCODE(QPOPTOX45   , 'SA4 A5-B1 ', QPOPTOX45+1);
69970        QCODE(QPOPTOX45+1 , 'SB6 A4    ', 0);
69980        ICODE(PX12TOX45   , 'BX4 X1    ', QX12TOX45    ,O  ,O  ,O  );
69990        QCODE(QX12TOX45   , 'BX5 X2    ', 0);
70000        ICODE(PX45TOX12   , 'BX1 X4    ', QX45TOX12    ,O  ,O  ,O  );
70010        QCODE(QX45TOX12   , 'BX2 X5    ', 0);
70020  ()+61*)
70030        ICODE(PDECM       , 'SX7 B0+   ', 0            ,O  ,O  ,O  );
70040        ICODE(PDECM+1     , 'SA3 B5+   ', QDECM        ,O  ,O  ,O  );
70050        QCODE(QDECM       , 'MX4 25    ', QDECM+1);
70060        QCODE(QDECM+1     , 'BX3 -X4*X3', QDECM+2);
70070        QCODE(QDECM+2     , 'LX7 35    ', QDECM+3);
70080        QCODE(QDECM+3     , 'BX7 X3+X7 ', QDECM+4);
70090        QCODE(QDECM+4     , 'SA7 A3    ', 0);
70100        END;
70110   (* *** CHANGES TO BE MADE ON PERQ  ***  *)
70120   (*     PPARM , PPARM+1, PLOADRTA, PDECM *)
70130   (*     PCALLA, PRANGENT, PDCLINIT       *)
70140    PROCEDURE INITPOPARRAY;
70150      VAR I, J:SBTTYP;
70160        BEGIN
70170        FOR I := SBTSTK TO SBTX1 DO
70180          FOR J:= SBTVOID TO SBTX1 DO POPARRAY [I, J] := PNONE;
70190        FOR I := SBTSTK TO SBTX1 DO
70200          BEGIN
70210          POPARRAY [I,I] := PNOOP;
70220          POPARRAY [I,SBTVOID] := PNOOP;
70230          POPARRAY [I,SBTVAR] := PLOADVAR;
70240          POPARRAY [I,SBTPROC] := PLOADRTA;
70250          POPARRAY [I,SBTRPROC]:= PLOADRTA;
70260          END;
70270  (*+61()
70280        POPARRAY [SBTSTK  , SBTSTK2 ] := PVARLISTEND+1;
70290        POPARRAY [SBTX12  , SBTX1   ] := PNOOP;
70300        POPARRAY [SBTSTK  , SBTX12  ] := PPUSHX1;
70310        POPARRAY [SBTSTK  , SBTX45  ] := PPUSHX5;
70320        POPARRAY [SBTSTK2 , SBTID   ] := PPUSH2;
70330        POPARRAY [SBTSTK2 , SBTIDV  ] := PPUSH2;
70340        POPARRAY [SBTSTK2 , SBTX12  ] := PPUSHX12;
70350        POPARRAY [SBTSTK2 , SBTX45  ] := PPUSHX45;
70360        POPARRAY [SBTX12  , SBTID   ] := PLOADX12;
70370        POPARRAY [SBTX12  , SBTIDV  ] := PLOADX12;
70380        POPARRAY [SBTX12  , SBTSTK2 ] := PPOPTOX12;
70390        POPARRAY [SBTX12  , SBTX45  ] := PX45TOX12;
70400        POPARRAY [SBTX45  , SBTID   ] := PLOADX45;
70410        POPARRAY [SBTX45  , SBTIDV  ] := PLOADX45;
70420        POPARRAY [SBTX45  , SBTSTK2 ] := PPOPTOX45;
70430        POPARRAY [SBTX45  , SBTX12  ] := PX12TOX45;
70440  ()+61*)
70450        POPARRAY [SBTSTK  , SBTID   ] := PPUSH;
70460        POPARRAY [SBTSTK  , SBTIDV  ] := PPUSH;
70470        POPARRAY [SBTSTK  , SBTLIT  ] := PPUSHIM;
70480        POPARRAY [SBTSTK  , SBTDEN  ] := PPUSHIM;
70490        POPARRAY [SBTSTK  , SBTX5   ] := PPUSHX5;
70500        POPARRAY [SBTSTK  , SBTX6   ] := PPUSHX6;
70510        POPARRAY [SBTSTK  , SBTX0   ] := PPUSHX0;
70520        POPARRAY [SBTSTK  , SBTX1   ] := PPUSHX1;
70530        POPARRAY [SBTX5   , SBTID   ] := PLOADX5;
70540        POPARRAY [SBTX5   , SBTIDV  ] := PLOADX5;
70550        POPARRAY [SBTX5   , SBTLIT  ] := PLOADX5IM;
70560        POPARRAY [SBTX5   , SBTDEN  ] := PLOADX5IM;
70570        POPARRAY [SBTX5   , SBTSTK  ] := PPOPTOX5;
70580        POPARRAY [SBTX5   , SBTX6   ] := PX6TOX5;
70590        POPARRAY [SBTX5   , SBTX1   ] := PX1TOX5;
70600        POPARRAY [SBTX6   , SBTID   ] := PLOADX6;
70610        POPARRAY [SBTX6   , SBTIDV  ] := PLOADX6;
70620        POPARRAY [SBTX6   , SBTLIT  ] := PLOADX6IM;
70630        POPARRAY [SBTX6   , SBTDEN  ] := PLOADX6IM;
70640        POPARRAY [SBTX6   , SBTSTK  ] := PPOPTOX6;
70650        POPARRAY [SBTX6   , SBTX5   ] := PX5TOX6;
70660        POPARRAY [SBTX6   , SBTX1   ] := PX1TOX6;
70670        POPARRAY [SBTX0   , SBTID   ] := PLOADX0;
70680        POPARRAY [SBTX0   , SBTIDV  ] := PLOADX0;
70690        POPARRAY [SBTX0   , SBTLIT  ] := PLOADX0IM;
70700        POPARRAY [SBTX0   , SBTDEN  ] := PLOADX0IM;
70710        POPARRAY [SBTX0   , SBTSTK  ] := PPOPTOX0;
70720        POPARRAY [SBTX0   , SBTX5   ] := PX5TOX0;
70730        POPARRAY [SBTX0   , SBTX6   ] := PX6TOX0;
70740        POPARRAY [SBTX0   , SBTX1   ] := PX1TOX0;
70750        POPARRAY [SBTX1   , SBTID   ] := PLOADX1;
70760        POPARRAY [SBTX1   , SBTIDV  ] := PLOADX1;
70770        POPARRAY [SBTX1   , SBTLIT  ] := PLOADX1IM;
70780        POPARRAY [SBTX1   , SBTDEN  ] := PLOADX1IM;
70790        POPARRAY [SBTX1   , SBTSTK  ] := PPOPTOX1;
70800        POPARRAY [SBTX1   , SBTX5   ] := PX5TOX1;
70810        POPARRAY [SBTX1   , SBTX6   ] := PX6TOX1;
70820        END;
70830    PROCEDURE INITLENARRAY;
70840      VAR I: SBTTYP;
70850        BEGIN
70860        FOR I := SBTSTK TO SBTX1 DO LENARRAY[I] := 0;
70870        LENARRAY[SBTSTK ] := SZWORD;
70880  (*+61()
70890        LENARRAY[SBTSTK2] := 2*SZWORD;
70900        LENARRAY[SBTX12 ] := 2*SZWORD;
70910        LENARRAY[SBTX45 ] := 2*SZWORD;
70920  ()+61*)
70930        LENARRAY[SBTX5  ] := SZWORD;
70940        LENARRAY[SBTX6  ] := SZWORD;
70950        LENARRAY[SBTX0  ] := SZWORD;
70960        LENARRAY[SBTX1  ] := SZWORD;
70970        END;
70980    PROCEDURE INITREGISTERS;
70990      VAR I: SBTTYP;
71000        BEGIN
71010        FOR I := SBTVOID TO SBTX1 DO REGISTERS[I] := [];
71020        REGISTERS[SBTDL   ] := [SBTX1];
71030        REGISTERS[SBTX5   ] := [SBTX5];
71040        REGISTERS[SBTX6   ] := [SBTX6];
71050        REGISTERS[SBTX0   ] := [SBTX0];
71060        REGISTERS[SBTX1   ] := [SBTX1];
71070  (*+61()
71080        REGISTERS[SBTX12  ] := [SBTX1]; (*THERE IS NO SBTX2+)
71090        REGISTERS[SBTX45  ] := [SBTX5]; (*THERE IS NO SBTX4+)
71100  ()+61*)
71110        END;
71120      BEGIN (*INITCODES*)
71130      FIRSTPART; SECONDPART; THIRDPART; INITPOPARRAY; INITLENARRAY; INITREGISTERS;
71140      END;
71150  (**)
71160  (**)
71170  (**)
71180  (**)
71190  (**)
71200  BEGIN
71210          LINELIMIT(OUTPUT,10000); LINELIMIT(LSTFILE,10000);
71220          DUMP(FIRSTSTACK);
71230  (*-01() DUMP(FIRSTSTACK,LASTSTACK); ()-01*)
71240  END    (*$G-*)    .
####S
