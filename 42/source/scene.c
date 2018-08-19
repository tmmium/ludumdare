// scene.c

static const int kSceneWidth     = 32;
static const int kSceneHeight    = 5;
static const int kTileSize       = 8;

typedef struct 
{
   f32 offset_x;
} scene_t;

static void
scene_init(scene_t *scene, allocator_t *allocator)
{
   scene->offset_x =  0.0f;
}

static void 
scene_update(scene_t *scene, const f32 dt)
{
   scene->offset_x += dt * 8.0f;
   if (scene->offset_x > 16)
      scene->offset_x = 0.0f;
}

static void 
scene_draw(scene_t *scene, render_commands_t *render_commands)
{
   const int offset_x = ceilf(scene->offset_x);
   for (int yy = 0; yy < kSceneHeight; yy++)
   {
      int sprite_index = SPRITE_SKY0;
      if (yy + 1 == kSceneHeight)
         sprite_index = SPRITE_GRASS0;

      for (int xx = 0; xx < kSceneWidth; xx++)
      {
         push_command(render_commands, 
                      sprite_index, 
                      COLOR_WHITE, 
                      xx * kTileSize - offset_x,
                      yy * kTileSize,
                      1);
      }
   }
}
