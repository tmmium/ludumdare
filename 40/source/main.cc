// main.cc

#include "broccoli.h"

#define VK_ESCAPE 0x1B

int __stdcall WinMain(void* hInstance,void* hPrevInstance,char* lpCmdLine,int nCmdShow)
{
  if (!bq_init("ludumdarefourty",1024,768)) {return -1;}
  m4 proj=bq_orthographic(320,240);

  while(bq_process())
  {
    if (bq_keyboard(VK_ESCAPE)) {break;}

    bq_projection(proj);
  }

  return 0;
}

