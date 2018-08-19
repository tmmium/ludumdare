// main.c

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <windowsx.h>
#include <gl/gl.h>
#include <dsound.h>
#include <math.h>
#include <stdio.h>

#include "interface.h"
#include "opengl.c"

#define KILOBYTE(x) (1024U * (x))
#define MEGABYTE(x) (1024U * KILOBYTE(x))

long long qk_get_ticks()
{
   static long long factor = 0, start = 0;
   if (!factor)
   {
      QueryPerformanceCounter((LARGE_INTEGER *)&start);
      QueryPerformanceFrequency((LARGE_INTEGER *)&factor);
      factor /= 1000000;
   }

   long long now = 0;
   QueryPerformanceCounter((LARGE_INTEGER *)&now);

   return (long long)((now - start) / factor);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   const char *window_title = "loot'er";
   const int window_width = 960;
   const int window_height = 540;

   int monitor_refresh_rate = 60;
   DEVMODEA dev_mode = {0};
   dev_mode.dmSize = sizeof(DEVMODEA);
   if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dev_mode))
   {
      monitor_refresh_rate = dev_mode.dmDisplayFrequency;
   }

   WNDCLASSA wc = {0};
   wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
   wc.lpfnWndProc = win_window_proc;
   wc.hInstance = GetModuleHandle(NULL);
   wc.lpszClassName = "ludumdareClassName";
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = CreateSolidBrush(0x00000000);
   wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(101));
   if (!RegisterClassA(&wc))
      return -1;

   DWORD window_style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
   RECT window_rect = {0};
   window_rect.right = window_width;
   window_rect.bottom = window_height;
   if (!AdjustWindowRect(&window_rect, window_style, FALSE))
      return -1;

   int screen_width = GetSystemMetrics(SM_CXSCREEN);
   int screen_height = GetSystemMetrics(SM_CYSCREEN);
   int center_x = ((screen_width) - (window_rect.right - window_rect.left)) >> 1;
   int center_y = ((screen_height) - (window_rect.bottom - window_rect.top)) >> 1;

   HWND window_handle = CreateWindowA(wc.lpszClassName,
                                      window_title,
                                      window_style,
                                      center_x,
                                      center_y,
                                      window_rect.right - window_rect.left,
                                      window_rect.bottom - window_rect.top,
                                      0, 0,
                                      wc.hInstance,
                                      0);
   if (!window_handle)
      return -1;

#if 0
   RAWINPUTDEVICE rid[1] = {0};
   rid[0].usUsagePage = 0x01;
   rid[0].usUsage = 0x02;
   rid[0].dwFlags = RIDEV_INPUTSINK;
   rid[0].hwndTarget = window_handle;
   RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
#endif

   HDC device_context = GetDC(window_handle);
   PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
                                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
                                32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0};
   SetPixelFormat(device_context, ChoosePixelFormat(device_context, &pfd), &pfd);
   if (!wglMakeCurrent(device_context, wglCreateContext(device_context)))
      return -1;

   typedef BOOL WINAPI wglSwapIntervalEXT_t(int interval);
   wglSwapIntervalEXT_t *wglSwapIntervalEXT = (wglSwapIntervalEXT_t *)wglGetProcAddress("wglSwapIntervalEXT");
   if (wglSwapIntervalEXT)
      wglSwapIntervalEXT(1);

   glViewport(0, 0, window_width, window_height);
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   HMODULE game_dll = LoadLibraryA("ludumdare.dll");
   if (!game_dll) 
   {
      MessageBoxA(NULL,
                  "unable to load ludum dare game!",
                  "error!",
                  MB_OK);
      return 0;
   }

   typedef int (game_update_and_render_t)(allocator_t *, input_t *, render_commands_t *, render_commands_t *);
   game_update_and_render_t *game_update_and_render = 
      (game_update_and_render_t *)GetProcAddress(game_dll, "game_update_and_render");
   
   if (!game_update_and_render)
   {
      MessageBoxA(NULL,
                  "could not find ludum dare game callback!",
                  "error!",
                  MB_OK);
      return 0;
   }

   int game_allocator_size = MEGABYTE(8);
   int render_command_capacity = KILOBYTE(64);
   int render_command_size = sizeof(drawcmd_t) * render_command_capacity * 2;
   int render_buffer_capacity = KILOBYTE(512);
   int render_buffer_size = sizeof(vertex_t) * render_buffer_capacity;

   int memory_size = game_allocator_size + render_command_size + render_buffer_size;
   char *memory_base = (char *)VirtualAlloc(NULL, 
                                            memory_size, 
                                            MEM_COMMIT | MEM_RESERVE,
                                            PAGE_READWRITE);
   char *game_allocator_base = memory_base;
   char *render_command_base = game_allocator_base + game_allocator_size;
   char *debug_render_command_base = render_command_base + game_allocator_size;
   char *render_buffer_base = render_command_base + render_command_size;

   allocator_t allocator_ = {0};
   allocator_t *allocator = &allocator_;
   allocator_.size        = game_allocator_size;
   allocator_.base        = game_allocator_base;
   allocator_.at          = allocator_.base;

   input_t input_ = {0};
   input_t *input = &input_;
   input_.dt = 1.0f / (f32)monitor_refresh_rate;

   render_commands_t render_commands_ = {0};
   render_commands_t *render_commands = &render_commands_;
   render_commands_.capacity          = render_command_capacity;
   render_commands_.base              = (drawcmd_t *)render_command_base;
   render_commands_.at                = render_commands_.base;

   render_commands_t debug_render_commands_ = {0};
   render_commands_t *debug_render_commands = &debug_render_commands_;
   debug_render_commands_.capacity          = render_command_capacity;
   debug_render_commands_.base              = (drawcmd_t *)debug_render_command_base;  
   debug_render_commands_.at                = debug_render_commands_.base;

   render_buffer_t render_buffer_ = {0};
   render_buffer_t *render_buffer = &render_buffer_;
   render_buffer_.current_count   = 0;
   render_buffer_.max_count       = 0;
   render_buffer_.min_count       = 0xffffffffu;
   render_buffer_.capacity        = render_buffer_capacity;
   render_buffer_.base            = (vertex_t *)render_buffer_base;
   render_buffer_.at              = render_buffer_.base;

   gl_create_texture_from_file("assets/texture.png");

   const char *ms = "ms";
   const char *us = "us";
   char window_title_ex[512];

   ShowWindow(window_handle, SW_SHOWNORMAL);

   while (1)
   {
      MSG msg = {0};
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
         if (msg.message == WM_QUIT)
            return 0;
         switch (msg.message)
         {
            case WM_MOUSEMOVE:
            {
               input_.position.x = GET_X_LPARAM(msg.lParam);
               input_.position.y = GET_Y_LPARAM(msg.lParam);
            } break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            {
               input_.buttons[0] = msg.message == WM_LBUTTONDOWN ? 1 : 0;
            } break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            {
               input_.buttons[1] = msg.message == WM_RBUTTONDOWN ? 1 : 0;
            } break;
            case WM_MOUSEWHEEL:
            {
               input_.wheel = GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA;
            } break;
         }

         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }

      long long u_start = qk_get_ticks();
      if (!game_update_and_render(allocator, input, render_commands, debug_render_commands)) 
         return 0;

      input_.wheel = 0;

      long long u_end = qk_get_ticks();
      long long u_diff = u_end - u_start;

      long long r_start = qk_get_ticks();
      gl_render_commands(render_commands, render_buffer);

      long long r_end = qk_get_ticks();
      long long r_diff = r_end - r_start;

      gl_render_buffer(render_buffer);

      gl_render_commands(debug_render_commands, render_buffer);
      gl_render_buffer(render_buffer);
      SwapBuffers(device_context);

      static long long s_prev = 0;
      long long s_now = qk_get_ticks();
      long long s_diff = s_now - s_prev; s_prev = s_now;

      uint32_t memory_used = (allocator_.at - allocator_.base);
      f32 memory_used_procent = (f32)memory_used / (f32)game_allocator_size;
      const char *memory_used_abbr = "B";
      if (memory_used > 1024)
      {
         memory_used /= 1024;
         memory_used_abbr = "kB";
      }
      if (memory_used > 1024)
      {
         memory_used /= 1024;
         memory_used_abbr = "MB";
      }

      sprintf(window_title_ex, 
              "%s - Mem: %u%s (%3.2f%%) DC:%u/%u FS:%llu%s FG:%llu%s R:%llu%s", 
              window_title, 
              memory_used,
              memory_used_abbr,
              memory_used_procent * 100.0f,
              render_buffer_.current_count / 6,
              render_buffer_.max_count / 6,
              s_diff >= 1000 ? s_diff / 1000 : s_diff, 
              s_diff >= 1000 ? ms : us,
              u_diff >= 1000 ? u_diff / 1000 : u_diff,
              u_diff >= 1000 ? ms : us,
              r_diff >= 1000 ? r_diff / 1000 : r_diff,
              r_diff >= 1000 ? ms : us);
      SetWindowTextA(window_handle, window_title_ex);
   }

   return 0;
}
