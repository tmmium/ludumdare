// opengl.c

#define STB_IMAGE_IMPLEMENTATION
#define STB_PNG_ONLY
#include <stb_image.h>

static const f32 kSpriteSizeSquare  = 8.0f;
static const f32 kSpriteScaleFactor = 8.0f / 256.0f;

static const uint32_t kColorTable[COLOR_COUNT] =
{
   0xff000000, // COLOR_BLACK
   0xffffffff, // COLOR_WHITE
   0xff0000ff, // COLOR_RED
   0xff00ff00, // COLOR_GREEN
   0xffff0000, // COLOR_BLUE
   0xffff00ff, // COLOR_MAGENTA
   0xffffff00, // COLOR_YELLOW
   0xff00ffff, // COLOR_AQUA
   0xffaaaaaa, // COLOR_LIGHT_GRAY
   0xff40ff40, // COLOR_LIGHT_GREEN
   0xffff0088, // COLOR_PURPLE
   0xff0099ff, // COLOR_ORANGE
};

typedef struct 
{
   v2 position;
   v2 texcoord;
   v4 color;
} vertex_t;

typedef struct 
{
   int use_clear_color;
   v4 clear_color;
   m4 projection;
   uint32_t current_count;
   uint32_t max_count;
   uint32_t min_count;
   uint32_t capacity;
   vertex_t *base;
   vertex_t *at;
} render_buffer_t;

static GLuint
gl_create_texture(int width, int height, const void *data)
{
   GLuint texture = 0;

   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexImage2D(GL_TEXTURE_2D, 
                0, GL_RGBA, width, height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, 
                data);

   return texture;
}

static GLuint 
gl_create_texture_from_file(const char *filename)
{
   GLuint result = 0;

   int width = 0, height = 0, unused = 0;
   void *bitmap = stbi_load(filename, &width, &height, &unused, STBI_rgb_alpha);
   if (bitmap)
   {
      result = gl_create_texture(width, height, bitmap);
      stbi_image_free(bitmap);
   }

   return result;
}

static v4 
gl_position(int x_pos, int y_pos, int scale)
{
   const f32 x = x_pos;
   const f32 y = y_pos;
   const f32 s = scale;

   return (v4) { x, y, x + s * kSpriteSizeSquare, y + s * kSpriteSizeSquare, };
}

static v4
gl_texcoord(uint32_t index)
{
   uint32_t x = index % 32;
   uint32_t y = index / 32;

   return (v4) 
   { 
      x * kSpriteScaleFactor, 
      y * kSpriteScaleFactor, 
      x * kSpriteScaleFactor + kSpriteScaleFactor, 
      y * kSpriteScaleFactor + kSpriteScaleFactor,  
   };
}

static v4 
gl_color(uint32_t color_index)
{
   uint32_t color = kColorTable[color_index];

   v4 result = {0};

   result.x = ((color >> 0)  & 0xff) / 255.0f;
   result.y = ((color >> 8)  & 0xff) / 255.0f;
   result.z = ((color >> 16) & 0xff) / 255.0f;
   result.w = ((color >> 24) & 0xff) / 255.0f;

   return result;
}

static void 
gl_render_buffer_push(render_buffer_t *buffer, v4 pos, v4 tex, v4 color)
{
   vertex_t *vert = buffer->at;

   int index = 0;
   vert[index].position = (v2) { pos.x, pos.y };
   vert[index].texcoord = (v2) { tex.x, tex.y };
   vert[index].color = color;
   index++;

   vert[index].position = (v2) { pos.z, pos.y };
   vert[index].texcoord = (v2) { tex.z, tex.y };
   vert[index].color = color;
   index++;

   vert[index].position = (v2) { pos.z, pos.w };
   vert[index].texcoord = (v2) { tex.z, tex.w };
   vert[index].color = color;
   index++;

   vert[index].position = (v2) { pos.z, pos.w };
   vert[index].texcoord = (v2) { tex.z, tex.w };
   vert[index].color = color;
   index++;

   vert[index].position = (v2) { pos.x, pos.w };
   vert[index].texcoord = (v2) { tex.x, tex.w };
   vert[index].color = color;
   index++;

   vert[index].position = (v2) { pos.x, pos.y };
   vert[index].texcoord = (v2) { tex.x, tex.y };
   vert[index].color = color;
   index++;

   buffer->at += index;
}

static void 
gl_render_buffer_flush(render_buffer_t *buffer)
{
   const char *base = (const char *)buffer->base;
   glVertexPointer(2, GL_FLOAT, sizeof(vertex_t), (const GLvoid *)(base + offsetof(vertex_t, position)));
   glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), (const GLvoid *)(base + offsetof(vertex_t, texcoord)));
   glColorPointer(4, GL_FLOAT, sizeof(vertex_t), (const GLvoid *)(base + offsetof(vertex_t, color)));

   uint32_t primitive_count = (uint32_t)(buffer->at - buffer->base);
   glDrawArrays(GL_TRIANGLES, 0, primitive_count);
}

static void 
gl_render_buffer_reset(render_buffer_t *buffer)
{
   buffer->current_count = (buffer->at - buffer->base);
   buffer->max_count = max(buffer->max_count, buffer->current_count);
   buffer->min_count = min(buffer->min_count, buffer->current_count);
   buffer->at = buffer->base;
}

static void 
gl_render_commands(render_commands_t *commands, render_buffer_t *buffer)
{
   uint32_t command_count  = commands->at - commands->base;
   drawcmd_t *command      = commands->base;
   commands->at            = commands->base; 

   for (uint32_t command_index = 0;
        command_index < command_count;
        command_index++)
   {
      v4 pos   = gl_position(command->transform.x_pos, 
                             command->transform.y_pos, 
                             command->transform.scale + 1);
      v4 tex   = gl_texcoord(command->sprite.index);
      v4 color = gl_color(command->sprite.color);
      gl_render_buffer_push(buffer, pos, tex, color);

      command++;
   }

   buffer->use_clear_color = commands->use_clear_color;
   buffer->clear_color = commands->clear_color;
   buffer->projection = commands->projection;

   commands->use_clear_color = 0;
}

static void 
gl_render_buffer(render_buffer_t *buffer)
{
   v4 clear_color = buffer->clear_color;
   m4 projection = buffer->projection;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glMultMatrixf((const GLfloat* )&projection);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   if (buffer->use_clear_color)
   {
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   gl_render_buffer_flush(buffer);
   gl_render_buffer_reset(buffer);
}
