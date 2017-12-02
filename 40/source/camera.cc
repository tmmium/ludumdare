// camera.cc

struct Camera
{
  m4 proj;
  m4 view;
  v3 axis[3];
  v3 position;
  float radians[3];
};

void init(Camera* camera,const v3 position)
{
  camera->proj=bq_perspective(320.f/180.f,kPI*0.25f,0.1f,100.0f);
  camera->view=bq_identity();
  camera->axis[0]={1.0f,0.0f,0.0f};
  camera->axis[1]={0.0f,1.0f,0.0f};
  camera->axis[2]={0.0f,0.0f,1.0f};
  camera->position=position;
  camera->radians[0]=0.0f;
  camera->radians[1]=0.0f;
  camera->radians[2]=0.0f;
}

void update(Camera* camera)
{
  v3 x={1.0f,0.0f,0.0f};
  v3 y={0.0f,1.0f,0.0f};
  v3 z={0.0f,0.0f,1.0f};

  m4 ry=rotate(y,camera->radians[1]);
  x=normalize(transform_normal(ry,x));
  z=normalize(transform_normal(ry,z));

  m4 rx=rotate(x,camera->radians[0]);
  y=normalize(transform_normal(rx,y));
  z=normalize(transform_normal(rx,z));

  camera->axis[0]=x;
  camera->axis[1]=y;
  camera->axis[2]=z;

  m4 view=bq_identity();
  view.x.x=x.x; view.x.y=y.x; view.x.z=z.x;
  view.y.x=x.y; view.y.y=y.y; view.y.z=z.y;
  view.z.x=x.z; view.z.y=y.z; view.z.z=z.z;
  view.w.x=-dot(camera->position,x);
  view.w.y=-dot(camera->position,y);
  view.w.z=-dot(camera->position,z);
  camera->view=view;
}

void rotate_x(Camera* camera,float amount)
{
  const float mm=0.85f;
  camera->radians[0]+=amount;
  camera->radians[0]=maxf(-mm,minf(camera->radians[0],mm));
}
void rotate_y(Camera* camera,float amount)
{
  camera->radians[1]+=amount;
}
void rotate_z(Camera* camera,float amount)
{
  camera->radians[2]+=amount;
}

void move_x(Camera* camera,float amount)
{
  camera->position=camera->position+camera->axis[0]*amount;
}

void move_y(Camera* camera,float amount)
{
  camera->position=camera->position+camera->axis[1]*amount;
}

void move_z(Camera* camera,float amount)
{
  v3 axis=camera->axis[2];
  camera->position=camera->position+camera->axis[2]*amount;
}

void move_z_locked_y(Camera* camera,float amount)
{
  v3 axis=camera->axis[2];
  axis.y=0.0f;
  axis=normalize(axis);
  camera->position=camera->position+axis*amount;
}
