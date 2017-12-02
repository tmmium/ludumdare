// player.cc

struct Player
{
  float radius;
  float x_limit;
  float timer;
  float y_bob;
  v3 local_position;
  v3 position;
  v3 axis[3];
  float radians[3];
};

void init(Player* player,const v3 position)
{
  player->radius=0.25f;
  player->x_limit=kPI*0.49f;
  player->timer=0.0f;
  player->y_bob=0.0f;
  player->local_position=position;
  player->position=position;
  player->axis[0]={1.f,0.f,0.f};
  player->axis[1]={0.f,1.f,0.f};
  player->axis[2]={0.f,0.f,1.f};
  player->radians[0]=0.0f;
  player->radians[1]=0.0f;
  player->radians[1]=0.0f;
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

void correct(Player* player,v3 correction)
{
  player->local_position=player->local_position+correction;
  
  v3 bob={0.0f,player->y_bob,0.0f};
  player->position=player->local_position+bob;
}
