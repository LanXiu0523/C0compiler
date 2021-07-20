#ifndef __OBJECTCODEPROCESSING_H__
#define __OBJECTCODEPROCESSING_H__

int GenerateVirtualMachineCode(enum functionCode F,int L,int A);
void listVirtualMachineCode(int initialCodeIndex);

#define GenerateVirtualMachineCodeDO(a,b,c) if(-1==GenerateVirtualMachineCode(a,b,c))return -1

#endif




