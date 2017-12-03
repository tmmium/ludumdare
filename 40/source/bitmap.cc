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

unsigned pixel_at(const Bitmap* bitmap,int x,int y)
{
  if (x<0||x>=bitmap->width) {return 0;}
  if (y<0||y>=bitmap->height) {return 0;}
  return bitmap->data[y*bitmap->width+x];
}
