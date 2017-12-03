// input.cc

#define VK_SHIFT          0x10
#define VK_ESCAPE         0x1B
#define VK_FORWARD        ((int)'W')
#define VK_BACKWARD       ((int)'S')
#define VK_STRAFE_LEFT    ((int)'A')
#define VK_STRAFE_RIGHT   ((int)'D')

enum InputState
{
  INPUT_STATE_MENU,
  INPUT_STATE_CAMERA,
};

struct Input
{
  InputState state;
  bool is_cursor_visible;
  bool was_escape_down;
  v2 mouse_position;
  v2 mouse_delta;
  bool mouse_buttons[2];
  float sensitivity;
  v2 inverse;
  float walk_speed;
  float run_speed;
};

void init(Input* input)
{
  input->state=INPUT_STATE_MENU;
  input->is_cursor_visible=true;
  input->was_escape_down=false;
  input->mouse_position=bq_mouse_position();
  input->mouse_delta={0.0f,0.0f};
  input->mouse_buttons[0]=bq_mouse_button(0);
  input->mouse_buttons[1]=bq_mouse_button(1);
  input->sensitivity=3.0f;
  input->inverse={0.022f,-0.022f};
  input->walk_speed=2.0f;
  input->run_speed=4.0f;
}

void process(Input* input)
{
  v2 mouse=bq_mouse_position();
  input->mouse_delta=mouse-input->mouse_position;
  input->mouse_position=mouse;
  input->mouse_buttons[0]=bq_mouse_button(0);
  input->mouse_buttons[1]=bq_mouse_button(1);

  if (!input->was_escape_down&&bq_keyboard(VK_ESCAPE))
  {
    input->was_escape_down=true;
    if (input->state==INPUT_STATE_MENU)
    {
      input->state=INPUT_STATE_CAMERA;
      input->is_cursor_visible=false;
    }
    else if(input->state==INPUT_STATE_CAMERA)
    {
      input->state=INPUT_STATE_MENU;
      input->is_cursor_visible=true;
    }
    bq_set_cursor(input->is_cursor_visible);
  }
  else if(input->was_escape_down&&!bq_keyboard(VK_ESCAPE))
  {
    input->was_escape_down=false;
  }

  if (!input->is_cursor_visible) 
  {
    bq_center_cursor();
  }
}

void controller(Input* input,Player* player,float dt)
{
  if (input->state!=INPUT_STATE_CAMERA) {return;}

  const float sensitivity=input->sensitivity*dt;
  if (fabsf(input->mouse_delta.x)>0.0f)
  {
    rotate_y(player,input->mouse_delta.x*sensitivity*input->inverse.x);
  }
  if (fabsf(input->mouse_delta.y)>0.0f)
  {
    rotate_x(player,input->mouse_delta.y*sensitivity*input->inverse.y);
  }

  bool moving=false;
  bool running=bq_keyboard(VK_SHIFT);
  const float speed=running?input->run_speed:input->walk_speed;
  const float movement=speed*dt;
  if (bq_keyboard(VK_FORWARD))      {forward(player, movement); moving=true;}
  if (bq_keyboard(VK_BACKWARD))     {forward(player, -movement); moving=true;}
  if (bq_keyboard(VK_STRAFE_LEFT))  {strafe(player,-movement); moving=true;}
  if (bq_keyboard(VK_STRAFE_RIGHT)) {strafe(player, movement); moving=true;}

  if (moving) 
  {
    const float bob_factor=running?0.05f:0.025f;
    player->timer+=dt;
    player->y_bob=bob_factor*sinf(player->timer*15.0f);
  }
  else 
  {
    player->timer=0.0f;
    player->y_bob*=0.9f;
  }
  correct(player,{0.0f,0.0f,0.0f});

  v3 x={1.0f,0.0f,0.0f};
  v3 y={0.0f,1.0f,0.0f};
  v3 z={0.0f,0.0f,1.0f};

  m4 ry=rotate(y,player->radians[1]);
  x=normalize(transform_normal(ry,x));
  z=normalize(transform_normal(ry,z));

  m4 rx=rotate(x,player->radians[0]);
  y=normalize(transform_normal(rx,y));
  z=normalize(transform_normal(rx,z));

  player->axis[0]=x;
  player->axis[1]=y;
  player->axis[2]=z;
}

void controller(Input* input,Camera* camera,Player* player)
{
  camera->radians[0]=player->radians[0];
  camera->radians[1]=player->radians[1];
  camera->radians[2]=player->radians[2];
  camera->position=player->position;
  update(camera);
}