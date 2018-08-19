// editor.cc

enum EditorMode
{
  EDITOR_MODE_SELECT,
  EDITOR_MODE_ERASE,
  EDITOR_MODE_PAINT,
  EDITOR_MODE_COUNT,
};

static const char* global_editor_modes[EDITOR_MODE_COUNT]=
{
  "SELECT",
  "ERASE",
  "PAINT:",
};

enum EditorIcon
{
  EDITOR_ICON_NEW,
  EDITOR_ICON_LOAD,
  EDITOR_ICON_SAVE,
  EDITOR_ICON_SAVE_AS,

  EDITOR_ICON_SELECT,
  EDITOR_ICON_ERASE,
  EDITOR_ICON_PAINT,
  EDITOR_ICON_UNUSED0,

  EDITOR_ICON_FLOOR,
  EDITOR_ICON_PLAYER,
  EDITOR_ICON_EXIT,
  EDITOR_ICON_LIGHT,
  EDITOR_ICON_AMMO,
  EDITOR_ICON_HEALTH,
  EDITOR_ICON_KEY,
  EDITOR_ICON_DOOR,
  EDITOR_ICON_TELEPORT,
  EDITOR_ICON_TRIGGER,

  EDITOR_ICON_COUNT,
};

enum BrushType
{
  BRUSH_FLOOR,
  BRUSH_PLAYER,
  BRUSH_EXIT,
  BRUSH_LIGHT,
  BRUSH_AMMO,
  BRUSH_HEALTH,
  BRUSH_KEY,
  BRUSH_DOOR,
  BRUSH_TELEPORT,
  BRUSH_TRIGGER,
};

static const char* global_brush_modes[]=
{
  "FLOOR",
  "PLAYER",
  "EXIT",
  "LIGHT",
  "AMMO",
  "HEALTH",
  "KEY",
  "DOOR",
  "TELEPORT",
  "TRIGGER",
};

enum TileType
{
  TILE_FLOOR,
  TILE_PLAYER,
  TILE_EXIT,
  TILE_LIGHT,
  TILE_AMMO,
  TILE_HEALTH,
  TILE_KEY,
  TILE_DOOR,
  TILE_TELEPORT,
  TILE_TRIGGER,
  TILE_VOID,
  TILE_COUNT,
};

static const char* global_tile_types[]=
{
  "FLOOR",
  "PLAYER",
  "EXIT",
  "LIGHT",
  "AMMO",
  "HEALTH",
  "KEY",
  "DOOR",
  "TELEPORT",
  "TRIGGER",
  "VOID",
};

struct MapTile
{
  TileType type;
  unsigned data[3];
};

static void set_floor(unsigned* floor,int x,int y,bool on)
{
  int index=0;
  int bit=0;
  floor[index]&=~(1<<bit);
}

static void set_tile(MapTile* tile,TileType type,unsigned data0,unsigned data1,unsigned data2)
{
  tile->type=type;
  tile->data[0]=data0;
  tile->data[1]=data1;
  tile->data[2]=data2;
}


struct Editor
{
  int width;
  int height;
  m4 projection;
  int texture;
  v2 mouse;

  v2 toolbar_position;
  v2 toolbar_dim;
  v2 icon_dim;
  v2 icon_positions[EDITOR_ICON_COUNT];
  Sprite icons[EDITOR_ICON_COUNT];
  Sprite tile_icons[TILE_COUNT];

  float grid_zoom;
  v2 grid_offset;
  v2 grid_tile_dim;
  m4 grid_transform;
  int count;
  v2 grid[512];

  bool has_been_edited;
  char filename[512];

  EditorMode mode;
  BrushType brush;
  int selected_index;
  int selected_x;
  int selected_y;
  int hover_index;
  int hover_x;
  int hover_y;
  unsigned floors[4];
  int num_tiles;
  MapTile tiles[4096];
};

bool init(Editor* editor,int width,int height)
{
  Bitmap bitmap;
  if (!init(&bitmap,"assets/editor.png")) {return false;}
  int texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);

  editor->width=width;
  editor->height=height;
  editor->projection=bq_orthographic(width,height);
  editor->texture=texture;
  editor->mouse={0.0f,0.0f};

  editor->toolbar_position={2.0f,2.0f};
  editor->toolbar_dim={editor->icon_dim.x*2.0f,editor->icon_dim.y*EDITOR_ICON_COUNT+100.0f};
  editor->icon_dim={16.0f,16.0f};
  const float icon_width=editor->icon_dim.x;
  const float icon_height=editor->icon_dim.y;
  for (int i=0;i<EDITOR_ICON_COUNT;i++)
  {
    int px=i%2;
    int py=i/2;
    v2 icon_position={px*icon_width,py*icon_height};
    editor->icon_positions[i]=editor->toolbar_position+icon_position;

    int tx=i%8;
    int ty=i/8;
    Sprite sprite;
    init(&sprite,&bitmap, tx*icon_width,ty*icon_height,icon_width,icon_height);
    editor->icons[i]=sprite;
  }
  for (int i=0;i<TILE_COUNT;i++)
  {
    int tx=i%8;
    int ty=i/8+4;

    Sprite sprite;
    init(&sprite,&bitmap, tx*icon_width,ty*icon_height,icon_width,icon_height);
    sprite.dim={1.0f,1.0f};
    editor->tile_icons[i]=sprite;
  }

  destroy(&bitmap);

  editor->grid_zoom=4.0f;
  editor->grid_offset=
  {
    (width*0.5f)-32.0f*editor->grid_zoom,
    (height*0.5f)-32.0f*editor->grid_zoom
  };
  editor->grid_tile_dim={editor->grid_zoom,editor->grid_zoom};
  m4 t=translate({editor->grid_offset.x,editor->grid_offset.y,0.0f});
  m4 s=scale({editor->grid_zoom,editor->grid_zoom,1.0f});
  editor->grid_transform=bq_multiply(t,s);
  editor->count=0;
  for (int y=0;y<65;y++)
  {
    float py=y;
    editor->grid[editor->count++]={ 0.0f,py};
    editor->grid[editor->count++]={64.0f,py};
  }
  for (int x=0;x<65;x++)
  {
    float px=x;
    editor->grid[editor->count++]={px, 0.0f};
    editor->grid[editor->count++]={px,64.0f};
  }

  editor->has_been_edited=false;
  memset(editor->filename,0,sizeof(editor->filename));

  editor->mode=EDITOR_MODE_SELECT;
  editor->brush=BRUSH_FLOOR;
  editor->selected_index=-1;
  editor->selected_x=-1;
  editor->selected_y=-1;
  editor->hover_index=-1;
  editor->hover_x=-1;
  editor->hover_y=-1;
  for (int i=0;i<4;i++)
  {
    editor->floors[i]=0;
  }
  for (int i=0;i<4096;i++)
  {
    set_tile(editor->tiles+i,TILE_VOID,0,0,0);
  }

  return true;
}

static void on_null_clicked(Editor* editor)
{
}

static void on_new_clicked(Editor* editor)
{
  for (int i=0;i<4096;i++)
  {
    set_tile(editor->tiles+i,TILE_VOID,0,0,0);
  }
}

static void on_load_clicked(Editor* editor)
{
  char filename[1024]={};
  int len=sizeof(filename);
  if (bq_open_file_dialog(filename,len))
  {
    bq_log("[editor] load file %s\n",filename);
  }
}

static void on_save_clicked(Editor* editor)
{
  char filename[1024]={};
  int len=sizeof(filename);
  if (bq_open_file_dialog(filename,len))
  {
    bq_log("[editor] save file %s\n",filename);
  }
}

static void on_save_as_clicked(Editor* editor)
{
}

static void on_select_clicked(Editor* editor)
{
  editor->mode=EDITOR_MODE_SELECT;
}

static void on_erase_clicked(Editor* editor)
{
  editor->mode=EDITOR_MODE_ERASE;
}

static void on_paint_clicked(Editor* editor)
{
  editor->mode=EDITOR_MODE_PAINT;
}

static void on_floor_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_FLOOR;
}

static void on_player_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_PLAYER;
}

static void on_exit_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_EXIT;
}

static void on_light_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_LIGHT;
}

static void on_ammo_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_AMMO;
}

static void on_health_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_HEALTH;
}

static void on_key_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_KEY;
}

static void on_door_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_DOOR;
}

static void on_teleport_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_TELEPORT;
}

static void on_trigger_brush_clicked(Editor* editor)
{
  editor->brush=BRUSH_TRIGGER;
}

typedef void (*on_button_clicked_callback)(Editor* editor);
static on_button_clicked_callback global_editor_callbacks[]=
{
  on_new_clicked,
  on_load_clicked,
  on_save_clicked,
  on_save_as_clicked,
  on_select_clicked,
  on_erase_clicked,
  on_paint_clicked,
  on_null_clicked, // unused0
  on_floor_brush_clicked,
  on_player_brush_clicked,
  on_exit_brush_clicked,
  on_light_brush_clicked,
  on_ammo_brush_clicked,
  on_health_brush_clicked,
  on_key_brush_clicked,
  on_door_brush_clicked,
  on_teleport_brush_clicked,
  on_trigger_brush_clicked,
};

bool update_editor(Editor* editor,const Input* input,GUI* gui,float dt)
{
  v2 mw=bq_mouse_wheel();
  if (fabsf(mw.y)>0.0f) 
  {
    editor->grid_zoom+=mw.y;
    if (editor->grid_zoom<4.0f) 
    {
      editor->grid_zoom=4.0f;
    }
    else if(editor->grid_zoom>32.0f)
    {
      editor->grid_zoom=32.0f;
    }
    editor->grid_tile_dim={editor->grid_zoom,editor->grid_zoom};
  }

  v2 mp=mouse_position_scaled(editor->width,editor->height);  
  v2 mpd=mp-editor->grid_offset;
  editor->hover_x=(int)(mpd.x/editor->grid_tile_dim.x);
  editor->hover_y=(int)(mpd.y/editor->grid_tile_dim.x);

  bool is_inside_grid=(editor->hover_x>=0&&
    editor->hover_x<64&&
    editor->hover_y>=0&&
    editor->hover_y<64);
  
  if (is_inside_grid)
  {
    editor->hover_index=editor->hover_x+editor->hover_y*64;
  }
  else 
  {
    editor->hover_index=-1;
    editor->hover_x=-1;
    editor->hover_y=-1;
  }

  // pan camera
  if (is_right_down(input))
  {
    v2 dm=(mp-editor->mouse);
    editor->grid_offset=editor->grid_offset+dm;
  }

  // select tile
  if (is_left_down_once(input)&&editor->mode==EDITOR_MODE_SELECT)
  {
    if (is_inside_grid)
    {
      editor->selected_index=editor->hover_index;
      editor->selected_x=editor->hover_x;
      editor->selected_y=editor->hover_y;
    }
    else
    {
      editor->selected_index=-1;
      editor->selected_x=-1;
      editor->selected_y=-1;
    }
  }
  editor->mouse=mp;

  v2 go=editor->grid_offset/editor->grid_zoom;
  m4 t=translate({go.x,go.y,0.0f});
  m4 s=scale({editor->grid_zoom,editor->grid_zoom,1.0f});
  editor->grid_transform=bq_multiply(t,s);

  const v2 dim=editor->icon_dim;
  const float icon_width=editor->icon_dim.x;
  const float icon_height=editor->icon_dim.y;
  const bool is_clicked=is_left_down_once(input);

  bool is_down=is_left_down(input);
  if (overlap(mp,{},editor->toolbar_dim))
  {
    is_down=false;
  }
  
  v2 offset={};
  gui_group(gui,editor->texture);
  for (int i=0;i<EDITOR_ICON_COUNT;i++)
  {
    if (i==EDITOR_ICON_UNUSED0) {continue;}

    v2 pos=editor->icon_positions[i];
    if (gui_button(gui,is_clicked,pos,dim,editor->icons[i]))
    {
      global_editor_callbacks[i](editor);
    }
  }
  /*if (editor->mode==EDITOR_MODE_PAINT)
  {
  // new, load, save, save-as, select, paint, erase
    // brushes
    for (int i=EDITOR_ICON_FLOOR;i<EDITOR_ICON_COUNT;i++)
    {
      v2 pos=editor->toolbar_position+offset;
      if (gui_button(gui,is_clicked,pos,dim,editor->icons[i]))
      {
        editor->brush=(BrushType)(i-EDITOR_ICON_FLOOR);
      }
      offset.x+=icon_width;
    }  
  }*/
  gui_pop_group(gui);

  offset.x=2.0f;
  offset.y=icon_height+2.0f;
  v2 pos=editor->toolbar_position+offset;
  gui_label(gui,0,pos,WHITE,global_editor_modes[editor->mode]);
  // todo: better?
  if (editor->mode==EDITOR_MODE_PAINT)
  {
    v2 lpos=pos;
    lpos.x+=text_width(gui,0,global_editor_modes[editor->mode])+2.0f;
    gui_label(gui,0,lpos,WHITE,global_brush_modes[editor->brush]);
  }
  pos.y+=10;
  if (editor->hover_index!=-1)
  {
    MapTile* tile=editor->tiles+editor->hover_index;
    gui_label(gui,0,pos,WHITE,"#%d (X:%d Y:%d T:%s)",
      editor->hover_index,
      editor->hover_x,
      editor->hover_y,
      global_tile_types[tile->type]);
  }
  else 
  {
    gui_label(gui,0,pos,WHITE,"#%d (X:%d Y:%d T:?)",
        editor->hover_index,
        editor->hover_x,
        editor->hover_y);
  }
    
  if (editor->selected_index!=-1)
  {
    pos.y+=10;
    MapTile* tile=editor->tiles+editor->selected_index;
    gui_label(gui,0,pos,WHITE,"TILE: %s",global_tile_types[tile->type]);
  }

  // select, paint and erase logic
  if (editor->mode==EDITOR_MODE_SELECT)
  {
    if (editor->selected_index!=-1)
    {
      v2 btn_dim={8.0f,8.0f};
      MapTile* tile=editor->tiles+editor->selected_index;
      switch(tile->type)
      {
        case TILE_KEY:
        case TILE_DOOR:
        case TILE_TELEPORT:
        {
          pos.y+=10;
          if (gui_button(gui,0,is_clicked,pos,btn_dim,"+"))
          {
            tile->data[0]=(tile->data[0]+1)%0xff;
          }
          pos.x+=10;
          if (gui_button(gui,0,is_clicked,pos,btn_dim,"-"))
          {
            tile->data[0]=(tile->data[0]-1)%0xff;
          }
          pos.x+=10;
          pos.y+=2;
          gui_label(gui,0,pos,WHITE,"ID: %X",tile->data[0]);
        } break;
      }
    }
  }
  else if (editor->mode==EDITOR_MODE_ERASE)
  {
    if (editor->hover_index!=-1&&is_down)
    {
      editor->has_been_edited=true;
      set_tile(editor->tiles+editor->hover_index,TILE_VOID,0,0,0);
    }
  }
  else if (editor->mode==EDITOR_MODE_PAINT)
  {
    if (editor->hover_index!=-1&&is_down)
    {
      editor->has_been_edited=true;
      MapTile* tile=editor->tiles+editor->hover_index;
      set_tile(tile,(TileType)editor->brush,0,0,0);
    }
  }

  return true;
}

void draw(Editor* editor)
{
  bq_prepare2d();
  bq_projection(editor->projection);

  bq_push_transform(editor->grid_transform);
  bq_bind_texture(editor->texture);
  for (int i=0;i<4096;i++)
  {
    MapTile* tile=editor->tiles+i;
    if (tile->type==TILE_VOID) {continue;}

    v2 pos={(float)(i%64),(float)(i/64)};
    Sprite* sprite=editor->tile_icons+tile->type;
    draw(sprite,pos,WHITE);
  }

  bq_bind_texture(0);
  bq_render2d_lines(GRAY,editor->count,editor->grid);
  bq_pop_transform();
}
