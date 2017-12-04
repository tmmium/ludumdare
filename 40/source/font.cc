// font.cc

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
  const float iw=1.0f/64.0f;
  for (int y=0,i=0;y<3;y++)
  {
    for (int x=0;x<21;x++)
    {
      float s=(x*3.0f)*iw;
      float t=(y*5.0f)*iw;
      font->glyphs[i++]={s,t};
    }
  }
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

void draw_centered(const Font* font,const v2 dim,const v2 offset,const v4 color,const char* str)
{
  v2 position=text_offset(font,dim,str)+offset;
  v2 offseted=position;
  offseted.x+=1; offseted.y+=1;
  draw(font,offseted,{0.0f,0.0f,0.0f,1.0f},str);
  draw(font,position,color,str);
}
