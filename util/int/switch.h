/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-10-23
 *
 */
#ifndef SWITCH_H_
#define SWITCH_H_

#include "global.h"	/* size */

/* switch.c */
void do_instr(unsigned int);

/* do_*.c */
void DoAAR(size);
void DoADF(size);
void DoADI(size);
void DoADP(long);
void DoADS(size);
void DoADU(size);
void DoAND(size);
void DoASP(long);
void DoASS(size);
void DoBEQ(long);
void DoBGE(long);
void DoBGT(long);
void DoBLE(long);
void DoBLM(size);
void DoBLS(size);
void DoBLT(long);
void DoBNE(long);
void DoBRA(long);
void DoCAI(void);
void DoCAL(long);
void DoCFF(void);
void DoCFI(void);
void DoCFU(void);
void DoCIF(void);
void DoCII(void);
void DoCIU(void);
void DoCMF(size);
void DoCMI(size);
void DoCMP(void);
void DoCMS(size);
void DoCMU(size);
void DoCOM(size);
void DoCSA(size);
void DoCSB(size);
void DoCUF(void);
void DoCUI(void);
void DoCUU(void);
void DoDCH(void);
void DoDEC(void);
void DoDEE(long);
void DoDEL(long);
void DoDUP(size);
void DoDUS(size);
void DoDVF(size);
void DoDVI(size);
void DoDVU(size);
void DoEXG(size);
void DoFEF(size);
void DoFIF(size);
void DoFIL(unsigned long);
void DoGTO(unsigned long);
void DoINC(void);
void DoINE(long);
void DoINL(long);
void DoINN(size);
void DoIOR(size);
void DoLAE(unsigned long);
void DoLAL(long);
void DoLAR(size);
void DoLDC(long);
void DoLDE(long);
void DoLDF(long);
void DoLDL(long);
void DoLFR(size);
void DoLIL(long);
void DoLIM(void);
void DoLIN(unsigned long);
void DoLNI(void);
void DoLOC(long);
void DoLOE(long);
void DoLOF(long);
void DoLOI(size);
void DoLOL(long);
void DoLOR(long);
void DoLOS(size);
void DoLPB(void);
void DoLPI(long);
void DoLXA(unsigned long);
void DoLXL(unsigned long);
void DoMLF(size);
void DoMLI(size);
void DoMLU(size);
void DoMON(void);
void DoNGF(size);
void DoNGI(size);
void DoNOP(void);
void DoRCK(size);
void DoRET(size);
void DoRMI(size);
void DoRMU(size);
void DoROL(size);
void DoROR(size);
void DoRTT(void);
void DoSAR(size);
void DoSBF(size);
void DoSBI(size);
void DoSBS(size);
void DoSBU(size);
void DoSDE(unsigned long);
void DoSDF(long);
void DoSDL(long);
void DoSET(size);
void DoSIG(void);
void DoSIL(long);
void DoSIM(void);
void DoSLI(size);
void DoSLU(size);
void DoSRI(size);
void DoSRU(size);
void DoSTE(unsigned long);
void DoSTF(long);
void DoSTI(size);
void DoSTL(long);
void DoSTR(long);
void DoSTS(size);
void DoTEQ(void);
void DoTGE(void);
void DoTGT(void);
void DoTLE(void);
void DoTLT(void);
void DoTNE(void);
void DoTRP(void);
void DoXOR(size);
void DoZEQ(long);
void DoZER(size);
void DoZGE(long);
void DoZGT(long);
void DoZLE(long);
void DoZLT(long);
void DoZNE(long);
void DoZRE(long);
void DoZRF(size);
void DoZRL(long);

#endif /* SWITCH_H_ */
