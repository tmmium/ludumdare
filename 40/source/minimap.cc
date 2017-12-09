// minimap.cc

struct Minimap
{
  v2 position;
  float refresh;
  float timer;
  Bitmap bitmap;
  int texture;
  Sprite sprite;
};

bool init(Minimap* minimap,const v2 position)
{
  minimap->refresh=0.1f;
  minimap->timer=0.0f;
  minimap->texture=bq_create_texture(16,16,NULL);
  create(&minimap->bitmap,16,16);
  init(&minimap->sprite,&minimap->bitmap,0,0,16,16);
  minimap->sprite.dim.x*=4.0f;
  minimap->sprite.dim.y*=4.0f;
  minimap->position=position;

  return true;
}

void update(Minimap* minimap,const Bitmap* src,const v3 position,float dt)
{
  minimap->timer-=dt;
  if (minimap->timer<0.0f)
  {
    minimap->timer=minimap->refresh;
    blit(&minimap->bitmap,src,(int)position.x,(int)position.z);
    upload(&minimap->bitmap,minimap->texture);
  }
}

void draw(Minimap* minimap,GUI* gui)
{
  gui_group(gui,minimap->texture);
  gui_icon(gui,minimap->position,minimap->sprite);
}