50000 (*  COPYRIGHT 1983 C.H.LINDSEY, UNIVERSITY OF MANCHESTER  *)
50010  (**)
50020  (**)
50030  (*+82()
50040  (**)
50050  (*+01()   (*+31()   (*$P+,T+*)   ()+31*)   ()+01*)
50060  (*+25()   (*+31()   (*$P+,T+*)   ()+31*)   ()+25*)
50070  (**)
50080  PROCEDURE PARSEPARSER;
50090    VAR ACOUNT, BCOUNT: INTEGER; CCOUNT: 0..10000;
50100        HTCOPY: HASHTAB;
50110        THIS, THAT: PLEX;
50120        BPRODTBL: ARRAY [1..40] OF PROD;
50130        SEXFR,FEXFR:ARRAY[0..PRODLEN] OF 0..PRODLEN;
50140        TEMP:INTEGER;
50150        I: INTEGER;
50160        J: INTEGER;
50165  (* MAP: RECORD CASE BOOLEAN OF
50166        TRUE : (INT:ADDRINT);
50167        FALSE : (POINT:^INTEGER);
50168   END; *)
50170  (*+01()
50180        FRED: PLEX;
50190        FRIG: RECORD CASE SEVERAL OF
50200                     1:(INT: INTEGER); 2:(POINT: PINTEGER) END;
50210   ()+01*)
50220  (*+25()
50230        FRIG: RECORD CASE SEVERAL OF
50240                     1:(INT: INTEGER); 2:(POINT: PINTEGER) END;
50250   ()+25*)
50260  (*+04()
50270    PROCEDURE INITIO;
50280    (*+01()   VAR AW66: PW66; ()+01*)
50290        BEGIN
50300        ERRDEV := FALSE;
50310    (*+23()   NUMPARAMS:=0;  (* TO COUNT NO OF P-OP PARAMETERS OUTPUT TO LSTFILE *)  ()+23*)
50320        LSTLINE := -1;  (*FOR FIRST TIME OF OUTSRC*)
50330        LSTCNT := 100;         (*TO FORCE NEWPAGE*)
50340        LSTPAGE := 0;
50350    (*-03() (*-04()
50360        RESET(INPUT);
50370        REWRITE(LSTFILE);
50380    ()-04*) ()-03*)
50390    (*+03()
50400    WRITE('SOURCE-FILE: ');
50410    OPEN(INPUT,'','SYMB',SEQRD);
50420    WRITE('LIST-FILE: ');
50430    OPEN(LSTFILE,'','DATA',SEQWR);
50440    OPEN(OUTPUT,'TERMINAL','SYMB',SEQWR);
50450    ()+03*)
50460        RESET(INPUT, 'INPUT');
50470        REWRITE(OUTPUT, 'CONSOLE');
50480        REWRITE(LSTFILE, 'LSTFILE');
50490        SRCBUF[0] := ' ';  (*IT WILL NEVER BE WRITTEN TO AGAIN*)
50500    (*+01()
50510        LINELIMIT(OUTPUT, 100000);
50520        AW66 := ASPTR(66B);
50530        ONLINE := AW66^.JOPR=3;
50540     ()+01*)
50550    (*+02() ONLINE := TRUE; ()+02*)
50560    (*+03() ONLINE := FILENR(LSTFILE)<>1; ()+03*)
50570        ONLINE := TRUE;
50580    (*-04() (*-02() DATE(DAT); TIME(TIM); ()-02*) ()-04*)
50590        END;
50600  ()+04*)
50610    PROCEDURE CLASS(TAG: ALFA);
50620      VAR DUMMY: PLEX;
50630          I: INTEGER;
50640        BEGIN WITH CURRENTLEX DO
50650          BEGIN
50660          LXV := LXVTAB;  LXTOKEN := TKTAG;
50670  (*+11() S10:=TAG; LXCOUNT:=1;   ()+11*)
50680  (*-11() STASHLEX(TAG); ()-11*)
50690          DUMMY := HASHIN
50700          END
50710        END;
50720    PROCEDURE TLEX(TAG: ALFA; SLEX: LXIOTYPE);
50730      VAR DUMMY: PLEX;
50740          I: INTEGER;
50750        BEGIN WITH CURRENTLEX DO
50760          BEGIN
50770          LXV := LXVTAG;  LXTOKEN := TKTAG;  LXV.LXPIO := SLEX;
50780  (*+11() S10:=TAG; LXCOUNT:=1;   ()+11*)
50790  (*-11() STASHLEX(TAG); ()-11*)
50800          DUMMY := HASHIN;
50810          END
50820        END;
50830    PROCEDURE SEMANTICROUTINE(SRTN: RTNTYPE);
50840      VAR C: INTEGER;
50850          SAE:  CHAR;
50860      PROCEDURE LABL(SEX, FEX, VALUE: INTEGER);
50870        VAR TEMP: INTEGER;
50880          BEGIN
50890          WHILE SEX<>0 DO
50900            BEGIN TEMP := PRODTBL[SEX].SEXIT; PRODTBL[SEX].SEXIT := VALUE; SEX := TEMP END;
50910          WHILE FEX<>0 DO
50920            BEGIN TEMP := PRODTBL[FEX].FEXIT; PRODTBL[FEX].FEXIT := VALUE; FEX := TEMP END
50930          END;
50940        BEGIN WITH SRPLSTK[PLSTKP]^ DO WITH PRODTBL[(BCOUNT-1) MOD PRODLEN + 1] DO CASE SRTN OF
50950            10: (*SR01*) (*START OF EACH RULE*)
50960              ACOUNT := 0;
50970            11: (*SR02*) (*TAG*)
50980              IF ACOUNT=0 THEN BEGIN PRSTKC := S; SYLXV.LX1IO := LXV.LXPIO END
50990              ELSE IF ACOUNT=1 THEN BEGIN PRSTKA := 2; PRINPC := SSA; SYLXV.LX2IO := LXV.LXPIO; ACOUNT := -99 END
51000              ELSE (*ACOUNT<0*) BEGIN PRINPC := S; SYLXV.LX2IO := LXV.LXPIO END;
51010            12: (*SR03A*) (*TAB*)
51020              BEGIN C := ORD(S10[4])-ORD('0');
51030              IF (C<0) OR (C>9) THEN C := ORD(S10[4])-ORD('A')+10;
51040              IF S10[1]='C' THEN WITH SYLXV DO
51050                IF ACOUNT=0 THEN CASE S10[3] OF
51060                  '0': BEGIN PRSTKC:=C0; LX1CL0:=C END; '1': BEGIN PRSTKC:=C1; LX1CL1:=C END;
51070                  '2': BEGIN PRSTKC:=C2; LX1CL2:=C END; END
51080                ELSE CASE S10[3] OF
51090                  '0': BEGIN PRINPC:=C0; LX2CL0:=C END; '1': BEGIN PRINPC:=C1; LX2CL1:=C END;
51100                  '2': BEGIN PRINPC:=C2; LX2CL2:=C END; END END;
51110            35: (*SR20B)* (*NO 2ND TAG OR TAB*)
51120              IF ACOUNT=-1 THEN BEGIN PRINPC := A; SYLXV.LX2IO := LXIODUMMY END;
51130            13: (*SR03B*) (*NO 1ST TAG OR TAB*)
51140              BEGIN ACOUNT := -1; PRSTKC := S; SYLXV.LX1IO := LXIODUMMY; PRSTKA := 3 END;
51150            14: (*SR04A*) (*AFTER COMMA*)
51160              ACOUNT := ACOUNT+1;
51170            15: (*SR04B*) (*AFTER STICK*)
51180              IF ACOUNT>=0 THEN
51190                BEGIN PRSTKA := ACOUNT; ACOUNT := -1 END;
51200            16: (*SR05*) (*RTN PRESENT*)
51210              BEGIN
51220                SAE := SRPLSTK[PLSTKP]^.S10[1];
51230                IF (SAE = 'S') OR (SAE = 'A') THEN C := 0
51240                ELSE IF SAE = 'E' THEN C := ESY01-1
51250                     ELSE SEMERR(ESE+16);
51260                RTN := C + (*-04() INP^.LXDENRP ()-04*)(*+04() SHRINK(INP^.LXDENRP) ()+04*)
51270              END;
51280            17: (*SR06*) (*RTN ABSENT*)
51290              RTN := DUMMY;
51300            18: (*SR07A*) (*POP PRESENT*)
51310              PRPOP := (*-04() INP^.LXDENRP ()-04*)(*+04() SHRINK(INP^.LXDENRP) ()+04*);
51320            19: (*SR07B*) (*POP ABSENT*)
51330              BEGIN PRPOP := 0; PRPUSH := LXIODUMMY END;
51340            20: (*SR08A*) (*PUSH PRESENT*)
51350              PRPUSH := INP^.LXV.LXPIO;
51360            21: (*SR08B*) (*PUSH ABSENT*)
51370              PRPUSH := LXIODUMMY;
51380            22: (*SR10*) (*SKIP PRESENT*)
51390              PRSKIP := TRUE;
51400            23: (*SR11*) (*SKIP ABSENT*)
51410              PRSKIP := FALSE;
51420            24: (*SR12*) (*SCAN=++*)
51430              PRSCAN := 2;
51440            25: (*SR14A)* (*SCAN=+*)
51450              PRSCAN := 1;
51460            26: (*SR14B*) (*SCAN ABSENT*)
51470              PRSCAN := 0;
51480            28: (*SR15*) (*SEX*)
51490              IF (LXV.LXP<PRODLEN) AND (LXV.LXPSTB<>NIL) THEN
51500                SEXIT := LXV.LXP
51510              ELSE
51520                BEGIN
51530                IF LXV.LXPSTB=NIL THEN
51540                  BEGIN
51550                  CCOUNT:=CCOUNT+1; LXV.LXP:=PRODLEN+CCOUNT;
51560                  SEXFR[CCOUNT]:=0; FEXFR[CCOUNT]:=0
51570                  END;
51580                TEMP:=LXV.LXP-PRODLEN;
51590                SEXIT:=SEXFR[TEMP]; SEXFR[TEMP]:=BCOUNT
51600                END;
51610            29: (*SR16A*) (*FEX*)
51620              IF (INP^.LXV.LXP<PRODLEN) AND (INP^.LXV.LXPSTB<>NIL) THEN
51630                FEXIT := INP^.LXV.LXP
51640              ELSE
51650                BEGIN
51660                IF INP^.LXV.LXPSTB=NIL THEN
51670                  BEGIN
51680                  CCOUNT:=CCOUNT+1; INP^.LXV.LXP:=PRODLEN+CCOUNT;
51690                  SEXFR[CCOUNT]:=0; FEXFR[CCOUNT]:=0
51700                  END;
51710                TEMP:=INP^.LXV.LXP-PRODLEN;
51720                FEXIT:=FEXFR[TEMP];FEXFR[TEMP]:=BCOUNT
51730                END;
51740            30: (*SR16B*) (*FEX ABSENT*)
51750              FEXIT := BCOUNT+1;
51760            31: (*SR16C*) (*END OF RULE*)
51770              BCOUNT := BCOUNT+1;
51780            32: (*SR16D*) (*ERROR*)
51790              OUTERR(ELX+7, ERRORR, NIL);
51800            34: (*SR20A*) (*AT LABEL*)
51810              BEGIN
51820              IF LXV.LXPSTB<>NIL THEN
51830                  BEGIN
51840                  TEMP:=LXV.LXP-PRODLEN;
51850                  LABL(SEXFR[TEMP],FEXFR[TEMP],BCOUNT);
51860                  SEXFR[TEMP]:=0; FEXFR[TEMP]:=0;
51870                  WHILE (CCOUNT>0) AND (SEXFR[CCOUNT]=0) AND (FEXFR[CCOUNT]=0)
51880                  DO CCOUNT:=CCOUNT-1
51890                  END;
51900              LXV.LXP := BCOUNT END;
51910            36: (*SR20C*) (*END OF FILE*)
51920              ENDOFPROG := TRUE;
51930            END
51940        END;
51950  ()+82*)
51960    (*OLD VERSION OF SEMANTICROUTINE WHICH WAS USED TO PRODUCE THE CALLS OF MPROD AND BLABL WHICH FOLLOW*)
51970    (*
51980                      PROCEDURE SEMANTICROUTINE(SRTN: RTNTYPE);
51990                        VAR C: INTEGER;
52000                            SAR:  CHAR;
52010                          BEGIN WITH SRPLSTK[PLSTKP]^ DO CASE SRTN OF
52020                          10: (*SR01+) BEGIN WRITE(LSTFILE, '      BMPROD(', BCOUNT:3, ', '); ACOUNT:=0; END;
52030                          11: (*SR02+): IF ACOUNT<>1 THEN WRITE(LSTFILE, 'S , LXIO', S1, ',  0, ')
52040                                      ELSE BEGIN WRITE(LSTFILE, '2, SSA,LXIO', S1, ',  0, '); ACOUNT:=-99 END;
52050                          12: (*SR03A+) BEGIN C := ORD(S1[4])-ORD('0');
52060                            IF (C<0) OR (C>9) THEN C := ORD(S1[4])-ORD('A')+10;
52070                            IF S1[1]='C' THEN WRITE(LSTFILE, 'C', S1[3], ', LXIODUMMY     , ', C:2, ', ') END;
52080                          35: (*SR20B+) IF ACOUNT=-1 THEN WRITE(LSTFILE, 'A , LXIODUMMY     ,  0, ');
52090                          13: (*SR03B+) BEGIN ACOUNT:=-1; WRITE(LSTFILE, 'S , LXIODUMMY     ,  0, 3, ') END;
52100                          14: (*SR04A+) ACOUNT := ACOUNT+1;
52110                          15: (*SR04B+) IF ACOUNT>=0 THEN
52120                            BEGIN WRITE(LSTFILE, ACOUNT:1, ', '); ACOUNT:=-1 END;
52130                          16: (*SR05+) BEGIN SAE := SRPLSTK[PLSTKP].S1[1];
52140                                       IF (SAE='S') OR (SAE = 'A') THEN C:=0
52150                                       ELSE IF SAE='E' THEN C:=ESY01-1
52160                                       ELSE SEMERR(ESE+16);
52170                                       WRITE(LSTFILE,C+INP^.LXDENRP:4)
52180                                       END;
52190                          17: (*SR06+) WRITE(LSTFILE, 'DUMMY     , ');
52200                          18: (*SR07A+) WRITE(LSTFILE, INP^.LXDENRP:1, ', ');
52210                          19: (*SR07B+) WRITE(LSTFILE, '0, LXIODUMMY     , ');
52220                          20: (*SR08A+) WRITE(LSTFILE, 'LXIO', INP^.S1, ', ');
52230                          21: (*SR08B+) WRITE(LSTFILE, 'LXIODUMMY     , ');
52240                          22: (*SR10+) WRITE(LSTFILE, 'TRUE , ');
52250                          23: (*SR11+) WRITE(LSTFILE, 'FALSE, ');
52260                          24: (*SR12+) WRITE(LSTFILE, '2, ');
52270                          25: (*SR14A+) WRITE(LSTFILE, '1, ');:
52280                          26: (*SR14B+) WRITE(LSTFILE, '0, ');:
52290                          28: (*SR15+) IF (LXV.LXP<PRODLEN) AND (LXV.LXPSTB<>NIL) THEN
52300                            WRITE(LSTFILE, LXV.LXP:4, ', ')
52310                            ELSE BEGIN
52320                              IF LXV.LXPSTB=NIL THEN BEGIN LXV.LXP := PRODLEN; CCOUNT := CCOUNT+1 END;
52330                              WRITE(LSTFILE, -(LXV.LXP DIV PRODLEN-1):4, ', ');
52340                              LXV.LXP := LXV.LXP MOD PRODLEN + (BCOUNT+1)*PRODLEN END;
52350                          29: (*SR16A+) IF (INP^.LXV.LXP<PRODLEN) AND (INP^.LXV.LXPSTB<>NIL) THEN
52360                            WRITE(LSTFILE, INP^.LXV.LXP:4)
52370                            ELSE BEGIN
52380                              IF INP^.LXV.LXPSTB=NIL THEN BEGIN INP^.LXV.LXP := PRODLEN; CCOUNT := CCOUNT+1 END;
52390                              WRITE(LSTFILE, -(INP^.LXV.LXP MOD PRODLEN):4);
52400                              INP^.LXV.LXP := INP^.LXV.LXP DIV PRODLEN * PRODLEN + BCOUNT END;
52410                          30: (*SR16B+) WRITE(LSTFILE, BCOUNT+1:4);
52420                          31: (*SR16C+) BEGIN WRITELN(LSTFILE,');'); BCOUNT := BCOUNT+1; END;
52430                          32: (*SR16D+) OUTERR(ELX+7, ERRORR, NIL);
52440                          34: (*SR20A+) BEGIN
52450                            IF LXV.LXPSTB<>NIL THEN BEGIN
52460                              WRITELN(LSTFILE, '        BLABL(', LXV.LXP DIV PRODLEN - 1:3, ', ',
52470                                LXV.LXP MOD PRODLEN:3, ', ', BCOUNT:3, ');');
52480                              CCOUNT := CCOUNT-1;
52490                              END;
52500                            LXV.LXP := BCOUNT END;
52510                          36: (*SR20C+) ENDOFPROG := TRUE;
52520                          END
52530                          END;
52540    *)
52550  (*+82()
52560    PROCEDURE INITPR;
52570        BEGIN
52580        PLINPQ := NIL;
52590        PLPTR := 1;
52600        SRPLSTK[SRPLSTKSIZE] := LEXSTOP;
52610        SRPLSTK[SRPLSTKSIZE-1] := LEXSTOP;
52620        PLSTKP := SRPLSTKSIZE-1;
52630        ENDOFPROG := FALSE;
52640        INP := LEXSTART
52650        END;
52660    PROCEDURE BMPROD(PTR: INTEGER;
52670                    CONFIG1: CONFIG; IO1: LXIOTYPE; CLA1: CL2TYPE; STKA: INTEGER;
52680                    CONFIG2: CONFIG; IO2: LXIOTYPE; CLA2: CL2TYPE;
52690                 SRTN: RTNTYPE; POP: INTEGER; PUSH: LXIOTYPE; SKIP: BOOLEAN; SCAN: INTEGER; SEX, FEX: INTEGER);
52700        BEGIN WITH BPRODTBL[PTR] DO
52710          BEGIN
52720          PRSTKA := STKA; PRSTKC := CONFIG1; PRINPC := CONFIG2;
52730          CASE CONFIG1 OF S: SYLXV.LX1IO := IO1;
52740                C0: SYLXV.LX1CL0 := CLA1; C1: SYLXV.LX1CL1 := CLA1; C2: SYLXV.LX1CL2 := CLA1 END;
52750          CASE CONFIG2 OF S, A, SSA: SYLXV.LX2IO := IO2;
52760                C0: SYLXV.LX2CL0 := CLA2; C1: SYLXV.LX2CL1 := CLA2; C2: SYLXV.LX2CL2 := CLA2 END;
52770          RTN := SRTN; PRPOP := POP; PRPUSH := PUSH; PRSKIP := SKIP; PRSCAN := SCAN;
52780          SEXIT := ABS(SEX); FEXIT := ABS(FEX);
52790          END
52800        END;
52810    PROCEDURE BLABL(SEX, FEX, VALUE: INTEGER);
52820      VAR TEMP: INTEGER;
52830        BEGIN
52840        WHILE SEX<>0 DO
52850          BEGIN TEMP := BPRODTBL[SEX].SEXIT; BPRODTBL[SEX].SEXIT := VALUE; SEX := TEMP END;
52860        WHILE FEX<>0 DO
52870          BEGIN TEMP := BPRODTBL[FEX].FEXIT; BPRODTBL[FEX].FEXIT := VALUE; FEX := TEMP END
52880        END;
52890    PROCEDURE PARSER;
52900      VAR MATCH: BOOLEAN;
52910      STK: PLEX;
52920      I: INTEGER;
52930        BEGIN
52940        WHILE NOT ENDOFPROG DO
52950          WITH BPRODTBL[PLPTR] DO
52960            BEGIN
52970            MATCH := TRUE;
52980            IF PRSTKA<3 THEN
52990              BEGIN
53000              STK := SRPLSTK[PLSTKP+PRSTKA];
53010              CASE PRSTKC OF
53020                S:  MATCH := SYLXV.LX1IO  = STK^.LXV.LXIO;
53030                C0: MATCH := SYLXV.LX1CL0 = STK^.LXV.LXCLASS0;
53040                C1: MATCH := SYLXV.LX1CL1 = STK^.LXV.LXCLASS1;
53050                C2: MATCH := SYLXV.LX1CL2 = STK^.LXV.LXCLASS2
53060                END
53070              END;
53080            IF MATCH THEN
53090              CASE PRINPC OF
53100                A:  ;
53110                S:  MATCH := SYLXV.LX2IO  = INP^.LXV.LXIO;
53120                C0: MATCH := SYLXV.LX2CL0 = INP^.LXV.LXCLASS0;
53130                C1: MATCH := SYLXV.LX2CL1 = INP^.LXV.LXCLASS1;
53140                C2: MATCH := SYLXV.LX2CL2 = INP^.LXV.LXCLASS2;
53150               SSA: MATCH := SYLXV.LX2IO = SRPLSTK[PLSTKP+1]^.LXV.LXIO
53160                END;
53170            IF MATCH THEN
53180              IF RTN>FINISH THEN
53190                SEMANTICROUTINE(RTN);
53200            IF MATCH THEN
53210              BEGIN
53220              PLSTKP := PLSTKP+PRPOP;
53230              IF PRPUSH<>LXIODUMMY THEN
53240                BEGIN PLSTKP := PLSTKP-1; SRPLSTK[PLSTKP] := PUSHTBL[PRPUSH] END;
53250              IF PRSKIP THEN
53260                INP := PARSIN;
53270              FOR I := 1 TO PRSCAN DO
53280                BEGIN PLSTKP := PLSTKP-1; SRPLSTK[PLSTKP] := INP; INP := PARSIN END;
53290              PLPTR := SEXIT
53300              END
53310            ELSE
53320              PLPTR := FEXIT
53330            END
53340        END;
53350  (*+01()   (*$T-+)   ()+01*)
53360  (*+25()   (*$T-+)   ()+25*)
53370    PROCEDURE CLASSES;
53380        BEGIN
53390        HTCOPY := HT;
53400  (*+01() ENEW(FRED, SZWORD); (*TO MARK THE PRESENT HEAP LIMIT*) ()+01*)
53410        CLASS('CL00      '); CLASS('CL01      ');
53420        CLASS('CL10      '); CLASS('CL11      ');
53430        CLASS('CL12      '); CLASS('CL13      ');
53440        CLASS('CL14      ');
53450        CLASS('CL20      '); CLASS('CL21      ');
53460        CLASS('CL22      '); CLASS('CL23      ');
53470        CLASS('CL24      '); CLASS('CL25      ');
53480        CLASS('CL26      '); CLASS('CL27      ');
53490        CLASS('CL28      '); CLASS('CL29      ');
53500        CLASS('CL2A      '); CLASS('CL2B      ');
53510        CLASS('CL2C      '); CLASS('CL2D      ');
53520        CLASS('CL2E      '); CLASS('CL2F      ');
53530        CLASS('ANY       ');
53540        END;
53550    PROCEDURE TLEXS;
53560        BEGIN
53570        TLEX('ACTPL     ', LXIOACTPL);
53580        TLEX('ACTRL     ', LXIOACTRL);
53590        TLEX('BOUNDS    ', LXIOBOUNDS);
53600        TLEX('BRINPT    ', LXIOBRINPT);
53610        TLEX('BRTHPT    ', LXIOBRTHPT);
53620        TLEX('CSTICK    ', LXIOCSTICK);
53630        TLEX('DCLL      ', LXIODCLL);
53640        TLEX('FLDSPL    ', LXIOFLDSPL);
53650        TLEX('FORDCL    ', LXIOFORDCL);
53660        TLEX('FORRLB    ', LXIOFORRLB);
53670        TLEX('IDEFL     ', LXIOIDEFL);
53680        TLEX('LABSQ     ', LXIOLABSQ);
53690        TLEX('MOIDDR    ', LXIOMOIDDR);
53700        TLEX('NONRDR    ', LXIONONRDR);
53710        TLEX('ODEFL     ', LXIOODEFL);
53720        TLEX('OPRAND    ', LXIOOPRAND);
53730        TLEX('PRIM      ', LXIOPRIM);
53740        TLEX('PRMDRL    ', LXIOPRMDRL);
53750        TLEX('RIDEFL    ', LXIORIDEFL);
53760        TLEX('RODEFL    ', LXIORODEFL);
53770        TLEX('RSPEC     ', LXIORSPEC);
53780        TLEX('RVDEFL    ', LXIORVDEFL);
53790        TLEX('TERT      ', LXIOTERT);
53800        TLEX('TRMSCL    ', LXIOTRMSCL);
53810        TLEX('UNITLC    ', LXIOUNLC);
53820        TLEX('UNITLP    ', LXIOUNLP);
53830        TLEX('UNITSR    ', LXIOUNSR);
53840        TLEX('VDEFL     ', LXIOVDEFL);
53850        TLEX('AGAIN     ', LXIOAGAIN);
53860        TLEX('AT        ', LXIOAT);
53870        TLEX('BEGIN     ', LXIOBEGIN);
53880        TLEX('BOOLDEN   ', LXIOBOOLDEN);
53890        TLEX('BUS       ', LXIOBUS);
53900        TLEX('BY        ', LXIOBY);
53910        TLEX('CASE      ', LXIOCASE);
53920        TLEX('COMMA     ', LXIOCOMMA);
53930        TLEX('COMMENT   ', LXIOCMMENT);
53940        TLEX('DO        ', LXIODO);
53950        TLEX('ELIF      ', LXIOELIF);
53960        TLEX('ELSE      ', LXIOELSE);
53970        TLEX('END       ', LXIOEND);
53980        TLEX('ERROR     ', LXIOERROR);
53990        TLEX('ESAC      ', LXIOESAC);
54000        TLEX('EXIT      ', LXIOEXIT);
54010        TLEX('FI        ', LXIOFI);
54020        TLEX('FOR       ', LXIOFOR);
54030        TLEX('FROM      ', LXIOFROM);
54040        TLEX('GO        ', LXIOGO);
54050        TLEX('GOTO      ', LXIOGOTO);
54060        TLEX('HEAP      ', LXIOHEAP);
54070        TLEX('IDTY      ', LXIOIDTY);
54080        TLEX('IF        ', LXIOIF);
54090        TLEX('IN        ', LXIOIN);
54100        TLEX('LOC       ', LXIOLOC);
54110        TLEX('LONG      ', LXIOLONG);
54120        TLEX('MDIND     ', LXIOMDIND);
54130        TLEX('MODE      ', LXIOMODE);
54140        TLEX('NIL       ', LXIONIL);
54150        TLEX('OD        ', LXIOOD);
54160        TLEX('OF        ', LXIOOF);
54170        TLEX('OP        ', LXIOOP);
54180        TLEX('OPR       ', LXIOOPR);
54190        TLEX('OTHDR     ', LXIOOTHDR);
54200        TLEX('OUSE      ', LXIOOUSE);
54210        TLEX('OUT       ', LXIOOUT);
54220        TLEX('PRAGMAT   ', LXIOPRAGMAT);
54230        TLEX('PRIMDR    ', LXIOPRDR);
54240        TLEX('PRIO      ', LXIOPRIO);
54250        TLEX('PROC      ', LXIOPROC);
54260        TLEX('REF       ', LXIOREF);
54270        TLEX('SHORT     ', LXIOSHORT);
54280        TLEX('SKIP      ', LXIOSKIP);
54290        TLEX('START     ', LXIOSTART);
54300        TLEX('STICK     ', LXIOSTICK);
54310        TLEX('STRGDEN   ', LXIOSTRGDEN);
54320        TLEX('STRUCT    ', LXIOSTRUCT);
54330        TLEX('SUB       ', LXIOSUB);
54340        TLEX('TAB       ', LXIOTAB);
54350        TLEX('TAG       ', LXIOTAG);
54360        TLEX('THEN      ', LXIOTHEN);
54370        TLEX('TO        ', LXIOTO);
54380        TLEX('VOID      ', LXIOVOID);
54390        TLEX('WHILE     ', LXIOWHILE);
54400        TLEX('BECOM     ', LXIOBECOM);
54410        TLEX('CLOSE     ', LXIOCLOSE);
54420        TLEX('COLON     ', LXIOCOLON);
54430        TLEX('EQUAL     ', LXIOEQUAL);
54440        TLEX('OPEN      ', LXIOOPEN);
54450        TLEX('PRIMDEN   ', LXIOPRDEN);
54460        TLEX('SEMIC     ', LXIOSEMIC);
54470        TLEX('STOP      ', LXIOSTOP);
54480        END;
54490  (*+01()   (*+31()   (*$T++)   ()+31+)   ()+01*)
54500  (*+25()   (*+31()   (*$T++)   ()+31+)   ()+25*)
54510      BEGIN (*PARSEPARSER*)
54520      CLASSES;
54530      TLEXS;
54540      (*FLOYD PRODUCTION RULES WHICH WERE USED WITH THE OLD VERSION OF SEMANTICROUTINE GIVEN ABOVE
54550        TO PRODUCE THE CALLS OF BMPROD AND BLABL WHICH FOLLOW*)
54560      (*
54570                        BEGIN:     !                    =>       ,          !     +  INIT;
54580                        INIT:      !                    =>       ,          !     +  PRODRL;
54590                        PRODRL:    STOP@!               => 36    ,1->       !        APRODRL;
54600                        APRODRL:   TAG@!COLON@          => 34    ,1->       ! (1) +  ALABEL;
54610                        ALABEL:    !                    => 10    ,          !        BLABEL;
54620                        BLABEL:    TAG@!AT@             => 11    ,1->       ! (1)    COMMA;
54630                                   TAB@!                => 12    ,1->       !        COMMA;
54640                                   TAG@!                => 11    ,1->       !        COMMA;
54650                                   STICK@!              => 13    ,1->       !     +  ASTICK,ERROR;
54660                        COMMA:     !COMMA@              => 14    ,          ! (1) +  BLABEL;
54670                        STICK:     !STICK@              => 15    ,          ! (1) +  ASTICK,ERROR;
54680                        ASTICK:    TAG@!AT@             => 11    ,1->       ! (1) +  EQUAL;
54690                                   TAB@!                => 12    ,1->       !     +  EQUAL;
54700                                   TAG@!                => 11    ,1->       !     +  EQUAL;
54710                                   EQUAL@!              => 35    ,          !        EQUAL;
54720                        EQUAL:     EQUAL@!OPR@          =>       ,1->       ! (1) +  AEQUAL,ERROR;
54730                        AEQUAL:    TAG@!OPR@            => 16    ,          ! (1)    FSEM;
54740                                   !                    => 17    ,          !        ASEM;
54750                        ASEM:      OPR@!                =>       ,1->       !     +  ASEM2;
54760                        ASEM2:     COMMA@!PRIMDEN@      => 18    ,1->       ! (1) ++ PUSH;
54770                        STICK2:    COMMA@!STICK@        => 19    ,1->       !     ++ ASTICK2,ERROR;
54780                        PUSH:      OPR@,ANY!TAG@        => 20    ,2->       ! (1) ++ ASTICK2;
54790                                   OPR@,ANY!STICK@      => 21    ,2->       !     ++ ASTICK2,ERROR;
54800                        ASTICK2:   OPEN@!PRIMDEN@       => 22    ,2->       ! (1) ++ STAR;
54810                                   !                    => 23    ,          !        STAR;
54820                        STAR:      OPR@!OPR@            => 24    ,1->       ! (1) +  SEX;
54830                                   OPR@!                => 25    ,1->       !     +  SEX;
54840                                   !                    => 26    ,          !        SEX;
54850                        SEX:       TAG@!                => 28    ,2->       !     +  FEX,ERROR;
54860                        FEX:       COMMA@!TAG@          => 29    ,1->       ! (1) +  SEMI;
54870                                   !                    => 30    ,          !        SEMI;
54880                        SEMI:      SEMIC@!              => 31    ,1->       !        INIT;
54890                        ERROR:     START@!              => 32    ,          !        ERR;
54900                                   !                    =>       ,1->       !        ERROR;
54910                        ERR:       !SEMIC@              => 31    ,          ! (1)    INIT;
54920                                   !STOP@               => 36    ,1->       !        ERROR;
54930                                   !                    =>       ,          ! (1)    ERR;
54940                        FSEM:      TAG@!PRSMDEN@        => 16    ,1->       ! (1) +  ASEM,ERROR;
54950      *)
54960      BMPROD(  1, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    00  , 0, LXIODUMMY  , FALSE, 1,    0,    2);
54970      BLABL(  1,   0,   2);
54980      BMPROD(  2, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    00  , 0, LXIODUMMY  , FALSE, 1,    0,    3);
54990      BLABL(  2,   0,   3);
55000      BMPROD(  3, S , LXIOSTOP   ,  0, 0, A , LXIODUMMY  ,  0,    36  , 1, LXIODUMMY  , FALSE, 0,    0,    4);
55010      BLABL(  3,   0,   4);
55020      BMPROD(  4, S , LXIOTAG    ,  0, 0, S , LXIOCOLON  ,  0,    34  , 1, LXIODUMMY  , TRUE , 1,    0,    5);
55030      BLABL(  4,   0,   5);
55040      BMPROD(  5, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    10  , 0, LXIODUMMY  , FALSE, 0,    0,    6);
55050      BLABL(  5,   0,   6);
55060      BMPROD(  6, S , LXIOTAG    ,  0, 0, S , LXIOAT     ,  0,    11  , 1, LXIODUMMY  , TRUE , 0,    0,    7);
55070      BMPROD(  7, S , LXIOTAB    ,  0, 0, A , LXIODUMMY  ,  0,    12  , 1, LXIODUMMY  , FALSE, 0,   -6,    8);
55080      BMPROD(  8, S , LXIOTAG    ,  0, 0, A , LXIODUMMY  ,  0,    11  , 1, LXIODUMMY  , FALSE, 0,   -7,    9);
55090      BMPROD(  9, S , LXIOSTICK  ,  0, 0, A , LXIODUMMY  ,  0,    13  , 1, LXIODUMMY  , FALSE, 1,    0,    0);
55100      BLABL(  8,   0,  10);
55110      BMPROD( 10, S , LXIODUMMY  ,  0, 3, S , LXIOCOMMA  ,  0,    14  , 0, LXIODUMMY  , TRUE , 1,    6,   11);
55120      BMPROD( 11, S , LXIODUMMY  ,  0, 3, S , LXIOSTICK  ,  0,    15  , 0, LXIODUMMY  , TRUE , 1,   -9,   -9);
55130      BLABL( 11,   0,  12);
55140      BMPROD( 12, S , LXIOTAG    ,  0, 0, S , LXIOAT     ,  0,    11  , 1, LXIODUMMY  , TRUE , 1,    0,   13);
55150      BMPROD( 13, S , LXIOTAB    ,  0, 0, A , LXIODUMMY  ,  0,    12  , 1, LXIODUMMY  , FALSE, 1,  -12,   14);
55160      BMPROD( 14, S , LXIOTAG    ,  0, 0, A , LXIODUMMY  ,  0,    11  , 1, LXIODUMMY  , FALSE, 1,  -13,   15);
55170      BMPROD( 15, S , LXIOEQUAL  ,  0, 0, A , LXIODUMMY  ,  0,    35  , 0, LXIODUMMY  , FALSE, 0,  -14,   16);
55180      BLABL( 15,   0,  16);
55190      BMPROD( 16, S , LXIOEQUAL  ,  0, 0, S , LXIOOPR    ,  0,    00  , 1, LXIODUMMY  , TRUE , 1,    0,  -11);
55200      BLABL( 16,   0,  17);
55210      BMPROD( 17, S , LXIOTAG    ,  0, 0, S , LXIOOPR    ,  0,    00 , 0, LXIODUMMY  , TRUE , 0,    0,   18);
55220      BMPROD( 18, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    17 , 0, LXIODUMMY  , FALSE, 0,    0,   19);
55230      BLABL( 18,   0,  19);
55240      BMPROD( 19, S , LXIOOPR    ,  0, 0, A , LXIODUMMY  ,  0,    00  , 1, LXIODUMMY  , FALSE, 1,    0,   20);
55250      BLABL( 19,   0,  20);
55260      BMPROD( 20, S , LXIOCOMMA  ,  0, 0, S , LXIOPRDEN  ,  0,    18  , 1, LXIODUMMY  , TRUE , 2,    0,   21);
55270      BMPROD( 21, S , LXIOCOMMA  ,  0, 0, S , LXIOSTICK  ,  0,    19  , 1, LXIODUMMY  , FALSE, 2,    0,  -16);
55280      BLABL( 20,   0,  22);
55290      BMPROD( 22, S , LXIOOPR    ,  0, 1, S , LXIOTAG    ,  0,    20  , 2, LXIODUMMY  , TRUE , 2,  -21,   23);
55300      BMPROD( 23, S , LXIOOPR    ,  0, 1, S , LXIOSTICK  ,  0,    21  , 2, LXIODUMMY  , FALSE, 2,  -22,  -21);
55310      BLABL( 23,   0,  24);
55320      BMPROD( 24, S , LXIOOPEN   ,  0, 0, S , LXIOPRDEN  ,  0,    22  , 2, LXIODUMMY  , TRUE , 2,    0,   25);
55330      BMPROD( 25, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    23  , 0, LXIODUMMY  , FALSE, 0,  -24,   26);
55340      BLABL( 25,   0,  26);
55350      BMPROD( 26, S , LXIOOPR    ,  0, 0, S , LXIOOPR    ,  0,    24 , 1, LXIODUMMY  , TRUE , 1,    0,   27);
55360      BMPROD( 27, S , LXIOOPR    ,  0, 0, A , LXIODUMMY  ,  0,    25  , 1, LXIODUMMY  , FALSE, 1,  -26,   28);
55370      BMPROD( 28, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    26  , 0, LXIODUMMY  , FALSE, 0,  -27,   29);
55380      BLABL( 28,   0,  29);
55390      BMPROD( 29, S , LXIOTAG    ,  0, 0, A , LXIODUMMY  ,  0,    28  , 2, LXIODUMMY  , FALSE, 1,    0,  -23);
55400      BLABL( 29,   0,  30);
55410      BMPROD( 30, S , LXIOCOMMA  ,  0, 0, S , LXIOTAG    ,  0,    29  , 1, LXIODUMMY  , TRUE , 1,    0,   31);
55420      BMPROD( 31, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    30  , 0, LXIODUMMY  , FALSE, 0,  -30,   32);
55430      BLABL( 31,   0,  32);
55440      BMPROD( 32, S , LXIOSEMIC  ,  0, 0, A , LXIODUMMY  ,  0,    31  , 1, LXIODUMMY  , FALSE, 0,    2,   33);
55450      BLABL(  0,  29,  33);
55460      BMPROD( 33, S , LXIOSTART  ,  0, 0, A , LXIODUMMY  ,  0,    32  , 0, LXIODUMMY  , FALSE, 0,    0,   34);
55470      BMPROD( 34, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    00  , 1, LXIODUMMY  , FALSE, 0,   33,   35);
55480      BLABL( 33,   0,  35);
55490      BMPROD( 35, S , LXIODUMMY  ,  0, 3, S , LXIOSEMIC  ,  0,    31  , 0, LXIODUMMY  , TRUE , 0,    2,   36);
55500      BMPROD( 36, S , LXIODUMMY  ,  0, 3, S , LXIOSTOP   ,  0,    36  , 1, LXIODUMMY  , FALSE, 0,   33,   37);
55510      BMPROD( 37, S , LXIODUMMY  ,  0, 3, A , LXIODUMMY  ,  0,    00  , 0, LXIODUMMY  , TRUE , 0,   35,   38);
55520      BLABL( 17,   0,  38);
55530      BMPROD( 38, S , LXIOTAG    ,  0, 0, S , LXIOPRDEN  ,  0,    16  , 1, LXIODUMMY  , TRUE , 1,   19,   33);
55540      ERRS := 0; INITIO; INITLX; INITPR;
55550      PRAGFLGS := PRAGFLGS + [PRGPOINT] - [PRGUPPER];
55560      BCOUNT := 1;
55570      CCOUNT := 0;
55580      PARSER;
55590      IF CCOUNT<>0 THEN WRITELN(LSTFILE,'CCOUNT ERROR', CCOUNT);
55600      WRITELN(LSTFILE,'LAST PROD', BCOUNT-1);
55610  (*+01() J := GETB(4); ()+01*)
55620      FOR I := 0 TO HTSIZE DO (*GET RID OF ALL UNWANTED LEXEMES*)
55630        BEGIN THIS := HT[I];
55640        WHILE THIS<>HTCOPY[I] DO
55650          BEGIN
55660          THAT := THIS^.LINK;
55670          EDISPOSE(THIS, THIS^.LXCOUNT*SZWORD+LEX1SIZE);
55680          THIS := THAT;
55690          END;
55700        END;
55710      HT := HTCOPY; (*RESTORE HT TO STATE BEFORE FRED*)
55720  (*+01()
55730      FOR I := J TO ORD(FRED) DO
55740        BEGIN FRIG.INT := I; FRIG.POINT^ := 0 END;
55750  ()+01*)
55760      END;
55770  (**)
55780  ()+82*)
