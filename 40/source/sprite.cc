// sprite.cc

struct Sprite
{
  v2 dim;
  v4 uv;
};

void init(Sprite* sprite,const Bitmap* bitmap,float x,float y,float w,float h)
{
  const float iw=1.0f/bitmap->width;
  const float ih=1.0f/bitmap->height;
  sprite->dim={w,h};
  sprite->uv={x,y,x+w,y+h};
  sprite->uv.x=sprite->uv.x*iw;
  sprite->uv.y=sprite->uv.y*ih;
  sprite->uv.z=sprite->uv.z*iw;
  sprite->uv.w=sprite->uv.w*ih;
}

void draw(const Sprite* sprite,const v2 position,const v4 color)
{
  float x=position.x;
  float y=position.y;
  float w=sprite->dim.x,h=sprite->dim.y;
  v2 p[]={x,y,x+w,y,x+w,y+h,x+w,y+h,x,y+h,x,y};
  v4 e=sprite->uv;
  v2 t[]={e.x,e.y,e.z,e.y,e.z,e.w,e.z,e.w,e.x,e.w,e.x,e.y};
  bq_render2d(color,6,p,t);
}

void r_draw(const Sprite* sprite,const v2 position)
{
  float x=position.x;
  float y=position.y;
  float w=sprite->dim.x,h=sprite->dim.y;
  v2 p[]={x,y,x+w,y,x+w,y+h,x+w,y+h,x,y+h,x,y};
  v4 e=sprite->uv;
  v2 t[]=
  {
    e.z,e.y, // 1
    e.x,e.y, // 0
    e.x,e.w, // 3
    e.x,e.w, // 3
    e.z,e.w, // 2
    e.z,e.y, // 1
  };
  bq_render2d({1.0f,1.0f,1.0f,1.0f},6,p,t);
}
