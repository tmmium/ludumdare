// camera.cc

struct Camera
{
  m4 proj;
  m4 view;
};

void init(Camera* camera,const m4 projection)
{
  camera->proj=projection;
  camera->view=bq_identity();
}

void update(Camera* camera,const v3* axis,const v3 position)
{
  m4 view=bq_identity();
  view.x.x=axis[0].x; view.x.y=axis[1].x; view.x.z=axis[2].x;
  view.y.x=axis[0].y; view.y.y=axis[1].y; view.y.z=axis[2].y;
  view.z.x=axis[0].z; view.z.y=axis[1].z; view.z.z=axis[2].z;
  view.w.x=-dot(position,axis[0]);
  view.w.y=-dot(position,axis[1]);
  view.w.z=-dot(position,axis[2]);
  camera->view=view;
}
