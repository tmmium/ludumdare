// main.cc

// "The more you have, the worse it is"

#include "broccoli.h"

#define VK_ESCAPE 0x1B

int __stdcall WinMain(void*,void*,char*,int)
{
  if (!bq_init("LD40: 'The more you have, the worse it is'",1024,768)) {return -1;}
  m4 proj=bq_orthographic(320,240);

  while(bq_process())
  {
    if (bq_keyboard(VK_ESCAPE)) {break;}

    bq_projection(proj);
  }

  return 0;
}
