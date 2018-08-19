// math.h

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

typedef struct 
{
  uint32_t z;
  uint32_t w;
} random_t;

static random_t _rnd = { 12345, 12345 };

static uint32_t 
random_init(uint32_t seed) 
{
	if (seed == 0) { seed = (uint32_t)time(0); }
  
  _rnd.z *= seed;
  _rnd.w *= seed;

	return seed;
}

static uint32_t 
random() 
{
  _rnd.z = 36969 * (_rnd.z & 65535) + (_rnd.z >> 16);
  _rnd.w = 18000 * (_rnd.w & 65535) + (_rnd.w >> 16);

  return (_rnd.z << 16) + _rnd.w;
}

static m4
orthographic(f32 width, f32 height)
{
   const f32 ww = width;
   const f32 hh = height;
   const f32 zn = -1.0f;
   const f32 zf = 1.0f;

   const f32 xx = 2.0f / ww;
   const f32 yy = 2.0f / -hh;
   const f32 zz = 1.0f / (zf - zn);
   const f32 wx = -1.0f; 
   const f32 wy = 1.0f; 
   const f32 wz = zn / (zn - zf); 

   m4 result =
   {
      xx  , 0.0f, 0.0f, 0.0f,
      0.0f, yy  , 0.0f, 0.0f,
      0.0f, 0.0f, zz  , 0.0f,
      wx  , wy  , wz  , 1.0f,
   };
   return result;
}
