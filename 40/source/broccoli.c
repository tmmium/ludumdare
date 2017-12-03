// broccoli.c

#include "broccoli.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")
#include <windows.h>
#include <windowsx.h>
#include <gl/gl.h>
#include <dsound.h>
#include <stdbool.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#if 0
#pragma warning(push)
#pragma warning(disable : 4456) // declaration of hides previous local declaration
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(disable : 4245)
#pragma warning(disable : 4457) //
#pragma warning(disable : 4701) // potentially uninitialized local variable used
#include <stb_vorbis.h>
#pragma warning(pop)
#endif

// globals
static bool global_logging_active=false;
static const char* global_window_title=0;
static HWND global_window_handle=0;
static HDC global_window_device=0;
static int global_window_width=0;
static int global_window_height=0;
static IDirectSound* global_sound_device=0;
static IDirectSoundBuffer* global_primary_buffer=0;
static v2 global_mouse_position={0};
static int global_mouse_buttons[2]={0};
static int global_keyboard_keys[256]={0};

static void bq__init_logging(int active)
{
  global_logging_active=active;
  if (global_logging_active)
  {
    freopen("debug.txt","w",stdout);
  }
}

void bq_log(const char* format, ...)
{
  if (!global_logging_active) {return;}
  va_list vl;
  va_start(vl,format);
  vfprintf(stdout,format,vl);
  va_end(vl);
}

int bq_get_ticks()
{
  static __int64 f=0,s=0;
  if (!s) 
  {
    QueryPerformanceFrequency((LARGE_INTEGER*)&f);
    QueryPerformanceCounter((LARGE_INTEGER*)&s);
    f/=1000;
  }
  __int64 c=0;
  QueryPerformanceCounter((LARGE_INTEGER*)&c);
  __int64 d=c-s;
  return (int)(d/f);
}

static LRESULT CALLBACK win_window_proc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  switch (uMsg) 
  {
    case WM_INPUT:
    {
      static BYTE buf[48]={0};
      UINT dwSize=sizeof(buf);
      GetRawInputData((HRAWINPUT)lParam,RID_INPUT,buf,&dwSize,sizeof(RAWINPUTHEADER));
      RAWINPUT* raw=(RAWINPUT*)buf;
      if (raw->header.dwType==RIM_TYPEMOUSE)
      {
        global_mouse_position.x+=raw->data.mouse.lLastX; 
        global_mouse_position.y+=raw->data.mouse.lLastY;
        if ((raw->data.mouse.usButtonFlags&RI_MOUSE_LEFT_BUTTON_DOWN)!=0)
          global_mouse_buttons[0]=1;
        if ((raw->data.mouse.usButtonFlags&RI_MOUSE_LEFT_BUTTON_UP)!=0)
          global_mouse_buttons[0]=0;
        if ((raw->data.mouse.usButtonFlags&RI_MOUSE_RIGHT_BUTTON_DOWN)!=0)
          global_mouse_buttons[1]=1;
        if ((raw->data.mouse.usButtonFlags&RI_MOUSE_RIGHT_BUTTON_UP)!=0)
          global_mouse_buttons[1]=0;
      }
    } break;
#if 0
    case WM_MOUSEMOVE: 
    {
      global_mouse_position.x=(float)GET_X_LPARAM(lParam);
      global_mouse_position.y=(float)GET_Y_LPARAM(lParam);
    } break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    {
      global_mouse_buttons[0]=uMsg==WM_LBUTTONDOWN?1:0;
    } break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
      global_mouse_buttons[1]=uMsg==WM_RBUTTONDOWN?1:0;
    } break;
#endif
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
      int index=wParam;
      if (index<=0xff)
      {
        global_keyboard_keys[index]=uMsg==WM_KEYDOWN?1:0;
      }
    } break;
    case WM_SIZE:
    {
      global_window_width=LOWORD(lParam);
      global_window_height=HIWORD(lParam);
      glViewport(0,0,global_window_width,global_window_height);
    } break;
    case WM_GETMINMAXINFO:
    {
      RECT cr={0}; cr.right=320; cr.bottom=180;
      AdjustWindowRect(&cr,WS_OVERLAPPEDWINDOW,FALSE);

      MINMAXINFO* mmi=(MINMAXINFO*)lParam;
      mmi->ptMinTrackSize.x=cr.right-cr.left;
      mmi->ptMinTrackSize.y=cr.bottom-cr.top;
    } break;
    case WM_CLOSE: 
    {
      PostQuitMessage(0); 
    } break;
    default:
    {
      return DefWindowProcA(hWnd,uMsg,wParam,lParam);
    } break;
  }
  return 0;
}

int bq_init(const char* title,int width,int height)
{
  bq__init_logging(true);
  bq_get_ticks();

  bq_log("[init] broccoli\n");
  bq_log("[init]   - because it's healthy\n");
  
  WNDCLASSA wc={0};
  wc.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
  wc.lpfnWndProc=win_window_proc;
  wc.hInstance=GetModuleHandle(NULL);
  wc.lpszClassName="broccoliClassName";
  wc.hCursor=LoadCursor(NULL,IDC_ARROW);
  wc.hbrBackground=CreateSolidBrush(0x00000000);
  wc.hIcon=LoadIcon(wc.hInstance,MAKEINTRESOURCE(101));
  if (!RegisterClassA(&wc)) 
  {
    return 0;
  }
  
  global_window_width=width;
  global_window_height=height;
  DWORD ws=WS_OVERLAPPEDWINDOW;//(WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);
  RECT wr={0}; wr.right=width; wr.bottom=height;
  if (!AdjustWindowRect(&wr,ws,0)) 
  {
    bq_log("[init] error: adjust rect\n");
    return 0;
  }

  HWND window=global_window_handle=CreateWindowA(wc.lpszClassName,
    title,ws,CW_USEDEFAULT,CW_USEDEFAULT,
    wr.right-wr.left,wr.bottom-wr.top,0,0,wc.hInstance,0);
  if (!window) 
  {
    bq_log("[init] error: window\n");
    return 0;
  }

  // raw input
  RAWINPUTDEVICE rid[1]={0};
  rid[0].usUsagePage=0x01;
  rid[0].usUsage=0x02;
  rid[0].dwFlags=RIDEV_INPUTSINK;
  rid[0].hwndTarget=window;
  BOOL r=RegisterRawInputDevices(rid,1,sizeof(rid[0]));

  HDC device=GetDC(window);
  PIXELFORMATDESCRIPTOR pfd={sizeof(PIXELFORMATDESCRIPTOR),1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
    32,0,0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,PFD_MAIN_PLANE,0,0,0,0};
  SetPixelFormat(device,ChoosePixelFormat(device,&pfd),&pfd);
  if (!wglMakeCurrent(device,wglCreateContext(device))) 
  {
    bq_log("[init] error: opengl create\n");
    return 0;
  }

  bq_log("[init] screen - %dx%d\n",width,height);
  bq_log("[init] opengl - %s %s %s\n",
    glGetString(GL_VENDOR),
    glGetString(GL_VERSION),
    glGetString(GL_RENDERER));
  
  glViewport(0,0,width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,width,height,0,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glClearDepth(1.0f);
  glClearColor(0.01f,0.05f,0.05f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  HRESULT hr=S_OK;
  IDirectSound* dsound=NULL;
  hr=DirectSoundCreate(NULL,&dsound,NULL);
  if (FAILED(hr)) 
  {
    bq_log("[init] error: dsound create [%u]\n",hr);
    return -1;
  }

  hr=dsound->lpVtbl->SetCooperativeLevel(dsound,window,DSSCL_PRIORITY);
  if (FAILED(hr)) 
  {
    bq_log("[init] error: dsound cooperative level [%u]\n",hr);
    return -1;
  }

  WAVEFORMATEX fmt={0};
  fmt.cbSize=sizeof(WAVEFORMATEX);
  fmt.wFormatTag=WAVE_FORMAT_PCM;
  fmt.nChannels=2;
  fmt.nSamplesPerSec=22050;
  fmt.wBitsPerSample=16;
  fmt.nBlockAlign=(fmt.nChannels*fmt.wBitsPerSample)/8;
  fmt.nAvgBytesPerSec=fmt.nSamplesPerSec*fmt.nBlockAlign;

  DSBUFFERDESC desc={0};
  desc.dwSize=sizeof(DSBUFFERDESC);
  desc.dwFlags=DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRLVOLUME;
  IDirectSoundBuffer* primary=NULL;
  hr=dsound->lpVtbl->CreateSoundBuffer(dsound,&desc,&primary,NULL);
  if (FAILED(hr)) {return -1;}
  hr=primary->lpVtbl->SetFormat(primary,&fmt);
  if (FAILED(hr)) {primary->lpVtbl->Release(primary); return -1;}
  
  global_window_title=title;
  global_window_handle=window;
  global_window_device=device;
  global_sound_device=dsound;
  global_primary_buffer=primary;
  ShowWindow(window,SW_SHOWNORMAL);

  bq_log("[init] ... ok!\n\n");

  return 1;
}

static void bq__process_audio();

int bq_process()
{
  SwapBuffers(global_window_device);
  //Sleep(15); // todo: remove this sleep

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  static unsigned prev=0;
  unsigned now=bq_get_ticks();
  unsigned diff=now-prev;
  prev=now;
  char title[128];
  sprintf_s(title,128,"%s [%ums]",global_window_title,diff);
  SetWindowTextA(global_window_handle,title);

  MSG msg={0};
  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) 
  {
    if (msg.message==WM_QUIT) {return 0;}
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  bq__process_audio();
  return 1;
}

int bq_create_texture(int width,int height,const void* data)
{
  GLuint id=0;
  glGenTextures(1,&id);
  glBindTexture(GL_TEXTURE_2D,id);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
  glBindTexture(GL_TEXTURE_2D,0);
  GLenum err=glGetError();
  if (err!=GL_NO_ERROR) {return 0;}
  return (int)id;
}

#if 0
int bq_load_texture(const char* filename)
{
  int width,height,c;
  stbi_uc* bitmap=stbi_load(filename,&width,&height,&c,4);
  if (!bitmap) 
  {
    bq_log("error: could not load texture\n");
    return 0;
  }
  int result=bq_create_texture(width,height,bitmap);
  stbi_image_free(bitmap);
  return result;
}
#endif

void bq_destroy_texture(const int texture_id)
{
  GLuint id=texture_id;
  glDeleteTextures(1,&id);
}

void bq_bind_texture(const int texture_id)
{
  static int bound=0;
  if (bound==texture_id) {return;}
  glBindTexture(GL_TEXTURE_2D,bound=texture_id);
}

#define MAX_SOUND_BUFFER_COUNT 64
#define MAX_PLAYING_BUFFER_COUNT 128
static int global_buffer_generations[MAX_SOUND_BUFFER_COUNT];
static IDirectSoundBuffer* global_sound_buffers[MAX_SOUND_BUFFER_COUNT];
static IDirectSoundBuffer* global_playing_buffers[MAX_PLAYING_BUFFER_COUNT];

static void bq__process_audio()
{
  for (int i=0;i<MAX_PLAYING_BUFFER_COUNT;i++)
  {
    IDirectSoundBuffer* buffer=global_playing_buffers[i];
    if (!buffer) {continue;}
    
    DWORD status=0;
    HRESULT hr=buffer->lpVtbl->GetStatus(buffer,&status);
    if (FAILED(hr)||(status&DSBSTATUS_PLAYING)==0)
    {
      buffer->lpVtbl->Release(buffer);
      global_playing_buffers[i]=NULL;
    }
  }
}

int bq_create_sound(int channels,int samples,const void* data)
{
  int index=-1;
  for (int i=0;i<MAX_SOUND_BUFFER_COUNT;i++)
  {
    if (!global_sound_buffers[i]) 
    {
      index=i; 
      break;
    }
  }
  if (index==-1) {return 0;}

  WAVEFORMATEX fmt={0};
  fmt.cbSize=sizeof(WAVEFORMATEX);
  fmt.wFormatTag=WAVE_FORMAT_PCM;
  fmt.nChannels=channels;
  fmt.nSamplesPerSec=22050;
  fmt.wBitsPerSample=16;
  fmt.nBlockAlign=(fmt.nChannels*fmt.wBitsPerSample)/8;
  fmt.nAvgBytesPerSec=fmt.nSamplesPerSec*fmt.nBlockAlign;

  DSBUFFERDESC desc={0};
  desc.dwSize=sizeof(DSBUFFERDESC);
  desc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS;
  desc.dwBufferBytes=channels*samples*sizeof(short);
  desc.lpwfxFormat=&fmt;

  IDirectSoundBuffer* temp=NULL;
  HRESULT hr=global_sound_device->lpVtbl->CreateSoundBuffer(global_sound_device,&desc,&temp,NULL);
  if (FAILED(hr)) 
  {
    bq_log("error: could not create sound buffer [%d]\n",hr);
    return 0;
  }

  IDirectSoundBuffer* secondary=NULL;
  hr=temp->lpVtbl->QueryInterface(temp,&IID_IDirectSoundBuffer,&secondary);
  temp->lpVtbl->Release(temp);
  if (FAILED(hr)) 
  {
    bq_log("error: something bad happend to that sound buffer of yours [%d]\n",hr);
    return 0;
  }

  DWORD size=desc.dwBufferBytes;
  void* dst_ptr=NULL;
  DWORD dst_size=0;
  if (SUCCEEDED(secondary->lpVtbl->Lock(secondary,0,size,&dst_ptr,&dst_size,NULL,NULL,0)))
  {
    memcpy(dst_ptr,data,size);
    secondary->lpVtbl->Unlock(secondary,&dst_ptr,dst_size,NULL,0);
  }
  else
  {
    bq_log("error: could not copy sound waves to sound buffer\n");
  }

  if (!global_buffer_generations[index])
    global_buffer_generations[index]++;

  global_sound_buffers[index]=secondary;
  int gen=global_buffer_generations[index];
  return ((gen&0xffff)<<16)|(index&0xffff);
}

#if 0
int bq_load_sound(const char* filename)
{
  int num_channels=0,sample_rate=0;
  short* samples=0;
  int num_samples=stb_vorbis_decode_filename(filename,&num_channels,&sample_rate,&samples);
  int result=bq_create_sound(num_channels,num_samples,samples);
  free(samples);
  return result;
}
#endif

void bq_destroy_sound(const int id)
{
  int index=(id&0xffff);
  int gen=(id>>16)&0xffff;
  if (gen!=global_buffer_generations[index]) {return;}
  global_sound_buffers[index]->lpVtbl->Release(global_sound_buffers[index]);
  global_sound_buffers[index]=NULL;
  global_buffer_generations[index]++;
}

void bq_play_sound(const int id,float volume)
{
  int bidx=(id&0xffff);
  int gen=(id>>16)&0xffff;
  if (gen!=global_buffer_generations[bidx]) {return;}
  int index=-1;
  for (int i=0;i<MAX_PLAYING_BUFFER_COUNT;i++)
  {
    if (!global_playing_buffers[i]) 
    {
      index=i;
      break;
    }
  }
  if (index==-1) {return;}
  
  long vol=0;
  if (volume<=0.0f) {vol=DSBVOLUME_MIN;}
  if (volume>=1.0f) {vol=DSBVOLUME_MAX;}
  vol=(long)(-2000.0f*log10(1.0f/volume));
  global_sound_device->lpVtbl->DuplicateSoundBuffer(global_sound_device,global_sound_buffers[bidx],&global_playing_buffers[index]);
  global_playing_buffers[index]->lpVtbl->SetVolume(global_playing_buffers[index],vol);
  global_playing_buffers[index]->lpVtbl->Play(global_playing_buffers[index],0,0,0);
}

m4 bq_identity()
{
  m4 result={0.0f};
  result.x.x=1.0f;
  result.y.y=1.0f;
  result.z.z=1.0f;
  result.w.w=1.0f;
  return result;
}

m4 bq_multiply(const m4 a,const m4 b)
{
  m4 res;
  res.x.x=a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x+a.x.w*b.w.z;
  res.x.y=a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y+a.x.w*b.w.y;
  res.x.z=a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z+a.x.w*b.w.z;
  res.x.w=a.x.x*b.x.w+a.x.y*b.y.w+a.x.z*b.z.w+a.x.w*b.w.w;
  res.y.x=a.y.z*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x+a.y.w*b.w.z;
  res.y.y=a.y.z*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y+a.y.w*b.w.y;
  res.y.z=a.y.z*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z+a.y.w*b.w.z;
  res.y.w=a.y.z*b.x.w+a.y.y*b.y.w+a.y.z*b.z.w+a.y.w*b.w.w;
  res.z.x=a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x+a.z.w*b.w.z;
  res.z.y=a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y+a.z.w*b.w.y;
  res.z.z=a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z+a.z.w*b.w.z;
  res.z.w=a.z.x*b.x.w+a.z.y*b.y.w+a.z.z*b.z.w+a.z.w*b.w.w;
  res.w.x=a.w.x*b.x.x+a.w.y*b.y.x+a.w.z*b.z.x+a.w.w*b.w.z;
  res.w.y=a.w.x*b.x.y+a.w.y*b.y.y+a.w.z*b.z.y+a.w.w*b.w.y;
  res.w.z=a.w.x*b.x.z+a.w.y*b.y.z+a.w.z*b.z.z+a.w.w*b.w.z;
  res.w.w=a.w.x*b.x.w+a.w.y*b.y.w+a.w.z*b.z.w+a.w.w*b.w.w;
  return res;
}

static v3 vec3(float x,float y,float z) 
{
  v3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static v3 sub(const v3 a,const v3 b)
{
  v3 c;
  c.x=a.x-b.x;
  c.y=a.y-b.y;
  c.z=a.z-b.z;
  return c;
}
static v3 add(const v3 a,const v3 b)
{
  v3 c;
  c.x=a.x+b.x;
  c.y=a.y+b.y;
  c.z=a.z+b.z;
  return c;
}
static v3 normalize(const v3 a)
{
  v3 r=a;
  float len=sqrtf(a.x*a.x+a.y*a.y+a.z*a.z);
  if (len>0.0f) 
  {
    r.x/=len;
    r.y/=len;
    r.z/=len;
  }
  return r;
}
static float dot(const v3 a,const v3 b)
{
  return a.x*b.x+a.y*b.y+a.z*b.z;
}
static v3 cross(const v3 a,const v3 b)
{
  v3 c;
  c.x=a.y*b.z-a.z*b.y;
  c.y=a.z*b.x-a.x*b.z;
  c.z=a.x*b.y-a.y*b.x;
  return c;
}

m4 bq_lookat(const v3 target,const v3 eye)
{
  v3 y=vec3(0,1,0);
  v3 x=normalize(sub(eye,target));
  v3 z=normalize(cross(y,x));
  y=normalize(cross(x,z));
  m4 res=bq_identity();
  res.x.x=x.x; res.x.y=y.x; res.x.z=z.x;
  res.y.x=x.y; res.y.y=y.y; res.y.z=z.y;
  res.z.x=x.z; res.z.y=y.z; res.z.z=z.z;
  res.w.x=-dot(eye,x);
  res.w.y=-dot(eye,y);
  res.w.z=-dot(eye,z);
  return res;
}

m4 bq_orthographic(int width,int height)
{
  float w=(float)width,h=(float)height,znear=-1.0f,zfar=1.0f;
  m4 result=bq_identity();
  result.x.x=2.0f/w;
  result.y.y=2.0f/-h;
  result.z.z=1.0f/(zfar-znear);
  result.w.x=-1.0f;
  result.w.y=1.0f;
  result.w.z=znear/(znear-zfar);
  return result;
}

m4 bq_perspective(float aspect,float fov,float znear,float zfar)
{
  m4 result=bq_identity();
  float h=1.0f/tanf(fov*0.5f);
  float w=h/aspect;
  result.x.x=w;
  result.y.y=h;
  result.z.z=zfar/(zfar-znear);
  result.z.w=1.0f;
  result.w.z=(-znear*zfar)/(zfar-znear);
  result.w.w=0.0f;
  return result;
}

void bq_projection(const m4 projection)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMultMatrixf((const GLfloat*)&projection);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void bq_view(const m4 view)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf((const GLfloat*)&view);
}

void bq_prepare2d()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
}

void bq_render2d(const v4 color,int count,const v2* positions,const v2* texcoords)
{
  if (count<=0||!positions||!texcoords) {return;}
  glColor4f(color.x,color.y,color.z,color.w);
  glVertexPointer(2,GL_FLOAT,sizeof(v2),positions);
  glTexCoordPointer(2,GL_FLOAT,sizeof(v2),texcoords);
  glDrawArrays(GL_TRIANGLES,0,count);
}

void bq_prepare3d()
{
  glEnable(GL_DEPTH_TEST);
}

void bq_render3d(const v4 color,int count,const v3* positions,const v2* texcoords,const v3* normals)
{
  if (count<=0||!positions||!texcoords) {return;}
  glColor4f(color.x,color.y,color.z,color.w);
  //glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,sizeof(v3),positions);
  glTexCoordPointer(2,GL_FLOAT,sizeof(v2),texcoords);
  //glNormalPointer(GL_FLOAT,sizeof(v3),normals);
  glDrawArrays(GL_TRIANGLES,0,count);
  //glDisableClientState(GL_NORMAL_ARRAY);
}

void bq_enable_fog(const v4 color,const float density,const float start,const float end)
{
  glEnable(GL_FOG);
  glHint(GL_FOG_HINT,GL_DONT_CARE);
  glFogfv(GL_FOG_COLOR,(const GLfloat*)&color);
  glFogi(GL_FOG_MODE,GL_EXP2);
  glFogf(GL_FOG_DENSITY,density);
  glFogf(GL_FOG_START,start);
  glFogf(GL_FOG_END,end);
}

void bq_disable_fog()
{
  glDisable(GL_FOG);
}

void bq_set_cursor(int state)
{
  ShowCursor(state==0?FALSE:TRUE);
}

void bq_center_cursor()
{
  POINT pt;
  pt.x=global_window_width>>1;
  pt.y=global_window_height>>1;
  ClientToScreen(global_window_handle,&pt);
  SetCursorPos(pt.x,pt.y);
}

v2 bq_mouse_position()
{
  return global_mouse_position;
}

int bq_mouse_button(int index)
{
  if (index<0||index>1) {return 0;}
  return global_mouse_buttons[index];
}

int bq_keyboard(int index)
{
  if (index<0||index>0xff) {return 0;}
  return global_keyboard_keys[index];
}

int bq_file_size(const char* filename)
{
  WIN32_FILE_ATTRIBUTE_DATA fad={0};
  if (!GetFileAttributesExA(filename,GetFileExInfoStandard,&fad)) {return 0;}
  return (int)fad.nFileSizeLow;
}

int bq_load_file(const char* filename,int size,void* dst)
{
  HANDLE hF=CreateFileA(filename,GENERIC_READ,FILE_SHARE_READ,
    NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if (hF==INVALID_HANDLE_VALUE) {return 0;}
  int n=(int)GetFileSize(hF,NULL);
  if (n<=size) {ReadFile(hF,dst,n,NULL,NULL);}
  CloseHandle(hF);
  return 1;
}
