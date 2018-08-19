// ludumdare42.c - "Running out of space" 

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>   // vsnprintf
#include <stdarg.h>  // va_list
//#include <string.h>  // strcpy, strcmp
#include <time.h>    // time
#include <math.h>    // fabsf

#include "interface.h"
#include "math.h"
#include "text.h"
#include "debug.h"
#include "sprite.h"

static const int kScreenWidth  = 240; // 540 / 4 
static const int kScreenHeight = 135; // 960 / 4

static f32 
get_dt();

#include "scene.c"
#include "hero.c"
#include "items.c"

typedef struct env_t env_t;
static env_t *env;

enum
{
   ENV_STATE_QUIT,
   ENV_STATE_MENU,
   ENV_STATE_PLAY,
   ENV_STATE_DEAD,
   ENV_STATE_TRANSITION,
};

struct env_t
{
   int state;
   int to_state;
   f32 transition_timer;
   f32 dt;
   input_t c_input;
   input_t p_input;
   scene_t scene;
   hero_t hero;
   backpack_t backpack;
};

static f32 
get_dt()
{
   return env->dt;
}

static void
mouse_scaled(input_t *input, v2 *mouse)
{
   mouse->x = input->position.x * 0.25f;
   mouse->y = input->position.y * 0.25f;
}

static void 
mouse_button_states(input_t *input, int *buttons)
{
   buttons[0] = input->buttons[0];
   buttons[1] = input->buttons[1];
}

static void
mouse_wheel_state(input_t *input, int *wheel)
{
   *wheel = input->wheel;
}

static void 
mouse_post_frame(input_t *input)
{
   env->p_input = env->c_input;

   mouse_scaled(input, &env->c_input.position);
   mouse_button_states(input, env->c_input.buttons);
   mouse_wheel_state(input, &env->c_input.wheel);
}

static int
mouse_button_down(int index)
{
   return env->c_input.buttons[index];
}

static int
mouse_button_down_once(int index)
{
   return env->c_input.buttons[index] && !env->p_input.buttons[index];
}

static int
mouse_wheel_delta()
{
   return env->c_input.wheel;
}

static void
env_init(allocator_t *allocator)
{
   random_init(0);

   env->state = ENV_STATE_PLAY;
   env->to_state = ENV_STATE_PLAY;
   env->transition_timer = 0.0f;
   env->dt = 1.0f / 60.0f;

   scene_init(&env->scene, allocator);
   hero_init(&env->hero, allocator);
   backpack_init(&env->backpack, allocator);
}

static int 
env_state_menu(input_t *input, render_commands_t *render_commands)
{
   const char *text = "-- Running out of space --";
   const int text_scale = 1;
   int text_x = text_width(text) * text_scale; 
   text_x = (kScreenWidth - text_x) / 2;
   int text_y = (kScreenHeight - 8) / 2;
   draw_text_shadowed(render_commands, 
                      COLOR_WHITE, 
                      COLOR_BLACK, 
                      text_x, 
                      text_y, 
                      text_scale, 
                      text);
   
   if (mouse_button_down_once(BUTTON_LEFT))
   {
      env->state = ENV_STATE_PLAY; 
   }
   else if (mouse_button_down_once(BUTTON_RIGHT))
   {
      env->state = ENV_STATE_QUIT;
   }

   return 1;
}

static int
env_state_play(input_t *input, render_commands_t *render_commands)
{
   scene_update(&env->scene, env->dt);
   scene_draw(&env->scene, render_commands);

   hero_update(&env->hero, env->dt);
   hero_draw(&env->hero, render_commands);

   backpack_update(&env->backpack, env->dt);
   backpack_draw(&env->backpack, render_commands);

   backpack_scroll_list(&env->backpack, -mouse_wheel_delta());

   if (mouse_button_down_once(BUTTON_RIGHT))
   {
      backpack_add_random_item(&env->backpack);
      //backpack_scroll_list(&env->backpack, -1);
   }

   if (mouse_button_down_once(BUTTON_LEFT))
   {
      //backpack_scroll_list(&env->backpack, 1);
   }

#if 0
   if (mouse_button_down_once(BUTTON_LEFT))
   {
      env->state = ENV_STATE_DEAD;
   }
   else if (mouse_button_down_once(BUTTON_RIGHT))
   {
      env->state = ENV_STATE_MENU;
   }
#endif

   return 1;
}

static int 
env_state_dead(input_t *input, render_commands_t *render_commands)
{
   const char *text = "DEAD";
   const int text_scale = 3;
   int text_x = text_width(text) * text_scale; 
   text_x = (kScreenWidth - text_x) / 2;
   int text_y = (kScreenHeight - 24) / 2;
   draw_text_shadowed(render_commands, 
                      COLOR_RED, 
                      COLOR_BLACK, 
                      text_x, 
                      text_y, 
                      text_scale, 
                      text);
   
   if (mouse_button_down_once(BUTTON_LEFT))
   {
      env->state = ENV_STATE_MENU;
   }

   return 1;
}

__declspec(dllexport) int 
game_update_and_render(allocator_t *allocator, input_t *input, 
                       render_commands_t *render_commands,
                       render_commands_t *debug_render_commands)
{
   if (!env)
   {
      env = (env_t *)qk_alloc(allocator, sizeof(env_t));
      env_init(allocator);
   }

   env->dt = input->dt;

   set_clear_color(render_commands, (v4) { 0.1f, 0.2f, 0.2f, 1.0f });
   set_projection(render_commands, orthographic(kScreenWidth, kScreenHeight));

   debug_init(debug_render_commands, kScreenWidth * 2, kScreenHeight * 2);
   debug_start();

   int result = 0;
   switch(env->state)
   {
      case ENV_STATE_MENU:
      {
         result = env_state_menu(input, render_commands);
      } break;
      case ENV_STATE_PLAY:
      {
         result = env_state_play(input, render_commands);
      } break;
      case ENV_STATE_DEAD:
      {
         result = env_state_dead(input, render_commands);
      } break;
   }

   debug_text("mouse x: %d y: %d", 
              (int)input->position.x,
              (int)input->position.y);
   mouse_post_frame(input);

   return result;
}
