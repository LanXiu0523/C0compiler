/*******************************************************************************

 Filename: ToMIPS_Better.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-07-07

 Description: Optimized object stack instruction code to MIPS code.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "ToMIPS.h"

    /*
     *╔════════════════════════════════════╗════════════════════════════════════════════════════════╗
     *║        指令助记符与指令格式        ║                      指令功能简述                      ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  li      $t0     Immde             ║  取立即数Immde至寄存器$t0                              ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  lw      $t0     Ofs($fp)          ║  取内存Ofs($fp)中的值至寄存器$t0                       ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  la      $a0     Msg               ║  取Msg的地址至$a0                                      ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  sw      $t0     Ofs($fp)          ║  存$t0的值至内存Ofs($fp)                               ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  subi    $t0     $t0     Immde     ║  减立即数,$t0 <- $t0-Immde                             ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  add     $t0     $t0     $t1       ║  加法运算,$t0 <- $t0+$t1                               ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  sub     $t0     $t0     $t1       ║  减法运算,$t0 <- $t0-$t1                               ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  mul     $t0     $t0     $t1       ║  乘法运算,$t0 <- $t0*$t1                               ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  div     $t0     $t0     $t1       ║  除法运算,$t0 <- $t0/$t1                               ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  beq     $t0     $t1     _Lable    ║  相等跳转,若$t0==$t1,则跳转至_Lable                    ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  j               _Lable            ║  无条件跳转,跳转至_Lable                               ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  jr              $ra               ║  无条件寄存器跳转,跳转至寄存器$ra储存的地址            ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  jal             Func              ║  无条件直接跳转至子程序Func,并保存返回地址至寄存器$ra  ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  move    $s0     $s1               ║  寄存器间值传递,$s0 <- $s1                             ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  ori     $s0     $s1     Immde     ║  立即数按位或,$s0 <- $s1 | (zero-extend)Immde          ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  nop                               ║  空转指令                                              ║
     *╠════════════════════════════════════╬════════════════════════════════════════════════════════╣
     *║  syscall                           ║  系统调用                                              ║
     *╚════════════════════════════════════╩════════════════════════════════════════════════════════╝
     */


FILE *f_objectCodeToMIPS_Better;
/*L型指令计数器*/
int L_Counter_Better=0;
/*RED型指令标记*/
int RED_Flag_Better=0;
/*Main函数开始的栈式指令序号*/
int SN_Main_Better=0;
/*外部函数计数器*/
int FUNC_Counter_Better=0;
/*外部函数数据寄存器，REG的index为该函数开始的栈式指令序号，值为FUNC序号*/
int FUNC_REG_Better[2047];
/*跳转计数器*/
int JUMP_Counter_Better=0;
/*跳转数据寄存器，REG的index为跳转目标的栈式指令行序号，值为Lable序号*/
int JUMP_REG_Better[2047];
/*While函数跳转索引预写计数器*/
int WHILE_START_JMP_Counter_Better=-1;
/*While函数跳转数据寄存器，REG的index为While函数跳转索引的序号，值为1表示完整While跳转，
 *反之则预期错误，后续删除多余的While函数跳转索引预写*/
int WHILE_START_JMP_REG_Better[127]= {0};


/*
 * 初始化
 */
void init_MIPS_Better()
{
    fprintf(f_objectCodeToMIPS_Better,".data");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"    Enter: .asciiz  \"\\n\"");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,".text");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"ori     $fp     $sp     0");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"li      $t9     0x7fffeffc	#global stack bottom");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"li      $t8     0x10010000	#save word");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"        j	__main");

}


/*
 * LIT指令处理
 */
void LIT_Processing_Better(int immediateNumber_Better)
{
    fprintf(f_objectCodeToMIPS_Better,"li      $t%d     %d",L_Counter_Better,immediateNumber_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better++;

}


/*
 * LIT指令处理(针对WRT前的LIT，取立即数至$a0)
 */
void LIT_FOR_WRT_Processing_Better(int immediateNumber_Better)
{
    fprintf(f_objectCodeToMIPS_Better,"li      $a0     %d",immediateNumber_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * LOD指令处理
 */
void LOD_Processing_Better(int offset_Better)
{
    fprintf(f_objectCodeToMIPS_Better,"lw      $t%d     %d($fp)",L_Counter_Better,(-4)*offset_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better++;

}


/*
 * LOD指令处理(针对WRT前的LOD，取立即数至$a0)
 */
void LOD_FOR_WRT_Processing_Better(int offset_Better)
{
    fprintf(f_objectCodeToMIPS_Better,"lw      $a0     %d($fp)",(-4)*offset_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * STO指令处理
 */
void STO_Processing_Better(int offset_Better)
{
    if(RED_Flag_Better)
    {
        fprintf(f_objectCodeToMIPS_Better,"sw      $v0     %d($fp)",(-4)*offset_Better);
        fprintf(f_objectCodeToMIPS_Better,"\n");
        RED_Flag_Better--;

    }

    else
    {
        fprintf(f_objectCodeToMIPS_Better,"sw      $t0     %d($fp)",(-4)*offset_Better);
        fprintf(f_objectCodeToMIPS_Better,"\n");
        L_Counter_Better--;

    }

}


/*
 * CAL指令处理
 */
void CAL_Processing_Better(int SN_FUNC_Better)
{
    fprintf(f_objectCodeToMIPS_Better,"move    $s0     $sp");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"jal             Func%d",FUNC_REG_Better[SN_FUNC_Better]);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"nop");
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * INT指令处理
 */
void INT_Processing_Better(int SN_FUNC_Better,int Size_Better)
{
    FUNC_Counter_Better++;
    FUNC_REG_Better[SN_FUNC_Better]=FUNC_Counter_Better;
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"Func%d:",FUNC_Counter_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"#Save Register");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $s0     0($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $fp     0($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"add     $fp     $sp     $0");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $ra     ($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"#Save Register Done!");
    fprintf(f_objectCodeToMIPS_Better,"\n");

    fprintf(f_objectCodeToMIPS_Better,"li      $t0     0");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $t0     ($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     %d",4*Size_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * INT指令处理(针对Main函数下的INT)
 */
void INT_FOR_MAIN_Processing_Better(int Size_Better)
{
    FUNC_Counter_Better++;
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"__main:");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"#Save Register");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $s0     0($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $fp     0($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"add     $fp     $sp     $0");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $ra     ($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"#Save Register Done!");
    fprintf(f_objectCodeToMIPS_Better,"\n");

    fprintf(f_objectCodeToMIPS_Better,"li      $t0     0");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"sw      $t0     ($sp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"subi    $sp     $sp     %d",4*Size_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * JMP指令处理
 */
void JMP_Processing_Better(int SN,int jumpAddress_Better)
{
    if(SN>jumpAddress_Better)
    {
        WHILE_START_JMP_REG_Better[WHILE_START_JMP_Counter_Better]=1;
        fprintf(f_objectCodeToMIPS_Better,"j               __WhileStart%02d",WHILE_START_JMP_Counter_Better);
        fprintf(f_objectCodeToMIPS_Better,"\n");

    }

    else
    {
        JUMP_REG_Better[jumpAddress_Better]=JUMP_Counter_Better;
        fprintf(f_objectCodeToMIPS_Better,"j               __Lable%d",JUMP_Counter_Better);
        fprintf(f_objectCodeToMIPS_Better,"\n");
        JUMP_Counter_Better++;

    }

}


/*
 * JPC指令处理
 */
void JPC_Processing_Better(int jumpAddress_Better)
{
    JUMP_REG_Better[jumpAddress_Better]=JUMP_Counter_Better;
    fprintf(f_objectCodeToMIPS_Better,"li      $t%d     0",L_Counter_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"beq     $t%d     $t%d     __Lable%d",L_Counter_Better-1,L_Counter_Better,JUMP_Counter_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better--;
    JUMP_Counter_Better++;

}


/*
 * 跳转索引Lable的写入
 */
void JUMP_Lable_Writing_Better(int Lable_NUM_Better)
{
    fprintf(f_objectCodeToMIPS_Better,"__Lable%d:",Lable_NUM_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * While函数索引WhileStart的预写，
 * 若预写非预期，在最后调用的redundant_WHILE_START_Cleaning_Better()函数中消除冗余索引
 */
void WHILE_START_PRE_Writing_Better()
{
    WHILE_START_JMP_Counter_Better++;
    fprintf(f_objectCodeToMIPS_Better,"__WhileStart%02d:",WHILE_START_JMP_Counter_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * ADD指令处理
 */
void ADD_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"add     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better--;

}


/*
 * SUB指令处理
 */
void SUB_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"sub     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better--;

}


/*
 * MUL指令处理
 */
void MUL_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"mul     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better--;

}


/*
 * DIV指令处理
 */
void DIV_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"div     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    L_Counter_Better--;

}


/*
 * RED指令处理
 */
void RED_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"li      $v0     5");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"syscall");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    RED_Flag_Better++;

}


/*
 * WRT指令处理
 */
void WRT_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"li      $v0     1");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"syscall");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"li      $v0     4");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"la      $a0     Enter");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"syscall");
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * RET指令处理
 */
void RET_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"__FEND_LAB_%d:",FUNC_Counter_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"lw      $ra     0($fp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"lw      $sp     8($fp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"lw      $fp     4($fp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"jr              $ra");
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * RET指令处理(针对Main函数下的RET)
 */
void RET_FOR_MAIN_Processing_Better()
{
    fprintf(f_objectCodeToMIPS_Better,"__FEND_LAB_%d:",FUNC_Counter_Better);
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"lw      $ra     0($fp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"lw      $sp     8($fp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"lw      $fp     4($fp)");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"li  	$v0 	10");
    fprintf(f_objectCodeToMIPS_Better,"\n");
    fprintf(f_objectCodeToMIPS_Better,"syscall");
    fprintf(f_objectCodeToMIPS_Better,"\n");

}


/*
 * 消除冗余的WhileStart索引
 *
 * 根据WHILE_START_JMP_REG_Better寄存器内容，将非完全While跳转的WhileStart索引删除
 */
void redundant_WHILE_START_Cleaning_Better()
{

    string tmpLineData;
    int WHILE_START_Counter = 0;
    ifstream inGO("f_objectCodeToMIPS_Better.asm.tmp");
    ofstream outGO("f_objectCodeToMIPS_TMP.tmp", ios::out | ios::trunc);

    while (!inGO.eof())
    {
        getline(inGO,tmpLineData);

        if(tmpLineData[0]=='_'&&
                tmpLineData[1]=='_'&&
                tmpLineData[2]=='W'&&
                tmpLineData[3]=='h'&&
                tmpLineData[4]=='i'&&
                tmpLineData[5]=='l'&&
                tmpLineData[6]=='e'&&
                tmpLineData[7]=='S'&&
                tmpLineData[8]=='t'&&
                tmpLineData[9]=='a'&&
                tmpLineData[10]=='r'&&
                tmpLineData[11]=='t'&&
                tmpLineData[14]==':')
        {
            if(WHILE_START_JMP_REG_Better[((tmpLineData[12]-'0')*10+(tmpLineData[13]-'0'))]==1)
            {
                outGO<<"__WhileStart"<<WHILE_START_Counter<<":"<<endl;
            }

        }

        else if(tmpLineData[0]=='j'&&
                tmpLineData[16]=='_'&&
                tmpLineData[16]=='_'&&
                tmpLineData[18]=='W'&&
                tmpLineData[19]=='h'&&
                tmpLineData[20]=='i'&&
                tmpLineData[21]=='l'&&
                tmpLineData[22]=='e'&&
                tmpLineData[23]=='S'&&
                tmpLineData[24]=='t'&&
                tmpLineData[25]=='a'&&
                tmpLineData[26]=='r'&&
                tmpLineData[27]=='t')
        {
            if(WHILE_START_JMP_REG_Better[((tmpLineData[28]-'0')*10+(tmpLineData[29]-'0'))]==1)
            {
                outGO<<"j               __WhileStart"<<WHILE_START_Counter<<endl;
                WHILE_START_Counter++;

            }

        }

        else
        {
            outGO<<tmpLineData<<endl;
        }
    }

    outGO.close();
    inGO.close();


    ifstream inBACK("f_objectCodeToMIPS_TMP.tmp");
    ofstream outBACK("f_objectCodeToMIPS_Better.asm.tmp", ios::out | ios::trunc);

    while (!inBACK.eof())
    {
        getline(inBACK, tmpLineData);
        outBACK<<tmpLineData<<endl;

    }

    outBACK.close();
    inBACK.close();

    system("del f_objectCodeToMIPS_TMP.tmp");

}


/*
 * 打印生成的MIPS指令列表
 */
void PrintMipsCodeList_Better(){
    string tmpLineData;
    ifstream inPrint("f_objectCodeToMIPS_Better.asm.tmp");

    printf("\n");
    system("pause");
    printf("\n\n");

    printf("Virtual Machine Object Code Better To MIPS Code:\n\n");

    while (!inPrint.eof())
    {
        getline(inPrint,tmpLineData);
        cout<<tmpLineData<<endl;

    }

}



/*
 * 将生成的优化后的栈式指令代码转化为优化后的MIPS指令代码
 */
void ToMIPS_Better()
{
    ifstream in("f_objectCodeBetter.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    f_objectCodeToMIPS_Better = fopen("f_objectCodeToMIPS_Better.asm.tmp","w");

    memset(FUNC_REG_Better,-1,2047);
    memset(JUMP_REG_Better,-1,2047);
    char SN[10],SN_NEXT[10];
    char F[10],F_NEXT[10];
    int L,L_NEXT;
    int A,A_NEXT;

    init_MIPS_Better();

    in>>SN>>F>>L>>A;

    while(in>>SN_NEXT>>F_NEXT>>L_NEXT>>A_NEXT)
    {
        if(JUMP_REG_Better[((SN[1]-'0')*10+(SN[2]-'0'))]!=(-1))
        {
            JUMP_Lable_Writing_Better(JUMP_REG_Better[((SN[1]-'0')*10+(SN[2]-'0'))]);
        }

        if(strcmp(F_NEXT,"JPC")==0)
        {
            WHILE_START_PRE_Writing_Better();
        }


        if(strcmp(F,"LIT")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LIT_FOR_WRT_Processing_Better(A);
            }

            else
            {
                LIT_Processing_Better(A);
            }

        }

        else if(strcmp(F,"LOD")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LOD_FOR_WRT_Processing_Better(A-2);
            }

            else
            {
                LOD_Processing_Better(A-2);
            }

        }

        else if(strcmp(F,"STO")==0)
        {
            STO_Processing_Better(A-2);
        }

        else if(strcmp(F,"CAL")==0)
        {
            CAL_Processing_Better(A);
        }

        else if(strcmp(F,"INT")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==SN_Main_Better)
            {
                INT_FOR_MAIN_Processing_Better(A-3);
            }

            else
            {
                INT_Processing_Better(((SN[1]-'0')*10+(SN[2]-'0')),A-3);
            }

        }

        else if(strcmp(F,"JMP")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==0)
            {
                SN_Main_Better=A;
            }

            else
            {
                JMP_Processing_Better(((SN[1]-'0')*10+(SN[2]-'0')),A);
            }

        }

        else if(strcmp(F,"JPC")==0)
        {
            JPC_Processing_Better(A);
        }

        else if(strcmp(F,"ADD")==0)
        {
            ADD_Processing_Better();
        }

        else if(strcmp(F,"SUB")==0)
        {
            SUB_Processing_Better();
        }

        else if(strcmp(F,"MUL")==0)
        {
            MUL_Processing_Better();
        }

        else if(strcmp(F,"DIV")==0)
        {
            DIV_Processing_Better();
        }

        else if(strcmp(F,"RED")==0)
        {
            RED_Processing_Better();
        }

        else if(strcmp(F,"WRT")==0)
        {
            WRT_Processing_Better();
        }

        else if((strcmp(F,"RET")==0))
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==((SN_NEXT[1]-'0')*10+(SN_NEXT[2]-'0')))
            {
                RET_FOR_MAIN_Processing_Better();
            }
            else
            {
                RET_Processing_Better();
            }
        }


        strcpy(SN,SN_NEXT);
        strcpy(F,F_NEXT);
        L=L_NEXT;
        A=A_NEXT;

    }


    do
    {
        if(JUMP_REG_Better[((SN[1]-'0')*10+(SN[2]-'0'))]!=(-1))
        {
            JUMP_Lable_Writing_Better(JUMP_REG_Better[((SN[1]-'0')*10+(SN[2]-'0'))]);
        }

        if(strcmp(F_NEXT,"JPC")==0)
        {
            WHILE_START_PRE_Writing_Better();
        }


        if(strcmp(F,"LIT")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LIT_FOR_WRT_Processing_Better(A);
            }

            else
            {
                LIT_Processing_Better(A);
            }

        }

        else if(strcmp(F,"LOD")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LOD_FOR_WRT_Processing_Better(A-2);
            }

            else
            {
                LOD_Processing_Better(A-2);
            }

        }

        else if(strcmp(F,"STO")==0)
        {
            STO_Processing_Better(A-2);
        }

        else if(strcmp(F,"CAL")==0)
        {
            CAL_Processing_Better(A);
        }

        else if(strcmp(F,"INT")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==SN_Main_Better)
            {
                INT_FOR_MAIN_Processing_Better(A-3);
            }

            else
            {
                INT_Processing_Better(((SN[1]-'0')*10+(SN[2]-'0')),A-3);
            }

        }

        else if(strcmp(F,"JMP")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==0)
            {
                SN_Main_Better=A;
            }

            else
            {
                JMP_Processing_Better(((SN[1]-'0')*10+(SN[2]-'0')),A);
            }

        }

        else if(strcmp(F,"JPC")==0)
        {
            JPC_Processing_Better(A);
        }

        else if(strcmp(F,"ADD")==0)
        {
            ADD_Processing_Better();
        }

        else if(strcmp(F,"SUB")==0)
        {
            SUB_Processing_Better();
        }

        else if(strcmp(F,"MUL")==0)
        {
            MUL_Processing_Better();
        }

        else if(strcmp(F,"DIV")==0)
        {
            DIV_Processing_Better();
        }

        else if(strcmp(F,"RED")==0)
        {
            RED_Processing_Better();
        }

        else if(strcmp(F,"WRT")==0)
        {
            WRT_Processing_Better();
        }

        else if((strcmp(F,"RET")==0))
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==((SN_NEXT[1]-'0')*10+(SN_NEXT[2]-'0')))
            {
                RET_FOR_MAIN_Processing_Better();
            }
            else
            {
                RET_Processing_Better();
            }
        }

    }
    while(0);

    fclose(f_objectCodeToMIPS_Better);

    redundant_WHILE_START_Cleaning_Better();

    PrintMipsCodeList_Better();

}


