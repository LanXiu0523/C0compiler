/*******************************************************************************

 Filename: error.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-18

 Description: Exception thrown and error handling.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "error.h"

/*错误处理函数，通过返回的错误序号，抛出相应报错*/
void error(int errorNumber)
{
    char errors[100];

    switch(errorNumber)
    {
    /*缺少左括号*/
    case 1:
        strcpy(errors,"Missing Left Parentheses...");
        break;

    /*缺少右括号*/
    case 2:
        strcpy(errors,"Missing Right Parentheses...");
        break;

    /*缺少分号*/
    case 3:
        strcpy(errors,"Missing Semicolon...");
        break;

    /*并非是标识符*/
    case 4:
        strcpy(errors,"Not Expected Identifier...");
        break;

    /*并非是void保留字*/
    case 5:
        strcpy(errors,"Not Expected VoidSymbol...");
        break;

    /*标识符未定义*/
    case 6:
        strcpy(errors,"Identifier Not Declared...");
        break;

    /*该标识符为void函数，并非是变量*/
    case 7:
        strcpy(errors,"It's Void_c0, Not Expected Variable...");
        break;

    /*当前的int整型超出规定的最大值*/
    case 8:
        strcpy(errors,"Current Integer Exceeds INT_MAX...");
        break;

    /*语句开始变量类型并非是变量或者void函数*/
    case 9:
        strcpy(errors,"Statement Start Identifier Kind Is Not Expected Variable or void_c0...");
        break;

    /*并非是赋值符号*/
    case 10:
        strcpy(errors,"Not Expected EqualSign...");
        break;

    /*void函数名并非是标识符*/
    case 11:
        strcpy(errors,"Void_c0 Function Name Is Not Expected Identifier...");
        break;
    /*当前的数字超出规定的最大长度*/
    case 12:
        strcpy(errors,"Current Digits ExceedsNumberOfDigits_MAX");
        break;
    }

    /*错误计数器，为零时程序正常编译，非0时抛出报错*/
    errorCounter++;

    printf("%s\n",errors);

}


