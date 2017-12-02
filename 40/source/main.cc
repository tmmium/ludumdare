// main.cc

#include "broccoli.h"

#include "math.cc"
#include "font.cc"
#include "sprite.cc"
#include "camera.cc"
#include "player.cc"
#include "world.cc"
#include "input.cc"

float deltatime(unsigned& prev)
{
  unsigned tick=bq_get_ticks();
  unsigned diff=tick-prev;
  prev=tick;
  return diff*0.001f;
}

int __stdcall WinMain(void*,void*,char*,int)
{
  if (!bq_init("LD40: \"The more you have, the worse it is...\"",1280,720)) {return -1;}

  const int screen_width=320,screen_height=180;
  m4 persp=bq_perspective((float)screen_width/(float)screen_height,kPI*0.35f,0.1f,100.0f);
  m4 ortho=bq_orthographic(screen_width,screen_height);

  int tex=bq_load_texture("assets/main.png");

  Font font;
  init(&font);
  
  Sprite crosshair;
  init(&crosshair,0,0,5,5);

  Player player;
  World world;
  init(&world,"assets/world.png");
  init(&player,world.spawn_point);

  Input input;
  init(&input);

  Camera camera;
  init(&camera,world.spawn_point);
  update(&camera);

  unsigned prev=bq_get_ticks();
  while(bq_process())
  {
    float dt=deltatime(prev);
    update(&input);
    controller(&input,&player,dt);
    collision(&world,&player);
    controller(&input,&camera,&player);

    // render world
    bq_prepare3d();
    bq_projection(persp);
    bq_view(camera.view);
    draw(&world);

    // render gui
    bq_prepare2d();
    bq_projection(ortho);
    bq_bind_texture(tex);
    draw(&crosshair,screen_width>>1,screen_height>>1);

    char msg[128];
    sprintf_s<128>(msg,"%d %d",
      (int)input.mouse_position.x,
      (int)input.mouse_position.y);
    draw(&font,2.0f,2.0f,msg);

    sprintf_s<128>(msg,"%f %f %f",
      player.position.x,
      player.position.z,
      player.y_bob);
    draw(&font,2.0f,10.0f,msg);
  }

  return 0;
}
