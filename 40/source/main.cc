// main.cc

#include "broccoli.h"

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <malloc.h>
#include <assert.h>

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

int __stdcall WinMain(void*,void*,char*,int)
{
  const int width=640,height=360;
  if (!bq_init("LD40",1280,720)) 
  {
    bq_log("[game] error: something bad happened, looks like you can't start consuming broccoli!");
    return -1;
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
