#ifndef __TOMIPS_BETTER_H__
#define __TOMIPS_BETTER_H__

void init_MIPS();
void LIT_Processing(int immediateNumber);
void LIT_FOR_WRT_Processing(int immediateNumber);
void LOD_Processing(int offset);
void LOD_FOR_WRT_Processing(int offset);
void STO_Processing(int offset);
void CAL_Processing(int SN_FUNC);
void INT_Processing(int SN_FUNC,int Size);
void INT_FOR_MAIN_Processing(int Size);
void JMP_Processing(int SN,int jumpAddress);
void JPC_Processing(int jumpAddress);
void JUMP_Lable_Writing(int Lable_NUM);
void WHILE_START_PRE_Writing();
void ADD_Processing();
void SUB_Processing();
void MUL_Processing();
void DIV_Processing();
void RED_Processing();
void WRT_Processing();
void RET_Processing();
void RET_FOR_MAIN_Processing();
void redundant_WHILE_START_Cleaning();
void PrintMipsCodeList();

void ToMIPS_Better();

#endif




