// world.cc

struct Map
{
  v3 spawn;
  v3 finish;
  Mesh mesh;
  Bitmap collision;
  int num_tiles;
  int num_entities;
};

bool init(MeshCache* cache,const char* filename,Map* map)
{
  Bitmap collision;
  if (!init(&collision,filename)) {return false;}

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

  Mesh mesh;
  build_world_mesh(cache,&mesh,&collision);

  map->mesh=mesh;
  map->spawn=spawn;
  map->finish=finish;
  map->collision=collision;
  map->num_tiles=num_tiles;
  map->num_entities=num_entities;

  return true;
}

void draw(const Map* map)
{
  draw(&map->mesh,{0.0f,0.0f,0.0f},0.0f);
}

struct World
{
  int world_texture;
  int props_texture;
  Mesh meshes[ENTITY_TYPE_COUNT];
  // entity_cache
  int num_entities;
  Entity* entities;
  Map map;
};

void reset(World* world)
{
  for (int i=0,e=world->num_entities;i<e;i++)
  {
    reset(world->entities+i);
  }
}

bool init(World* world,MeshCache* cache,const char* mapname)
{
  Bitmap bitmap;
  if (!init(&bitmap,"assets/world.png")) {return false;}
  int world_texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  destroy(&bitmap);
  if (world_texture==0) {return false;}

  if (!init(&bitmap,"assets/props.png")) {return false;}
  int props_texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);
  destroy(&bitmap);
  if (props_texture==0) {return false;}

  Map map;
  if (!init(cache,mapname,&map)) {return false;}

  Entity* entities=(Entity*)malloc(sizeof(Entity)*map.num_entities);
  int num_entities=0;

  for (int y=0;y<map.collision.height;y++)
  {
    for (int x=0;x<map.collision.width;x++)
    {
      unsigned pixel=pixel_at(&map.collision,x,y);
      if (is_entity_type(pixel,ENTITY_INVALID)) {continue;}

      if (is_entity_type(pixel,ENTITY_FINISH))
      {
        init(entities+num_entities,ENTITY_FINISH,{x+0.5f,0.0f,y+0.5f});
        num_entities++;
      }
      else if(is_entity_type(pixel,ENTITY_PICKUP))
      {
        init(entities+num_entities,ENTITY_PICKUP,{x+0.5f,0.0f,y+0.5f});
        num_entities++;
      }
      else if(is_entity_type(pixel,ENTITY_SPIKE))
      {
        init(entities+num_entities,ENTITY_SPIKE,{x+0.5f,0.0f,y+0.5f});
        num_entities++;
      }
    }
  }

  world->world_texture=world_texture;
  world->props_texture=props_texture;
  build_finish_mesh(cache,world->meshes+ENTITY_FINISH);
  build_pickup_mesh(cache,world->meshes+ENTITY_PICKUP);
  build_spike_mesh(cache,world->meshes+ENTITY_SPIKE);
  world->num_entities=num_entities;
  world->entities=entities;
  world->map=map;

  return true;
}

void contain(Map* map,Player* player)
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
  check[0]=is_wall(&map->collision,x-1,y-1);
  check[1]=is_wall(&map->collision,x  ,y-1);
  check[2]=is_wall(&map->collision,x+1,y-1);
  check[3]=is_wall(&map->collision,x-1,y  );
  check[4]=is_wall(&map->collision,x  ,y  );
  check[5]=is_wall(&map->collision,x+1,y  );
  check[6]=is_wall(&map->collision,x-1,y+1);
  check[7]=is_wall(&map->collision,x  ,y+1);
  check[8]=is_wall(&map->collision,x+1,y+1);

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

bool is_goal_reached(const v3 finish,const v3 position)
{
  v3 diff=finish-position;
  float dist=length(diff);
  if (dist<0.7f) {return true;}
  return false;
}

// todo: move to entity
void update(World* world,Player* player,const Audio* audio,float dt)
{
  const v3 pp=player->local_position;
  for (int i=0,e=world->num_entities;i<e;i++)
  {
    Entity* entity=world->entities+i;
    if (!entity->active) {continue;}
    if (entity->type==ENTITY_PICKUP)
    {
      entity->timer+=dt;
      entity->y_bob=0.2f+0.01f*sinf(entity->timer*10.0f);
      
      float dist=length(pp-entity->position);
      if (dist<0.4f)
      {
        entity->active=false;
        player->score+=1;
        if (player->score>0&&player->score%20==0)
        {
          player->health++;
          play(audio,SOUND_BONUS,0.15f);          
        }
        else 
          play(audio,SOUND_PICKUP,0.15f);
      }
    }
    else if(entity->type==ENTITY_SPIKE)
    {
      if (entity->triggered)
      {
        entity->y_bob=0.0f;
      }
      else 
      {
        entity->y_bob=-0.25f;
      }

      float dist=length(pp-entity->position);
      if (dist<0.3f)
      {
        if (!entity->triggered)
        {
          entity->triggered=true;
          play(audio,SOUND_SPIKE,0.2f);
          take_damage(player);
        }
      }
      else 
      {
        entity->triggered=false;
      }
    }
  }
}

void draw(const World* world,const Camera* camera,GameState state)
{
  if (state==GAME_STATE_LOADING) {return;}

  bq_prepare3d();
  bq_enable_fog({0,0,0,1},0.5f,1.0f,10.0f);
  bq_projection(camera->proj);
  bq_view(camera->view);

  bq_bind_texture(world->world_texture);
  draw(&world->map);

  bq_bind_texture(world->props_texture);
  for (int i=0;i<world->num_entities;i++)
  {
    const Entity* entity=world->entities+i;
    if (!entity->active) {continue;}
    const Mesh* mesh=world->meshes+entity->type;
    const v3 position=entity_position(entity);
    const float rotation=entity_rotation(entity);
    draw(mesh,position,rotation);
  }
}

