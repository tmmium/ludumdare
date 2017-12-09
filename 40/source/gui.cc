// gui.cc

#define MAX_STRING_CACHE_LENGTH 4096
struct UIStringCache
{
  int capacity;
  int count;
  char data[MAX_STRING_CACHE_LENGTH];
};

bool init(UIStringCache* cache)
{
  cache->capacity=MAX_STRING_CACHE_LENGTH;
  cache->count=0;
  return true;
}

void reset(UIStringCache* cache)
{
  cache->count=0;
}

char* allocate(UIStringCache* cache,int len)
{
  assert(cache->count+len<cache->capacity);
  char* result=cache->data+cache->count;
  cache->count+=len;
  return result;
}

struct UIGroup
{
  int texture;
};

struct UILabel
{
  v4 color;
  const Font* font;
  const char* text;
};

struct UIIcon
{
  Sprite sprite;
};

enum UIButtonState 
{
  BUTTON_NORMAL,
  BUTTON_HOVER,
  BUTTON_DOWN,
};

struct UIButton
{
  UIButtonState state;
  Sprite sprite;
};

enum UIWidgetType
{
  WIDGET_GROUP,
  WIDGET_LABEL,
  WIDGET_ICON,
  WIDGET_BUTTON,
  WIDGET_INVALID,
};

struct UIWidget
{
  UIWidgetType type;
  v2 position;
  union
  {
    UIGroup group;
    UILabel label;
    UIIcon icon;
    UIButton button;
  };
};

static UIGroup make_group(const int texture)
{
  UIGroup result={};
  result.texture=texture;
  return result;
}

static UILabel make_label(const Font* font,const v4 color,const char* text)
{
  UILabel result={};
  result.font=font;
  result.color=color;
  result.text=text;
  return result;
}

static UIIcon make_icon(const Sprite sprite)
{
  UIIcon result={};
  result.sprite=sprite;
  return result;
}

static UIButton make_button(const UIButtonState state,const Sprite sprite)
{
  UIButton result={};
  result.state=state;
  result.sprite=sprite;
  return result;
}

static void draw(const UIWidget* widget)
{
  switch(widget->type)
  {
    case WIDGET_GROUP:
    {
      bq_bind_texture(widget->group.texture);
    } break;
    case WIDGET_LABEL:
    { 
      draw(widget->label.font,widget->position,widget->label.color,widget->label.text);
    } break;
    case WIDGET_ICON:  
    {
      draw(&widget->icon.sprite,widget->position,WHITE);
    } break;
    case WIDGET_BUTTON: 
    { 
      const v4 colors[]={{1.0f,1.0f,1.0f,1.0f},{0.7f,0.7f,0.7f,1.0f},{0.5f,0.5f,0.5f,1.0f}};
      const v4 color=colors[widget->button.state];
      draw(&widget->button.sprite,widget->position,color);
    } break;
  }
}

#define MAX_WIDGET_COUNT 1024
struct UIWidgetCache
{
  int capacity;
  int count;
  UIWidget widgets[MAX_WIDGET_COUNT];
};

bool init(UIWidgetCache* cache)
{
  cache->capacity=MAX_WIDGET_COUNT;
  cache->count=0;
  return true;
}

void reset(UIWidgetCache* cache)
{
  cache->count=0;
}

UIWidget* allocate(UIWidgetCache* cache)
{
  assert(cache->count<cache->capacity);
  UIWidget* result=cache->widgets+cache->count++;
  return result;
}

static UIWidget* push_widget(UIWidgetCache* cache,const UIWidgetType type,const v2 position)
{
  UIWidget* result=allocate(cache);
  result->type=type;
  result->position=position;
  return result;
}

static void push_group(UIWidgetCache* cache,const int texture)
{
  UIWidget* widget=push_widget(cache,WIDGET_GROUP,{0.0f,0.0f});
  widget->group=make_group(texture);
}

static void push_label(UIWidgetCache* cache,const Font* font,const v2 position,const v4 color,const char* text)
{
  UIWidget* widget=push_widget(cache,WIDGET_LABEL,position);
  widget->label=make_label(font,color,text);
}

static void push_icon(UIWidgetCache* cache,const v2 position,const Sprite sprite)
{
  UIWidget* widget=push_widget(cache,WIDGET_ICON,position);
  widget->icon=make_icon(sprite);
}

static void push_button(UIWidgetCache* cache,const v2 position,const UIButtonState state,const Sprite sprite)
{
  UIWidget* widget=push_widget(cache,WIDGET_BUTTON,position);
  widget->button=make_button(state,sprite);  
}

struct GUI
{
  int width;
  int height;
  m4 projection;
  int texture;
  Bitmap bitmap;
  FontCache font_cache;
  UIWidgetCache widget_cache;
  UIStringCache string_cache;

#if 0
  bool is_crosshair_visible;

  float minimap_refresh;
  float minimap_timer;
  Bitmap minimap;
  int mini_texture;
  Sprite minisprite;
  v2 minimap_position;
  int player_score;
  int player_health;
#endif
};

bool init(GUI* gui,const char* filename,int width,int height)
{
  Bitmap bitmap;
  if (!init(&bitmap,filename)) {return false;}

  FontCache font_cache;
  if (!init(&font_cache)) {return false;}

  UIWidgetCache widget_cache;
  if (!init(&widget_cache)) {return false;}

  UIStringCache string_cache;
  if (!init(&string_cache)) {return false;}

  gui->width=width;
  gui->height=height;
  gui->projection=bq_orthographic(width,height);
  gui->texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  gui->bitmap=bitmap;
  gui->font_cache=font_cache;
  gui->widget_cache=widget_cache;
  gui->string_cache=string_cache;
#if 0
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

  gui->player_score=0;
  gui->player_health=0;
#endif
  return true;
}

void reset(GUI* gui)
{
  reset(&gui->widget_cache);
  reset(&gui->string_cache);
  push_group(&gui->widget_cache,gui->texture);
}

int gui_load_font(GUI* gui,const char* filename)
{
  Font* font=allocate(&gui->font_cache);
  if (!init(font,filename)) {return -1;}
  return gui->font_cache.count-1; 
}

static int text_width(const GUI* gui,int font_id,const char* str)
{
  return text_width(gui->font_cache.fonts+font_id,str);
}

static int text_height(const GUI* gui,int font_id,const char* str)
{
  return text_height(gui->font_cache.fonts+font_id,str);
}

static bool overlap(const v2 point,const v2 position,const v2 dim)
{
  if (point.x<position.x||point.x>=position.x+dim.x) {return false;}
  if (point.y<position.y||point.y>=position.y+dim.y) {return false;}
  return true;
}

void gui_group(GUI* gui,int texture)
{
  push_group(&gui->widget_cache,texture);
}

void gui_label(GUI* gui,int font_id,const v2 position,const v4 color,const char* format, ...)
{
  int len=(int)strlen(format)+16;
  char* text=allocate(&gui->string_cache,len);
  va_list vl;
  va_start(vl,format);
  vsprintf_s(text,len,format,vl);
  va_end(vl);

  Font* font=gui->font_cache.fonts+font_id;
  push_label(&gui->widget_cache,font,position,color,text);
  push_group(&gui->widget_cache,gui->texture);
}

void gui_icon(GUI* gui,const v2 position,const Sprite sprite)
{
  push_icon(&gui->widget_cache,position,sprite);
  push_group(&gui->widget_cache,gui->texture);
}

bool gui_button(GUI* gui,int font_id,bool down,const v2 position,const v2 dim,const char* text)
{
  v2 mouse=mouse_position_scaled(gui->width,gui->height);
  
  bool inside=false;
  UIButtonState state=BUTTON_NORMAL;
  if (overlap(mouse,position,dim))
  {
    inside=true; 
    state=BUTTON_HOVER;
  }
  if (down&&inside) 
  {
    state=BUTTON_DOWN;
  }

  const Font* font=gui->font_cache.fonts+font_id;

  Sprite sprite;
  init(&sprite,&gui->bitmap,63,0,1,1);
  sprite.dim=dim;
  v2 offset=text_offset(font,dim,text);

  push_button(&gui->widget_cache,position,state,sprite);
  push_label(&gui->widget_cache,font,position+offset,{0.0f,0.0f,0.0f,1.0f},text);
  push_group(&gui->widget_cache,gui->texture);

  return inside&&down;
}

static void draw_widgets(GUI* gui)
{
  for (int i=0;i<gui->widget_cache.count;i++)
  {
    draw(gui->widget_cache.widgets+i);
  }
}

#if 0
void update(GUI* gui,const Input* input,const World* world,const Player* player,GameState state,float dt)
{
  gui->is_crosshair_visible=!input->is_cursor_visible;
  gui->minimap_timer-=dt;
  if (gui->minimap_timer<0.0f)
  {
    gui->minimap_timer=gui->minimap_refresh;
    blit(&gui->minimap,&world->map.collision,(int)player->position.x,(int)player->position.z);
    upload(&gui->minimap,gui->mini_texture);
  }
  gui->player_score=player->score;
  gui->player_health=player->health;
}
#endif

void draw(GUI* gui)
{
  bq_prepare2d();
  bq_disable_fog();
  bq_projection(gui->projection);
  draw_widgets(gui);
  
#if 0
  bq_bind_texture(gui->mini_texture);
  r_draw(&gui->minisprite,gui->minimap_position);

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
    sprintf_s<64>(text,"SCORE: %d",gui->player_score);
    draw(&gui->font,{x,y+11.0f},{0.0f,0.0f,0.0f,1.0f},text);
    draw(&gui->font,{x,y+10.0f},{1.0f,1.0f,1.0f,1.0f},text);
  }
  
  if (state==GAME_STATE_PLAY)
  {
    char text[64]={};
    sprintf_s<64>(text,"%d",gui->player_health);
    v2 o=gui->heart_position;
    o.x+=10.0f;
    o.y+=1.0f;

    draw(&gui->font,{o.x,o.y+1},{0.0f,0.0f,0.0f,1.0f},text);
    draw(&gui->font,{o.x,o.y},{1.0f,1.0f,1.0f,1.0f},text);
    draw(&gui->heart,gui->heart_position,{1.0f,1.0f,1.0f,1.0f});

    sprintf_s<64>(text,"%d",gui->player_score);
    o=gui->coin_position;
    o.x+=10.0f;
    o.y+=1.0f;

    draw(&gui->font,{o.x,o.y+1},{0.0f,0.0f,0.0f,1.0f},text);
    draw(&gui->font,{o.x,o.y},{1.0f,1.0f,1.0f,1.0f},text);
    draw(&gui->coin,gui->coin_position,{1.0f,1.0f,1.0f,1.0f});
  }
#endif
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
