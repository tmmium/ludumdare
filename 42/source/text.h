// text.h

#define STRING_BUFFER_SIZE 2048

static const int kSpaceAdvanceX           = 4;
static const int kNewLineAdvanceY         = 8;
static const int kGlyphKerningTable[92]   = 
{
// !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  @  
   2, 4, 6, 5, 5, 6, 2, 3, 3, 4, 4, 2, 4, 2, 4, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 3, 5, 3, 5, 7,
// A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  
   5, 5, 4, 5, 4, 4, 5, 5, 4, 5, 5, 4, 6, 5, 5, 5, 5, 5, 5, 4, 5, 5, 6, 5, 5, 4, 3, 4, 3, 4, 5, 3,
// a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }
   5, 5, 4, 5, 5, 4, 5, 5, 2, 3, 5, 2, 6, 5, 5, 5, 5, 4, 5, 4, 5, 5, 6, 4, 5, 5, 4, 2, 
};

static int 
text_width_va(const char *format, va_list args)
{
   char string[STRING_BUFFER_SIZE];
   int string_len = vsnprintf(string, STRING_BUFFER_SIZE, format, args);

   int first_valid_character = (int)'!';
   int last_valid_character = (int)'}';
   int invalid_character_index = (int)'?' - first_valid_character;

   int x = 0;

   for (int character_index = 0;
        character_index < string_len;
        character_index++)
   {
      int character = (int)string[character_index];
      if (character == ' ')
      {
         x += kSpaceAdvanceX;
         continue;
      } 
      else if (character == '\n')
      {
         x = 0;
         continue;
      }

      int glyph_index = invalid_character_index;
      if (character >= first_valid_character && character <= last_valid_character)
         glyph_index = character - first_valid_character;

      x += kGlyphKerningTable[glyph_index];
   }

   return x;
}

static int 
text_width(const char *format, ...)
{
   va_list args;

   va_start(args, format);
   int result = text_width_va(format, args);
   va_end(args);

   return result;
}

static void 
draw_text_va(render_commands_t *render_commands, uint32_t color, 
             int xx, int yy, int scale, const char *format, va_list args)
{
   char string[STRING_BUFFER_SIZE];
   int string_len = vsnprintf(string, STRING_BUFFER_SIZE, format, args);

   int first_valid_character = (int)'!';
   int last_valid_character = (int)'}';
   int invalid_character_index = (int)'?' - first_valid_character;

   int x = xx;
   int y = yy;
   
   for (int character_index = 0;
        character_index < string_len;
        character_index++)
   {
      int character = (int)string[character_index];
      if (character == ' ')
      {
         x += kSpaceAdvanceX * scale;
         continue;
      } 
      else if (character == '\n')
      {
         x = xx;
         y += kNewLineAdvanceY * scale;
         continue;
      }

      int glyph_index = invalid_character_index;
      if (character >= first_valid_character && character <= last_valid_character)
         glyph_index = character - first_valid_character;
      
      push_command(render_commands, glyph_index, color, x, y, scale);

      x += kGlyphKerningTable[glyph_index] * scale;
   }
}

static void
draw_text(render_commands_t *render_commands, uint32_t color, 
          int x, int y, int scale, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   draw_text_va(render_commands, color, x, y, scale, format, args);
   va_end(args);
}

static void 
draw_text_shadowed(render_commands_t *render_commands, uint32_t fg_color, 
                   uint32_t bg_color, int x, int y, int scale, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   draw_text_va(render_commands, bg_color, x + 1, y + 1, scale, format, args);
   draw_text_va(render_commands, fg_color, x    , y    , scale, format, args);
   va_end(args);
}
