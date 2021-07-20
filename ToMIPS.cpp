/*******************************************************************************

 Filename: ToMIPS.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-07-06

 Description: Object stack instruction code to MIPS code.

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


FILE *f_objectCodeToMIPS;
/*L型指令计数器*/
int L_Counter=0;
/*RED型指令标记*/
int RED_Flag=0;
/*Main函数开始的栈式指令序号*/
int SN_Main=0;
/*外部函数计数器*/
int FUNC_Counter=0;
/*外部函数数据寄存器，REG的index为该函数开始的栈式指令序号，值为FUNC序号*/
int FUNC_REG[2047];
/*跳转计数器*/
int JUMP_Counter=0;
/*跳转数据寄存器，REG的index为跳转目标的栈式指令行序号，值为Lable序号*/
int JUMP_REG[2047];
/*While函数跳转索引预写计数器*/
int WHILE_START_JMP_Counter=-1;
/*While函数跳转数据寄存器，REG的index为While函数跳转索引的序号，值为1表示完整While跳转，
 *反之则预期错误，后续删除多余的While函数跳转索引预写*/
int WHILE_START_JMP_REG[127]= {0};


/*
 * 初始化
 */
void init_MIPS()
{
    fprintf(f_objectCodeToMIPS,".data");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"    Enter: .asciiz  \"\\n\"");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,".text");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"ori     $fp     $sp     0");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"li      $t9     0x7fffeffc	#global stack bottom");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"li      $t8     0x10010000	#save word");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"        j	__main");
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * LIT指令处理
 */
void LIT_Processing(int immediateNumber)
{
    fprintf(f_objectCodeToMIPS,"li      $t%d     %d",L_Counter,immediateNumber);
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter++;

}


/*
 * LIT指令处理(针对WRT前的LIT，取立即数至$a0)
 */
void LIT_FOR_WRT_Processing(int immediateNumber)
{
    fprintf(f_objectCodeToMIPS,"li      $a0     %d",immediateNumber);
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * LOD指令处理
 */
void LOD_Processing(int offset)
{
    fprintf(f_objectCodeToMIPS,"lw      $t%d     %d($fp)",L_Counter,(-4)*offset);
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter++;

}


/*
 * LOD指令处理(针对WRT前的LOD，取立即数至$a0)
 */
void LOD_FOR_WRT_Processing(int offset)
{
    fprintf(f_objectCodeToMIPS,"lw      $a0     %d($fp)",(-4)*offset);
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * STO指令处理
 */
void STO_Processing(int offset)
{
    if(RED_Flag)
    {
        fprintf(f_objectCodeToMIPS,"sw      $v0     %d($fp)",(-4)*offset);
        fprintf(f_objectCodeToMIPS,"\n");
        RED_Flag--;

    }

    else
    {
        fprintf(f_objectCodeToMIPS,"sw      $t0     %d($fp)",(-4)*offset);
        fprintf(f_objectCodeToMIPS,"\n");
        L_Counter--;

    }

}


/*
 * CAL指令处理
 */
void CAL_Processing(int SN_FUNC)
{
    fprintf(f_objectCodeToMIPS,"move    $s0     $sp");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"jal             Func%d",FUNC_REG[SN_FUNC]);
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"nop");
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * INT指令处理
 */
void INT_Processing(int SN_FUNC,int Size)
{
    FUNC_Counter++;
    FUNC_REG[SN_FUNC]=FUNC_Counter;
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"Func%d:",FUNC_Counter);
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"#Save Register");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"sw      $s0     0($sp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"sw      $fp     0($sp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"add     $fp     $sp     $0");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"sw      $ra     ($sp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"#Save Register Done!");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"\n");

    for(int i=0; i<Size; i++)
    {
        fprintf(f_objectCodeToMIPS,"li      $t0     0");
        fprintf(f_objectCodeToMIPS,"\n");
        fprintf(f_objectCodeToMIPS,"sw      $t0     ($sp)");
        fprintf(f_objectCodeToMIPS,"\n");
        fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
        fprintf(f_objectCodeToMIPS,"\n");

    }

    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * INT指令处理(针对Main函数下的INT)
 */
void INT_FOR_MAIN_Processing(int Size)
{
    FUNC_Counter++;
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"__main:");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"#Save Register");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"sw      $s0     0($sp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"sw      $fp     0($sp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"add     $fp     $sp     $0");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"sw      $ra     ($sp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"#Save Register Done!");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"\n");

    for(int i=0; i<Size; i++)
    {
        fprintf(f_objectCodeToMIPS,"li      $t0     0");
        fprintf(f_objectCodeToMIPS,"\n");
        fprintf(f_objectCodeToMIPS,"sw      $t0     ($sp)");
        fprintf(f_objectCodeToMIPS,"\n");
        fprintf(f_objectCodeToMIPS,"subi    $sp     $sp     4");
        fprintf(f_objectCodeToMIPS,"\n");

    }
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * JMP指令处理
 */
void JMP_Processing(int SN,int jumpAddress)
{
    if(SN>jumpAddress)
    {
        WHILE_START_JMP_REG[WHILE_START_JMP_Counter]=1;
        fprintf(f_objectCodeToMIPS,"j               __WhileStart%02d",WHILE_START_JMP_Counter);
        fprintf(f_objectCodeToMIPS,"\n");

    }

    else
    {
        JUMP_REG[jumpAddress]=JUMP_Counter;
        fprintf(f_objectCodeToMIPS,"j               __Lable%d",JUMP_Counter);
        fprintf(f_objectCodeToMIPS,"\n");
        JUMP_Counter++;

    }

}


/*
 * JPC指令处理
 */
void JPC_Processing(int jumpAddress)
{
    JUMP_REG[jumpAddress]=JUMP_Counter;
    fprintf(f_objectCodeToMIPS,"li      $t%d     0",L_Counter);
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"beq     $t%d     $t%d     __Lable%d",L_Counter-1,L_Counter,JUMP_Counter);
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter--;
    JUMP_Counter++;

}


/*
 * 跳转索引Lable的写入
 */
void JUMP_Lable_Writing(int Lable_NUM)
{
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"__Lable%d:",Lable_NUM);
    fprintf(f_objectCodeToMIPS,"\n");

}

/*
 * While函数索引WhileStart的预写，
 * 若预写非预期，在最后调用的redundant_WHILE_START_Cleaning()函数中消除冗余索引
 */
void WHILE_START_PRE_Writing()
{
    WHILE_START_JMP_Counter++;
    fprintf(f_objectCodeToMIPS,"__WhileStart%02d:",WHILE_START_JMP_Counter);
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * ADD指令处理
 */
void ADD_Processing()
{
    fprintf(f_objectCodeToMIPS,"add     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter--;

}


/*
 * SUB指令处理
 */
void SUB_Processing()
{
    fprintf(f_objectCodeToMIPS,"sub     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter--;

}


/*
 * MUL指令处理
 */
void MUL_Processing()
{
    fprintf(f_objectCodeToMIPS,"mul     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter--;

}


/*
 * DIV指令处理
 */
void DIV_Processing()
{
    fprintf(f_objectCodeToMIPS,"div     $t0     $t0     $t1");
    fprintf(f_objectCodeToMIPS,"\n");
    L_Counter--;

}


/*
 * RED指令处理
 */
void RED_Processing()
{
    fprintf(f_objectCodeToMIPS,"li      $v0     5");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"syscall");
    fprintf(f_objectCodeToMIPS,"\n");
    RED_Flag++;

}


/*
 * WRT指令处理
 */
void WRT_Processing()
{
    fprintf(f_objectCodeToMIPS,"li      $v0     1");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"syscall");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"li      $v0     4");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"la      $a0     Enter");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"syscall");
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * RET指令处理
 */
void RET_Processing()
{
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"__FEND_LAB_%d:",FUNC_Counter);
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"lw      $ra     0($fp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"lw      $sp     8($fp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"lw      $fp     4($fp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"jr              $ra");
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * RET指令处理(针对Main函数下的RET)
 */
void RET_FOR_MAIN_Processing()
{
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"__FEND_LAB_%d:",FUNC_Counter);
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"lw      $ra     0($fp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"lw      $sp     8($fp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"lw      $fp     4($fp)");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"li  	$v0 	10");
    fprintf(f_objectCodeToMIPS,"\n");
    fprintf(f_objectCodeToMIPS,"syscall");
    fprintf(f_objectCodeToMIPS,"\n");

}


/*
 * 消除冗余的WhileStart索引
 *
 * 根据WHILE_START_JMP_REG寄存器内容，将非完全While跳转的WhileStart索引删除
 */
void redundant_WHILE_START_Cleaning()
{

    string tmpLineData;
    int WHILE_START_Counter = 0;
    ifstream inGO("f_objectCodeToMIPS.asm.tmp");
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
            if(WHILE_START_JMP_REG[((tmpLineData[12]-'0')*10+(tmpLineData[13]-'0'))]==1)
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
            if(WHILE_START_JMP_REG[((tmpLineData[28]-'0')*10+(tmpLineData[29]-'0'))]==1)
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
    ofstream outBACK("f_objectCodeToMIPS.asm.tmp", ios::out | ios::trunc);

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
void PrintMipsCodeList(){
    string tmpLineData;
    ifstream inPrint("f_objectCodeToMIPS.asm.tmp");

    printf("\n");
    system("pause");
    printf("\n\n");

    printf("Virtual Machine Object Code To MIPS Code:\n\n");

    while (!inPrint.eof())
    {
        getline(inPrint,tmpLineData);
        cout<<tmpLineData<<endl;

    }

}



/*
 * 将生成的优化后的栈式指令代码转化为优化后的MIPS指令代码
 */
void ToMIPS()
{
    ifstream in("f_objectCode.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    f_objectCodeToMIPS = fopen("f_objectCodeToMIPS.asm.tmp","w");

    memset(FUNC_REG,-1,2047);
    memset(JUMP_REG,-1,2047);
    char SN[10],SN_NEXT[10];
    char F[10],F_NEXT[10];
    int L,L_NEXT;
    int A,A_NEXT;

    init_MIPS();

    in>>SN>>F>>L>>A;

    while(in>>SN_NEXT>>F_NEXT>>L_NEXT>>A_NEXT)
    {
        if(JUMP_REG[((SN[1]-'0')*10+(SN[2]-'0'))]!=(-1))
        {
            JUMP_Lable_Writing(JUMP_REG[((SN[1]-'0')*10+(SN[2]-'0'))]);
        }

        if(strcmp(F_NEXT,"JPC")==0)
        {
            WHILE_START_PRE_Writing();
        }


        if(strcmp(F,"LIT")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LIT_FOR_WRT_Processing(A);
            }

            else
            {
                LIT_Processing(A);
            }

        }

        else if(strcmp(F,"LOD")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LOD_FOR_WRT_Processing(A-2);
            }

            else
            {
                LOD_Processing(A-2);
            }

        }

        else if(strcmp(F,"STO")==0)
        {
            STO_Processing(A-2);
        }

        else if(strcmp(F,"CAL")==0)
        {
            CAL_Processing(A);
        }

        else if(strcmp(F,"INT")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==SN_Main)
            {
                INT_FOR_MAIN_Processing(A-3);
            }

            else
            {
                INT_Processing(((SN[1]-'0')*10+(SN[2]-'0')),A-3);
            }

        }

        else if(strcmp(F,"JMP")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==0)
            {
                SN_Main=A;
            }

            else
            {
                JMP_Processing(((SN[1]-'0')*10+(SN[2]-'0')),A);
            }

        }

        else if(strcmp(F,"JPC")==0)
        {
            JPC_Processing(A);
        }

        else if(strcmp(F,"ADD")==0)
        {
            ADD_Processing();
        }

        else if(strcmp(F,"SUB")==0)
        {
            SUB_Processing();
        }

        else if(strcmp(F,"MUL")==0)
        {
            MUL_Processing();
        }

        else if(strcmp(F,"DIV")==0)
        {
            DIV_Processing();
        }

        else if(strcmp(F,"RED")==0)
        {
            RED_Processing();
        }

        else if(strcmp(F,"WRT")==0)
        {
            WRT_Processing();
        }

        else if((strcmp(F,"RET")==0))
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==((SN_NEXT[1]-'0')*10+(SN_NEXT[2]-'0')))
            {
                RET_FOR_MAIN_Processing();
            }
            else
            {
                RET_Processing();
            }
        }


        strcpy(SN,SN_NEXT);
        strcpy(F,F_NEXT);
        L=L_NEXT;
        A=A_NEXT;

    }


    do
    {
        if(JUMP_REG[((SN[1]-'0')*10+(SN[2]-'0'))]!=(-1))
        {
            JUMP_Lable_Writing(JUMP_REG[((SN[1]-'0')*10+(SN[2]-'0'))]);
        }

        if(strcmp(F_NEXT,"JPC")==0)
        {
            WHILE_START_PRE_Writing();
        }


        if(strcmp(F,"LIT")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LIT_FOR_WRT_Processing(A);
            }

            else
            {
                LIT_Processing(A);
            }

        }

        else if(strcmp(F,"LOD")==0)
        {
            if(strcmp(F_NEXT,"WRT")==0)
            {
                LOD_FOR_WRT_Processing(A-2);
            }

            else
            {
                LOD_Processing(A-2);
            }

        }

        else if(strcmp(F,"STO")==0)
        {
            STO_Processing(A-2);
        }

        else if(strcmp(F,"CAL")==0)
        {
            CAL_Processing(A);
        }

        else if(strcmp(F,"INT")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==SN_Main)
            {
                INT_FOR_MAIN_Processing(A-3);
            }

            else
            {
                INT_Processing(((SN[1]-'0')*10+(SN[2]-'0')),A-3);
            }

        }

        else if(strcmp(F,"JMP")==0)
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==0)
            {
                SN_Main=A;
            }

            else
            {
                JMP_Processing(((SN[1]-'0')*10+(SN[2]-'0')),A);
            }

        }

        else if(strcmp(F,"JPC")==0)
        {
            JPC_Processing(A);
        }

        else if(strcmp(F,"ADD")==0)
        {
            ADD_Processing();
        }

        else if(strcmp(F,"SUB")==0)
        {
            SUB_Processing();
        }

        else if(strcmp(F,"MUL")==0)
        {
            MUL_Processing();
        }

        else if(strcmp(F,"DIV")==0)
        {
            DIV_Processing();
        }

        else if(strcmp(F,"RED")==0)
        {
            RED_Processing();
        }

        else if(strcmp(F,"WRT")==0)
        {
            WRT_Processing();
        }

        else if((strcmp(F,"RET")==0))
        {
            if(((SN[1]-'0')*10+(SN[2]-'0'))==((SN_NEXT[1]-'0')*10+(SN_NEXT[2]-'0')))
            {
                RET_FOR_MAIN_Processing();
            }
            else
            {
                RET_Processing();
            }
        }

    }
    while(0);

    fclose(f_objectCodeToMIPS);

    redundant_WHILE_START_Cleaning();

    PrintMipsCodeList();

}


