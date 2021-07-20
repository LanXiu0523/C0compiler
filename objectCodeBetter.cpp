/*******************************************************************************

 Filename: objectCodeBetter.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-07-02

 Description: Optimization of object ​stack instruction code.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "objectCodeBetter.h"


/*
 *优化过程产生的中间临时文件
 */
FILE *f_objectCodeBetter_TMP1;
FILE *f_objectCodeBetter_TMP2;
FILE *f_objectCodeBetter_TMP3;

/*
 *窥孔优化中的死码消除技术
 *
 *窥孔大小为2
 *LIT 0 0  JPC 0 a  ->  窥孔头直接移至第a行
 *跳转修正偏移量增量为A2-SN2+1
 */
void deadCodeElimination()
{
    ifstream in("f_objectCode.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    char SN1[10],SN2[10];
    char F1[10],F2[10];
    int L1,L2;
    int A1,A2;
    int i = 0;
    int offset = 0;
    int offset_SUM = 0;

    in>>SN1>>F1>>L1>>A1;

    while(in>>SN2>>F2>>L2>>A2)
    {
        if((strcmp(F1,"LIT")==0)
                &&(strcmp(F2,"JPC")==0)
                &&(A1==0))
        {
            offset = A2-((SN2[1]-'0')*10+(SN2[2]-'0'));
            offset_SUM += offset+1;

            while(offset)
            {
                in>>SN1>>F1>>L1>>A1;
                offset--;
            }
        }

        else
        {
            if((strcmp(F1,"JMP")==0)
                    ||(strcmp(F1,"JPC")==0))
            {
                A1 -= offset_SUM;
            }

            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
            i++;

            strcpy(F1,F2);
            A1=A2;
            L1=L2;
        }
    }

    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
    i++;

}


/*
 *窥孔优化中的删除冗余的取和存技术
 *
 *窥孔大小为2
 *LOD 0 a  STO 0 b  LOD 0 b  STO 0 a  ->  LOD 0 a  STO 0 b
 *跳转修正偏移量增量为2
 */
void eliminareRedundantLODandSTO()
{
    ifstream in("f_objectCodeBetter_TMP1.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    char SN1[10],SN2[10],SN3[10],SN4[10];
    char F1[10],F2[10],F3[10],F4[10];
    int L1,L2,L3,L4;
    int A1,A2,A3,A4;
    int i = 0;
    int offset_SUM = 0;

    in>>SN1>>F1>>L1>>A1;
    in>>SN2>>F2>>L2>>A2;
    in>>SN3>>F3>>L3>>A3;

    while(in>>SN4>>F4>>L4>>A4)
    {

        if(((strcmp(F1,F3)==0)&&(strcmp(F3,"LOD")==0))
                &&((strcmp(F2,F4)==0)&&(strcmp(F4,"STO")==0))
                &&(A1==A4)
                &&(A2==A3))
        {
            offset_SUM += 2;

            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
            i++;
            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F2,L2,A2);
            i++;

            in>>SN1>>F1>>L1>>A1;
            in>>SN2>>F2>>L2>>A2;
            in>>SN3>>F3>>L3>>A3;
        }

        else
        {
            if((strcmp(F1,"JMP")==0)
                    ||(strcmp(F1,"JPC")==0))
            {
                A1 -= offset_SUM;
            }

            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
            i++;

            strcpy(F1,F2);
            A1=A2;
            L1=L2;

            strcpy(F2,F3);
            A2=A3;
            L2=L3;

            strcpy(F3,F4);
            A3=A4;
            L3=L4;
        }
    }

    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
    i++;
    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F2,L2,A2);
    i++;
    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F3,L3,A3);
    i++;

}


/*
 *窥孔优化中的常量合并技术
 *
 *窥孔大小为3
 *LIT 0 a  LIT 0 b  ADD|SUB|MUL|DIV 0 0 ->LIT 0 c  (c=a<op>b)
 *跳转修正偏移量增量为2
 */
void constantFolding()
{
    ifstream in("f_objectCodeBetter_TMP2.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    char SN1[10],SN2[10],SN3[10];
    char F1[10],F2[10],F3[10];
    int L1,L2,L3;
    int A1,A2,A3;
    int i = 0;
    int A_result = 0;
    int offset_SUM = 0;

    in>>SN1>>F1>>L1>>A1;
    in>>SN2>>F2>>L2>>A2;

    while(in>>SN3>>F3>>L3>>A3)
    {
        if(((strcmp(F1,F2)==0)&&(strcmp(F2,"LIT")==0))
                &&((strcmp(F3,"ADD")==0)||(strcmp(F3,"SUB")==0)||(strcmp(F3,"MUL")==0)||(strcmp(F3,"DIV")==0)))
        {
            offset_SUM += 2;

            if(strcmp(F3,"ADD")==0)
            {
                A_result = A1+A2;
            }

            else if(strcmp(F3,"SUB")==0)
            {
                A_result = A1-A2;
            }

            else if(strcmp(F3,"MUL")==0)
            {
                A_result = A1*A2;
            }

            else
            {
                A_result = A1/A2;
            }

            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A_result);
            i++;

            in>>SN1>>F1>>L1>>A1;
            in>>SN2>>F2>>L2>>A2;
        }

        else
        {
            if((strcmp(F1,"JMP")==0)
                    ||(strcmp(F1,"JPC")==0))
            {
                A1 -= offset_SUM;
            }

            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
            i++;

            strcpy(F1,F2);
            A1=A2;
            L1=L2;

            strcpy(F2,F3);
            A2=A3;
            L2=L3;
        }
    }

    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
    i++;
    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F2,L2,A2);
    i++;

}


/*
 *窥孔优化中的代数化简技术
 *
 *窥孔大小为4
 *<1> LOD 0 a   LIT 0 0   ADD|SUB 0 0  STO 0 a ->  NULL
 *<2> LIT 0 0   LOD 0 a   ADD 0 0      STO 0 a ->  NULL
 *<3> LOD 0 a   LIT 0 1   MUL|DIV 0 0  STO 0 a ->  NULL
 *<4> LIT 0 1   LOD 0 a   MUL 0 0      STO 0 a ->  NULL
 *跳转修正偏移量增量为4
 */
void algebraicSimplification()
{
    ifstream in("f_objectCodeBetter_TMP3.tmp",ios::in);

    if(!in)
    {
        cerr<<"file could not be open";
        return;
    }

    char SN1[10],SN2[10],SN3[10],SN4[10];
    char F1[10],F2[10],F3[10],F4[10];
    int L1,L2,L3,L4;
    int A1,A2,A3,A4;
    int i = 0;
    int offset_SUM = 0;

    in>>SN1>>F1>>L1>>A1;
    in>>SN2>>F2>>L2>>A2;
    in>>SN3>>F3>>L3>>A3;

    while(in>>SN4>>F4>>L4>>A4)
    {
        /* LOD 0 a   LIT 0 0   ADD|SUB 0 0  STO 0 a ->  NULL */
        if((strcmp(F1,"LOD")==0)
                &&(strcmp(F2,"LIT")==0)
                &&((strcmp(F3,"ADD")==0)||(strcmp(F3,"SUB")==0))
                &&(strcmp(F4,"STO")==0)
                &&(A1==A4)
                &&(A2==0))
        {
            offset_SUM += 4;

            in>>SN1>>F1>>L1>>A1;
            in>>SN2>>F2>>L2>>A2;
            in>>SN3>>F3>>L3>>A3;
        }

        /* LIT 0 0   LOD 0 a   ADD 0 0  STO 0 a ->  NULL */
        else if((strcmp(F2,"LOD")==0)
                &&(strcmp(F1,"LIT")==0)
                &&(strcmp(F3,"ADD")==0)
                &&(strcmp(F4,"STO")==0)
                &&(A2==A4)
                &&(A1==0))
        {
            offset_SUM += 4;

            in>>SN1>>F1>>L1>>A1;
            in>>SN2>>F2>>L2>>A2;
            in>>SN3>>F3>>L3>>A3;
        }

        /* LOD 0 a   LIT 0 1   MUL|DIV 0 0  STO 0 a ->  NULL */
        else if((strcmp(F1,"LOD")==0)
                &&(strcmp(F2,"LIT")==0)
                &&((strcmp(F3,"MUL")==0)||(strcmp(F3,"DIV")==0))
                &&(strcmp(F4,"STO")==0)
                &&(A1==A4)
                &&(A2==1))
        {
            offset_SUM += 4;

            in>>SN1>>F1>>L1>>A1;
            in>>SN2>>F2>>L2>>A2;
            in>>SN3>>F3>>L3>>A3;
        }

        /* LIT 0 1   LOD 0 a   MUL 0 0  STO 0 a ->  NULL */
        else if((strcmp(F2,"LOD")==0)
                &&(strcmp(F1,"LIT")==0)
                &&(strcmp(F3,"MUL")==0)
                &&(strcmp(F4,"STO")==0)
                &&(A2==A4)
                &&(A1==1))
        {
            offset_SUM += 4;

            in>>SN1>>F1>>L1>>A1;
            in>>SN2>>F2>>L2>>A2;
            in>>SN3>>F3>>L3>>A3;
        }

        else
        {
            if((strcmp(F1,"JMP")==0)
                    ||(strcmp(F1,"JPC")==0))
            {
                A1 -= offset_SUM;
            }

            fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
            i++;

            strcpy(F1,F2);
            A1=A2;
            L1=L2;

            strcpy(F2,F3);
            A2=A3;
            L2=L3;

            strcpy(F3,F4);
            A3=A4;
            L3=L4;
        }
    }

    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F1,L1,A1);
    i++;
    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F2,L2,A2);
    i++;
    fprintf(f_objectCodeBetter,"(%02d) %s %d %d\n",i,F3,L3,A3);
    i++;

}

/*
 *窥孔优化
 *
 *包含死代码删除、删除冗余的取和存、常量合并、代数化简等优化方式
 *还可以根据对应虚拟机，追加常量传播、控制流优化、强度削弱、使用目标机惯用指令等优化方式
 */
void peepholeOptimization()
{

    f_objectCodeBetter = fopen("f_objectCodeBetter_TMP1.tmp","w");
    deadCodeElimination();
    fclose(f_objectCodeBetter);

    f_objectCodeBetter = fopen("f_objectCodeBetter_TMP2.tmp","w");
    eliminareRedundantLODandSTO();
    fclose(f_objectCodeBetter);

    f_objectCodeBetter = fopen("f_objectCodeBetter_TMP3.tmp","w");
    constantFolding();
    fclose(f_objectCodeBetter);

    f_objectCodeBetter = fopen("f_objectCodeBetter.tmp","w");
    algebraicSimplification();
    fclose(f_objectCodeBetter);

    remove("f_objectCodeBetter_TMP1.tmp");
    remove("f_objectCodeBetter_TMP2.tmp");
    remove("f_objectCodeBetter_TMP3.tmp");

}


/*
 *目标代码优化
 *
 *目前采用了效果明显且易于实现的窥孔优化
 *还可以采用局部优化(DAG图着色)、循环优化、全局优化等方式
 */
void objectCodeBetter()
{
    peepholeOptimization();

}


