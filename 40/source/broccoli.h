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

BQ_API void bq_log(const char* format, ...);
BQ_API int  bq_get_ticks();
BQ_API int  bq_init(const char* title,int width,int height);
BQ_API int  bq_process();

BQ_API int  bq_create_texture(int width,int height,const void* data);
BQ_API void bq_destroy_texture(const int texture_id);
BQ_API void bq_bind_texture(const int texture_id);
BQ_API void bq_draw_triangles(int count,const v2* positions,const v2* texcoords);

BQ_API int  bq_create_sound(int channels,int samples,const void* data);
BQ_API void bq_destroy_sound(const int sound_id);
BQ_API void bq_play_sound(const int sound_id,float volume);

#ifdef __cplusplus
}
#endif

#endif // BROCCOLI_H_INCLUDED
