// atlas.cc

struct Atlas
{
  Bitmap bitmap;
  int count;
  v4* uvs;
};

int init(Atlas* atlas)
{
  atlas->count=0;
  atlas->uvs=NULL;
  return 1;
}

v4 atlas_get(Atlas* atlas,int index)
{
  if (!atlas||index<0||index>=atlas->count) {return {};}

  return {};
}
