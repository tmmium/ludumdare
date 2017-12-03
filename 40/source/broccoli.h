// brocolli.h

#ifndef BROCCOLI_H_INCLUDED
#define BROCCOLI_H_INCLUDED

#ifdef _WIN32
# define BQ_API __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct v2 { float x, y; } v2;
typedef struct v3 { float x, y, z; } v3;
typedef struct v4 { float x, y, z, w; } v4;
typedef struct m4 { v4 x, y, z, w; } m4;

BQ_API void bq_log(const char* format, ...);
BQ_API int  bq_get_ticks();

BQ_API int  bq_init(const char* title,int width,int height);
BQ_API int  bq_process();

//BQ_API int  bq_load_texture(const char* filename);
BQ_API int  bq_create_texture(int width,int height,const void* data);
BQ_API int  bq_update_texture(const int texture_id,int width,int height,const void* data);
BQ_API void bq_destroy_texture(const int texture_id);
BQ_API void bq_bind_texture(const int texture_id);

BQ_API int  bq_load_sound(const char* filename);
BQ_API int  bq_create_sound(int channels,int samples,const void* data);
BQ_API void bq_destroy_sound(const int sound_id);
BQ_API void bq_play_sound(const int sound_id,float volume);

BQ_API m4   bq_identity();
BQ_API m4   bq_multiply(const m4 a,const m4 b);
BQ_API m4   bq_lookat(const v3 target,const v3 eye);
BQ_API m4   bq_orthographic(int width,int height);
BQ_API m4   bq_perspective(float aspect,float fov,float znear,float zfar);
BQ_API void bq_projection(const m4 projection);
BQ_API void bq_view(const m4 view);
BQ_API void bq_push_transform(const m4 transform);
BQ_API void bq_pop_transform();

BQ_API void bq_prepare2d();
BQ_API void bq_render2d(const v4 color,int count,const v2* positions,const v2* texcoords);

BQ_API void bq_prepare3d();
BQ_API void bq_render3d(const v4 color,int count,const v3* positions,const v2* texcoords,const v3* normals);

BQ_API void bq_enable_fog(const v4 color,const float density,const float start,const float end);
BQ_API void bq_disable_fog();

BQ_API void bq_set_cursor(int state);
BQ_API void bq_center_cursor();

BQ_API v2   bq_mouse_position();
BQ_API int  bq_mouse_button(int index);
BQ_API int  bq_keyboard(int index);

BQ_API v2   bq_mouse_position_in_window();
BQ_API v2   bq_window_size();

BQ_API int bq_file_size(const char* filename);
BQ_API int bq_load_file(const char* filename,int size,void* dst);

#ifdef __cplusplus
}
#endif

#endif // BROCCOLI_H_INCLUDED
