// gui.cc

struct GUI
{
  m4 projection;
  Font font;
  Bitmap bitmap;
  int texture;
  bool is_crosshair_visible;
  Sprite crosshair;
  v2 crosshair_position;
};

bool init(GUI* gui,const char* filename,int screen_width,int screen_height)
{
  Bitmap bitmap;
  if (!init(&bitmap,filename)) {return false;}

  gui->projection=bq_orthographic(screen_width,screen_height);
  init(&gui->font);
  gui->bitmap=bitmap;
  gui->texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  gui->is_crosshair_visible=false;
  init(&gui->crosshair,59,59,5,5);
  gui->crosshair_position={screen_width*0.5f,screen_height*0.5f};

  return true;
}

void update(GUI* gui,Input* input)
{
  gui->is_crosshair_visible=!input->is_cursor_visible;
}

void draw(GUI* gui)
{
  bq_prepare2d();
  bq_disable_fog();
  bq_projection(gui->projection);
  bq_bind_texture(gui->texture);
  if (gui->is_crosshair_visible)
    draw(&gui->crosshair,gui->crosshair_position.x,gui->crosshair_position.y);
}
