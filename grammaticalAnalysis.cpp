/*******************************************************************************

 Filename: grammaticalAnalysis.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-20

 Description: Grammatical analysis and semantic analysis.

*******************************************************************************/


#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "grammaticalAnalysis.h"
#include "lexicalAnalysis.h"
#include "error.h"
#include "objectCodeProcessing.h"
#include "global.h"


/*
 *在名字表中加入一项
 */
void enter(enum identifierKindStruct kind, int* Ptr_tableIndex, int level, int* Ptr_dataAllocationIndex)
{

    (*Ptr_tableIndex)++;
    strcpy(identifierTable[(*Ptr_tableIndex)].identifierName, identifier_NEW);
    identifierTable[(*Ptr_tableIndex)].identifierKind = kind;
    identifierTable[(*Ptr_tableIndex)].identifierLever = level;

    switch(kind)
    {
    case variable:

        identifierTable[(*Ptr_tableIndex)].identifierAddress = (*Ptr_dataAllocationIndex);
        (*Ptr_dataAllocationIndex)++;
        var_NUM++;
        break;

    case void_c0:
        identifierTable[(*Ptr_tableIndex)].identifierAddress = codeIndex;
        identifierTable[(*Ptr_tableIndex)].identifierSize = 0;
        break;
    }

}


/*
 *查找名字的位置
 *找到则返回在名字表中的位置，否则返回0
 */
int position(char* inquiredIdentifier, int tableIndex)
{
    int index;

    strcpy(identifierTable[0].identifierName, inquiredIdentifier);

    index=tableIndex;
    while(strcmp(identifierTable[index].identifierName, inquiredIdentifier)!=0)
    {
        index--;
    }

    return index;

}


/*
 *判断一个Symbol是否在Set中
 *用数组实现字符集合的集合与字符集合的运算
 */
int inset(int Symbol,bool *Set)
{
    return Set[Symbol];
}


/*
 *变量声明处理
 *
 *＜变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;}
 *＜变量定义＞ ::= ＜类型标识符＞＜标识符＞{,＜标识符＞ }
 */
int vardeclaration(int* Ptr_tableIndex, int level, int* Ptr_dataAllocationIndex)
{
    if(sym==identifier)
    {
        enter(variable, Ptr_tableIndex, level, Ptr_dataAllocationIndex);
        getsymdo;
    }

    else
    {
        /*缺少左括号*/
        error(1);
    }

    return 0;

}


/*
 *语句声明处理
 *
 * ＜语句＞ ::= ＜条件语句＞｜＜循环语句＞｜＜无返回值函数调用语句＞;｜
 *              ＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
 */
int statementProcessing(int* Ptr_tableIndex,int level)
{
    int i;

    /* ::= ＜赋值语句＞; | ＜无返回值函数调用语句＞; */
    if(sym==identifier)
    {
        /*在table中查找identifier位置*/
        i=position(identifier_NEW,*Ptr_tableIndex);

        /*若identifier不在table中*/
        if(i==0)
        {
            unknownFunctionFLAG=true;
            strcpy(unknownFunction_REG[unknownFunction_INDEX],identifier_NEW);

            getsymdo;

            if(sym==semicolon)
            {
                /*标识符未定义*/
                error(6);
            }

            else
            {
                if(sym!=leftParentheses)
                {
                    /*缺少左括号*/
                    error(1);
                }

                else
                {
                    getsymdo;
                }

                if(sym!=rightParentheses)
                {
                    /*缺少右括号*/
                    error(2);
                }

                else
                {
                    getsymdo;
                }

                if(sym!=semicolon)
                {
                    /*缺少分号*/
                    error(3);
                }

                else
                {
                    GenerateVirtualMachineCodeDO(CAL,0,0);
                    codeIndex_CAL[unknownFunction_INDEX] = codeIndex;
                    unknownFunction_INDEX++;
                }
            }
        }

        else
        {
            if(identifierTable[i].identifierKind!=variable && identifierTable[i].identifierKind!=void_c0)
            {
                /*语句开始变量类型并非是变量或者void函数*/
                error(9);
                i=0;
            }

            /* ::= ＜赋值语句＞; */
            else if(identifierTable[i].identifierKind==variable)
            {
                getsymdo;

                if(sym==equalSign)
                {
                    getsymdo;
                }

                else
                {
                    /*并非是赋值符号*/
                    error(10);
                }

                expressionProcessingDO(Ptr_tableIndex,level);

                if(i!=0)
                {
                    GenerateVirtualMachineCodeDO(STO,identifierTable[i].identifierLever,identifierTable[i].identifierAddress);
                }
            }

            /* ::= ＜无返回值函数调用语句＞; */
            else if(identifierTable[i].identifierKind==void_c0)
            {
                getsymdo;

                if(sym!=leftParentheses)
                {
                    /*缺少左括号*/
                    error(1);
                }

                else
                {
                    getsymdo;
                }

                if(sym!=rightParentheses)
                {
                    /*缺少右括号*/
                    error(2);
                }

                else
                {
                    getsymdo;
                }

                GenerateVirtualMachineCodeDO(CAL,0,identifierTable[i].identifierAddress);
                getsymdo;
            }
        }
    }

    else
    {
        /* ::= ＜读语句＞; */
        if(sym==scanfSymbol)
        {
            getsymdo;

            if(sym!=leftParentheses)
            {
                /*缺少左括号*/
                error(1);
            }

            else
            {
                getsymdo;

                if(sym==identifier)
                {
                    i=position(identifier_NEW,*Ptr_tableIndex);
                }

                else
                {
                    i=0;
                }

                if(i==0)
                {
                    /*标识符未定义*/
                    error(6);
                }

                else
                {
                    GenerateVirtualMachineCodeDO(RED,0,0);
                    GenerateVirtualMachineCodeDO(STO,level-identifierTable[i].identifierLever,identifierTable[i].identifierAddress);
                }

                getsymdo;
            }

            if(sym!=rightParentheses)
            {
                /*缺少右括号*/
                error(2);
            }

            else
            {
                getsymdo;
            }
        }

        else
        {
            /* ＜写语句＞; */
            if(sym==printfSymbol)
            {
                getsymdo;

                if(sym==leftParentheses)
                {
                    getsymdo;
                    expressionProcessingDO(Ptr_tableIndex,level);
                    GenerateVirtualMachineCodeDO(WRT,0,0);

                    if(sym!=rightParentheses)
                    {
                        /*缺少右括号*/
                        error(2);
                    }

                    else
                    {
                        getsymdo;
                    }

                    if(sym!=semicolon)
                    {
                        /*缺少分号*/
                        error(3);
                    }
                }
            }

            else
            {
                /* ::= ＜无返回值函数调用语句＞; */
                if(sym==voidSymbol)
                {
                    getsymdo;

                    if(sym!=identifier)
                    {
                        /*void函数名并非是标识符*/
                        error(11);
                    }

                    else
                    {
                        i=position(identifier_NEW,*Ptr_tableIndex);

                        if(i!=0)
                        {
                            if(identifierTable[i].identifierKind==void_c0)
                            {
                                GenerateVirtualMachineCodeDO(CAL,0,identifierTable[i].identifierAddress);
                            }

                            else
                            {
                                /*缺少左括号*/
                                error(1);
                            }
                        }

                        getsymdo;
                    }
                }

                else
                {
                    /* ::= ＜条件语句＞; */
                    if(sym==ifSymbol)
                    {

                        whileIfElseFLAG=2;
                        getsymdo;

                        conditionProcessingDO(Ptr_tableIndex,level);

                        ifJPC_REG[leftBraceLevel_INDEX]=codeIndex;
                        GenerateVirtualMachineCodeDO(JPC,0,0);

                        if(sym==leftBrace)
                        {
                            getsymdo;
                        }

                        if(sym==rightBrace)
                        {
                            getsymdo;
                        }

                        statementProcessingDO(Ptr_tableIndex,level);
                    }

                    else if(sym==elseSymbol)
                    {
                        /*此时检测到该If{}后有Else{}，令Else{}语句的第一条指令为JMP跳转至Else{}语句结束。*/
                        /*若该If{}语句为真，执行结束后会通过该JMP指令跳出Else{}函数体。*/
                        /*若该If{}语句为假，则通过JPC指令跳转至JMP指令的下一条指令处，即Else{}函数体的开始*/
                        GenerateVirtualMachineCodeDO(JMP,0,0);

                        /*此时codeIndex为该Else{}语句JMP指令的下一条指令*/
                        /*反填该codeIndex至该If条件语句的JPC指令的A值*/
                        code[ifJPC_REG[leftBraceLevel_INDEX+1]].A = codeIndex;

                        whileIfElseFLAG=3;
                        getsymdo;

                        if(sym==leftBrace)
                            getsymdo;

                        if(sym==rightBrace)
                            getsymdo;

                        conditionProcessingDO(Ptr_tableIndex,level);
                        statementProcessingDO(Ptr_tableIndex,level);
                    }

                    else
                    {
                        /* ::= ＜循环语句＞; */
                        if(sym==whileSymbol)
                        {
                            whileIfElseFLAG = 1;
                            getsymdo;

                            conditionProcessingDO(Ptr_tableIndex,level);

                            codeIndex_JPC[braceLevelCounter] = codeIndex;
                            GenerateVirtualMachineCodeDO(JPC,0,0);

                            getsymdo;
                            statementProcessingDO(Ptr_tableIndex,level);
                        }
                    }
                }
            }
        }
    }

    return 0;

}

/*
 *表达式处理
 *
 *＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
 */
int expressionProcessing(int* Ptr_tableIndex,int level)
{
    enum symbol addOP_REG;

    /* ::= ［＋｜－］＜项＞ */
    if(sym==positive||sym==negative)
    {
        /*保存当前＜乘法运算符＞*/
        addOP_REG=sym;
        getsymdo;

        /*保存当前＜项＞*/
        termProcessingDO(Ptr_tableIndex,level);
    }

    /* ::= ＜项＞ */
    else
    {
        termProcessingDO(Ptr_tableIndex,level);
    }

    /* ::= {＜加法运算符＞＜项＞} */
    while(sym==positive||sym==negative)
    {
        /*保存当前＜乘法运算符＞*/
        addOP_REG=sym;
        getsymdo;

        /*保存当前＜项＞*/
        termProcessingDO(Ptr_tableIndex,level);

        if(addOP_REG==positive)
        {
            GenerateVirtualMachineCodeDO(ADD,0,0);
        }

        else
        {
            GenerateVirtualMachineCodeDO(SUB,0,0);
        }
    }

    return 0;
}

/*
 *项处理
 *
 *＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
 */
int termProcessing(int* Ptr_tableIndex,int level)
{
    enum symbol mulOP_REG;

    /* ::= ＜因子＞ */
    factorProcessingDO(Ptr_tableIndex,level);

    /* ::= {＜乘法运算符＞＜因子＞} */
    while(sym==multiply || sym==divide)
    {
        /*保存当前＜乘法运算符＞*/
        mulOP_REG=sym;
        getsymdo;

        /* 获取当前＜因子＞*/
        factorProcessingDO(Ptr_tableIndex,level);

        if(mulOP_REG==multiply)
        {
            GenerateVirtualMachineCodeDO(MUL,0,0);
        }

        else
        {
            GenerateVirtualMachineCodeDO(DIV,0,0);
        }
    }

    return 0;

}

/*
 *因子处理
 *
 * ＜因子＞ ::= ＜标识符＞｜＜整数＞｜‘(’＜表达式＞‘)’
 */
int factorProcessing(int* Ptr_tableIndex,int level)
{
    int i;

    /* factorStartSymbolSet ::= identifier | number | leftParentheses */
    while(inset(sym,factorStartSymbolSet))
    {
        /* ::= ＜标识符＞ */
        if(sym==identifier)
        {
            i=position(identifier_NEW,*Ptr_tableIndex);

            if(i==0)
            {
                /*标识符未定义*/
                error(6);
            }

            else
            {
                switch(identifierTable[i].identifierKind)
                {
                case variable:
                    GenerateVirtualMachineCodeDO(LOD,identifierTable[i].identifierLever,identifierTable[i].identifierAddress);
                    break;

                case void_c0:
                    /*该标识符为void函数，并非是变量*/
                    error(7);
                    break;
                }
            }

            getsymdo;
        }

        /* ::= ＜整数＞ | ‘(’＜表达式＞‘)’ */
        else
        {
            /* ::= ＜整数＞ */
            if(sym==number)
            {
                if(num>int_MAX)
                {
                    /*当前的int整型超出规定的最大值*/
                    error(8);
                    num=0;
                }

                GenerateVirtualMachineCodeDO(LIT,0,num);
                getsymdo;
            }

            /* ::= ‘(’＜表达式＞‘)’ */
            else
            {
                if(sym==leftParentheses)
                {
                    getsymdo;
                    expressionProcessingDO(Ptr_tableIndex,level);

                    if(sym==rightParentheses)
                    {
                        getsymdo;
                    }

                    else
                    {
                        /*缺少左括号*/
                        error(1);
                    }
                }
            }
        }
    }

    return 0;

}

/*
 *条件处理
 */
int conditionProcessing(int* Ptr_tableIndex,int level)
{
    expressionProcessingDO(Ptr_tableIndex,level);
    return 0;
}


/*
 *编译程序主体
 *
 *＜程序＞ ::= ［＜变量说明＞］{＜无返回值函数定义＞}＜主函数＞
 */
int block(int level,int tableIndex)
{
    int i;
    int dataAllocationIndex;
    int tableIndex_REG;

    /*默认跳到1，后需要修改为主函数地址*/
    GenerateVirtualMachineCodeDO(JMP,0,1);

    /*每遇到一个新的函数时跳转至loop*/
loop:
    /*栈中分配的数据段长度为3(DL、RA、局部变量)，亦代表标识符的相对地址*/
    dataAllocationIndex = 3;

    /*记录本层开始时table的位置*/
    tableIndex_REG = tableIndex;

    do
    {
        if(sym==leftBrace)
        {
            getsymdo;
        }

        if(sym==rightBrace)
        {
            getsymdo;
        }

        /* ::= ［＜变量说明＞］ */
        if(sym==intSymbol)
        {
            getsymdo;

            if(sym==identifier)
            {
                vardeclarationdo(&tableIndex,level,&dataAllocationIndex);

                while(sym==comma)
                {
                    getsymdo;
                    vardeclarationdo(&tableIndex,level,&dataAllocationIndex);
                }

                if(sym==semicolon)
                {
                    getsymdo;
                }

                else
                {
                    /*缺少分号*/
                    error(3);
                }
            }

            else
            {
                /*并非是标识符*/
                error(4);
            }
        }

        /* ::= {＜无返回值函数定义＞} ＜主函数＞*/
        while(sym==voidSymbol)
        {
            getsymdo;

            if(sym==identifier)
            {
                /*若该函数名是main*/
                /* ::= ＜主函数＞*/
                if(strcmp(identifier_NEW,"main")==0)
                {
                    /*反填第一条指令JMP的A值，使其跳转到main函数后*/
                    code[0].A = codeIndex;
                }

                /*将函数名写入名字表*/
                enter(void_c0,&tableIndex,level,&dataAllocationIndex);

                /*若之前有未在table中定位的函数，再次在table中搜索，若成功定位，反填该函数地址*/
                /* ::= ＜无返回值函数定义＞*/
                if(unknownFunctionFLAG)
                {
                    int j=0;

                    for(int k=0; k<255; k++)
                    {

                        j=position(unknownFunction_REG[k],tableIndex);

                        if(j!=0)
                        {
                            code[codeIndex_CAL[k]-1].A=identifierTable[j].identifierAddress;
                            j=0;
                        }
                    }
                    unknownFunctionFLAG=false;
                }
                getsymdo;
            }

            else
            {
                /*并非是void保留字*/
                error(5);
            }

            /*函数名后应跟‘(’ ‘)’ ‘;’*/
            if(sym==leftParentheses)
            {
                getsymdo;
            }

            else
            {
                /*缺少左括号*/
                error(1);
            }

            if(sym==rightParentheses)
            {
                getsymdo;
            }

            else
            {
                /*缺少右括号*/
                error(2);
            }

            if(sym==semicolon)
            {
                getsymdo;
            }
        }
    }
    while(inset(sym,declareStartSymbolSet));

    /*根据数据段长度分配内存*/
    GenerateVirtualMachineCodeDO(INT,0,dataAllocationIndex);

    /* ＜复合语句＞ ::= ［＜变量说明＞］＜语句列＞*/
    /* ＜语句列＞ ::=｛＜语句＞｝ */
    while(inset(sym, statementStartSymbolSet) || sym==semicolon || sym==rightBrace || sym==identifier)
    {
        if(sym==semicolon || sym==rightBrace)
        {
            getsymdo;

            /*当函数里面嵌套{}时(不包括最外面函数体的{}),遇到}后，跳转到{之前*/
            if((sym==rightBrace) && braceLevelCounter>0)
            {
                /*根据whileIfElseFLAG_REG确定该层{}的类型*/
                whileIfElseFLAG = whileIfElseFLAG_REG[whileIfElseFLAG_INDEX];

                /*若是While{}型语句结束*/
                if(whileIfElseFLAG==1)
                {
                    /*While循环结束时，通过JMP指令跳转至该While循环的JPC指令的codeIndex*/
                    GenerateVirtualMachineCodeDO(JMP,0,codeIndex_JPC[braceLevelCounter+1]-1);

                    /*此时codeIndex为该While循环的下一条指令，反填该codeIndex至该While循环的JPC指令的A值*/
                    code[codeIndex_JPC[braceLevelCounter+1]].A = codeIndex;
                }

                /*若是If{}型语句结束*/
                if(whileIfElseFLAG==2)
                {
                    if((leftBraceLevel_REG[rightBraceLevel_INDEX]==1) && (rightBraceLevel_REG[rightBraceLevel_INDEX]==1))
                    {
                        /*此时codeIndex为该If{}语句的下一条指令*/
                        codeIndex_ELSE = codeIndex;

                        /*此时认为该If{}后没有Else{}，该If条件语句的JPC跳转至该If{}语句的下一条指令，若If{}后有Else{}，则会在statementProcessing()函数发现并处理*/
                        /*反填该codeIndex至该If条件语句的JPC指令的A值*/
                        code[ifJPC_REG[leftBraceLevel_INDEX+1]].A = codeIndex;

                        rightBraceLevel_INDEX--;
                    }
                }


                if(whileIfElseFLAG==3)
                {
                    /*此时codeIndex为该Else语句的下一条指令*/
                    /*反填该codeIndex至该Else语句的JMP指令的A值*/
                    code[codeIndex_ELSE].A = codeIndex;

                    whileIfElseFLAG=2;
                }
            }
        }


        if(braceLevelCounter==0)
        {
            /*将函数的Size写入Table表*/
            tableIndex_CUR++;
            identifierTable[tableIndex_CUR].identifierSize = var_NUM+3;
            tableIndex_CUR += var_NUM;

            var_NUM=0;

            getsymdo;

            if(sym==voidSymbol)
            {
                /*上一个函数体结束，调用RET指令*/
                GenerateVirtualMachineCodeDO(RET,0,0);

                goto loop;
            }
        }

        /*开始进行语句处理*/
        statementProcessingDO(&tableIndex,level);
    }


    GenerateVirtualMachineCodeDO(RET,0,0);


    system("pause");

    /*输出nameTable*/
    if(PrintNameTableEnableSwitch && errorCounter==0)
    {
        printf("\n\n\nPrint Name Table:\n\n");

        if(tableIndex_REG>=tableIndex)
        {
            printf("NULL\n");
        }

        for(i=1; i<=tableIndex; i++)
        {
            switch(identifierTable[i].identifierKind)
            {
            case variable:

                /*打印nameTable*/
                printf(" <%02d>  int  %4s ",i,identifierTable[i].identifierName);
                printf(" level=%d  addr=%d\n",identifierTable[i].identifierLever,identifierTable[i].identifierAddress);

                /*将nameTable写入f_nameList.txt*/
                fprintf(f_nameList," <%02d>  var  %8s ",i,identifierTable[i].identifierName);
                fprintf(f_nameList," level=%d  addr=%d\n",identifierTable[i].identifierLever,identifierTable[i].identifierAddress);
                break;

            case void_c0:

                /*打印nameTable*/
                printf(" <%02d>  void  %4s",i,identifierTable[i].identifierName);
                printf(" level=%d  addr=%d  identifierSize=%d\n",identifierTable[i].identifierLever,identifierTable[i].identifierAddress,identifierTable[i].identifierSize);

                /*将nameTable写入f_nameList.txt*/
                fprintf(f_nameList," <%02d>  void  %8s ",i,identifierTable[i].identifierName);
                fprintf(f_nameList," level=%d  addr=%d  identifierSize=%d\n",identifierTable[i].identifierLever,identifierTable[i].identifierAddress,identifierTable[i].identifierSize);
                break;
            }
        }
        printf("\n");

        system("pause");
        printf("\n\n");
    }

    fclose(f_nameList);

    /*输出虚拟机栈式指令代码*/
    listVirtualMachineCode(0);

    return 0;

}


