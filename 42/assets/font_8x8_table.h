// font_8x8_table.h

typedef struct 
{
   char x, y, w, h;
} glyph_t;

static const glyph_t kGlyphTable[92] = 
{
   { 0, }, 
};

static const int kGlyphKerningTable[92]   = 
{
// !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  @  
   2, 4, 6, 5, 5, 6, 2, 3, 3, 4, 4, 2, 4, 2, 4, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 3, 5, 3, 5, 7,
// A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  
   5, 5, 4, 5, 4, 4, 5, 5, 4, 5, 5, 4, 6, 5, 5, 5, 5, 5, 5, 4, 5, 5, 6, 5, 5, 4, 3, 4, 3, 4, 5, 3,
// a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }
   5, 5, 4, 5, 5, 4, 5, 5, 2, 3, 5, 2, 6, 5, 5, 5, 5, 4, 5, 4, 5, 5, 6, 4, 5, 5, 4, 2, 
};
