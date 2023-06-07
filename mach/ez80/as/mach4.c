/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Zilog Z80 parsing rules
 */
operation
    : NOOPOP
        { emit1($1); }

    | LDOP ldargs

    | PSHPOP R24
        {
            switch ($2) {
                case BC: emit1($1); break;
                case DE: emit1($1 | 020); break;
                case AF: emit1($1 | 060); break;
                case HL:
                case IX:
                case IY: xyreg($2,$1 | 040); break;
                default: serror("register error");
            }
        }

    | EXOP R24 ',' R24
        {
            if ($2==DE && $4==HL)
                emit1(0353);
            else if ($2==AF && $4==AF2)
                emit1(010);
            else
                serror("register error");
        }

    |   EXOP '(' R24 ')' ',' R24
        {
            if ($3!=SP)
                serror("register error");
            xyreg($6,$1);
        }

    |   E_ED
            {   emit1(0355); emit1($1);}
    |   ADDOP R24 ',' R24
            {   if ($4 == $2)
                    xyreg($2,051);
                else {
                    if ($4==HL || $4>SP)
                        serror("register error");
                    xyreg($2,011 | $4<<4);
                }
            }
    |   ADCSBC R24 ',' R24
            {   if ($2!=HL || $4>SP) serror("register error");
                emit1(0355);
                emit1(($1==0210 ? 0112 : 0102) | ($4<<4));
            }
    |   INCDEC R24
            {   switch ($2) {
                case BC: case DE: case SP:
                    emit1(($1==04 ? 03 : 013) | $2<<4);
                    break;
                case HL: case IX: case IY:
                    xyreg($2,$1==04 ? 043 : 053);
                    break;
                default: serror("register error");
                }
            }
    |   INCDEC r8
            {   emit1($1 | $2<<3);}
    |   INCDEC ind
            {   xymem($2,$1 | 060);}
    |   ari8 r8
            {   emit1($1 | $2);}
    |   ari8 expr
            {   emit1($1 | 0106); emit1($2.val);}
    |   ari8 ind
            {   xymem($2,$1 | 06);}
    |   IMOP absexp
            {   emit1(0355);
                switch ($2) {
                case 0: emit1(0106); break;
                case 1: emit1(0126); break;
                case 2: emit1(0136); break;
                default: serror("range 0..2");
                }
            }
    |   ROTATE r8
            {   emit1(0313); emit1($1 | $2);}
    |   ROTATE ind
            {   xymem($2,0313); emit1($1 | 06);}
    |   BITS absexp ',' r8
            {   fit(fit3($2));
                emit1(0313); emit1($1 | low3($2)<<3 | $4);
            }
    |   BITS absexp ',' ind
            {   fit(fit3($2));
                xymem($4,0313);
                emit1($1 | low3($2)<<3 | 06);
            }
    |   JP expr
            {   emit1($1);
#ifdef RELOCATION
                newrelo($2.typ, RELO3);
#endif
                emit3($2.val);
            }

    |   JP coco ',' expr
            {   emit1(0302 | $2<<3);
#ifdef RELOCATION
                newrelo($4.typ, RELO3);
#endif
                emit3($4.val);
            }

    |   JP indir
            {   xyreg($2,0351);}
    |   JR expr
            {   branch($1,$2);}
    |   JR coco ',' expr
            {   if ($2 > 3) serror("bad condition code");
                branch(040 | ($2<<3), $4);
            }
    |   DJNZ expr
            {   branch($1,$2);}
    |   CALL expr
            {   emit1($1);
#ifdef RELOCATION
                newrelo($2.typ, RELO3);
#endif
                emit3($2.val);
            }

    |   CALL coco ',' expr
            {   emit1(0304 | $2<<3);
#ifdef RELOCATION
                newrelo($4.typ, RELO3);
#endif
                emit3($4.val);
            }

    |   RET
            {   emit1($1);}
    |   RET coco
            {   emit1(0300 | $2<<3);}
    |   RST absexp
            {   if (($2&070)!=$2)
                    serror("rst expression out of range");
                emit1($1 | $2);
            }
    |   IN R8 ',' expr
            {   if ($2 != A) serror("register error");
                emit1($1);
#ifdef RELOCATION
                newrelo($4.typ, RELO1);
#endif
                emit1($4.val);
            }
    |   IN R8 ',' R8
            {   if ($4 != C) serror("register error");
                emit1(0355); emit1(0100 | $2<<3);
            }
    |   OUT expr ',' R8
            {   if ($4 != A) serror("register error");
                emit1($1);
#ifdef RELOCATION
                newrelo($2.typ, RELO1);
#endif
                emit1($2.val);
            }
    |   OUT R8 ',' R8
            {   if ($2 != C) serror("register error");
                emit1(0355); emit1(0101 | $4<<3);
            }

    | LEA R24 ',' R24 '+' expr
            {
                exp_ind = $6;
                RELOMOVE(rel_ind, relonami);

                xylea($2, $4);
            }

    | LEA R24 ',' R24 '-' expr
            {
                fit(fitb($6.val));
                exp_ind = $6;
                RELOMOVE(rel_ind, relonami);

                xylea($2, $4);
            }

    | PEA R24 '+' expr
            {
                exp_ind = $4;
                RELOMOVE(rel_ind, relonami);

                xylea(SP, $2);
            }

    | PEA R24 '-' expr
            {
                fit(fitb($4.val));
                exp_ind = $4;
                RELOMOVE(rel_ind, relonami);

                xylea(SP, $2);
            }
    ;

ari8    :   ARI8
    |   ADDOP R8 ','
            {   if ($2 != A) serror("register error");}
    |   ADCSBC R8 ','
            {   if ($2 != A) serror("register error");}
    ;

ldargs:
    R8 ',' R8
            {
                if ($1==F || $3==F)
                    serror("register error");
                if ($1<=A && $3<=A)
                    emit1(0100 | $1<<3 | $3);
                else {
                    emit1(0355);
                    if ((($1==A) ^ ($3==A)) == 0)
                        serror("register error");
                    emit1(  0107 |
                        ($1==R||$3==R)<<3 |
                        ($1==A)<<4
                    );
                }
            }

    |   R8 ',' expr
            {
                if ($1==F || $1==I || $1==R)
                    serror("register error");
                emit1(06 | $1<<3);
#ifdef RELOCATION
                newrelo($3.typ, RELO1);
#endif
                emit1($3.val);
            }

    |   R8 ',' indir
            {
                if ($1==F || $1==I || $1==R)
                    serror("register error");
                if ($1==A && ($3==BC || $3==DE))
                    emit1($3==BC ? 012 : 032);
                else
                    xymem($3,0106 | $1<<3);
            }

    |   R8 ',' index
            {   if ($1==F || $1==I || $1==R)
                    serror("register error");
                xymem($3,0106 | $1<<3);
            }

    |   R8 ',' '(' expr ')'
            {
                if ($1!=A) serror("register error");
                    emit1(072);
#ifdef RELOCATION
                newrelo($4.typ, RELO3);
#endif
                emit3($4.val);
            }

    |   indir ',' r8
            {
                if ($3==A && ($1==BC || $1==DE))
                    emit1($1==BC ? 02 : 022);
                else
                    xymem($1,0160 | $3);
            }

    |   index ',' r8
            {
                xymem($1,0160 | $3);
            }

    |   indir ',' expr
            {
                xymem($1,066);
#ifdef RELOCATION
                newrelo($3.typ, RELO1);
#endif
                emit1($3.val);
            }

    |   index ',' expr
            {   xymem($1,066);
#ifdef RELOCATION
                newrelo($3.typ, RELO1);
#endif
                emit1($3.val);
            }
    |   R24 ',' expr
            {   switch ($1) {
                case BC: case DE: case HL: case SP:
                    emit1(01 | $1<<4); break;
                case IX: case IY:
                    xyreg($1,041); break;
                default: serror("register error");
                }
#ifdef RELOCATION
                newrelo($3.typ, RELO3);
#endif
                emit3($3.val);
            }

    |   R24 ',' R24
            {   if ($1!=SP) serror("register error");
                xyreg($3,0371);
            }
    |   R24 ',' '(' expr ')'
            {
                switch ($1)
                {
                    case BC: case DE: case SP:
                        emit1(0355); emit1(0113 | $1<<4); break;
                    case HL: case IX: case IY:
                        xyreg($1,052); break;
                    default: serror("register error");
                }
#ifdef RELOCATION
                newrelo($4.typ, RELO3);
#endif
                emit3($4.val);
            }

    | R24 ',' ind
            {
                switch ($1)
                {
                    case BC: case DE: case HL:
                        xymem($3, ($1 << 4) | 0x07);
                        break;

                    case IX: case IY:
                        xymem($3, (($1 == IX) ^ ($3 == IY)) ? 0x37 : 0x31);
                        break;

                    default:
                        serror("register error");
                }
            }

    | indir ',' R24
            {
                if ($1 != HL)
                    serror("register error");

                emit1(0xed);
                emit1(($3 << 4) | 0x0f);
            }

    | index ',' R24
            {
                switch ($3)
                {
                    case BC:
                    case DE:
                    case HL:
                        xymem($1, ($3 << 4) | 0x0f);
                        break;

                    default:
                        serror("register error");
                }
            }

    |   '(' expr ')' ',' R8
            {   if ($5!=A) serror("register error");
                emit1(062);
#ifdef RELOCATION
                newrelo($2.typ, RELO3);
#endif
                emit3($2.val);
            }
    |   '(' expr ')' ',' R24
            {   switch ($5) {
                case BC: case DE: case SP:
                    emit1(0355); emit1(0103 | $5<<4); break;
                case HL: case IX: case IY:
                    xyreg($5,042); break;
                default: serror("register error");
                }
#ifdef RELOCATION
                newrelo($2.typ, RELO3);
#endif
                emit3($2.val);
            }
    ;

r8  :   R8
            {   if ($1==F || $1==I || $1==R)
                    serror("register error");
            }
    ;

indir:
    '(' R24 ')'
            {
                if ($2>=SP && $2!=IX && $2!=IY)
                    serror("register error");
                exp_ind.typ = S_ABS; exp_ind.val = 0;
                $$ = $2;
            }
    ;

index:
    '(' R24 '+' expr ')'
            {
                if ($2!=IX && $2!=IY)
                    serror("register error");
                exp_ind = $4;
                RELOMOVE(rel_ind, relonami);
                $$ = $2;
            }

    | '(' R24 '-' expr ')'
            {
                if ($2!=IX && $2!=IY)
                    serror("register error");
                fit(fitb($4.val));
                exp_ind = $4;
                RELOMOVE(rel_ind, relonami);
                $$ = $2;
            }
    ;

ind:
    indir
    | index
    ;

coco:
    CC
    | R8
            {
                if ($1 != C) serror("bad condition code");
                    $$ = 3;
            }
    ;

// vim: ts=4 sw=4 et
