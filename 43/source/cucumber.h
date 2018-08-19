// cucumber.h

#ifndef CUCUMBER_H_INCLUDED
#define CUCUMBER_H_INCLUDED

#ifdef _WIN32
# define CUCUMBER_API __declspec(dllexport)
#endif

// config
#define QU_MAX_VERTEX_ATTRIBUTES    8
#define QU_MAX_TEXTURE_BINDINGS     4
#define QU_MAX_UNIFORM_BINGINGS     16

#ifdef __cplusplus
extern "C" {
#endif

typedef void         qu_void_t;
typedef int          qu_int32_t;
typedef long long    qu_int64_t;
typedef enum { QU_FALSE, QU_TRUE, } qu_bool32_t;

CUCUMBER_API qu_int64_t 
qu_get_ticks();

CUCUMBER_API qu_bool32_t 
qu_init(int width, int height, const char *title);

CUCUMBER_API qu_bool32_t 
qu_process();

// render api
typedef struct qu_shader_t { unsigned int id; } qu_shader_t; 
typedef struct qu_texture_t { unsigned int id; } qu_texture_t;
typedef struct qu_buffer_t { unsigned int id; } qu_buffer_t;

typedef enum 
{ 
   QU_TEXTURE_FORMAT_RGB8, 
   QU_TEXTURE_FORMAT_RGBA8, 
} qu_texture_format_t;

typedef enum 
{
   QU_TEXTURE_FILTER_NEAREST,
   QU_TEXTURE_FILTER_LINEAR,
} qu_texture_filter_t;

typedef enum 
{
   QU_TEXTURE_ADDRESS_CLAMP,
   QU_TEXTURE_ADDRESS_REPEAT,
   QU_TEXTURE_ADDRESS_MIRROR,
} qu_texture_address_t;

typedef enum 
{
   QU_BUFFER_TYPE_VERTEX,
   QU_BUFFER_TYPE_INDEX,
} qu_buffer_type_t;

typedef enum
{
   QU_BUFFER_ACCESS_STATIC,
   QU_BUFFER_ACCESS_DYNAMIC,
} qu_buffer_access_t;

typedef enum
{
   QU_BLEND_EQ_ADD,
   QU_BLEND_EQ_SUBTRACT,
   QU_BLEND_EQ_REVERSE_SUBTRACT,
   QU_BLEND_EQ_MIN,
   QU_BLEND_EQ_MAX,
} qu_blend_eq_t;

typedef enum 
{
   QU_BLEND_FACTOR_ZERO,
   QU_BLEND_FACTOR_ONE,
   QU_BLEND_FACTOR_SRC_COLOR,
   QU_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
   QU_BLEND_FACTOR_DST_COLOR,
   QU_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
   QU_BLEND_FACTOR_SRC_ALPHA,
   QU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
   QU_BLEND_FACTOR_DST_ALPHA,
   QU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
   QU_BLEND_FACTOR_CONSTANT_COLOR,
   QU_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
   QU_BLEND_FACTOR_CONSTANT_ALPHA,
   QU_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
   QU_BLEND_FACTOR_SRC_ALPHA_SATURATE,
} qu_blend_factor_t;

typedef struct 
{
   qu_bool32_t enabled;
   qu_blend_eq_t eq_rgb;
   qu_blend_factor_t src_rgb;
   qu_blend_factor_t dst_rgb;
   qu_blend_eq_t eq_alpha;
   qu_blend_factor_t src_alpha;
   qu_blend_factor_t dst_alpha;
} qu_blend_state_t;

typedef enum 
{
   QU_COMPARE_FUNC_NEVER,
   QU_COMPARE_FUNC_LESS,
   QU_COMPARE_FUNC_EQUAL,
   QU_COMPARE_FUNC_LEQUAL,
   QU_COMPARE_FUNC_GREATER,
   QU_COMPARE_FUNC_NOTEQUAL,
   QU_COMPARE_FUNC_GEQUAL,
   QU_COMPARE_FUNC_ALWAYS,
} qu_compare_func_t;

typedef enum
{
   QU_STENCIL_OP_KEEP,
   QU_STENCIL_OP_ZERO,
   QU_STENCIL_OP_REPLACE,
   QU_STENCIL_OP_INCR,
   QU_STENCIL_OP_INCR_WRAP,
   QU_STENCIL_OP_DECR,
   QU_STENCIL_OP_DECR_WRAP,
   QU_STENCIL_OP_INVERT,
} qu_stencil_op_t;

typedef struct 
{
   qu_bool32_t depth_write_enabled;   // glDepthMask - enable or disable writing into the depth buffer
   qu_bool32_t depth_read_enabled;    // GL_DEPTH_TEST - enable or disable depth testing/reading 
   float depth_range_near; // -1.0f
   float depth_range_far;  // 1.0f
   qu_compare_func_t depth_func;
   qu_bool32_t stencil_write_enabled;
   qu_bool32_t stencil_read_enabled;
   int stencil_mask;    // 0 - 2^n-1
   int stencil_ref;     // 0 - 2^n-1
   qu_compare_func_t stencil_func;
   qu_stencil_op_t stencil_fail; // http://docs.gl/gl3/glStencilOp
   qu_stencil_op_t stencil_depth_fail;
   qu_stencil_op_t stencil_depth_pass;
} qu_depth_stencil_state_t;

typedef enum
{
   QU_CULL_MODE_NONE,
   QU_CULL_MODE_BACK,
   QU_CULL_MODE_FRONT,
   QU_CULL_MODE_BOTH,
} qu_cull_mode_t;

typedef enum 
{
   QU_FRONT_FACE_CCW,
   QU_FRONT_FACE_CW,
} qu_front_face_t;

typedef struct 
{
   qu_cull_mode_t cull_mode;
   qu_front_face_t front_face;
} qu_rasterizer_state_t;

typedef enum 
{
   QU_PRIMITIVE_TOPOLOGY_POINT_LIST,
   QU_PRIMITIVE_TOPOLOGY_LINE_LIST,
   QU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
} qu_primitive_topology_t;

typedef enum 
{
   QU_ATTRIBUTE_TYPE_FLOAT,
   QU_ATTRIBUTE_TYPE_BYTE,
} qu_attribute_type_t;

typedef struct 
{
   int index;
   int size;
   int offset;
   qu_attribute_type_t type;
   qu_bool32_t normalized;
} qu_vertex_attribute_t;

typedef struct
{
   int stride;
   int attribute_count;
   qu_vertex_attribute_t attributes[QU_MAX_VERTEX_ATTRIBUTES];
} qu_vertex_format_t;

CUCUMBER_API void 
qu_render_create_shader(qu_shader_t *shader, 
                        const char *vertex_shader_source, 
                        const char *fragment_shader_source);

CUCUMBER_API void 
qu_render_destroy_shader(qu_shader_t shader);

CUCUMBER_API void
qu_render_get_attribute_locations(qu_shader_t shader, int count,
                                  const char **names, int *locations);

CUCUMBER_API void
qu_render_get_uniform_locations(qu_shader_t shader, int count, 
                                const char **names, int *locations);

CUCUMBER_API void
qu_render_create_texture(qu_texture_t *texture, 
                         qu_texture_format_t format, qu_texture_filter_t filter, 
                         qu_texture_address_t addr_u, qu_texture_address_t addr_v,
                         int width, int height, const void *data);

CUCUMBER_API void
qu_render_destroy_texture(qu_texture_t texture);

CUCUMBER_API void
qu_render_update_texture(qu_texture_t texture, qu_texture_format_t format, 
                         int width, int height, const void *data);

CUCUMBER_API void
qu_render_create_buffer(qu_buffer_t *buffer, 
                        qu_buffer_type_t type, qu_buffer_access_t access,
                        int size, const void *data);

CUCUMBER_API void
qu_render_destroy_buffer(qu_buffer_t buffer);

CUCUMBER_API void
qu_render_update_buffer(qu_buffer_t buffer, qu_buffer_type_t type, 
                        int size, const void *data);

CUCUMBER_API void
qu_render_add_attribute(qu_vertex_format_t *format, int index, int size,
                        qu_attribute_type_t type, qu_bool32_t normalized);

typedef struct 
{
   int x, y;
   int width;
   int height;
} qu_viewport_t;

typedef enum
{
   QU_UNIFORM_TYPE_FLOAT,
   QU_UNIFORM_TYPE_INT,
   QU_UNIFORM_TYPE_BOOL,
   QU_UNIFORM_TYPE_SAMPLER,
   QU_UNIFORM_TYPE_MATRIX,
} qu_uniform_type_t;

typedef struct 
{
   qu_uniform_type_t type;
   int location;
   int size;
   void *data;
} qu_uniform_t;

typedef struct 
{
   qu_shader_t program;
   qu_blend_state_t blend_state;
   qu_depth_stencil_state_t depth_stencil_state;
   qu_rasterizer_state_t rasterizer_state;
} qu_pipeline_t;

typedef struct 
{
   qu_vertex_format_t vertex_format;
   qu_buffer_t vertex_buffer;
   qu_buffer_t index_buffer;
} qu_assembly_t;

typedef struct 
{
   int texture_count;
   qu_texture_t textures[QU_MAX_TEXTURE_BINDINGS];
   int uniform_count;
   qu_uniform_t uniforms[QU_MAX_UNIFORM_BINGINGS];
} qu_bindings_t;

typedef struct 
{
   qu_primitive_topology_t primitive_topology;
   int vertex_count;
   int instance_count;
   int index_buffer_offset;
   int vertex_buffer_offset;
} qu_drawcall_t;

CUCUMBER_API void
qu_render_add_texture(qu_bindings_t *bindings, qu_texture_t texture);

CUCUMBER_API void
qu_render_add_uniform(qu_bindings_t *bindings, qu_uniform_type_t type,
                      int location, int size, const void *data);

CUCUMBER_API void
qu_render_bind_pipeline(qu_pipeline_t *pipeline, qu_viewport_t *viewport);

CUCUMBER_API void
qu_render_bind_assembly(qu_assembly_t *assembly);

CUCUMBER_API void
qu_render_bind_bindings(qu_bindings_t *bindings);

CUCUMBER_API void
qu_render_submit_drawcall(qu_drawcall_t *drawcall);

#ifdef __cplusplus
}
#endif

#endif // CUCUMBER_H_INCLUDED
