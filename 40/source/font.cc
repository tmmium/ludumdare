// font.cc

#include <string.h>

struct Font
{
  float width;
  float height;
  v2 glyphs[64];
};

void init(Font* font)
{
  font->width=3.0f;
  font->height=5.0f;
  const float iw=1.0f/256.0f;
  for (int i=0;i<64;i++)
  {
    font->glyphs[i]={font->width*i,251.0f};
    font->glyphs[i].x*=iw;
    font->glyphs[i].y*=iw;
  }
}

void draw(Font* font,float x,float y,const char* str)
{
  v2 p[1024];
  v2 t[1024];
  float ox=x,w=font->width,h=font->height;
  float iw=w/256.0f,ih=h/256.0f;
  int len=(int)strlen(str),k=0;
  for (int i=0;i<len;i++)
  {
    char ch=str[i];
    if (ch=='\n') {x=ox; y+=h+2.0f; continue;}
    else if(ch==' ') {x+=w; continue;}

    int index=(int)ch-33;
    if (index<0||index>62) {index=31;}

    v2 r=font->glyphs[index];
    v4 s={r.x,r.y,r.x+iw,r.y+ih};

    p[k]={x,y};     t[k]={s.x,s.y}; k++;
    p[k]={x+w,y};   t[k]={s.z,s.y}; k++;
    p[k]={x+w,y+h}; t[k]={s.z,s.w}; k++;
    p[k]={x+w,y+h}; t[k]={s.z,s.w}; k++;
    p[k]={x,y+h};   t[k]={s.x,s.w}; k++;
    p[k]={x,y};     t[k]={s.x,s.y}; k++;

    x+=w+1.0f;
  }
  bq_render2d({0.1f,0.2f,0.3f,1.0f},k,p,t);
}
