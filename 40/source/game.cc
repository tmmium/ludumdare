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
  if (!init(&game->gui,"assets/main.png",width,height))
  {
    bq_log("[game] error: could not load gui\n");
    return false;
  }
  if (!init(&game->mesh_cache,256*1024)) 
  {
    bq_log("[game] error: could create mesh cache\n");
    return false;
  }
  if (!init(&game->world,&game->mesh_cache,"assets/material/main.png","assets/map/first.png"))
  {
    bq_log("[game] error: could not load world\n");
    return false;
  }
  
  init(&game->camera,bq_perspective((float)width/(float)height,kPI*0.35f,0.25f,200.0f));
  init(&game->player,game->world.map.spawn);

  return true;
}

static void respawn(Game* game)
{
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

  float x=game->width*0.5f-25.0f;
  float y=game->height*0.5f-10.0f;
  
  if (button(&game->gui,is_clicked,{x,y,50,20},"PLAY"))
  {
    play(&game->audio,SOUND_SPAWN,0.15f);
    change_state(game,GAME_STATE_PLAY);
  }
  if (button(&game->gui,is_clicked,{x,y+30.0f,50,20},"EXIT"))
  {
    return false;
  }

  char* text=NULL;
  bool is_inverted=is_mouse_axis_y_inverted(&game->input.mouse);
  text=allocate(&game->gui.string_cache,32);
  sprintf_s(text,32,"INVERT Y      %s",is_inverted?"TRUE":"FALSE");
  label(&game->gui,{32,332},text);
  if (button(&game->gui,is_clicked,{10,330,8,8},"+"))
  {
    invert_mouse_axis_y(&game->input.mouse);
  }

  const float sens=game->input.mouse.sensitivity;
  text=allocate(&game->gui.string_cache,32);
  sprintf_s(text,32, "SENSITIVITY  %3.2f",sens);
  label(&game->gui,{32,343},text);
  if (button(&game->gui,is_clicked,{10,340,8,8},"+"))
  {
    mouse_increment_sensitivity(&game->input.mouse,0.2f);
  }
  if (button(&game->gui,is_clicked,{22,340,8,8},"-"))
  {
    mouse_decrement_sensitivity(&game->input.mouse,0.2f);
  }

  make_label(&game->gui,
    {game->gui.width-text_width(&game->gui.font,GAME_VERSION),
     game->gui.height-text_height(&game->gui.font,GAME_VERSION)},
    {1.0f,1.0f,1.0f,1.0f},GAME_VERSION);

  return true;
}

static bool update_play(Game* game)
{
  const bool is_clicked=is_left_down_once(&game->input);

  controller(&game->player,&game->input,&game->audio,game->dt);
  contain(&game->world.map,&game->player);
  update(&game->camera,game->player.axis,game->player.position);
  
  update(&game->world,&game->player,&game->audio,game->dt);
  update(&game->gui,&game->input,&game->world,game->player.position,game->state,game->dt);

  if (was_escape_down_once(&game->input))
  {
    set_cursor_visible(&game->input,!game->input.is_cursor_visible);
  }

  if (game->input.is_cursor_visible)
  {
    float x=game->gui.width*0.5f-25.0f;
    float y=game->gui.height*0.5f-10.0f;
    if (button(&game->gui,is_clicked,{x,y,50,20},"QUIT"))
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

  float x=game->width*0.5f-25.0f;
  float y=game->height*0.5f-10.0f;
  
  if (button(&game->gui,is_clicked,{x,y,50,20}, "QUIT"))
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
  draw(&game->gui,&game->player,game->state);
}
