// main.cc

#include "broccoli.h"

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <malloc.h>

#include "states.h"
#include "math.cc"
#include "bitmap.cc"
#include "mesh.cc"
#include "font.cc"
#include "sprite.cc"
#include "camera.cc"
#include "player.cc"
#include "entity.cc"
#include "audio.cc"
#include "input.cc"
#include "world.cc"
#include "gui.cc"
#include "game.cc"

static void save(const char* filename,Game* game)
{
  FILE* fout=fopen(filename,"wb");
  if (!fout) {return;}
  fwrite(&game->input.sensitivity,sizeof(float),1,fout);
  fflush(fout);
  fclose(fout);
}

static bool load(const char* filename,Game* game)
{
  FILE* fin=fopen(filename,"rb");
  if (!fin) {return false;}
  fread(&game->input.sensitivity,sizeof(float),1,fin);
  fclose(fin);
  return true;
}

int __stdcall WinMain(void*,void*,char*,int)
{
  const int width=320,height=180;

  if (!bq_init("LD40: \"The more you have, the worse it is...\"",640,360)) 
  {
    bq_log("[game] error: something bad happened, looks like you can't start consuming broccoli!");
    return -1;
  }

  Game* game=(Game*)malloc(sizeof(Game));
  if (!init(game,width,height)) {return -1;}

  if (!load("assets/config.bin",game)) 
  {
    bq_log("[config] file not found\n");
  }

  while(bq_process())
  {
    if (!update(game)) {break;}
    draw(game);
  }

  save("assets/config.bin",game);
  bq_log("[config] saved\n");

  return 0;
}
