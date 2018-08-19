// interface.h

#include <stdint.h>

#define ASSERT(x) if (!(x)) {*(int *)0=0;}
#define ARRAYCOUNT(x) (sizeof(x) / sizeof(x[0]))

typedef float f32;
typedef struct v2 { f32 x, y;       } v2;
typedef struct v3 { f32 x, y, z;    } v3;
typedef struct v4 { f32 x, y, z, w; } v4;
typedef struct m4 { v4 x, y, z, w;  } m4;

typedef struct
{
   int size;
   char *base;
   char *at;
} allocator_t;

void *qk_alloc(allocator_t *allocator, int size)
{
   ASSERT((allocator->at - allocator->base) < allocator->size);

   void *result = allocator->at;
   allocator->at += size;

   return result;
}

typedef enum 
{
   BUTTON_LEFT,
   BUTTON_RIGHT,
} mouse_button_t;

typedef struct 
{
   f32 dt;
   v2 position;
   int buttons[2];
   int wheel;
} input_t;

typedef enum
{
   COLOR_BLACK,
   COLOR_WHITE,
   COLOR_RED,
   COLOR_GREEN,
   COLOR_BLUE,
   COLOR_MAGENTA,
   COLOR_YELLOW,
   COLOR_AQUA,
   COLOR_LIGHT_GRAY,
   COLOR_LIGHT_GREEN,
   COLOR_PURPLE,
   COLOR_ORANGE,
   COLOR_COUNT,
} color_t;

typedef struct 
{
   struct 
   {
      uint32_t color : 8;
      uint32_t index : 10;
   } sprite;
   struct 
   {
      int32_t x_pos : 10;
      int32_t y_pos : 10;
      int32_t scale : 4;
   } transform;
} drawcmd_t;

typedef struct 
{
   int use_clear_color;
   v4 clear_color;
   m4 projection;
   uint32_t capacity;
   drawcmd_t *base;
   drawcmd_t *at;
} render_commands_t;

void set_clear_color(render_commands_t *render_commands, v4 clear_color)
{
   render_commands->use_clear_color = 1;
   render_commands->clear_color = clear_color;
}

void set_projection(render_commands_t *render_commands, m4 projection)
{
   render_commands->projection = projection;
}

void push_command(render_commands_t *render_commands, uint32_t index, 
                  color_t color, int x_pos, int y_pos, int scale)
{
   if (scale == 0)
      scale = 1;

   drawcmd_t command = {0};

   command.sprite.index = index;
   command.sprite.color = color;
   command.transform.x_pos = x_pos;
   command.transform.y_pos = y_pos;
   command.transform.scale = scale - 1;

   *render_commands->at++ = command;
}

