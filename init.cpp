/*******************************************************************************

 Filename: init.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-06

 Description: Initialization of partial variables and symbol sets.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "init.h"

/*初始化*/
void init()
{
    /*部分全局变量初始化*/
    braceLevelCounter=0;
    var_NUM=0;
    tableIndex_CUR=0;
    whileIfElseFLAG=0;
    unknownFunctionFLAG=false;
    leftBraceLevel_INDEX = rightBraceLevel_INDEX = 0;
    whileIfElseFLAG_INDEX=0;


    /*设置单字符符号*/
    for(int i=0;i<=255;i++)
    {
        singleSymbol[i]=nul;
    }

    singleSymbol['+']=positive;
    singleSymbol['-']=negative;
    singleSymbol['*']=multiply;
    singleSymbol['/']=divide;
    singleSymbol[',']=comma;
    singleSymbol[';']=semicolon;
    singleSymbol['(']=leftParentheses;
    singleSymbol[')']=rightParentheses;
    singleSymbol['{']=leftBrace;
    singleSymbol['}']=rightBrace;


    /*设置保留字名字，按照字母顺序，后用于折半查找*/
    strcpy(&reservedWord[0][0],"else");
    strcpy(&reservedWord[1][0],"if");
    strcpy(&reservedWord[2][0],"int");
    strcpy(&reservedWord[3][0],"printf");
    strcpy(&reservedWord[4][0],"return");
    strcpy(&reservedWord[5][0],"scanf");
    strcpy(&reservedWord[6][0],"void");
    strcpy(&reservedWord[7][0],"while");


    /*设置保留字符号*/
    reservedWordSymbol[0]=elseSymbol;
    reservedWordSymbol[1]=ifSymbol;
    reservedWordSymbol[2]=intSymbol;
    reservedWordSymbol[3]=printfSymbol;
    reservedWordSymbol[4]=returnSymbol;
    reservedWordSymbol[5]=scanfSymbol;
    reservedWordSymbol[6]=voidSymbol;
    reservedWordSymbol[7]=whileSymbol;


    /*栈式指令定义*/
    /*
     *╔══════════╦════════════════════════════════════════════════════╗
     *║ 指令格式 ║                 指 令 功 能                        ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ LIT 0 A  ║ 将常数值取到栈顶，A为常数值                        ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ LOD L A  ║ 将层差为L、偏移量为A的存储单元的值取到栈顶         ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ STO L A  ║ 将栈顶元素的值存入层差为L、偏移量为A的单元         ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ CAL 0 A  ║ 调用地址为A的函数过程                              ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ INT 0 A  ║ 在运行栈中为被调用的函数过程开辟A个单元的存储区域  ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ JMP 0 A  ║ 无条件跳转至地址A                                  ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ JPC 0 A  ║ 条件跳转，当栈顶元素为0则跳转至地址A，否则顺序执行 ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ ADD 0 0  ║ 次栈顶的值与栈顶的值相加，退两个栈元素，结果值进栈 ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ SUB 0 0  ║ 次栈顶的值与栈顶的值相减，退两个栈元素，结果值进栈 ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ MUL 0 0  ║ 次栈顶的值与栈顶的值相乘，退两个栈元素，结果值进栈 ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ DIV 0 0  ║ 次栈顶的值除以栈顶的值，退两个栈元素，结果值进栈   ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ RED 0 0  ║ 从控制台读取一个输入，并将其置于栈顶               ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ WRT 0 0  ║ 将栈顶元素输出至控制台，并换行                     ║
     *╠══════════╬════════════════════════════════════════════════════╣
     *║ RET 0 0  ║ 函数调用结束后，返回调用点，并退栈                 ║
     *╚══════════╩════════════════════════════════════════════════════╝
     */
    strcpy(&(definitionOfStackInstruction[LIT][0]),"LIT");
    strcpy(&(definitionOfStackInstruction[LOD][0]),"LOD");
    strcpy(&(definitionOfStackInstruction[STO][0]),"STO");
    strcpy(&(definitionOfStackInstruction[CAL][0]),"CAL");
    strcpy(&(definitionOfStackInstruction[INT][0]),"INT");
    strcpy(&(definitionOfStackInstruction[JMP][0]),"JMP");
    strcpy(&(definitionOfStackInstruction[JPC][0]),"JPC");
    strcpy(&(definitionOfStackInstruction[ADD][0]),"ADD");
    strcpy(&(definitionOfStackInstruction[SUB][0]),"SUB");
    strcpy(&(definitionOfStackInstruction[MUL][0]),"MUL");
    strcpy(&(definitionOfStackInstruction[DIV][0]),"DIV");
    strcpy(&(definitionOfStackInstruction[RED][0]),"RED");
    strcpy(&(definitionOfStackInstruction[WRT][0]),"WRT");
    strcpy(&(definitionOfStackInstruction[RET][0]),"RET");


    /*设置开始符号集*/
    for(int i=0;i<symbol_NUM;i++)
    {
        declareStartSymbolSet[i]=false;
        statementStartSymbolSet[i]=false;
        factorStartSymbolSet[i]=false;
    }


    /*设置声明开始符号集*/
    declareStartSymbolSet[voidSymbol]=true;
    declareStartSymbolSet[intSymbol]=true;
    declareStartSymbolSet[leftBrace]=true;


    /*设置语句开始符号集*/
    statementStartSymbolSet[ifSymbol]=true;
    statementStartSymbolSet[whileSymbol]=true;
    statementStartSymbolSet[voidSymbol]=true;
    statementStartSymbolSet[intSymbol]=true;
    statementStartSymbolSet[scanfSymbol]=true;
    statementStartSymbolSet[printfSymbol]=true;


    /*设置因子开始符号集*/
    factorStartSymbolSet[identifier]=true;
    factorStartSymbolSet[number]=true;
    factorStartSymbolSet[leftParentheses]=true;

}


