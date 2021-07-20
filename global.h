#ifndef __GLOBAL_H__
#define __GLOBAL_H__


/*
 *标识符常量
 */
enum symbol
{
		nul,		  identifier,    number,		   positive,	      negative,
		multiply,	  divide,		 leftParentheses,  rightParentheses,  comma,
		semicolon,	  becomes,	     equalSign,	       ifSymbol,		  whileSymbol,
		writesym,	  readsym,	     dosym,		       callsym,	          varsym,
		leftBrace,	  rightBrace,	 intSymbol,		   voidSymbol,	      elseSymbol,
		scanfSymbol,  printfSymbol,  returnSymbol,
};
#define symbol_NUM 28


/*
 *栈式结构虚拟机汇编指令助记符
 */
enum functionCode
{
		LIT,  LOD,  STO,
		CAL,  INT,  JMP,
		JPC,  ADD,  SUB,
		MUL,  DIV,  RED,
		WRT,  RET,
};
#define functionCode_NUM 14


/*
 *栈式结构虚拟机的汇编指令格式如下
 * F  L  A
 *它由3个部分构成
 *F: 指令的操作码
 *L: 若起作用，则表示引用层与表明层之间的层次差，若不起作用，则置为0
 *A: 不同的指令含义不同
*/
struct instruction
{
	enum functionCode F;
	int L;
	int A;
};


#define reservedWord_NUM 8
#define wordLength_MAX 10
#define identifierTable_MAX 100
#define numberOfDigits_MAX 10
#define int_MAX 2147483647
#define codeLength_MAX 200
#define stackSize 500


enum identifierKindStruct
{
	variable,void_c0
};


struct identifierTableStruct
{
	char identifierName[wordLength_MAX];
	enum identifierKindStruct identifierKind;
	int identifierLever;
	int identifierAddress;
	int identifierSize;
};


extern FILE *file;
extern FILE *f_nameList;
extern FILE *f_objectCode;
extern FILE *f_objectCodeBetter;

extern struct identifierTableStruct identifierTable[identifierTable_MAX];

extern instruction code[100];
extern char definitionOfStackInstruction[functionCode_NUM][5];

extern enum symbol sym;
extern enum symbol singleSymbol[256];
extern enum symbol reservedWordSymbol[reservedWord_NUM];

extern bool declareStartSymbolSet[symbol_NUM];
extern bool statementStartSymbolSet[symbol_NUM];
extern bool factorStartSymbolSet[symbol_NUM];
extern char reservedWord[reservedWord_NUM][wordLength_MAX];

extern unsigned char ch;
extern char line[81];
extern int readOut_index,writeIn_index;

extern char charString_REG[wordLength_MAX+1];
extern char identifier_NEW[wordLength_MAX+1];
extern int num;

extern bool unknownFunctionFLAG;
extern char unknownFunction_REG[255][wordLength_MAX+1];
extern int unknownFunction_INDEX;

extern int braceLevelCounter;
extern int leftBraceLevel_REG[255];
extern int rightBraceLevel_REG[255];
extern int leftBraceLevel_INDEX,rightBraceLevel_INDEX;

extern int codeIndex;
extern int codeIndex_CAL[255];
extern int codeIndex_JPC[255];
extern int codeIndex_ELSE;

extern int whileIfElseFLAG;
extern int whileIfElseFLAG_REG[255];
extern int whileIfElseFLAG_INDEX;
extern int ifJPC_REG[255];

extern int tableIndex_CUR;
extern int var_NUM;

extern bool WhileEnableSwitch;
extern bool PrintNameTableEnableSwitch;
extern bool PrintObjectCodeListEnableSwitch;
extern bool PrintObjectCodeBetterEnableSwitch;

extern char fileName[wordLength_MAX];

extern int errorCounter;

#endif




