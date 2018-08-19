// cucumber.c

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

#define _CRT_SECURE_NO_WARNINGS
#include "cucumber.h"
#include <windows.h>
#include <windowsx.h>
#include <stdbool.h>
#include <stdio.h>

#include "opengl.h"
#include "opengl.c"

#define ASSERT(x) if (!(x)) { *(int *)0 = 0; }
#define ARRAYCOUNT(x) (sizeof(x) / sizeof(x[0]))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
   struct 
   {
      HWND handle;
      HDC device;
      HGLRC context;
      const char *title;
   } window;
   struct 
   {
      long long factor;
      long long start;
   } ticks;
} env_t;

static env_t *env;

qu_int64_t qu_get_ticks()
{
  long long now = 0;
  QueryPerformanceCounter((LARGE_INTEGER *)&now);
  long long diff = now - env->ticks.start;
  return (long long)(diff / env->ticks.factor);
}

static LRESULT CALLBACK 
win_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      case WM_CLOSE:
      {
         PostQuitMessage(0);
      } break;
      default:
      {
         return DefWindowProcA(hWnd, uMsg, wParam, lParam);
      } break;
   }

   return 0;
}

qu_bool32_t qu_init(int width, int height, const char *title)
{
   WNDCLASSA wc = {0};
   wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
   wc.lpfnWndProc = win_window_proc;
   wc.hInstance = GetModuleHandle(NULL);
   wc.lpszClassName = "cucumberClassName";
   wc.hCursor = LoadCursor(NULL,IDC_ARROW);
   wc.hbrBackground = CreateSolidBrush(0x00000000);
   wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(101));
   if (!RegisterClassA(&wc)) 
      return QU_FALSE;
   
   DWORD window_style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
   RECT window_rect = {0}; 
   window_rect.right = width; 
   window_rect.bottom = height;
   if (!AdjustWindowRect(&window_rect, window_style, FALSE)) 
      return QU_FALSE;

   int window_width = window_rect.right - window_rect.left;
   int window_height = window_rect.bottom - window_rect.top;
   HWND window = CreateWindowA(wc.lpszClassName,
      title, 
      window_style, 
      CW_USEDEFAULT, 
      CW_USEDEFAULT,
      window_width,
      window_height,
      0, 0,
      wc.hInstance,
      0);
   if (!window) 
      return QU_FALSE;

   if (!opengl_init(window, 3, 2))
      return QU_FALSE;

   HGLRC context = wglGetCurrentContext_();

   // note: opengl 3.x core context _requires_ a bound 
   //       vertex array object to render
   //       so, we oblige
   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glViewport(0, 0, width, height);
   glClearDepth(1.0f);
   glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   long long s = 0, f = 0;
   QueryPerformanceFrequency((LARGE_INTEGER*)&f);
   QueryPerformanceCounter((LARGE_INTEGER*)&s);
   f /= 1000;

   env = (env_t *)VirtualAlloc(NULL, sizeof(env_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
   env->window.title   = title;
   env->window.handle  = window;
   env->window.device  = GetDC(window);
   env->window.context = context;
   env->ticks.factor   = f;
   env->ticks.start    = s;

   ShowWindow(window, SW_SHOWNORMAL);

   return QU_TRUE;
}

qu_bool32_t qu_process()
{
   SwapBuffers(env->window.device);

   MSG msg = {0};
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
   {
      if (msg.message == WM_QUIT) 
         return QU_FALSE;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   static long long prev = 0;
   long long now = qu_get_ticks();
   long long diff = now - prev; prev = now;

   char title[128];
   sprintf(title, "%s [%llums]", env->window.title, diff);
   SetWindowTextA(env->window.handle, title);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   return QU_TRUE;
}

#include "cucumber_render_api.c"

#ifdef __cplusplus
extern "C" {
#endif
