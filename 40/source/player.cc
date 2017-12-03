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
  v3 local_position;
  v3 position;
  v3 axis[3];
  float radians[3];
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
  player->health=8;

  player->timer=0.0f;
  player->y_bob=0.0f;
  player->step=0.0f;
  player->local_position=position;
  player->position=position;
  player->axis[0]={1.f,0.f,0.f};
  player->axis[1]={0.f,1.f,0.f};
  player->axis[2]={0.f,0.f,1.f};
  player->radians[0]=0.0f;
  player->radians[1]=0.0f;
  player->radians[2]=0.0f;
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
  player->radians[0]+=amount;
  player->radians[0]=maxf(-mm,minf(player->radians[0],mm));
}

void rotate_y(Player* player,float amount)
{
  player->radians[1]+=amount;
  if (player->radians[1]>kPI2) {player->radians[1]-=kPI2;}
  if (player->radians[1]<kPI2) {player->radians[1]+=kPI2;}
}

void take_damage(Player* player)
{
  int damage=(player->score+2)/2;
  player->health-=damage;
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
