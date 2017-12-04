// game.cc

#define GAME_VERSION "V1.2.0"

struct Game
{
  GameState state;
  unsigned tick;
  float dt;
  Input input;
  World world;
  GUI gui;
  Audio audio;
  Camera camera;
  Player player;
};

bool init(Game* game,int width,int height)
{
  game->state=GAME_STATE_LOADING;
  game->tick=bq_get_ticks();

  init(&game->input);
  if (!init(&game->world,"assets/material/main.png","assets/map/first.png"))
  {
    bq_log("[game] error: could not load world\n");
    return false;
  }
  if (!init(&game->gui,"assets/main.png",width,height))
  {
    bq_log("[game] error: could not load gui\n");
    return false;
  }
  if (!init(&game->audio))
  {
    bq_log("[game] error: could not load audio\n");
    return false;
  }
  
  init(&game->camera,bq_perspective((float)width/(float)height,kPI*0.35f,0.25f,200.0f));
  init(&game->player,game->world.spawn);

  return true;
}

void deltatime(Game* game)
{
  const float upper=0.016f;
  unsigned tick=bq_get_ticks();
  unsigned diff=tick-game->tick;
  float result=diff*0.001f;
  game->tick=tick;
  game->dt=result>upper?upper:result;
}

static void change_state(Game* game,GameState state)
{
  bq_log("[game] changing game state '%s' -> '%s'\n",
    game_state_string(game->state),
    game_state_string(state));
  game->state=state;
}

static bool update_loading(Game* game)
{
  controller(&game->input,&game->camera,&game->player);
  change_state(game,GAME_STATE_START);
  return true;
}

static bool update_start(Game* game)
{  
  float x=game->gui.width*0.5f-25.0f;
  float y=game->gui.height*0.5f-10.0f;
  if (button(&game->gui,game->input.left_button_once,{x,y,50,20},"PLAY"))
  {
    game->input.state=INPUT_STATE_CAMERA;
    game->input.is_cursor_visible=false;
    game->input.mouse_position=bq_mouse_position();
    bq_set_cursor(false);
    reset(&game->world);
    change_state(game,GAME_STATE_PLAY);
    play(&game->audio,SOUND_SPAWN,0.15f);
  }

  if (button(&game->gui,game->input.left_button_once,{10,134,30,12},"EXIT"))
  {
    return false;
  }

  sprintf_s<32>(game->gui.label_mouse_inv_y,"INVERT Y      %s",game->input.inverse.y<0.0f?"TRUE":"FALSE");
  make_label(&game->gui,{32,153},{0.0f,0.0f,0.0f,1.0f},game->gui.label_mouse_inv_y);
  make_label(&game->gui,{32,152},{1.0f,1.0f,1.0f,1.0f},game->gui.label_mouse_inv_y);
  if (button(&game->gui,game->input.left_button_once,{10,150,8,8},"+"))
  {
    game->input.inverse.y=-game->input.inverse.y;
  }

  sprintf_s<32>(game->gui.label_mouse_sens, "SENSITIVITY  %3.2f",game->input.sensitivity);
  make_label(&game->gui,{32,163},{0.0f,0.0f,0.0f,1.0f},game->gui.label_mouse_sens);
  make_label(&game->gui,{32,162},{1.0f,1.0f,1.0f,1.0f},game->gui.label_mouse_sens);
  if (button(&game->gui,game->input.left_button_once,{10,160,8,8},"+"))
  {
    game->input.sensitivity+=0.2f;
  }
  if (button(&game->gui,game->input.left_button_once,{22,160,8,8},"-"))
  {
    game->input.sensitivity-=0.2f;
    if (game->input.sensitivity<0.2f) 
    {
      game->input.sensitivity=0.2f;
    }
  }

  make_label(&game->gui,
    {game->gui.width-text_width(&game->gui.font,GAME_VERSION),
     game->gui.height-text_height(&game->gui.font,GAME_VERSION)},
    {1.0f,1.0f,1.0f,1.0f},GAME_VERSION);

  return true;
}

static bool update_play(Game* game)
{
  controller(&game->input,&game->player,&game->audio,game->dt);
  contain(&game->world,&game->player);
  controller(&game->input,&game->camera,&game->player);

  if (game->input.is_cursor_visible)
  {
    float x=game->gui.width*0.5f-25.0f;
    float y=game->gui.height*0.5f-10.0f;
    if (button(&game->gui,game->input.left_button_once,{x,y,50,20},"QUIT"))
    {
      play(&game->audio,SOUND_DEAD,0.2f);
      change_state(game,GAME_STATE_START);
    }
  }
  if (!is_alive(&game->player))
  {
    play(&game->audio,SOUND_DEAD,0.2f);
    //init(&game->input);
    bq_set_cursor(true);
    change_state(game,GAME_STATE_END);
  }
  if (is_goal_reached(&game->world,&game->player))
  {
    play(&game->audio,SOUND_FINISH,0.1f);
    //init(&game->input);
    bq_set_cursor(true);
    change_state(game,GAME_STATE_END);
  }

  return true;
}

static bool update_end(Game* game)
{
  float x=game->gui.width*0.5f-25.0f;
  if (button(&game->gui,game->input.left_button_once,{x,140,50,20}, "QUIT"))
  {
    reset(&game->player,game->world.spawn);
    controller(&game->input,&game->camera,&game->player);
    change_state(game,GAME_STATE_START);
  }
  return true;
}

bool update(Game* game)
{
  deltatime(game);
  process(&game->input,game->state);
  update(&game->world,&game->player,&game->audio,game->dt);
  update(&game->gui,&game->input,&game->world,game->player.position,game->state,game->dt);

  bool result=false;
  switch(game->state)
  {
    case GAME_STATE_LOADING: { result=update_loading(game); } break;
    case GAME_STATE_START:   { result=update_start(game); } break;
    case GAME_STATE_PLAY:    { result=update_play(game); } break;
    case GAME_STATE_END:     { result=update_end(game); } break;
  }
  return result;
}

void draw(Game* game)
{
  draw(&game->world,&game->camera);
  draw(&game->gui,&game->player,game->state);

#if DEVELOPMENT==1
  draw_debug_info(&game->gui.font,&game->input,&game->player);
#endif
}
