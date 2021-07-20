#ifndef __GRAMMATICALANALYSIS_H__
#define __GRAMMATICALANALYSIS_H__

#include "global.h"

void enter(enum identifierKindStruct kind, int* Ptr_tableIndex, int level, int* Ptr_dataAllocationIndex);
int position(char* inquiredIdentifier, int tableIndex);
int inset(int Symbol,bool *Set);
int vardeclaration(int* Ptr_tableIndex, int level, int* Ptr_dataAllocationIndex);
int statementProcessing(int* Ptr_tableIndex,int level);
int expressionProcessing(int* Ptr_tableIndex,int level);
int termProcessing(int* Ptr_tableIndex,int level);
int factorProcessing(int* Ptr_tableIndex,int level);
int conditionProcessing(int* Ptr_tableIndex,int level);
int block(int level,int tableIndex);

#define vardeclarationdo(a,b,c) if(-1==vardeclaration(a,b,c))return -1
#define statementProcessingDO(a,b) if(-1==statementProcessing(a,b))return -1
#define expressionProcessingDO(a,b) if(-1==expressionProcessing(a,b))return -1
#define termProcessingDO(a,b) if(-1==termProcessing(a,b))return -1
#define factorProcessingDO(a,b) if(-1==factorProcessing(a,b))return -1
#define conditionProcessingDO(a,b) if(-1==conditionProcessing(a,b))return -1

#endif




