// debug.h

typedef struct 
{
   render_commands_t *render_commands;
   int width;
   int height;
   int x, y;
} debug_t;

static debug_t debug_;
static debug_t *debug = &debug_;

static void 
debug_init(render_commands_t *render_commands, int width, int height)
{
   debug->render_commands = render_commands;
   debug->width = width;
   debug->height = height;
   
   set_projection(render_commands, orthographic(width, height));
}

static void
debug_start()
{
   debug->x = 2;
   debug->y = 2;
}

static void
debug_text(const char *format, ...)
{
   va_list args;

   va_start(args, format);
   draw_text_va(debug->render_commands, COLOR_MAGENTA, 
                debug->x, debug->y, 1, format, args);
   va_end(args);

   debug->y += kNewLineAdvanceY;
}
