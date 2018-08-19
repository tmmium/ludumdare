// hero.c

typedef struct
{
   int x_pos;
   int y_pos;
   f32 timer;
   f32 y_offset;
   int sprite_index;
} hero_t;

static void
hero_init(hero_t *hero, allocator_t *allocator)
{
   hero->timer = 0.0f;
   hero->x_pos = 40;
   hero->y_pos = 24;
   hero->y_offset = 0;
   hero->sprite_index = SPRITE_HERO1;
}

static void 
hero_update(hero_t *hero, const f32 dt)
{
   hero->timer += dt;
   hero->y_offset = fabsf(sinf(hero->timer * 10.0f)) * 2;
}

static void 
hero_draw(hero_t *hero, render_commands_t *render_commands)
{
   int y_offset = (int)(hero->y_offset + 0.5f);
   push_command(render_commands, 
                hero->sprite_index, 
                COLOR_WHITE, 
                hero->x_pos,
                hero->y_pos - y_offset,
                1);
}
