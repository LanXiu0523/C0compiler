/*******************************************************************************

 Filename: global.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-11

 Description: Definition of global variable and enumeration type.

*******************************************************************************/


#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include "global.h"


FILE *file;
FILE *f_nameList;
FILE *f_objectCode;
FILE *f_objectCodeBetter;

struct identifierTableStruct identifierTable[identifierTable_MAX];

instruction code[100];
char definitionOfStackInstruction[functionCode_NUM][5];

enum symbol sym;
enum symbol singleSymbol[256];
enum symbol reservedWordSymbol[reservedWord_NUM];

bool declareStartSymbolSet[symbol_NUM];
bool statementStartSymbolSet[symbol_NUM];
bool factorStartSymbolSet[symbol_NUM];
char reservedWord[reservedWord_NUM][wordLength_MAX];

unsigned char ch;
char line[81];
int  readOut_index,writeIn_index;

int  num;
char charString_REG[wordLength_MAX+1];
char identifier_NEW[wordLength_MAX+1];


bool unknownFunctionFLAG;
char unknownFunction_REG[255][wordLength_MAX+1];
int  unknownFunction_INDEX;


int  braceLevelCounter;
int  leftBraceLevel_REG[255];
int  rightBraceLevel_REG[255];
int  leftBraceLevel_INDEX,rightBraceLevel_INDEX;

int  codeIndex;
int  codeIndex_CAL[255];
int  codeIndex_JPC[255];
int  codeIndex_ELSE;

int  whileIfElseFLAG;
int  whileIfElseFLAG_REG[255];
int  whileIfElseFLAG_INDEX;
int  ifJPC_REG[255];

int  tableIndex_CUR;
int  var_NUM;

bool WhileEnableSwitch;
bool PrintNameTableEnableSwitch;
bool PrintObjectCodeListEnableSwitch;
bool PrintObjectCodeBetterEnableSwitch;

char fileName[wordLength_MAX];

int  errorCounter;


