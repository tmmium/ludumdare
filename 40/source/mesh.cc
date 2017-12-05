// mesh.cc

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
  bq_log("[game] mesh: capacity %d\n",capacity);
}

static void push(v3* p,v3 q0,v3 q1,v3 q2,v3 q3)
{
  p[0]=q0; p[1]=q1; p[2]=q2;
  p[3]=q2; p[4]=q3; p[5]=q0;
}

static void push3(v3* p,v3 q0,v3 q1,v3 q2)
{
  p[0]=q0; 
  p[1]=q1; 
  p[2]=q2;
}

static void push(v2* t,v2 min,v2 max)
{
  t[0]={min.x,min.y};
  t[1]={max.x,min.y};
  t[2]={max.x,max.y};
  t[3]={max.x,max.y};
  t[4]={min.x,max.y};
  t[5]={min.x,min.y};
}

static void push3(v2* t,v2 s0,v2 s1,v2 s2)
{
  t[0]=s0;
  t[1]=s1;
  t[2]=s2;
}

bool is_wall(const Bitmap* bitmap,const int x,const int y)
{
  if (x<0||x>=bitmap->width) {return true;}
  if (y<0||y>=bitmap->height) {return true;}
  return pixel_at(bitmap,x,y)==0xff000000;
}

void build_cube_mesh(Mesh* mesh,const v4 top,const v4 sides,const v3 min,const v3 max)
{
  const v3 cube[8]=
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

  // top
  push(mesh->positions+mesh->count,cube[1],cube[0],cube[4],cube[5]);
  push(mesh->texcoords+mesh->count,{top.x,top.y},{top.z,top.w}); 
  mesh->count+=6;
  // bottom  
  push(mesh->positions+mesh->count,cube[2],cube[3],cube[7],cube[6]);
  push(mesh->texcoords+mesh->count,{top.x,top.y},{top.z,top.w}); 
  mesh->count+=6;
  // north
  push(mesh->positions+mesh->count,cube[0],cube[1],cube[2],cube[3]);
  push(mesh->texcoords+mesh->count,{sides.x,sides.y},{sides.z,sides.w});
  mesh->count+=6;
  // south
  push(mesh->positions+mesh->count,cube[4],cube[5],cube[6],cube[7]);
  push(mesh->texcoords+mesh->count,{sides.x,sides.y},{sides.z,sides.w});
  mesh->count+=6;
  // east 
  push(mesh->positions+mesh->count,cube[4],cube[0],cube[3],cube[7]);
  push(mesh->texcoords+mesh->count,{sides.x,sides.y},{sides.z,sides.w});
  mesh->count+=6;
  // west
  push(mesh->positions+mesh->count,cube[1],cube[5],cube[6],cube[2]);
  push(mesh->texcoords+mesh->count,{sides.x,sides.y},{sides.z,sides.w});
  mesh->count+=6;
}

void build_spawn_mesh(Mesh* mesh)
{
  const v4 top=uvcoords({0.000f,0.250f,0.109375f,0.375f},1.0f);
  const v4 sides=uvcoords({0.109375f,0.250f,0.125f,0.375f},1.0f);
  const float Q=0.3f;
  const v3 min={-Q,0.00f,-Q};
  const v3 max={ Q,0.05f, Q};
  build_cube_mesh(mesh,top,sides,min,max);
}

void build_finish_mesh(Mesh* mesh)
{
  v4 top=uvcoords({16.f,32.f,30.f,48.f},1.0f/128.0f);
  v4 sides=uvcoords({30.f,32.f,32.f,48.f},1.0f/128.0f);
  const float Q=0.3f;
  const v3 min={-Q,0.00f,-Q};
  const v3 max={ Q,0.05f, Q};
  build_cube_mesh(mesh,top,sides,min,max);
}

void build_pickup_mesh(Mesh* mesh)
{
  const v4 top=uvcoords({32.f,32.f,46.f,48.f},1.0f/128.0f);
  const v4 sides=uvcoords({46.f,32.f,48.f,48.f},1.0f/128.0f);
  const float Q=0.1f;
  const v3 min={-Q,-Q,-Q*0.2f};
  const v3 max={ Q, Q, Q*0.2f};
  build_cube_mesh(mesh,top,sides,min,max);
}

void build_pyramid_mesh(Mesh* mesh,const v2* uvs,const v3* corners,const v3 offset={0.0f,0.0f,0.0f})
{
  push3(mesh->positions+mesh->count,corners[0]+offset,corners[1]+offset,corners[2]+offset);
  push3(mesh->texcoords+mesh->count,uvs[0],uvs[1],uvs[2]);
  mesh->count+=3;
  push3(mesh->positions+mesh->count,corners[0]+offset,corners[2]+offset,corners[3]+offset);
  push3(mesh->texcoords+mesh->count,uvs[0],uvs[1],uvs[2]);
  mesh->count+=3;
  push3(mesh->positions+mesh->count,corners[0]+offset,corners[3]+offset,corners[1]+offset);
  push3(mesh->texcoords+mesh->count,uvs[0],uvs[1],uvs[2]);
  mesh->count+=3;
}

void build_spike_mesh(Mesh* mesh)
{
  const v2 uvs[3]=
  {
    uvcoord({55.5f,32.0f},1.0f/128.0f),
    uvcoord({63.0f,47.0f},1.0f/128.0f),
    uvcoord({48.0f,47.0f},1.0f/128.0f),
  };
  const v3 offset[3]=
  {
    {  0.0f, 0.0f, 0.15f},
    { 0.15f, 0.0f,-0.15f},
    {-0.15f, 0.0f,-0.15f},
  };
  const v3 corners[4]=
  {
    { 0.0f, 0.3f, 0.0f },
    { 0.0f, 0.0f, 0.1f },
    { 0.1f, 0.0f,-0.1f },
    {-0.1f, 0.0f,-0.1f },
  };

  for (int i=0;i<3;i++)
  {
    build_pyramid_mesh(mesh,uvs,corners,offset[i]);
  }
}

void build_world_mesh(Mesh* mesh,const Bitmap* bitmap)
{
  // todo: calculate 4x4 matrix of uvs
  const v4 uvs[]=
  {
    0.00f,0.0f, 0.25f,0.25f, // floor
    0.25f,0.0f, 0.50f,0.25f, // wall
    0.50f,0.0f, 0.75f,0.25f, // ceiling
    0.75f,0.0f, 1.00f,0.25f  // end?
  };

  for (int y=0;y<bitmap->height;y++)
  {
    for (int x=0;x<bitmap->width;x++)
    {
      unsigned pixel=pixel_at(bitmap,x,y);
      if (pixel==0xff000000) {continue;}

      if (pixel==0xffff0000) 
      {
        // goal...
      }

      const v3 min={(float)x,0.0f,(float)y};
      const v3 max={x+1.0f,1.0f,y+1.0f};

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
      push(mesh->texcoords+mesh->count,{uvs[0].x,uvs[0].y},{uvs[0].z,uvs[0].w});
      mesh->count+=6;

      // roof
      push(mesh->positions+mesh->count,cube[1],cube[0],cube[4],cube[5]);
      push(mesh->texcoords+mesh->count,{uvs[2].x,uvs[2].y},{uvs[2].z,uvs[2].w});
      mesh->count+=6;

      if (is_wall(bitmap,x,y-1))
      {
        // north
        push(mesh->positions+mesh->count,cube[0],cube[1],cube[2],cube[3]);
        push(mesh->texcoords+mesh->count,{uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
        mesh->count+=6;
      }
      if (is_wall(bitmap,x,y+1))
      {
        // south
        push(mesh->positions+mesh->count,cube[4],cube[5],cube[6],cube[7]);
        push(mesh->texcoords+mesh->count,{uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
        mesh->count+=6;
      }
      if (is_wall(bitmap,x-1,y))
      {
        // east 
        push(mesh->positions+mesh->count,cube[4],cube[0],cube[3],cube[7]);
        push(mesh->texcoords+mesh->count,{uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
        mesh->count+=6;
      }
      if (is_wall(bitmap,x+1,y))
      {
        // west
        push(mesh->positions+mesh->count,cube[1],cube[5],cube[6],cube[2]);
        push(mesh->texcoords+mesh->count,{uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
        mesh->count+=6;
      }
    }
  }
}

void draw(const Mesh* mesh,const v3 position,const float rotation)
{
  m4 transform=rotate({0.f,1.f,0.f},rotation);
  transform.w.x=position.x;
  transform.w.y=position.y;
  transform.w.z=position.z;
  bq_push_transform(transform);
  bq_render3d({1.0f,1.0f,1.0f,1.0f},mesh->count,mesh->positions,mesh->texcoords,nullptr);
  bq_pop_transform();
}
