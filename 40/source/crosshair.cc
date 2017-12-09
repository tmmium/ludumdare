// crosshair.cc

struct Crosshair
{
  bool is_visible;
  v2 position;
  int texture;
  Sprite sprite;
};

void init(Crosshair* crosshair,const v2 position)
{
  Bitmap bitmap;
  create(&bitmap,3,3);
  memset(bitmap.data,0,sizeof(unsigned)*3*3);
  bitmap.data[0]=0xff00ff00;
  bitmap.data[2]=0xff00ff00;
  bitmap.data[4]=0xff00ff00;
  bitmap.data[6]=0xff00ff00;
  bitmap.data[8]=0xff00ff00;

  int texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  Sprite sprite;
  init(&sprite,&bitmap,0.0f,0.0f,bitmap.width,bitmap.height);
  destroy(&bitmap);

  crosshair->is_visible=false;
  crosshair->position=position;
  crosshair->texture=texture;
  crosshair->sprite=sprite;
}

void update(Crosshair* crosshair,bool visible)
{
  crosshair->is_visible=visible;
}

void draw(Crosshair* crosshair,GUI* gui)
{
  gui_group(gui,crosshair->texture);
  gui_icon(gui,crosshair->position,crosshair->sprite);
}
