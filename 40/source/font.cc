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

void draw(Font* font,float x,float y,const v4 color,const char* str)
{
  v2 p[1024];
  v2 t[1024];
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
