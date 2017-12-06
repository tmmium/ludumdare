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
  bq_log("[game] mesh: capacity %d\n",capacity);
  int offset=sizeof(v3)*capacity;
  int size=offset+sizeof(v2)*capacity;
  char* base=(char*)malloc(size);
  mesh->capacity=capacity;
  mesh->count=0;
  mesh->positions=(v3*)base;
  mesh->texcoords=(v2*)(base+offset);
}

static void push_quad(Mesh* mesh,v3 p0,v3 p1,v3 p2,v3 p3, v2 min,v2 max)
{
  v3* pos=mesh->positions+mesh->count;
  pos[0]=p0; pos[1]=p1; pos[2]=p2;
  pos[3]=p2; pos[4]=p3; pos[5]=p0;

  v2* t=mesh->texcoords+mesh->count;
  t[0]={min.x,min.y};
  t[1]={max.x,min.y};
  t[2]={max.x,max.y};
  t[3]={max.x,max.y};
  t[4]={min.x,max.y};
  t[5]={min.x,min.y};

  mesh->count+=6;
}

static void push_tris(Mesh* mesh,v3 p0,v3 p1,v3 p2, v2 s0,v2 s1,v2 s2)
{
  v3* pos=mesh->positions+mesh->count;
  pos[0]=p0; pos[1]=p1; pos[2]=p2;

  v2* tex=mesh->texcoords+mesh->count;
  tex[0]=s0; tex[1]=s1; tex[2]=s2;

  mesh->count+=3;
}

bool is_wall(const Bitmap* bitmap,const int x,const int y)
{
  if (x<0||x>=bitmap->width) {return true;}
  if (y<0||y>=bitmap->height) {return true;}
  return pixel_at(bitmap,x,y)==0xff000000;
}

void build_cube_mesh(Mesh* mesh,const v4 poles,const v4 sides,const v3 min,const v3 max)
{
  const v3 corners[8]=
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

  push_quad(mesh,corners[1],corners[0],corners[4],corners[5],{poles.x,poles.y},{poles.z,poles.w}); // top
  push_quad(mesh,corners[2],corners[3],corners[7],corners[6],{poles.x,poles.y},{poles.z,poles.w}); // bottom  
  push_quad(mesh,corners[0],corners[1],corners[2],corners[3],{sides.x,sides.y},{sides.z,sides.w}); // north
  push_quad(mesh,corners[4],corners[5],corners[6],corners[7],{sides.x,sides.y},{sides.z,sides.w}); // south
  push_quad(mesh,corners[4],corners[0],corners[3],corners[7],{sides.x,sides.y},{sides.z,sides.w}); // east 
  push_quad(mesh,corners[1],corners[5],corners[6],corners[2],{sides.x,sides.y},{sides.z,sides.w}); // west
}

void build_spawn_mesh(Mesh* mesh)
{
  const v4 poles=uvcoords({0.000f,0.250f,0.109375f,0.375f},1.0f);
  const v4 sides=uvcoords({0.109375f,0.250f,0.125f,0.375f},1.0f);
  const float Q=0.3f;
  const v3 min={-Q,0.00f,-Q};
  const v3 max={ Q,0.05f, Q};
  build_cube_mesh(mesh,poles,sides,min,max);
}

void build_finish_mesh(Mesh* mesh)
{
  v4 poles=uvcoords({16.f,32.f,30.f,48.f},1.0f/128.0f);
  v4 sides=uvcoords({30.f,32.f,32.f,48.f},1.0f/128.0f);
  const float Q=0.3f;
  const v3 min={-Q,0.00f,-Q};
  const v3 max={ Q,0.05f, Q};
  build_cube_mesh(mesh,poles,sides,min,max);
}

void build_pickup_mesh(Mesh* mesh)
{
  const v4 poles=uvcoords({32.f,32.f,46.f,48.f},1.0f/128.0f);
  const v4 sides=uvcoords({46.f,32.f,48.f,48.f},1.0f/128.0f);
  const float Q=0.1f;
  const v3 min={-Q,-Q,-Q*0.2f};
  const v3 max={ Q, Q, Q*0.2f};
  build_cube_mesh(mesh,poles,sides,min,max);
}

void build_pyramid_mesh(Mesh* mesh,const v2* uvs,const v3* corners,const v3 offset={0.0f,0.0f,0.0f})
{
  push_tris(mesh,corners[0]+offset,corners[1]+offset,corners[2]+offset, uvs[0],uvs[1],uvs[2]);
  push_tris(mesh,corners[0]+offset,corners[2]+offset,corners[3]+offset, uvs[0],uvs[1],uvs[2]);
  push_tris(mesh,corners[0]+offset,corners[3]+offset,corners[1]+offset, uvs[0],uvs[1],uvs[2]);
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
  const v4 tex[]=
  {
    // floor
    0.00f,0.00f,0.25f,0.25f,
    0.25f,0.00f,0.50f,0.25f,
    0.50f,0.00f,0.75f,0.25f,
    0.75f,0.00f,1.00f,0.25f,
    // wall
    0.00f,0.25f,0.25f,0.50f,
    0.25f,0.25f,0.50f,0.50f,
    0.50f,0.25f,0.75f,0.50f,
    0.75f,0.25f,1.00f,0.50f,
    // roof
    0.00f,0.50f,0.25f,0.75f,
    0.25f,0.50f,0.50f,0.75f,
    0.50f,0.50f,0.75f,0.75f,
    0.75f,0.50f,1.00f,0.75f,
    // misc
    0.00f,0.75f,0.25f,1.00f,
    0.25f,0.75f,0.50f,1.00f,
    0.50f,0.75f,0.75f,1.00f,
    0.75f,0.75f,1.00f,1.00f,
  };

  const v4 uvs[]=
  {
    0.00f, 0.0f, 0.25f, 0.25f, // floor
    0.25f, 0.0f, 0.50f, 0.25f, // wall
    0.50f, 0.0f, 0.75f, 0.25f, // ceiling
    0.75f, 0.0f, 1.00f, 0.25f  // end?
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
      push_quad(mesh,cube[2],cube[3],cube[7],cube[6], {uvs[0].x,uvs[0].y},{uvs[0].z,uvs[0].w});
      // roof
      push_quad(mesh,cube[1],cube[0],cube[4],cube[5], {uvs[2].x,uvs[2].y},{uvs[2].z,uvs[2].w});

      // north
      if (is_wall(bitmap,x,y-1))
      {
        push_quad(mesh,cube[0],cube[1],cube[2],cube[3], {uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
      }
      // south
      if (is_wall(bitmap,x,y+1))
      {
        push_quad(mesh,cube[4],cube[5],cube[6],cube[7], {uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
      }
      // east
      if (is_wall(bitmap,x-1,y))
      { 
        push_quad(mesh,cube[4],cube[0],cube[3],cube[7], {uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
      }
      // west
      if (is_wall(bitmap,x+1,y))
      {
        push_quad(mesh,cube[1],cube[5],cube[6],cube[2], {uvs[1].x,uvs[1].y},{uvs[1].z,uvs[1].w});
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

void build_exit_mesh(Mesh* mesh) 
{
  // four cubes, three for frame, one for exit area? 
}

void build_tele_mesh(Mesh* mesh) 
{
  // same as exit?
}

void build_door_mesh(Mesh* mesh) 
{
  // like exit and teleport
}

void build_ammo_mesh(Mesh* mesh)
{
  const v4 poles={0.00f,0.25f,0.25f,0.50f};
  const v4 sides={0.25f,0.25f,0.50f,0.50f};
}

void build_health_mesh(Mesh* mesh)
{
  const v4 poles={0.50f,0.25f,0.75f,0.50f};
  const v4 sides={0.75f,0.25f,1.00f,0.50f};
}

void build_key_mesh(Mesh* mesh) 
{
  // just a cube, like a multi-pass key-card
  // maybe a cut corner, just for kicks
}
