// world.cc

#pragma warning(push)
#pragma warning(disable : 4456) // declaration of hides previous local declaration
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

struct Mesh
{
  int capacity;
  int count;
  v3* positions;
  v2* texcoords;
};

void init(Mesh* mesh,int capacity)
{
  mesh->capacity=capacity;
  mesh->count=0;
  mesh->positions=(v3*)malloc(sizeof(v3)*capacity);
  mesh->texcoords=(v2*)malloc(sizeof(v2)*capacity);
}

void push(v3* p,v3 q0,v3 q1,v3 q2,v3 q3)
{
  p[0]=q0; p[1]=q1; p[2]=q2;
  p[3]=q2; p[4]=q3; p[5]=q0;
}

void push(v2* t,v2 min,v2 max)
{
  t[0]={min.x,min.y};
  t[1]={max.x,min.y};
  t[2]={max.x,max.y};
  t[3]={max.x,max.y};
  t[4]={min.x,max.y};
  t[5]={min.x,min.y};
}

bool wall(const unsigned* bitmask,const int x,const int y)
{
  if (x<0||x>64) {return true;}
  if (y<0||y>48) {return true;}
  return bitmask[64*y+x]==0xff000000;
}

#pragma warning(push)
#pragma warning(disable:4838)
void build(Mesh* mesh,const unsigned* bitmask)
{
  const v4 uv[]=
  {
    0.00f,0.0f, 0.25f,0.25f,
    0.25f,0.0f, 0.50f,0.25f,
    0.50f,0.0f, 0.75f,0.25f,
    0.75f,0.0f, 1.00f,0.25f
  };

  for (int y=0;y<48;y++)
  {
    for (int x=0;x<64;x++)
    {
      unsigned pixel=bitmask[64*y+x];
      if (pixel==0xff000000) {continue;}

      const v3 min={x,0,y};
      const v3 max={x+1,1,y+1};

      v3 cube[8]=
      {
        { min.x, max.y, min.z },
        { max.x, max.y, min.z },
        { max.x, min.y, min.z },
        { min.x, min.y, min.z },

        { min.x, max.y, max.z },
        { max.x, max.y, max.z },
        { max.x, min.y, max.z },
        { min.x, min.y, max.z },
      };

      // floor
      push(mesh->positions+mesh->count,cube[2],cube[3],cube[7],cube[6]);
      push(mesh->texcoords+mesh->count,{uv[0].x,uv[0].y},{uv[0].z,uv[0].w});
      mesh->count+=6;

      // roof
      push(mesh->positions+mesh->count,cube[1],cube[0],cube[4],cube[5]);
      push(mesh->texcoords+mesh->count,{uv[2].x,uv[2].y},{uv[2].z,uv[2].w});
      mesh->count+=6;

      if (wall(bitmask,x,y-1))
      {
        // north
        push(mesh->positions+mesh->count,cube[0],cube[1],cube[2],cube[3]);
        push(mesh->texcoords+mesh->count,{uv[1].x,uv[1].y},{uv[1].z,uv[1].w});
        mesh->count+=6;
      }
      if (wall(bitmask,x,y+1))
      {
        // south
        push(mesh->positions+mesh->count,cube[4],cube[5],cube[6],cube[7]);
        push(mesh->texcoords+mesh->count,{uv[1].x,uv[1].y},{uv[1].z,uv[1].w});
        mesh->count+=6;
      }
      if (wall(bitmask,x-1,y))
      {
        // east 
        push(mesh->positions+mesh->count,cube[4],cube[0],cube[3],cube[7]);
        push(mesh->texcoords+mesh->count,{uv[1].x,uv[1].y},{uv[1].z,uv[1].w});
        mesh->count+=6;
      }
      if (wall(bitmask,x+1,y))
      {
        // west
        push(mesh->positions+mesh->count,cube[1],cube[5],cube[6],cube[2]);
        push(mesh->texcoords+mesh->count,{uv[1].x,uv[1].y},{uv[1].z,uv[1].w});
        mesh->count+=6;
      }
    }
  }
}

struct World
{
  int texture;
  unsigned* bitmap;
  unsigned* bitmask;
  v3 spawn_point;
  Mesh mesh;
};

int init(World* world,const char* filename)
{
  int width,height,c;
  stbi_uc* bitmap=stbi_load(filename,&width,&height,&c,4);
  if (!bitmap) {return 0;}

  world->texture=bq_create_texture(width,height,bitmap);
  world->bitmap=(unsigned*)bitmap;
  world->bitmask=world->bitmap+1024;

  int num_tiles=0;
  for (int y=0;y<48;y++)
  {
    for (int x=0;x<64;x++)
    {
      unsigned pixel=world->bitmask[64*y+x];
      if (pixel==0xff000000) {continue;}
      num_tiles++;
      if (pixel==0xff00ff00) {world->spawn_point={(float)x+0.5f,0.65f,(float)y+0.5f};}
    }
  }

  init(&world->mesh,num_tiles*6*6);
  build(&world->mesh,world->bitmask);

  return 1;
}

void collision(World* world,Player* player)
{
  const unsigned* bitmask=world->bitmask;
  int x=player->local_position.x;
  int y=player->local_position.z;

  v4 mm[]=
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
  check[0]=wall(bitmask,x-1,y-1);
  check[1]=wall(bitmask,x  ,y-1);
  check[2]=wall(bitmask,x+1,y-1);
  check[3]=wall(bitmask,x-1,y  );
  check[4]=wall(bitmask,x  ,y  );
  check[5]=wall(bitmask,x+1,y  );
  check[6]=wall(bitmask,x-1,y+1);
  check[7]=wall(bitmask,x  ,y+1);
  check[8]=wall(bitmask,x+1,y+1);

  v3 correction={};  
  v2 position={player->local_position.x,player->local_position.z};
  const float radius=player->radius; 
  for (int i=0;i<9;i++)
  {
    if (check[i])
    {
      v4 box=mm[i];
      
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
#pragma warning(pop)

void draw(World* world)
{
  bq_bind_texture(world->texture);
  bq_render3d({1.0f,1.0f,1.0f,1.0f},world->mesh.count,world->mesh.positions,world->mesh.texcoords,nullptr);
}
