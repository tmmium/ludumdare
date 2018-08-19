// openal.cc

//#if defined(PLATFORM_WINDOWS)

static bool
win32_load_openal_dll(HMODULE *handle)
{
   *handle = LoadLibraryA("OpenAL64.dll");

   return (*handle) != 0;
}

static bool
win32_load_openal_functions(HMODULE handle)
{
#define ALCF(ret, name, ...)                            \
   name = (type_##name *)GetProcAddress(handle, #name); \
   if (!name)                                           \
   {                                                    \
      return false;                                     \
   }

   ALC_FUNCLIST_0_1;
#undef ALCF

#define ALF(ret, name, ...)                             \
   name = (type_##name *)GetProcAddress(handle, #name); \
   if (!name)                                           \
   {                                                    \
      return false;                                     \
   }

   AL_FUNCLIST_1_x;
#undef ALF

   return true;
}

static bool
win32_create_openal_context(HWND window_handle)
{
   ALCdevice *device = alcOpenDevice(0);
   if (!device)
   {
      return false;
   }

   ALCcontext *context = alcCreateContext(device, 0);
   if (!context)
   {
      return false;
   }

   if (!alcMakeContextCurrent(context))
   {
      return false;
   }

   return true;
}

static bool
platform_create_openal_context(void *handle)
{
   HMODULE openal_dll_handle = 0;
   if (!win32_load_openal_dll(&openal_dll_handle))
   {
      return false;
   }

   if (!win32_load_openal_functions(openal_dll_handle))
   {
      return false;
   }

   if (!win32_create_openal_context((HWND)handle))
   {
      return false;
   }

   return true;
}

//#endif // PLATFORM_WINDOWS

int openal_init(void *window_handle)
{
   if (!platform_create_openal_context(window_handle))
   {
      return 0;
   }

   return 1;
}
