// world.cc

struct World
{
  m4 perspective;
  Camera camera;
  Player player;

  v3 spawn_point;
  v3 goal_point;
  Bitmap collision;
  Bitmap bitmap;
  int texture;
  Mesh world;

  int num_entities;
  Entity* entities;
};

const unsigned ENTITYCOLOR[ENTITY_TYPE_COUNT]=
{
  0xff000000, // invalid
  0xff0000ff, // spawn
  0xff00ff00, // goal
};

static unsigned entity_color(EntityType type)
{
  return ENTITYCOLOR[type];
}

static bool is_entity_type(unsigned color,EntityType type)
{
  return ENTITYCOLOR[type]==color;
}

bool init(World* world,const char* filename,int screen_width,int screen_height)
{
  Bitmap bitmap;
  if (!init(&bitmap,filename)) {return false;}
  int texture=bq_create_texture(bitmap.width,bitmap.height,bitmap.data);

  const int MAX_WIDTH=bitmap.width;
  const int MAX_HEIGHT=bitmap.height>>1;

  int num_tiles=0;
  int num_entities=0;
  v3 spawn={},goal={};
  for (int y=0;y<MAX_HEIGHT;y++)
  {
    for (int x=0;x<MAX_WIDTH;x++)
    {
      unsigned pixel=pixel_at(&bitmap,x,y+64);
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
        goal={(float)x+0.5f,0.0f,(float)y+0.5f};
      }
    }
  }

  if (length(spawn)<0.1f||length(goal)<0.1f) {return false;}

  world->perspective=bq_perspective((float)screen_width/(float)screen_height,kPI*0.35f,0.25f,200.0f);

  init(&world->camera);
  update(&world->camera);
  init(&world->player,spawn);

  world->spawn_point=spawn;
  world->goal_point=goal;
  world->texture=texture;
  init(&world->world,num_tiles*6*6);

  world->bitmap=bitmap;
  world->collision.width=MAX_WIDTH;
  world->collision.height=MAX_HEIGHT;
  world->collision.data=bitmap.data+MAX_WIDTH*MAX_HEIGHT;
  build_world(&world->world,&world->collision);

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

void update(World* world,Input* input,float dt)
{
  controller(input,&world->player,dt);
  contain(world,&world->player);
  controller(input,&world->camera,&world->player);
}

bool finish(World* world)
{
  v3 diff=world->goal_point-world->player.position;
  float dist=length(diff);
  if (dist<0.7f) {return true;}
  return false;
}

void draw(World* world)
{
  bq_prepare3d();
  bq_enable_fog({0,0,0,1},0.3f,1.0f,10.0f);
  bq_projection(world->perspective);
  bq_view(world->camera.view);
  bq_bind_texture(world->texture);
  bq_render3d({1.0f,1.0f,1.0f,1.0f},world->world.count,world->world.positions,world->world.texcoords,nullptr);
  // objects
}

