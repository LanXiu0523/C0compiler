/*******************************************************************************

 Filename: virtualMachineOfStackStructure.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-30

 Description: A user-defined virtual machine for interpreting
              and executing object stack instruction code.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "virtualMachineOfStackStructure.h"


/*
 *指令解释程序
 *
 *用于将读到的指令在虚拟机中进行相应操作而实现
 */
    /*
     *╔══════════╦═════════════════════════════════════════════════════════════════╗
     *║ 指令格式 ║                 指 令 功 能                                     ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ LIT 0 A  ║ 将常数值取到栈顶,A为常数值,stackTop_Ptr+1                       ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ LOD L A  ║ 将层差为L、偏移量为A的存储单元的值取到栈顶,stackTop_Ptr+1       ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ STO L A  ║ 将栈顶元素的值存入层差为L、偏移量为A的单元,stackTop_Ptr-1       ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ CAL 0 A  ║ 调用地址为A的函数过程                                           ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ INT 0 A  ║ 在运行栈中为被调用的函数过程开辟A个单元的存储区域               ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ JMP 0 A  ║ 无条件跳转至地址A                                               ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ JPC 0 A  ║ 条件跳转,当栈顶元素为0则跳转至地址A,否则顺序执行,stackTop_Ptr-1 ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ ADD 0 0  ║ 次栈顶的值与栈顶的值相加,退两个栈元素,结果值进栈,stackTop_Ptr-1 ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ SUB 0 0  ║ 次栈顶的值与栈顶的值相减,退两个栈元素,结果值进栈,stackTop_Ptr-1 ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ MUL 0 0  ║ 次栈顶的值与栈顶的值相乘,退两个栈元素,果值进栈,stackTop_Ptr-1   ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ DIV 0 0  ║ 次栈顶的值除以栈顶的值,退两个栈元素,结果值进栈,stackTop_Ptr-1   ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ RED 0 0  ║ 从控制台读取一个输入,并将其置于栈顶,stackTop_Ptr+1              ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ WRT 0 0  ║ 将栈顶元素输出至控制台,并换行,stackTop_Ptr-1                    ║
     *╠══════════╬═════════════════════════════════════════════════════════════════╣
     *║ RET 0 0  ║ 函数调用结束后，返回调用点，并退栈                              ║
     *╚══════════╩═════════════════════════════════════════════════════════════════╝
     */
void instructionInterpreter()
{
    int instruction_Ptr;
    int instruction_BaseAddress;
    int stackTop_Ptr;
    struct instruction i;
    int s[stackSize];

    instruction_Ptr=0;
    instruction_BaseAddress=0;
    stackTop_Ptr=0;

    s[0]=s[1]=s[2]=0;

    do
    {
        i=code[instruction_Ptr];
        instruction_Ptr++;

        switch(i.F)
        {
        case LIT:
            s[stackTop_Ptr]=i.A;
            stackTop_Ptr++;
            break;

        case LOD:
            s[stackTop_Ptr]=s[instruction_BaseAddress+i.A];
            stackTop_Ptr++;
            break;

        case STO:
            stackTop_Ptr--;
            s[instruction_BaseAddress+i.A]=s[stackTop_Ptr];
            break;

        case CAL:
            s[stackTop_Ptr]=instruction_BaseAddress;
            s[stackTop_Ptr+1]=instruction_BaseAddress;
            s[stackTop_Ptr+2]=instruction_Ptr;
            instruction_BaseAddress=stackTop_Ptr;
            instruction_Ptr=i.A;
            break;

        case INT:
            stackTop_Ptr=stackTop_Ptr+i.A;
            break;

        case JMP:
            instruction_Ptr=i.A;
            break;

        case JPC:
            stackTop_Ptr--;

            if(s[stackTop_Ptr]==0)
            {
                instruction_Ptr=i.A;
            }
            break;

        case ADD:
            stackTop_Ptr--;
            s[stackTop_Ptr-1]=s[stackTop_Ptr-1]+s[stackTop_Ptr];
            break;

        case SUB:
            stackTop_Ptr--;
            s[stackTop_Ptr-1]=s[stackTop_Ptr-1]-s[stackTop_Ptr];
            break;

        case MUL:
            stackTop_Ptr--;
            s[stackTop_Ptr-1]=s[stackTop_Ptr-1]*s[stackTop_Ptr];
            break;

        case DIV:
            stackTop_Ptr--;

            /*除数不能为零*/
            if(s[stackTop_Ptr]!=0)
            {
                s[stackTop_Ptr-1]=s[stackTop_Ptr-1]/s[stackTop_Ptr];
            }

            else
            {
                cout<<"input error"<<endl;
            }
            break;

        case RED:
            cout<<"Scanf:";
            cin>>s[stackTop_Ptr];
            stackTop_Ptr++;
            break;

        case WRT:
            cout<<"Printf:"<<s[stackTop_Ptr-1]<<endl;
            stackTop_Ptr--;
            break;

        case RET:
            stackTop_Ptr=instruction_BaseAddress;
            instruction_Ptr=s[stackTop_Ptr+2];
            instruction_BaseAddress=s[stackTop_Ptr+1];
            break;

        }
    }
    while(instruction_Ptr!=0);

}


/*
 *指令翻译程序
 *
 *用于将读入的指令助记符翻译成code表的内容
 */
void instructionTranslator(char *F,int L,int A,int i)
{
    if(strcmp(F,"LIT")==0)
    {
        code[i].F=(functionCode)0;
    }

    else if(strcmp(F,"LOD")==0)
    {
        code[i].F=(functionCode)1;
    }

    else if(strcmp(F,"STO")==0)
    {
        code[i].F=(functionCode)2;
    }

    else if(strcmp(F,"CAL")==0)
    {
        code[i].F=(functionCode)3;
    }

    else if(strcmp(F,"INT")==0)
    {
        code[i].F=(functionCode)4;
    }

    else if(strcmp(F,"JMP")==0)
    {
        code[i].F=(functionCode)5;
    }

    else if(strcmp(F,"JPC")==0)
    {
        code[i].F=(functionCode)6;
    }

    else if(strcmp(F,"ADD")==0)
    {
        code[i].F=(functionCode)7;
    }

    else if(strcmp(F,"SUB")==0)
    {
        code[i].F=(functionCode)8;
    }

    else if(strcmp(F,"MUL")==0)
    {
        code[i].F=(functionCode)9;
    }

    else if(strcmp(F,"DIV")==0)
    {
        code[i].F=(functionCode)10;
    }

    else if(strcmp(F,"RED")==0)
    {
        code[i].F=(functionCode)11;
    }

    else if(strcmp(F,"WRT")==0)
    {
        code[i].F=(functionCode)12;
    }

    else if(strcmp(F,"RET")==0)
    {
        code[i].F=(functionCode)13;
    }

    code[i].L=L;
    code[i].A=A;

}


void interpretiveProcedure()
{

    cout<<"........Instructions Interpretive Procedure:........\n"<<endl;

    ifstream in("f_objectCode.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    char SN[10];
    char F[10];
    int L;
    int A;
    int i=0;

    while(in>>SN>>F>>L>>A)
    {
        instructionTranslator(F,L,A,i);
        i++;
    }

    instructionInterpreter();

}
