// sprite.h

typedef enum
{
   SPRITE_GLYPH_LAST = 92,

   // interaction
   SPRITE_HOVER,
   SPRITE_SELECTED,
   SPRITE_RESERVED,
   
   // 
   SPRITE_SKY0, SPRITE_SKY1, SPRITE_SKY2,
   SPRITE_GRASS0, SPRITE_GRASS1, SPRITE_GRASS2,

   // 
   SPRITE_HERO0 = 128,
   SPRITE_HERO1,

} sprite_t;