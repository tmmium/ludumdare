// main.cc

#include "broccoli.h"

// game
#if DEVELOPMENT==0
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
  if (!bq_init("ludumdarefourty",640,480)) {return -1;}
  while(bq_process())
  {
    
  }
  return 0;
}
