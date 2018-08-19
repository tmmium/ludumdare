// main.cc

#include "broccoli.h"

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <malloc.h>
#include <assert.h>

int bq_open_file_dialog(char* dst,int len);
int bq_save_file_dialog(char* dst,int len);

#include "states.h"
#include "math.cc"
#include "bitmap.cc"
#include "mesh.cc"
#include "font.cc"
#include "sprite.cc"
#include "camera.cc"
#include "input.cc"
#include "audio.cc"
#include "player.cc"
#include "entity.cc"
#include "world.cc"
#include "gui.cc"
#include "minimap.cc"
#include "crosshair.cc"
#include "editor.cc"
#include "game.cc"

#pragma comment(lib,"Comdlg32.lib")
#include <Windows.h>
int __stdcall WinMain(HINSTANCE,HINSTANCE,char*,int)
{
  const int width=640,height=360;
  if (!bq_init("LD40",1280,720)) 
  {
    bq_log("[game] error: something bad happened, looks like you can't start consuming broccoli!");
  }

  Game* game=(Game*)malloc(sizeof(Game));
  if (!init(game,width,height)) {return -1;}

  while(bq_process())
  {
    if (!update(game)) {break;}
    draw(game);
  }
  
  return 0;
}

int bq_open_file_dialog(char* dst,int len)
{
  OPENFILENAMEA ofn={};
  ofn.lStructSize=sizeof(OPENFILENAMEA);
  ofn.lpstrTitle="Open MAP";
  ofn.lpstrFile=dst;
  ofn.nMaxFile=len;
  ofn.lpstrFilter="All files|*.*";
  ofn.nFilterIndex=0;
  ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
  return GetOpenFileNameA(&ofn);
}

int bq_save_file_dialog(char* dst,int len)
{
  OPENFILENAMEA ofn={};
  ofn.lStructSize=sizeof(OPENFILENAMEA);
  ofn.lpstrTitle="Open MAP";
  ofn.lpstrFile=dst;
  ofn.nMaxFile=len;
  ofn.lpstrFilter="All files|*.txt";
  ofn.nFilterIndex=0;
  ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
  return GetSaveFileNameA(&ofn);
}
