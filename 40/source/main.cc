// main.cc

#include "broccoli.h"

int __stdcall WinMain(void* hInstance,void* hPrevInstance,char* lpCmdLine,int nCmdShow)
{
  if (!bq_init("ludumdarefourty",1024,768)) {return -1;}
  m4 proj=bq_orthographic(320,240);

  while(bq_process())
  {
    bq_projection(proj);
  }

  return 0;
}

