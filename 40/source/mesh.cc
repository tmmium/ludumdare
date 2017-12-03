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
  bq_log("mesh: capacity %d\n",capacity);
}

static void push(v3* p,v3 q0,v3 q1,v3 q2,v3 q3)
{
  p[0]=q0; p[1]=q1; p[2]=q2;
  p[3]=q2; p[4]=q3; p[5]=q0;
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


bool is_wall(const Bitmap* bitmap,const int x,const int y)
{
  if (x<0||x>=bitmap->width) {return true;}
  if (y<0||y>=bitmap->height) {return true;}
  return pixel_at(bitmap,x,y)==0xff000000;
}

void build_world(Mesh* mesh,const Bitmap* bitmap)
{
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
