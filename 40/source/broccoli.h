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

BQ_API int  bq_create_texture(const char* filename);
BQ_API void bq_destroy_texture(const int texture_id);
BQ_API void bq_bind_texture(const int texture_id);

BQ_API void bq_projection(const m4 projection);
BQ_API void bq_render2d(int count,const v2* positions,const v2* texcoords);
BQ_API void bq_render3d(int count,const v3* positions,const v2* texcoords,const v3* normals);

BQ_API int  bq_create_sound(const char* filename);
BQ_API void bq_destroy_sound(const int sound_id);
BQ_API void bq_play_sound(const int sound_id,float volume);

BQ_API m4 bq_orthographic(int width,int height);
BQ_API m4 bq_perspective(float aspect,float fov,float znear,float zfar);

#ifdef __cplusplus
}
#endif

#endif // BROCCOLI_H_INCLUDED
