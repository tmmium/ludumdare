// input.cc

#define VK_SHIFT          0x10
#define VK_ESCAPE         0x1B
#define VK_F1             0x70
#define VK_FORWARD        ((int)'W')
#define VK_BACKWARD       ((int)'S')
#define VK_STRAFE_LEFT    ((int)'A')
#define VK_STRAFE_RIGHT   ((int)'D')

struct Mouse
{
  v2 look_delta;
  v2 position;
  bool buttons[2];
  bool buttons_once[2];
  float sensitivity;
  v2 inverse;
};

void init(Mouse* mouse)
{
  mouse->position=bq_mouse_position();
  mouse->look_delta={0.0f,0.0f};
  mouse->buttons[0]=bq_mouse_button(0);
  mouse->buttons[1]=bq_mouse_button(1);
  mouse->buttons_once[0]=false;
  mouse->buttons_once[1]=false;
  mouse->sensitivity=3.0f;
  mouse->inverse={0.022f,-0.022f};
}

void process(Mouse* mouse)
{
  v2 pos=bq_mouse_position();
  mouse->look_delta=pos-mouse->position;
  mouse->position=pos;
  bool is_left_button_down=bq_mouse_button(0);
  bool is_right_button_down=bq_mouse_button(1);
  mouse->buttons_once[0]=!mouse->buttons[0]&&is_left_button_down;
  mouse->buttons_once[1]=!mouse->buttons[1]&&is_right_button_down;
  mouse->buttons[0]=is_left_button_down;
  mouse->buttons[1]=is_right_button_down;
}

void mouse_sensitivity(Mouse* mouse,float sensitivity)
{
  mouse->sensitivity=sensitivity;
}

void mouse_increment_sensitivity(Mouse* mouse,float sensitivity)
{
  mouse->sensitivity+=sensitivity;
}

void mouse_decrement_sensitivity(Mouse* mouse,float sensitivity)
{
  mouse->sensitivity-=sensitivity;
  mouse->sensitivity=maxf(mouse->sensitivity,0.2f);
}

void invert_mouse_axis_y(Mouse* mouse)
{
  mouse->inverse.y=-mouse->inverse.y;
}

bool is_mouse_axis_y_inverted(Mouse* mouse)
{
  return mouse->inverse.y<0.0f?true:false;
}

struct Keyboard
{
  bool escape[2];
  bool boost;
  bool forward;
  bool backward;
  bool left;
  bool right;
};

void init(Keyboard* keyboard)
{
  keyboard->escape[0]=false;
  keyboard->escape[1]=false;
  keyboard->boost=false;
  keyboard->forward=false;
  keyboard->backward=false;
  keyboard->left=false;
  keyboard->right=false;
}

void process(Keyboard* keyboard)
{
  keyboard->escape[1]=keyboard->escape[0];
  keyboard->escape[0]=bq_keyboard(VK_ESCAPE);
  keyboard->boost=bq_keyboard(VK_SHIFT);
  keyboard->forward=bq_keyboard(VK_FORWARD);
  keyboard->backward=bq_keyboard(VK_BACKWARD);
  keyboard->left=bq_keyboard(VK_STRAFE_LEFT);
  keyboard->right=bq_keyboard(VK_STRAFE_RIGHT);
}

struct Input
{
  bool is_cursor_visible;
  Mouse mouse;
  Keyboard keyboard;
};

void init(Input* input)
{
  input->is_cursor_visible=true;
  init(&input->mouse);
  init(&input->keyboard);  
}

void set_cursor_visible(Input* input,bool state)
{
  if (input->is_cursor_visible==state) {return;}
  input->is_cursor_visible=state;
  bq_set_cursor(input->is_cursor_visible);
}

void process(Input* input,GameState state)
{
  process(&input->mouse);
  process(&input->keyboard);
  if (!input->is_cursor_visible) 
  {
    bq_center_cursor();
  }
}

bool is_left_down(const Input* input)
{
  return input->mouse.buttons[0];
}

bool is_right_down(const Input* input)
{
  return input->mouse.buttons[1];
}

bool is_left_down_once(const Input* input)
{
  return input->mouse.buttons_once[0];
}

bool is_right_down_once(const Input* input)
{
  return input->mouse.buttons_once[1];
}

bool was_escape_down_once(const Input* input)
{
  return input->keyboard.escape[0]&&!input->keyboard.escape[1];
}
