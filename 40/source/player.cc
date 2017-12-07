// player.cc

struct Player
{
  int score;
  int health;
  float height;
  float radius;
  float x_limit;
  float timer;
  float y_bob;
  float step;
  float walk_speed;
  float run_speed;
  v3 local_position;
  v3 position;
  v3 axis[3];
  v3 radians;
};

void correct(Player* player,v3 offset)
{
  player->local_position=player->local_position+offset;
  v3 bob={0.0f,player->height+player->y_bob,0.0f};
  player->position=player->local_position+bob;
}

void reset(Player* player,v3 position)
{
  player->score=0;
  player->health=3;

  player->timer=0.0f;
  player->y_bob=0.0f;
  player->step=0.0f;
  player->walk_speed=2.0f;
  player->run_speed=4.0f;
  player->local_position=position;
  player->position=position;
  player->axis[0]={1.f,0.f,0.f};
  player->axis[1]={0.f,1.f,0.f};
  player->axis[2]={0.f,0.f,1.f};
  player->radians={0.0f,0.0f,0.0f};
  correct(player,{0.0f,0.0f,0.0f});
}

void init(Player* player,v3 position)
{
  player->height=0.5f;
  player->radius=0.25f;
  player->x_limit=kPI*0.49f;
  reset(player,position);
}

void forward(Player* player,float amount)
{
  v3 axis=player->axis[2];
  axis.y=0.0f;
  axis=normalize(axis);
  player->local_position=player->local_position+axis*amount;
}

void strafe(Player* player,float amount)
{
  player->local_position=player->local_position+player->axis[0]*amount;
}

void rotate_x(Player* player,float amount)
{
  const float mm=player->x_limit;
  player->radians.x+=amount;
  player->radians.x=maxf(-mm,minf(player->radians.x,mm));
}

void rotate_y(Player* player,float amount)
{
  player->radians.y+=amount;
  if (player->radians.y>kPI2) {player->radians.y-=kPI2;}
  if (player->radians.y<kPI2) {player->radians.y+=kPI2;}
}

void controller(Player* player,const Input* input,const Audio* audio,float dt)
{
  if (input->is_cursor_visible) {return;}

  const Mouse* mouse=&input->mouse;
  const float sens=mouse->sensitivity*dt;
  const v2 look_delta=mouse->look_delta;
  const v2 look_inverse=mouse->inverse;
  if (fabsf(look_delta.x)>0.0f)
  {
    rotate_y(player,look_delta.x*sens*look_inverse.x);
  }
  if (fabsf(look_delta.y)>0.0f)
  {
    rotate_x(player,look_delta.y*sens*look_inverse.y);
  }

  const Keyboard* keyboard=&input->keyboard;
  bool moving=false;
  bool running=keyboard->boost;
  const float speed=running?player->run_speed:player->walk_speed;
  const float movement=speed*dt;
  if (keyboard->forward)      {forward(player, movement); moving=true;}
  if (keyboard->backward)     {forward(player, -movement); moving=true;}
  if (keyboard->left)         {strafe(player,-movement); moving=true;}
  if (keyboard->right)        {strafe(player, movement); moving=true;}

  if (moving) 
  {
    const float bob_factor=running?0.05f:0.025f;
    player->timer+=dt;
    player->y_bob=bob_factor*sinf(player->timer*15.0f);
    player->step+=dt;
    if (player->step>0.42f)
    {
      player->step=0.0f;
      play(audio,SOUND_STEP,0.2f);
    }
  }
  else 
  {
    player->timer=0.0f;
    player->y_bob*=0.9f;
    player->step=0.42f;
  }
  correct(player,{0.0f,0.0f,0.0f});

  v3 x={1.0f,0.0f,0.0f};
  v3 y={0.0f,1.0f,0.0f};
  v3 z={0.0f,0.0f,1.0f};

  m4 ry=rotate(y,player->radians.y);
  x=normalize(transform_normal(ry,x));
  z=normalize(transform_normal(ry,z));

  m4 rx=rotate(x,player->radians.x);
  y=normalize(transform_normal(rx,y));
  z=normalize(transform_normal(rx,z));

  player->axis[0]=x;
  player->axis[1]=y;
  player->axis[2]=z;
}

void take_damage(Player* player)
{
  player->health--;
}

bool is_alive(const Player* player)
{
  return player->health>0;
}

int calculate_score(const Player* player)
{
  int score=player->score*2;
  int health=player->health;
  if (health<=0) {return -score*10;}
  return (1000+score*10+health*20);
}
