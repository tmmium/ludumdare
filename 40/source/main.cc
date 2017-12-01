// main.cc

#include "broccoli.h"

// game
int __stdcall WinMain(void* hInstance,void* hPrevInstance,char* lpCmdLine,int nCmdShow)
{
  if (!bq_init("ludumdarefourty",640,480)) {return -1;}
  while(bq_process())
  {
    
  }
  return 0;
}

