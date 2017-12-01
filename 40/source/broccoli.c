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

// globals
static bool global_logging_active=false;
static const char* global_window_title=0;
static HWND global_window_handle=0;
static HDC global_window_device=0;
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
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
      int index=wParam;
      if (index<=0xff)
      {
        global_keyboard_keys[index]=WM_KEYDOWN?1:0;
      }
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
  bq_log("tiny tmmium started\n");
  bq_get_ticks();
  
  WNDCLASSA wc={0};
  wc.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
  wc.lpfnWndProc=win_window_proc;
  wc.hInstance=GetModuleHandle(NULL);
  wc.lpszClassName="LDclassName";
  wc.hCursor=LoadCursor(NULL,IDC_ARROW);
  wc.hbrBackground=CreateSolidBrush(0x00000000);
  if (!RegisterClassA(&wc)) 
  {
    return 0;
  }
  
  DWORD ws=(WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);
  RECT wr={0}; wr.right=width; wr.bottom=height;
  if (!AdjustWindowRect(&wr,ws,0)) 
  {
    bq_log("error: adjust rect\n");
    return 0;
  }

  HWND window=global_window_handle=CreateWindowA(wc.lpszClassName,
    title,ws,CW_USEDEFAULT,CW_USEDEFAULT,
    wr.right-wr.left,wr.bottom-wr.top,0,0,wc.hInstance,0);
  if (!window) 
  {
    bq_log("error: window\n");
    return 0;
  }

  HDC device=GetDC(window);
  PIXELFORMATDESCRIPTOR pfd={sizeof(PIXELFORMATDESCRIPTOR),1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
    32,0,0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,PFD_MAIN_PLANE,0,0,0,0};
  SetPixelFormat(device,ChoosePixelFormat(device,&pfd),&pfd);
  if (!wglMakeCurrent(device,wglCreateContext(device))) 
  {
    bq_log("error: opengl create\n");
    return 0;
  }

  glViewport(0,0,width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,width,height,0,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glClearDepth(1.0f);
  glClearColor(0.1f,0.125f,0.15f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  HRESULT hr=S_OK;
  IDirectSound* dsound=NULL;
  hr=DirectSoundCreate(NULL,&dsound,NULL);
  if (FAILED(hr)) 
  {
    bq_log("error: dsound create [%u]\n",hr);
    return -1;
  }

  hr=dsound->lpVtbl->SetCooperativeLevel(dsound,window,DSSCL_PRIORITY);
  if (FAILED(hr)) 
  {
    bq_log("error: dsound cooperative level [%u]\n",hr);
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

  return 1;
}

static void bq__process_audio();

int bq_process()
{
  SwapBuffers(global_window_device);
  Sleep(16);

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

void bq_destroy_texture(const int texture_id)
{
  GLuint id=texture_id;
  glDeleteTextures(1,&id);
}

void bq_bind_texture(const int texture_id)
{
  static int bound=0;
  if (bound!=texture_id)
  {
    glBindTexture(GL_TEXTURE_2D,bound=texture_id);
  }
}

void bq_draw_triangles(int count,const v2* positions,const v2* texcoords)
{
  if (count<=0||!positions||!texcoords) {return;}
  glVertexPointer(2,GL_FLOAT,sizeof(v2),positions);
  glTexCoordPointer(2,GL_FLOAT,sizeof(v2),texcoords);
  glDrawArrays(GL_TRIANGLES,0,count);
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

  if (!global_buffer_generations[index])
    global_buffer_generations[index]++;
  int gen=global_buffer_generations[index];
  return ((gen&0xffff)<<16)|(index&0xffff);
}

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
