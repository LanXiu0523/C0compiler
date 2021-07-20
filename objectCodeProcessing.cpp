/*******************************************************************************

 Filename: objectCodeProcessing.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-06-27

 Description: Generation and processing of object stack instruction code.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include "global.h"
#include "objectCodeProcessing.h"
#include "objectCodeBetter.h"


/*
 *生成虚拟机代码
 */
int GenerateVirtualMachineCode(enum functionCode F,int L,int A)
{
    if(codeIndex >= codeLength_MAX)
    {
        printf("Program too long");
        return -1;
    }

    code[codeIndex].F = F;
    code[codeIndex].L = L;
    code[codeIndex].A = A;

    codeIndex++;

    return 0;

}



/*
 *输出虚拟机代码清单
 */
void listVirtualMachineCode(int initialCodeIndex)
{
    int i;
    if(PrintObjectCodeListEnableSwitch && errorCounter==0)
    {
        printf("Virtual Machine Object Code:\n\n");
        for(i=initialCodeIndex; i<codeIndex; i++)
        {
            printf("(%02d) %s %d %d\n",i,definitionOfStackInstruction[code[i].F],code[i].L,code[i].A);
            fprintf(f_objectCode,"(%02d) %s %d %d\n",i,definitionOfStackInstruction[code[i].F],code[i].L,code[i].A);
        }

        printf("\n");
        system("pause");
        printf("\n\n");

    }

    fclose(f_objectCode);


    if(PrintObjectCodeBetterEnableSwitch)
    {
        objectCodeBetter();

        ifstream in("f_objectCodeBetter.tmp",ios::in);
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

        printf("Virtual Machine Object Code Better:\n\n");

        while(in>>SN>>F>>L>>A)
        {
            printf("(%02d) %s %d %d\n",i,F,L,A);
            i++;
        }

        printf("\n");
        system("pause");
        printf("\n\n");

    }

}


