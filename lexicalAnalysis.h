#ifndef __LEXICALANALYSIS_H__
#define __LEXICALANALYSIS_H__

int getch();
int getsym();

#define getchdo if(-1==getch())return -1
#define getsymdo if(-1==getsym()) return -1

#endif




