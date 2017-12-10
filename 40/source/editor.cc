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
  v2 icon_dim;
  Sprite icons[EDITOR_ICON_COUNT];

  float grid_zoom;
  v2 grid_offset;
  v2 grid_tile_dim;
  m4 grid_transform;
  int count;
  v2 grid[512];

  EditorMode mode;
  BrushType brush;
  int selected_index;
  int selected_x;
  int selected_y;
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
  editor->icon_dim={16.0f,16.0f};
  const float icon_width=editor->icon_dim.x;
  const float icon_height=editor->icon_dim.y;
  for (int i=0;i<EDITOR_ICON_COUNT;i++)
  {
    int tx=i%8;
    int ty=i/8;

    Sprite sprite;
    init(&sprite,&bitmap, tx*icon_width,ty*icon_height,icon_width,icon_height);
    editor->icons[i]=sprite;
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

  editor->mode=EDITOR_MODE_SELECT;
  editor->brush=BRUSH_FLOOR;
  editor->selected_index=-1;
  editor->selected_x=-1;
  editor->selected_y=-1;
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
  // select tile
  if (is_left_down_once(input))
  {
    v2 mpd=mp-editor->grid_offset;
    int x=(int)(mpd.x/editor->grid_tile_dim.x);
    int y=(int)(mpd.y/editor->grid_tile_dim.x);

    if (x>=0&&x<64&&y>=0&&y<64)
    {
      editor->selected_index=y*64+x;
      editor->selected_x=x;
      editor->selected_y=y;
    }
    else
    {
      editor->selected_index=-1;
      editor->selected_x=-1;
      editor->selected_y=-1;
    }
  }
  // move grid around
  if (is_right_down(input))
  {
    v2 dm=(mp-editor->mouse);
    editor->grid_offset=editor->grid_offset+dm;
  }
  editor->mouse=mp;

  v2 go=editor->grid_offset/editor->grid_zoom;
  m4 t=translate({go.x,go.y,0.0f});
  m4 s=scale({editor->grid_zoom,editor->grid_zoom,1.0f});
  editor->grid_transform=bq_multiply(t,s);

  const float icon_width=editor->icon_dim.x;
  const float icon_height=editor->icon_dim.y;
  const bool is_clicked=is_left_down_once(input);
  const v2 dim=editor->icon_dim;

  v2 offset={};
  gui_group(gui,editor->texture);

  // new, load, save, save-as, select, paint, erase
  for (int i=0;i<EDITOR_ICON_UNUSED0;i++)
  {
    v2 pos=editor->toolbar_position+offset;
    if (gui_button(gui,is_clicked,pos,dim,editor->icons[i]))
    {
      if (i>=EDITOR_ICON_SELECT)
      {
        editor->mode=(EditorMode)(i-EDITOR_ICON_SELECT);
      }
    }
    offset.x+=icon_width;
  }
  if (editor->mode==EDITOR_MODE_PAINT)
  {
    for (int i=EDITOR_ICON_FLOOR;i<EDITOR_ICON_COUNT;i++)
    {
      v2 pos=editor->toolbar_position+offset;
      if (gui_button(gui,is_clicked,pos,dim,editor->icons[i]))
      {
        editor->brush=(BrushType)(i-EDITOR_ICON_FLOOR);
      }
      offset.x+=icon_width;
    }  
  }
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
  gui_label(gui,0,pos,WHITE,"#%d (X:%d Y:%d)",editor->selected_index,editor->selected_x,editor->selected_y);

  if (editor->selected_index!=-1)
  {
    pos.y+=10;
    MapTile* tile=editor->tiles+editor->selected_index;
    gui_label(gui,0,pos,WHITE,"TILE: %s",global_tile_types[tile->type]);
  }

  // select, paint and erase logic
  if (editor->mode==EDITOR_MODE_SELECT)
  {
    
  }
  else if (editor->mode==EDITOR_MODE_ERASE)
  {
    if (editor->selected_index!=-1)
    {
      set_tile(editor->tiles+editor->selected_index,TILE_VOID,0,0,0);
    }
  }
  else if (editor->mode==EDITOR_MODE_PAINT)
  {
    if (editor->selected_index!=-1)
    {
      MapTile* tile=editor->tiles+editor->selected_index;
      set_tile(tile,(TileType)editor->brush,0,0,0);
    }
  }

  return true;
}

void draw(Editor* editor)
{
  bq_prepare2d();
  bq_projection(editor->projection);
  bq_bind_texture(0);
  bq_push_transform(editor->grid_transform);
  bq_render2d_lines(GRAY,editor->count,editor->grid);
  bq_pop_transform();
}
