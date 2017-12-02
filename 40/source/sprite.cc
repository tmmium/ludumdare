// sprite.cc

struct Sprite
{
  v2 dim;
  v4 uv;
};

void init(Sprite* sprite,float x,float y,float w,float h)
{
  sprite->dim={w,h};
  sprite->uv={x,y,x+w,y+h};
  sprite->uv=sprite->uv/256.0f;
}

void draw(Sprite* sprite,float x,float y)
{
  float w=sprite->dim.x,h=sprite->dim.y;
  v2 p[]={x,y,x+w,y,x+w,y+h,x+w,y+h,x,y+h,x,y};
  v4 e=sprite->uv;
  v2 t[]={e.x,e.y,e.z,e.y,e.z,e.w,e.z,e.w,e.x,e.w,e.x,e.y};
  bq_render2d({1.0f,1.0f,1.0f,1.0f},6,p,t);
}
