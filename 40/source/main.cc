// main.cc

#include "broccoli.h"

#include <string.h>

#include "math.cc"
#include "bitmap.cc"
#include "mesh.cc"
#include "font.cc"
#include "sprite.cc"
#include "camera.cc"
#include "player.cc"
#include "entity.cc"
#include "input.cc"
#include "gui.cc"
#include "world.cc"

float deltatime(unsigned& prev)
{
  const float upper=0.016f;
  unsigned tick=bq_get_ticks();
  unsigned diff=tick-prev;
  prev=tick;
  float result=diff*0.001f;
  return result>upper?upper:result;
}

void draw2d()
{
}

static void draw_debug_info(Font* font,Input* input,Player* player)
{
  const v4 color={0.1f,0.2f,0.3f,1.0f};
  char msg[128];
  sprintf_s<128>(msg,"%d %d",
    (int)input->mouse_position.x,
    (int)input->mouse_position.y);
  draw(font,2.0f,2.0f,color,msg);

  sprintf_s<128>(msg,"%f %f %f",
    player->position.x,
    player->position.z,
    player->y_bob);
  draw(font,2.0f,10.0f,color,msg);
}

int __stdcall WinMain(void*,void*,char*,int)
{
  const int screen_width=320,screen_height=180;

  if (!bq_init("LD40: \"The more you have, the worse it is...\"",1280,720)) 
  {
    bq_log("[game] error: something bad happened, looks like you can't start consuming broccoli!");
    return -1;
  }

  Input input;
  init(&input);
  
  World world;
  if (!init(&world,"assets/world.png",screen_width,screen_height)) 
  {
    bq_log("[game] error: could not load world\n");
    return -1;
  }

  GUI gui;
  if (!init(&gui,"assets/main.png",screen_width,screen_height))
  {
    bq_log("[game] error: could not load gui\n");
    return -1;
  }

  unsigned prev=bq_get_ticks();
  
  while(bq_process())
  {
    float dt=deltatime(prev);

    process(&input);
    update(&world,&input,dt);
    update(&gui,&input);

    draw(&world);
    draw(&gui);

    if (finish(&world))
    {
      draw(&gui.font,100,100,{1.0f,1.0f,1.0f,1.0f},"FINISH!");
    }
    
    draw_debug_info(&gui.font,&input,&world.player);
  }

  return 0;
}
