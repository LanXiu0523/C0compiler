/*******************************************************************************

 Filename: main.cpp

 Copyright: © 2021年 Peiqi Li. All rights reserved.

 Author: Peiqi Li

 Date: 2021-07-07

 Description: Main function in program.

*******************************************************************************/


#include<iostream>
#include<fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "error.h"
#include "lexicalAnalysis.h"
#include "grammaticalAnalysis.h"
#include "virtualMachineOfStackStructure.h"
#include "ToMIPS.h"
#include "ToMIPS_Better.h"
#include "global.h"


int main()
{
    /*测试用
    WhileEnableSwitch = true;
    PrintNameTableEnableSwitch = true;
    PrintObjectCodeListEnableSwitch = true;
    PrintObjectCodeBetterEnableSwitch = true;
    */

    char Flag[10];

    cout<<"Open While Enable Switch?(Y/N)"<<endl;
    cin>>Flag;

    WhileEnableSwitch = (Flag[0]=='y'||Flag[0]=='Y');

    cout<<"Open Print Name Table Enable Switch?(Y/N)"<<endl;
    cin>>Flag;

    PrintNameTableEnableSwitch = (Flag[0]=='y'||Flag[0]=='Y');

    cout<<"Open Print Object Code List Enable Switch?(Y/N)"<<endl;
    cin>>Flag;

    PrintObjectCodeListEnableSwitch = (Flag[0]=='y'||Flag[0]=='Y');

    cout<<"Open Print Object Code Better Enable Switch?(Y/N)"<<endl;
    cin>>Flag;

    PrintObjectCodeBetterEnableSwitch = (Flag[0]=='y'||Flag[0]=='Y');


    while(WhileEnableSwitch)
    {
        cout<<"Please input the name of the C0 file to be compiled:  ";
        cin>>fileName;
        cout<<endl;
        file=fopen(fileName,"r");

        /*若文件存在，则打开，否则报错*/
        if(file)
        {
            /*初始化*/
            init();
            errorCounter=0;
            readOut_index = codeIndex = writeIn_index=0;
            ch=' ';

            /*调用词法分析程序*/
            if(-1!=getsym())
            {
                f_objectCode = fopen("f_objectCode.tmp","w");
                f_nameList = fopen("f_nameList.tmp","w");

                /*调用分程序编译程序，若出错直接关闭tmp文件*/
                if(-1==block(0,0))
                {
                    fclose(f_objectCode);
                    fclose(f_nameList);
                    fclose(file);
                    printf("\n");
                    return 0;
                }

                /*若编译过程中未出现错误*/
                if(errorCounter==0)
                {
                    /*调用解释程序*/
                    interpretiveProcedure();
                }

                /*抛出异常信息*/
                else
                {
                    cout<<"Errors in c0 program"<<endl;
                }
            }

            fclose(file);

            /*将目标代码进一步生成MIPS汇编指令*/
            ToMIPS();

            /*若目标代码以优化，进一步生成优化后的MIPS汇编指令*/
            if(PrintObjectCodeBetterEnableSwitch){
                ToMIPS_Better();
            }

        }

        else
        {
            cout<<"can not open the file"<<endl;
        }

        system("pause");
        printf("\n\n");
    }

    return 0;

}




