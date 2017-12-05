// gui.cc

struct Label
{
  v4 color;
  const char* text;
};

enum ButtonState 
{
  BUTTON_NORMAL,
  BUTTON_HOVER,
  BUTTON_DOWN,
};

struct Button
{
  ButtonState state;
  Sprite sprite;
};

enum WidgetType
{
  WIDGET_LABEL,
  WIDGET_BUTTON,
};

struct Widget
{
  WidgetType type;
  v2 position;
  union
  {
    Label label;
    Button button;
  };
};

struct GUI
{
  int width;
  int height;
  m4 projection;
  Font font;
  Bitmap bitmap;
  int texture;
  bool is_crosshair_visible;
  Sprite crosshair;
  v2 crosshair_position;
  Sprite heart;
  v2 heart_position;
  Sprite coin;
  v2 coin_position;
  float minimap_refresh;
  float minimap_timer;
  Bitmap minimap;
  int mini_texture;
  Sprite minisprite;
  v2 minimap_position;
  int num_widgets;
  Widget widgets[64];

  char label_mouse_sens[32];
  char label_mouse_inv_y[32];
};

static void make_label(GUI* gui,const v2 position,const v4 color,const char* text)
{
  Widget* widget=gui->widgets+gui->num_widgets++;
  widget->type=WIDGET_LABEL;
  widget->position=position;
  widget->label.color=color;
  widget->label.text=text;
  gui->num_widgets++;
}

static void make_button(GUI* gui,const v2 position,const ButtonState state,const Sprite sprite)
{
  Widget* widget=gui->widgets+gui->num_widgets++;
  widget->type=WIDGET_BUTTON;
  widget->position=position;
  widget->button.state=state;
  widget->button.sprite=sprite;
  gui->num_widgets++;
}

static void draw_label(const Widget* widget,const Font* font)
{
  draw(font,widget->position,widget->label.color,widget->label.text);
}

static void draw_button(const Widget* widget)
{
  const v4 colors[]={{1.0f,1.0f,1.0f,1.0f},{1.0f,0.9f,0.9f,1.0f},{0.8f,0.9f,0.8f,1.0f}};
  const v4 color=colors[widget->button.state];
  draw(&widget->button.sprite,widget->position,color);
}

static void draw_widget(const GUI* gui,const Widget* widget)
{
  switch(widget->type)
  {
    case WIDGET_LABEL:  { draw_label(widget,&gui->font); } break;
    case WIDGET_BUTTON: { draw_button(widget); } break;
  }
}

bool init(GUI* gui,const char* filename,int width,int height)
{
  Bitmap bitmap;
  if (!init(&bitmap,filename)) {return false;}

  gui->width=width;
  gui->height=height;

  gui->projection=bq_orthographic(width,height);
  init(&gui->font);
  gui->bitmap=bitmap;
  gui->texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);

  gui->is_crosshair_visible=false;
  init(&gui->crosshair,&gui->bitmap,59,59,5,5);
  gui->crosshair_position={width*0.5f,height*0.5f};

  init(&gui->heart,&gui->bitmap,0,15,8,8);
  gui->heart_position={2.0f,2.0f};

  init(&gui->coin,&gui->bitmap,8,15,8,8);
  gui->coin_position={2.0f,12.0f};

  gui->minimap_refresh=0.2f;
  gui->minimap_timer=0.0f;
  create(&gui->minimap,16,16);
  gui->mini_texture=bq_create_texture(16,16,NULL);
  init(&gui->minisprite,&gui->minimap,0,0,16,16);
  gui->minisprite.dim.x*=4.0f;
  gui->minisprite.dim.y*=4.0f;
  gui->minimap_position={width-70.0f,4.0f};

  gui->num_widgets=0;
  
  return true;
}

static bool overlap(const v2 position,const v4 rect)
{
  if (position.x<rect.x||position.x>=rect.x+rect.z) {return false;}
  if (position.y<rect.y||position.y>=rect.y+rect.w) {return false;}
  return true;
}

void update(GUI* gui,const Input* input,const World* world,v3 pp,GameState state,float dt)
{
  gui->is_crosshair_visible=!input->is_cursor_visible;
  gui->num_widgets=0;
  gui->minimap_timer-=dt;
  if (gui->minimap_timer<0.0f)
  {
    gui->minimap_timer=gui->minimap_refresh;
    blit(&gui->minimap,&world->collision,(int)pp.x,(int)pp.z);
    upload(&gui->minimap,gui->mini_texture);
  }
}

static void draw_widgets(GUI* gui,GameState state)
{
  for (int i=0,e=gui->num_widgets;i<e;i++)
  {
    Widget* widget=gui->widgets+i;
    draw_widget(gui,widget);
  }
}

void draw(GUI* gui,const Player* player,GameState state)
{
  bq_prepare2d();
  bq_disable_fog();
  bq_projection(gui->projection);

  {
    bq_bind_texture(gui->mini_texture);
    r_draw(&gui->minisprite,gui->minimap_position);
  }

  bq_bind_texture(gui->texture);

  if (state==GAME_STATE_PLAY&&gui->is_crosshair_visible)
    draw(&gui->crosshair,gui->crosshair_position,{1.0f,1.0f,1.0f,1.0f});

  if (state==GAME_STATE_END)
  {
    const char* game_over_text="GAME OVER";
    float x=(gui->width-text_width(&gui->font,game_over_text))*0.5f;
    float y=(gui->height-text_height(&gui->font,game_over_text))*0.5f-30.0f;
    draw(&gui->font,{x,y+1.0f},{0.0f,0.0f,0.0f,1.0f},game_over_text);
    draw(&gui->font,{x,y},{1.0f,1.0f,1.0f,1.0f},game_over_text);

    char text[64]={};
    sprintf_s<64>(text,"SCORE: %d",calculate_score(player));
    draw(&gui->font,{x,y+11.0f},{0.0f,0.0f,0.0f,1.0f},text);
    draw(&gui->font,{x,y+10.0f},{1.0f,1.0f,1.0f,1.0f},text);
  }
  
  if (state==GAME_STATE_PLAY)
  {
    char text[64]={};
    sprintf_s<64>(text,"%d",player->health);
    v2 o=gui->heart_position;
    o.x+=10.0f;
    o.y+=1.0f;

    draw(&gui->font,{o.x,o.y+1},{0.0f,0.0f,0.0f,1.0f},text);
    draw(&gui->font,{o.x,o.y},{1.0f,1.0f,1.0f,1.0f},text);
    draw(&gui->heart,gui->heart_position,{1.0f,1.0f,1.0f,1.0f});

    sprintf_s<64>(text,"%d",player->score);
    o=gui->coin_position;
    o.x+=10.0f;
    o.y+=1.0f;

    draw(&gui->font,{o.x,o.y+1},{0.0f,0.0f,0.0f,1.0f},text);
    draw(&gui->font,{o.x,o.y},{1.0f,1.0f,1.0f,1.0f},text);
    draw(&gui->coin,gui->coin_position,{1.0f,1.0f,1.0f,1.0f});
  }

  draw_widgets(gui,state);
}

static void draw_debug_info(Font* font,Input* input,Player* player)
{
  char msg[128];
  sprintf_s<128>(msg,"X: %f Z:%f BOB:%f",
    player->position.x,
    player->position.z,
    player->y_bob);
  draw(font,{2.0f,350.0f},{0.0f,0.0f,0.0f,1.0f},msg);
}

static bool overlap2(const v2 position,const v4 rect)
{
  if (position.x<rect.x||position.x>=rect.x+rect.z) {return false;}
  if (position.y<rect.y||position.y>=rect.y+rect.w) {return false;}
  return true;
}

v2 mouse_position_scaled(const GUI* gui)
{
  v2 ss={(float)gui->width,(float)gui->height};
  v2 ws=bq_window_size();
  v2 res=bq_mouse_position_in_window(); 
  res.x=res.x*(ss.x/ws.x);
  res.y=res.y*(ss.y/ws.y);
  return res; 
}

bool button(GUI* gui,bool down,const v4 rect,const char* text)
{
  v2 position={rect.x,rect.y};
  v2 mouse=mouse_position_scaled(gui);
  
  bool inside=false;
  ButtonState state=BUTTON_NORMAL;
  if (overlap2(mouse,rect))
  {
    inside=true; 
    state=BUTTON_HOVER;
  }
  if (down) {state=BUTTON_DOWN;}

  Sprite sprite;
  init(&sprite,&gui->bitmap,63,0,1,1);
  sprite.dim.x=rect.z;
  sprite.dim.y=rect.w;

  make_button(gui,position,state,sprite);

    v2 offset=text_offset(&gui->font,{rect.z,rect.w},text);
  make_label(gui,position+offset,{0.0f,0.0f,0.0f,1.0f},text);

  return inside&&state==BUTTON_DOWN;
}
