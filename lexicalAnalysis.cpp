/*******************************************************************************

 Filename: lexicalAnalysis.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-13

 Description: Lexical analysis.

*******************************************************************************/


#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "error.h"
#include "lexicalAnalysis.h"

/*
 *基于C0文法
 *
 *＜加法运算符＞ ::= +｜-
 *＜乘法运算符＞ ::= *｜/
 *＜字母＞       ::= ＿｜a｜．．．｜z｜A｜．．．｜Z
 *＜数字＞       ::= ０｜＜非零数字＞
 *＜非零数字＞   ::= １｜．．．｜９
 *＜字符＞       ::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
 *＜字符串＞     ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
 *＜程序＞       ::= ［＜变量说明＞］{＜无返回值函数定义＞}＜主函数＞
 *＜无符号整数＞ ::= ＜非零数字＞｛＜数字＞｝
 *＜整数＞       ::= ［＋｜－］＜无符号整数＞｜０
 *＜标识符＞     ::=  ＜字母＞｛＜字母＞｜＜数字＞｝
 *＜声明头部＞   ::=  int＜标识符＞
 *＜变量说明＞   ::= ＜变量定义＞;{＜变量定义＞;}
 *＜变量定义＞   ::= ＜类型标识符＞＜标识符＞{,＜标识符＞ }
 *＜类型标识符＞ ::=  int
 *＜复合语句＞   ::= ［＜变量说明＞］＜语句列＞
 *＜参数表＞     ::= ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}|＜空＞
 *＜主函数＞     ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
 *＜表达式＞     ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
 *＜项＞         ::= ＜因子＞{＜乘法运算符＞＜因子＞}
 *＜因子＞       ::= ＜标识符＞｜＜整数＞|＜字符＞｜‘(’＜表达式＞‘)’
 *＜语句＞       ::= ＜条件语句＞｜＜循环语句＞｜＜无返回值函数调用语句＞;｜
 *                   ＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
 *＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞
 *＜条件语句＞   ::=  if ‘(’＜表达式＞‘)’＜语句＞
 *＜循环语句＞   ::=  while ‘(’＜表达式＞‘)’＜语句＞
 *＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
 *＜语句列＞     ::=｛＜语句＞｝
 *＜读语句＞     ::=  scanf ‘(’＜标识符＞‘)’
 *＜写语句＞     ::=  printf‘(’＜表达式＞‘)’
 *＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
 *＜无返回值函数定义＞ ::= void＜标识符＞‘(’＜参数表＞‘)’‘{’＜复合语句＞‘}’
 *＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
 */


/*
 *字符获取函数
 *
 *被函数getsym调用
 *跳过空格一次读入一行字符，存入line缓冲区
 *逐个取出直至line取空再进行读入
 */
int getch()
{
    /*当取出数=读入数，则line取空，再次读入一行字符*/
    if(readOut_index==writeIn_index)
    {
        writeIn_index=0;
        readOut_index=0;
        ch=' ';

        /*若读入line字符非回车*/
        while(ch!=10)
        {
            /*若文件结束*/
            if(EOF==fscanf(file,"%c",&ch))
            {
                line[writeIn_index]=0;
                break;
            }

            /*将读入字符存入line缓冲区，并打印*/
            printf("%c",ch);
            line[writeIn_index]=ch;
            writeIn_index++;
        }
    }

    /*每次调用读出一个字符，并将读出索引+1*/
    ch=line[readOut_index];
    readOut_index++;

    return 0;

}


/*
 *词法分析函数
 *
 *调用使之获取一个符号
 */
int getsym()
{
    int i,j,k;

    /*忽略空格、TAB和换行*/
    while(ch==' '||ch==9||ch==10)
    {
        getchdo;
    }

    /*名字或保留字以(a-z||A-Z)开头*/
    if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z'))
    {
        k=0;

        /*记录当前符号，并将后面多余的字符清除掉*/
        do
        {
            if(k<wordLength_MAX)
            {
                charString_REG[k]=ch;
                k++;
            }

            getchdo;
        }
        while(((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z'))||(ch>='0'&&ch<='9'));

        charString_REG[k]=0;

        strcpy(identifier_NEW,charString_REG);

        /*搜索当前符号是否为保留字*/
        i=0;
        j = reservedWord_NUM-1;

        do
        {
            k=(i+j)/2;
            if(strcmp(identifier_NEW,reservedWord[k])<=0)
            {
                j=k-1;
            }

            if(strcmp(identifier_NEW,reservedWord[k])>=0)
            {
                i=k+1;
            }

        }
        while(i<=j);


        if(i-1>j)
        {
            sym = reservedWordSymbol[k];
        }

        /*若搜索失败，则当前符号为名字或数字*/
        else
        {
            sym = identifier;
        }

    }

    /*若当前符号非名字或保留字，则可能为数字或单字符符号*/
    else
    {
        /*若当前符号为数字*/
        if(ch>='0'&&ch<='9')
        {
            k=0;
            num=0;
            sym=number;

            /*将读出的数字字符串转化为int型*/
            do
            {
                num=10*num+ch-'0';
                k++;
                getchdo;
            }
            while(ch>='0'&&ch<='9');
            k--;

            /*若超出数字最大合法长度，抛出报错*/
            if(k>numberOfDigits_MAX)
            {
                /*当前的数字超出规定的最大长度*/
                error(12);
            }
        }

        /*若当前符号非数字，则只可能为单字符符号*/
        else
        {
            if(ch=='=')
            {
                sym=equalSign;
                getchdo;
            }

            else
            {
                sym=nul;
            }

            if(ch=='+'||ch=='-'||ch=='*'||ch=='/'||ch==','||ch==';'||ch=='('||ch==')'||ch=='{'||ch=='}'||ch=='#')
            {
                sym=singleSymbol[ch];
                getchdo;
            }
        }
    }

    /*Brace在循环语句语句或条件语句中的匹配处理*/
    if(sym==leftBrace)
    {
        braceLevelCounter++;

        /*While语句{}处理*/
        if(whileIfElseFLAG==1)
        {
            whileIfElseFLAG_REG[whileIfElseFLAG_INDEX]=1;
            whileIfElseFLAG_INDEX++;
        }

        /*If语句的{}处理*/
        if(whileIfElseFLAG==2)
        {
            leftBraceLevel_INDEX++;
            leftBraceLevel_REG[leftBraceLevel_INDEX]=1;
            rightBraceLevel_INDEX++;

            whileIfElseFLAG_REG[whileIfElseFLAG_INDEX]=2;
            whileIfElseFLAG_INDEX++;
        }

        /*Else语句的{}处理*/
        if(whileIfElseFLAG==3)
        {
            whileIfElseFLAG_REG[whileIfElseFLAG_INDEX]=3;
            whileIfElseFLAG_INDEX++;
        }
    }

    else if(sym==rightBrace)
    {
        braceLevelCounter--;

        if(whileIfElseFLAG==2)
        {
            rightBraceLevel_REG[leftBraceLevel_INDEX]=1;
            leftBraceLevel_INDEX--;
        }

        whileIfElseFLAG_INDEX--;
    }

    return 0;
}
