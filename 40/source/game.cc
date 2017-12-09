// game.cc

#define GAME_VERSION "V1.2.0"

struct Game
{
  GameState state;
  int width;
  int height;
  unsigned tick;
  float dt;
  Input input;
  MeshCache mesh_cache;
  Audio audio;
  GUI gui;
  World world;
  Camera camera;
  Player player;
  
  int font_id;

  // todo: HUD
  Crosshair crosshair;
  Minimap minimap;
  Sprite heart;
  Sprite coin;
};

static void deltatime(Game* game)
{
  const float upper=0.016f;
  unsigned tick=bq_get_ticks();
  unsigned diff=tick-game->tick;
  float result=diff*0.001f;
  game->tick=tick;
  game->dt=result>upper?upper:result;
}

bool init(Game* game,int width,int height)
{
  game->state=GAME_STATE_LOADING;
  game->width=width;
  game->height=height;
  game->tick=bq_get_ticks();
  deltatime(game);

  init(&game->input);
  if (!init(&game->audio))
  {
    bq_log("[game] error: could not load audio\n");
    return false;
  }
  if (!init(&game->gui,"assets/hud.png",width,height))
  {
    bq_log("[game] error: could not load gui\n");
    return false;
  }
  if (!init(&game->mesh_cache,256*1024)) 
  {
    bq_log("[game] error: could create mesh cache\n");
    return false;
  }
  if (!init(&game->world,&game->mesh_cache,"assets/map/first.png"))
  {
    bq_log("[game] error: could not load world\n");
    return false;
  }
  
  const float aspect=(float)width/(float)height;
  init(&game->camera,bq_perspective(aspect,kPI*0.35f,0.25f,200.0f));
  init(&game->player,game->world.map.spawn);

  game->font_id=gui_load_font(&game->gui,"assets/main.png");
  
  init(&game->crosshair,{width*0.5f,height*0.5f});
  if (!init(&game->minimap,{width-70.0f,4.0f})) {return false;}

  init(&game->heart,&game->gui.bitmap,0,0,8,8);
  init(&game->coin,&game->gui.bitmap,8,0,8,8);

  return true;
}

static void respawn(Game* game)
{
  reset(&game->world);
  reset(&game->player,game->world.map.spawn);
  update(&game->camera,game->player.axis,game->player.position);
}

static void change_state(Game* game,GameState state)
{
  bq_log("[game] changing game state '%s' -> '%s'\n",
    game_state_string(game->state),
    game_state_string(state));

  switch(state)
  {
    case GAME_STATE_MENU:
    {
      set_cursor_visible(&game->input,true);
    } break;
    case GAME_STATE_PLAY:
    {
      reset(&game->world);
      set_cursor_visible(&game->input,false);
    } break;
    case GAME_STATE_END:
    {
      set_cursor_visible(&game->input,true);
    } break;
  }

  game->state=state;
}

static bool update_loading(Game* game)
{
  const Player* player=&game->player;
  update(&game->camera,player->axis,player->position);
  change_state(game,GAME_STATE_MENU);
  return true;
}

static bool update_menu(Game* game)
{  
  const bool is_clicked=is_left_down_once(&game->input);
  const int font_id=game->font_id;

  v2 dim={50,20};
  v2 position;
  position.x=game->width*0.5f-25.0f;
  position.y=game->height*0.5f-10.0f;
  
  if (gui_button(&game->gui,font_id,is_clicked,position,dim,"PLAY"))
  {
    play(&game->audio,SOUND_SPAWN,0.15f);
    change_state(game,GAME_STATE_PLAY);
  }
  
  position.y+=30.0f;
  if (gui_button(&game->gui,font_id,is_clicked,position,dim,"EXIT"))
  {
    return false;
  }

  dim={8.0f,8.0f};
  position={10.0f,330.0f};
  gui_label(&game->gui,font_id,{32,332},WHITE,"INVERT Y      %s",
    is_mouse_axis_y_inverted(&game->input.mouse)?"TRUE":"FALSE");
  if (gui_button(&game->gui,font_id,is_clicked,position,dim,"+"))
  {
    invert_mouse_axis_y(&game->input.mouse);
  }

  position.y+=10.0f;
  gui_label(&game->gui,font_id,{32,342},WHITE,"SENSITIVITY  %3.1f",
    game->input.mouse.sensitivity);
  if (gui_button(&game->gui,font_id,is_clicked,position,dim,"+"))
  {
    mouse_increment_sensitivity(&game->input.mouse,0.2f);
  }

  position.x+=10.0f;
  if (gui_button(&game->gui,font_id,is_clicked,position,dim,"-"))
  {
    mouse_decrement_sensitivity(&game->input.mouse,0.2f);
  }

  position=
  {
    (float)(game->gui.width-text_width(&game->gui,font_id,GAME_VERSION)),
    (float)(game->gui.height-text_height(&game->gui,font_id,GAME_VERSION))
  };
  gui_label(&game->gui,font_id,position,WHITE,GAME_VERSION);

  return true;
}

static bool update_play(Game* game)
{
  const bool is_clicked=is_left_down_once(&game->input);
  const int font_id=game->font_id;

  controller(&game->player,&game->input,&game->audio,game->dt);
  contain(&game->world.map,&game->player);
  update(&game->camera,game->player.axis,game->player.position);
  
  update(&game->world,&game->player,&game->audio,game->dt);

  update(&game->crosshair,!game->input.is_cursor_visible);
  draw(&game->crosshair,&game->gui);

  update(&game->minimap,&game->world.map.collision,game->player.position,game->dt);
  draw(&game->minimap,&game->gui);

  gui_icon(&game->gui,{2.0f, 2.0f},game->heart);
  gui_label(&game->gui,game->font_id,{12.0f,4.0f},WHITE,"%d",game->player.health);

  gui_icon(&game->gui,{2.0f,12.0f},game->coin);
  gui_label(&game->gui,game->font_id,{12.0f,14.0f},WHITE,"%d",game->player.score);

  if (was_escape_down_once(&game->input))
  {
    set_cursor_visible(&game->input,!game->input.is_cursor_visible);
  }

  if (game->input.is_cursor_visible)
  {
    v2 dim={50,20};
    v2 position;
    position.x=game->gui.width*0.5f-25.0f;
    position.y=game->gui.height*0.5f-10.0f;
    if (gui_button(&game->gui,font_id,is_clicked,position,dim,"QUIT"))
    {
      respawn(game);
      play(&game->audio,SOUND_DEAD,0.2f);
      change_state(game,GAME_STATE_MENU);
    }
  }

  if (!is_alive(&game->player))
  {
    play(&game->audio,SOUND_DEAD,0.2f);
    change_state(game,GAME_STATE_END);
  }
  if (is_goal_reached(game->world.map.finish,game->player.position))
  {
    play(&game->audio,SOUND_FINISH,0.1f);
    change_state(game,GAME_STATE_END);
  }

  return true;
}

static bool update_end(Game* game)
{
  const bool is_clicked=is_left_down_once(&game->input);
  const int font_id=game->font_id;

  const float center_screen_x=game->width*0.5f;
  const float center_screen_y=game->height*0.5f;

  v2 dim={50,20};
  v2 position;
  const char* game_over_text="GAME OVER";
  position.x=center_screen_x-(text_width(&game->gui,game->font_id,game_over_text))*0.5f;
  position.y=center_screen_y-(text_height(&game->gui,game->font_id,game_over_text))*0.5f-30.0f;
  gui_label(&game->gui,font_id,position,WHITE,game_over_text);

  position.y+=11.0f;
  gui_label(&game->gui,font_id,position,WHITE,"SCORE: %d",game->player.score);

  position.x=center_screen_x-25.0f;
  position.y=center_screen_y-10.0f;
  if (gui_button(&game->gui,font_id,is_clicked,position,dim,"QUIT"))
  {
    respawn(game);
    change_state(game,GAME_STATE_MENU);
  }
  return true;
}

bool update(Game* game)
{
  deltatime(game);
  process(&game->input,game->state);
  reset(&game->gui);

  bool result=false;
  switch(game->state)
  {
    case GAME_STATE_LOADING: { result=update_loading(game); } break;
    case GAME_STATE_MENU:    { result=update_menu(game); } break;
    case GAME_STATE_PLAY:    { result=update_play(game); } break;
    case GAME_STATE_END:     { result=update_end(game); } break;
  }
  return result;
}

void draw(Game* game)
{
  draw(&game->world,&game->camera,game->state);
  draw(&game->gui);
}
