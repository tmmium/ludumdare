// math.cc

#include <math.h>

const float kPI=3.14159265359f;
const float kPI2=kPI*2.0f;

inline float minf(const float a,const float b) {return a<b?a:b;}
inline float maxf(const float a,const float b) {return a>b?a:b;}
inline float signf(const float a) {return a<0.0f?-1.f:1.f;}

inline v2 operator+(const v2& l,const v2& r) {return {l.x+r.x,l.y+r.y};}
inline v2 operator-(const v2& l,const v2& r) {return {l.x-r.x,l.y-r.y};}
inline v2 operator*(const v2& l,const float r) {return {l.x*r,l.y*r};}
inline v2 operator/(const v2& l,const float r) {return {l.x/r,l.y/r};}

inline v3 operator+(const v3& l,const v3& r) {return {l.x+r.x,l.y+r.y,l.z+r.z};}
inline v3 operator-(const v3& l,const v3& r) {return {l.x-r.x,l.y-r.y,l.z-r.z};}
inline v3 operator*(const v3& l,const float r) {return {l.x*r,l.y*r,l.z*r};}

inline float length(const v3 a)
{
  return sqrtf(a.x*a.x+a.y*a.y+a.z*a.z);
}

inline float dot(const v3 a,const v3 b)
{
  return a.x*b.x+a.y*b.y+a.z*b.z;
}

inline v3 normalize(const v3 a)
{
  v3 r=a;
  float len=sqrtf(a.x*a.x+a.y*a.y+a.z*a.z);
  if (len>0.0f) 
  {
    r.x/=len;
    r.y/=len;
    r.z/=len;
  }
  return r;
}

inline v3 cross(const v3 a,const v3 b)
{
  v3 c;
  c.x=a.y*b.z-a.z*b.y;
  c.y=a.z*b.x-a.x*b.z;
  c.z=a.x*b.y-a.y*b.x;
  return c;
}

inline v3 transform_normal(const m4& m,const v3& v)
{
  v3 res;

  res.x=m.x.x*v.x+m.x.y*v.y+m.x.z*v.z;
  res.y=m.y.x*v.x+m.y.y*v.y+m.y.z*v.z;
  res.z=m.z.x*v.x+m.z.y*v.y+m.z.z*v.z;

  return res;
}

inline v4 operator*(const v4& l,const float r) {return {l.x*r,l.y*r,l.z*r,l.w*r};}
inline v4 operator/(const v4& l,const float r) {return {l.x/r,l.y/r,l.z/r,l.w/r};}
inline v4 uvcoords(const v4 uv,const float scale)
{
  return uv*scale;
}

inline m4 rotate(const v3& axis,const float radians)
{
  const float c=cosf(radians);
  const float s=sinf(radians);
  const float c1=1.0f-c;
  const float xx=axis.x*axis.x;
  const float xy=axis.x*axis.y;
  const float xz=axis.x*axis.z;
  const float yy=axis.y*axis.y;
  const float yz=axis.y*axis.z;
  const float zz=axis.z*axis.z;

  m4 res=bq_identity();

  res.x.x=xx*c1+c;
  res.x.y=xy*c1-axis.z*s;
  res.x.z=xz*c1+axis.y*s;

  res.y.x=xy*c1+axis.z*s;
  res.y.y=yy*c1+c;
  res.y.z=yz*c1-axis.x*s;

  res.z.x=xz*c1-axis.y*s;
  res.z.y=yz*c1+axis.x*s;
  res.z.z=zz*c1+c;

  return res;
}