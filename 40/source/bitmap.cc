// bitmap.cc

#pragma warning(push)
#pragma warning(disable : 4456) // declaration of hides previous local declaration
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

struct Bitmap
{
  int width;
  int height;
  unsigned* data;
};

bool init(Bitmap* bitmap,const char* filename)
{
  int width,height,c;
  stbi_uc* data=stbi_load(filename,&width,&height,&c,4);
  if (!data) {return false;}

  bitmap->width=width;
  bitmap->height=height;
  bitmap->data=(unsigned*)data;

  return true;
}

void create(Bitmap* bitmap,int width,int height)
{
  bitmap->width=width;
  bitmap->height=height;
  bitmap->data=(unsigned*)malloc(sizeof(unsigned)*width*height);
}

void clear(Bitmap* bitmap,unsigned color)
{
  for (int y=0;y<bitmap->height;y++)
  {
    for (int x=0;x<bitmap->width;x++)
    {
      bitmap->data[bitmap->width*y+x]=color;
    }
  }
}

unsigned pixel_at(const Bitmap* bitmap,int x,int y)
{
  if (x<0||x>=bitmap->width) {return 0;}
  if (y<0||y>=bitmap->height) {return 0;}
  return bitmap->data[y*bitmap->width+x];
}

bool is_wall(const Bitmap* bitmap,const int x,const int y);
void blit(Bitmap* dst,const Bitmap* src,int px,int py)
{
  for (int y=0,hy=-(dst->height/2);y<dst->height;y++,hy++)
  {
    for (int x=0,hx=-(dst->width/2);x<dst->width;x++,hx++)
    {
      float dx=fabsf(hx);
      float dy=fabsf(hy);
      float dist=1.0f-sqrtf(dx*dx+dy*dy)/12.0f;
      float power=is_wall(src,px+hx,py+hy)?0.4f:0.8f;
      unsigned scale=(unsigned)(255.0f*power*dist)&0xff;
      unsigned color=0xff000000;
      color|=(scale)<<16;
      color|=(scale)<<8;
      color|=(scale)<<0;
      dst->data[y*dst->width+x]=color;
      if (hy==0&&hx==0)
      {
        dst->data[y*dst->width+x]=0xff00ff00;
      }
    }
  }
}

void upload(const Bitmap* bitmap,int texture)
{
  bq_update_texture(texture,bitmap->width,bitmap->height,bitmap->data);
}
