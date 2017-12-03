// world.cc

struct World
{
  v3 spawn;
  v3 finish;
  Bitmap collision;
  Bitmap bitmap;
  int texture;
  Mesh world_mesh;
  Mesh entity_mesh[4];
  int num_entities;
  Entity* entities;
};

const unsigned ENTITYCOLOR[ENTITY_TYPE_COUNT]=
{
  0xff0000ff, // spawn
  0xff00ff00, // finish
  0xff00ffff, // pickup
  0xffff00ff, // spike
  0xff000000, // invalid
};

static unsigned entity_color(EntityType type)
{
  return ENTITYCOLOR[type];
}

static bool is_entity_type(unsigned color,EntityType type)
{
  return ENTITYCOLOR[type]==color;
}

void reset(World* world)
{
  for (int i=0,e=world->num_entities;i<e;i++)
  {
    reset(world->entities+i);
  }
}

bool init(World* world,const char* filename,int screen_width,int screen_height)
{
  Bitmap bitmap;
  if (!init(&bitmap,filename)) {return false;}
  int texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  if (texture==0) {return false;}

  Bitmap collision;
  collision.width=bitmap.width;
  collision.height=bitmap.height>>1;
  collision.data=bitmap.data+collision.width*collision.height;

  int num_tiles=0,num_entities=0;
  v3 spawn={},finish={};

  for (int y=0;y<collision.height;y++)
  {
    for (int x=0;x<collision.width;x++)
    {
      unsigned pixel=pixel_at(&collision,x,y);
      if (is_entity_type(pixel,ENTITY_INVALID)) {continue;}

      num_tiles++;
      if (is_entity_type(pixel,ENTITY_SPAWN)) 
      {
        num_entities++;
        spawn={(float)x+0.5f,0.0f,(float)y+0.5f};
      }
      else if (is_entity_type(pixel,ENTITY_FINISH))
      {
        num_entities++;
        finish={(float)x+0.5f,0.0f,(float)y+0.5f};
      }
      else if(is_entity_type(pixel,ENTITY_PICKUP))
      {
        num_entities++;
      }
      else if(is_entity_type(pixel,ENTITY_SPIKE))
      {
        num_entities++;
      }
    }
  }

  if (length(spawn)<0.1f||length(finish)<0.1f) {return false;}

  world->spawn=spawn;
  world->finish=finish;
  world->texture=texture;
  world->bitmap=bitmap;
  world->collision=collision;
  init(&world->world_mesh,num_tiles*6*6);
  build_world_mesh(&world->world_mesh,&world->collision);
  init(world->entity_mesh+ENTITY_SPAWN,6*6);
  build_spawn_mesh(world->entity_mesh+ENTITY_SPAWN);
  init(world->entity_mesh+ENTITY_FINISH,6*6);
  build_finish_mesh(world->entity_mesh+ENTITY_FINISH);
  init(world->entity_mesh+ENTITY_PICKUP,6*6);
  build_pickup_mesh(world->entity_mesh+ENTITY_PICKUP);
  init(world->entity_mesh+ENTITY_SPIKE,27);
  build_spike_mesh(world->entity_mesh+ENTITY_SPIKE);
  
  world->num_entities=0;
  world->entities=(Entity*)malloc(sizeof(Entity)*num_entities);
  
  for (int y=0;y<collision.height;y++)
  {
    for (int x=0;x<collision.width;x++)
    {
      unsigned pixel=pixel_at(&collision,x,y);
      if (is_entity_type(pixel,ENTITY_INVALID)) {continue;}

      if (is_entity_type(pixel,ENTITY_SPAWN)) 
      {
        init(world->entities+world->num_entities,ENTITY_SPAWN,{x+0.5f,0.0f,y+0.5f});
        world->num_entities++;
      }
      else if (is_entity_type(pixel,ENTITY_FINISH))
      {
        init(world->entities+world->num_entities,ENTITY_FINISH,{x+0.5f,0.0f,y+0.5f});
        world->num_entities++;
      }
      else if(is_entity_type(pixel,ENTITY_PICKUP))
      {
        init(world->entities+world->num_entities,ENTITY_PICKUP,{x+0.5f,0.0f,y+0.5f});
        world->num_entities++;
      }
      else if(is_entity_type(pixel,ENTITY_SPIKE))
      {
        init(world->entities+world->num_entities,ENTITY_SPIKE,{x+0.5f,0.0f,y+0.5f});
        world->num_entities++;
      }
    }
  }

  return true;
}

void contain(World* world,Player* player)
{
  // todo: out of bounds, kill!
  const float x=floor(player->local_position.x);
  const float y=floor(player->local_position.z);

  const v4 collision_grid[]=
  {
    {x-1,y-1, x  ,y  },
    {x  ,y-1, x+1,y  },
    {x+1,y-1, x+2,y  },
    {x-1,y  , x  ,y+1},
    {x  ,y  , x+1,y+1},
    {x+1,y  , x+2,y+1},
    {x-1,y+1, x  ,y+2},
    {x  ,y+1, x+1,y+2},
    {x+1,y+1, x+2,y+2},
  };

  bool check[9]={};
  check[0]=is_wall(&world->collision,x-1,y-1);
  check[1]=is_wall(&world->collision,x  ,y-1);
  check[2]=is_wall(&world->collision,x+1,y-1);
  check[3]=is_wall(&world->collision,x-1,y  );
  check[4]=is_wall(&world->collision,x  ,y  );
  check[5]=is_wall(&world->collision,x+1,y  );
  check[6]=is_wall(&world->collision,x-1,y+1);
  check[7]=is_wall(&world->collision,x  ,y+1);
  check[8]=is_wall(&world->collision,x+1,y+1);

  v3 correction={};  
  v2 position={player->local_position.x,player->local_position.z};
  const float radius=player->radius; 
  for (int i=0;i<9;i++)
  {
    if (check[i])
    {
      v4 box=collision_grid[i];
      
      // box metrics
      float box_half_width=(box.z-box.x)*0.5f;
      float box_half_height=(box.w-box.y)*0.5f;
      float box_center_x=box.x+box_half_width;
      float box_center_y=box.y+box_half_height;

      // nearest point in rectangle
      float dcx=position.x-box_center_x;
      float dcy=position.y-box_center_y;
      float nx=maxf(fabsf(dcx)-box_half_width,0.0f);
      float ny=maxf(fabsf(dcy)-box_half_height,0.0f);
      float dist=sqrtf(nx*nx+ny*ny);

      // check
      if (dist<radius)
      {
        float diff=radius-dist;
        if (fabsf(dcx)>fabsf(dcy))
        {
          float lx=diff*signf(dcx);
          if (fabsf(correction.x)<fabsf(lx)) 
          {
            correction.x=lx;
          }
        }
        else 
        {
          float lz=diff*signf(dcy);
          if (fabsf(correction.z)<fabsf(lz)) 
          {
            correction.z=lz;
          }
        }
      }
    }  
  }

  correct(player,correction);
}

bool is_goal_reached(const World* world,const Player* player)
{
  v3 diff=world->finish-player->position;
  float dist=length(diff);
  if (dist<0.7f) {return true;}
  return false;
}

void update(World* world,Player* player,const Audio* audio,float dt)
{
  const v3 pp=player->local_position;
  for (int i=0,e=world->num_entities;i<e;i++)
  {
    Entity* ent=world->entities+i;
    if (!ent->active) {continue;}
    if (ent->type==ENTITY_PICKUP)
    {
      ent->timer+=dt;
      ent->y_bob=0.2f+0.01f*sinf(ent->timer*10.0f);
      
      float dist=length(pp-ent->position);
      if (dist<0.4f)
      {
        ent->active=false;
        player->score+=1;
        if (player->score>10)
        {
          player->health++;
          player->score=0;
          play(audio,SOUND_BONUS,0.15f);          
        }
        else 
          play(audio,SOUND_PICKUP,0.15f);
      }
    }
    else if(ent->type==ENTITY_SPIKE)
    {
      if (ent->triggered)
      {
        ent->y_bob=0.0f;
      }
      else 
      {
        ent->y_bob=-0.25f;
      }

      float dist=length(pp-ent->position);
      if (dist<0.3f)
      {
        if (!ent->triggered)
        {
          ent->triggered=true;
          play(audio,SOUND_SPIKE,0.2f);
          take_damage(player);
        }
      }
      else 
      {
        ent->triggered=false;
      }
    }
  }
}

void draw(const World* world,const Camera* camera)
{
  bq_prepare3d();
  bq_enable_fog({0,0,0,1},0.5f,1.0f,10.0f);
  bq_projection(camera->proj);
  bq_view(camera->view);
  bq_bind_texture(world->texture);

  // objects
  draw(&world->world_mesh,{0.f,0.f,0.f},0.0f);
  for (int i=0;i<world->num_entities;i++)
  {
    const Entity* ent=world->entities+i;
    if (!ent->active) {continue;}
    const Mesh* mesh=world->entity_mesh+ent->type;
    draw(mesh,entity_position(ent),entity_rotation(ent));
  }
}

