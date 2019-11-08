//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __C_INCLUDE_H___
#define __C_INCLUDE_H___

//must implements
int OSLoadImage(const char* szDir, const char* szFileName, const char* szExt, unsigned char** retPtr, unsigned int* retWidth, unsigned int* retHeight, unsigned int* retBpp, int nDiv);

//optinal implements
typedef int (*GetRealPathProc)(char* retRealPath, const char* szDir, const char* szFileName, const char* szExt, int bFindExist);
void    SetRealPathGetting(GetRealPathProc realPathProc);

//phantom implemented
char*   PhantomAlloc(unsigned int size);
void    PhantomFree(char* v);

typedef int (*ThreadProc)(const void* param);
//create the thread from objective c, and return the thread id, return -1 if it not working.
//int           ThreadCreate(const void* param, ThreadProc proc);
//void          ThreadDelete(int id);
//create thread lock
void*         CreateLock();
void          DeleteLock(void* id);
void          SetLock(void* id);
void          SetUnlock(void* id);


#endif
