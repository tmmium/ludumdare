// font.cc

struct Font
{
  int texture;
  float width;
  float height;
  v2 glyphs[64];
};

bool init(Font* font,const char* filename)
{
  Bitmap bitmap;
  if (!init(&bitmap,filename)) {return false;}
  int texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  int width=bitmap.width;
  int height=bitmap.height;
  destroy(&bitmap);
  if (texture==0) {return false;}

  font->texture=texture;
  font->width=3.0f;
  font->height=5.0f;
  const float iw=1.0f/(float)width;
  const float ih=1.0f/(float)height;
  // todo: make all 'fonts' use same layout
  for (int y=0,i=0;y<3;y++)
  {
    for (int x=0;x<21;x++)
    {
      float s=(x*3.0f)*iw;
      float t=(y*5.0f)*ih;
      font->glyphs[i++]={s,t};
    }
  }

  return true;
}

#define MAX_VERTICES_FONT 1024
void draw(const Font* font,const v2 position,const v4 color,const char* str)
{
  v2 p[MAX_VERTICES_FONT];
  v2 t[MAX_VERTICES_FONT];
  float x=position.x;
  float y=position.y;
  float ox=x,w=font->width,h=font->height;
  float iw=w/64.0f,ih=h/64.0f;
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

  bq_bind_texture(font->texture);
  bq_render2d(color,k,p,t);
}

float text_width(const Font* font,const char* str)
{
  int len=(int)strlen(str);
  float res=0.0f;
  for (int i=0;i<len;i++)
  {
    char ch=str[i];
    if (ch=='\n') 
      res=0; 
    else if(ch==' ') 
      res+=font->width; 
    else 
      res+=font->width+1;
  }
  return res;
}

float text_height(const Font* font,const char* str)
{
  int len=(int)strlen(str);
  const float line_height=font->height+2.0f;
  float res=line_height;
  for (int i=0;i<len;i++)
  {
    char ch=str[i];
    if (ch=='\n') 
      res+=line_height;
  }
  return res;
}

v2 text_offset(const Font* font,const v2 dim,const char* text)
{
  v2 res;
  res.x=ceilf((dim.x-text_width(font,text))*0.5f);
  res.y=ceilf((dim.y-text_height(font,text))*0.5f)+1.0f;
  return res;
}

#define MAX_FONT_COUNT 8
struct FontCache
{
  int capacity;
  int count;
  Font fonts[MAX_FONT_COUNT];
};

bool init(FontCache* cache)
{
  cache->capacity=MAX_FONT_COUNT;
  cache->count=0;
  return true;
}

Font* allocate(FontCache* cache)
{
  assert(cache->count<cache->capacity);
  Font* result=cache->fonts+cache->count++;
  return result;
}
