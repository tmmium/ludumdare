// main.cc

#include <math.h>

#pragma comment(lib, "cucumber.lib")
#include "cucumber.h"

int __stdcall WinMain(void *,void *,char *,int)
{
   if (qu_init(1280, 720, "ludumdare"))
   {
      qu_shader_t shader = {};
      {
         const char *vertex_shader_source = 
            "#version 330\n"
            "layout(location = 0) in vec3 in_vertex;\n"
            "layout(location = 1) in vec2 in_texcoord;\n"
            "uniform mat4 u_proj;\n"
            "uniform mat4 u_view;\n"
            "uniform mat4 u_world;\n"
            "out vec2 v_texcoord;\n"
            "void main() {\n"
            "  gl_Position = u_proj * u_view * u_world * vec4(in_vertex, 1);\n"
            "  v_texcoord = in_texcoord;\n"
            "}";
         const char *fragment_shader_source = 
            "#version 330\n"
            "uniform sampler2D u_diffuse;\n"
            "in vec2 v_texcoord;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  frag_color = texture(u_diffuse, v_texcoord);\n"
            "}";

         qu_render_create_shader(&shader, vertex_shader_source, fragment_shader_source);
      }

      qu_blend_state_t blend_state = {};
      blend_state.enabled = QU_TRUE;
      blend_state.eq_rgb = QU_BLEND_EQ_ADD;
      blend_state.src_rgb = QU_BLEND_FACTOR_SRC_ALPHA;
      blend_state.dst_rgb = QU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      blend_state.eq_alpha = QU_BLEND_EQ_ADD;
      blend_state.src_alpha = QU_BLEND_FACTOR_ONE;
      blend_state.dst_alpha = QU_BLEND_FACTOR_ONE;

      qu_depth_stencil_state_t depth_stencil_state = {};
      depth_stencil_state.depth_write_enabled = QU_TRUE;
      depth_stencil_state.depth_read_enabled = QU_TRUE;
      depth_stencil_state.depth_range_near = -1.0f;
      depth_stencil_state.depth_range_far = 1.0f;
      depth_stencil_state.depth_func = QU_COMPARE_FUNC_LESS;

      qu_rasterizer_state_t rasterizer_state = {};
      rasterizer_state.cull_mode = QU_CULL_MODE_NONE;
      rasterizer_state.front_face = QU_FRONT_FACE_CW;

      const char *attribute_names[2] =
      {
         "in_vertex",
         "in_texcoord",
      };
      int attribute_locations[2] = {};
      qu_render_get_attribute_locations(shader, 2, attribute_names, attribute_locations);

      qu_vertex_format_t vertex_format = {};
      qu_render_add_attribute(&vertex_format, 
                              attribute_locations[0],
                              3,
                              QU_ATTRIBUTE_TYPE_FLOAT,
                              QU_FALSE);
      qu_render_add_attribute(&vertex_format, 
                              attribute_locations[1],
                              2,
                              QU_ATTRIBUTE_TYPE_FLOAT,
                              QU_FALSE);

      const float ww = 1280.0f;
      const float hh = 720.0f;
      const float zn = -1.0f;
      const float zf = 1.0f;

      const float xx = 2.0f / ww;
      const float yy = 2.0f / -hh;
      const float zz = 1.0f / (zf - zn);
      const float wx = -1.0f; 
      const float wy = 1.0f; 
      const float wz = zn / (zn - zf); 

      const float orthographic[] =
      {
         xx  , 0.0f, 0.0f, 0.0f,
         0.0f, yy  , 0.0f, 0.0f,
         0.0f, 0.0f, zz  , 0.0f,
         wx  , wy  , wz  , 1.0f,
      };

      const float aspect = 1280.0f / 720.0f;
      const float fov = 3.141592f * 0.25f;
      const float znear = 0.5f;
      const float zfar = 100.0f;
      const float ph = 1.0f / tanf(fov * 0.5f);
      const float pw = ph / aspect;

      const float pxx = pw;
      const float pyy = ph;
      const float pzz = zfar / (zfar - znear);
      const float pzw = 1.0f;
      const float pwz = (-znear * zfar) / (zfar - znear);
      const float pww = 0.0f;

      const float perspective[] =
      {
         pxx , 0.0f, 0.0f, 0.0f,
         0.0f, pyy , 0.0f, 0.0f,
         0.0f, 0.0f, pzz , pzw ,
         0.0f, 0.0f, pwz , pww ,
      };

      const float identity[] =
      {
         1.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f,
      };

      float world[] =
      {
         1.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,10.0f, 1.0f,
      };
      
      const char *uniform_names[] =
      {
         "u_proj",
         "u_view",
         "u_world",
         "u_diffuse",
      };
      int uniform_locations[4] = {};
      qu_render_get_uniform_locations(shader, 4, uniform_names, uniform_locations);

      float vertices[] =
      {
         -1.0f, 2.0f, 0.0f, 0.0f, 0.0f, 
          1.0f, 2.0f, 0.0f, 1.0f, 0.0f,
          1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
         -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      };

      qu_buffer_t vertex_buffer = {};
      qu_render_create_buffer(&vertex_buffer, 
                              QU_BUFFER_TYPE_VERTEX, 
                              QU_BUFFER_ACCESS_STATIC, 
                              sizeof(vertices), 
                              vertices);

      unsigned short indices[] = { 0, 1, 2, 2, 3, 0 };
      qu_buffer_t index_buffer = {};
      qu_render_create_buffer(&index_buffer, 
                              QU_BUFFER_TYPE_INDEX, 
                              QU_BUFFER_ACCESS_STATIC, 
                              sizeof(indices), 
                              indices);

      unsigned int bitmap[] =
      {
         0x66ffffff, 0xff0000ff,
         0xff00ff00, 0xffff0000,
      };
      qu_texture_t texture = {};
      qu_render_create_texture(&texture, 
                               QU_TEXTURE_FORMAT_RGBA8,
                               QU_TEXTURE_FILTER_NEAREST,
                               QU_TEXTURE_ADDRESS_CLAMP,
                               QU_TEXTURE_ADDRESS_CLAMP,
                               2, 2, bitmap);

      qu_viewport_t viewport = { 0, 0, 1280, 720 };

      qu_pipeline_t pipeline = {};
      pipeline.program = shader;
      pipeline.blend_state = blend_state;
      pipeline.depth_stencil_state = depth_stencil_state;
      pipeline.rasterizer_state = rasterizer_state;

      qu_assembly_t assembly = {};
      assembly.vertex_format = vertex_format;
      assembly.vertex_buffer = vertex_buffer;
      assembly.index_buffer = index_buffer;

      int diffuse_sampler_index = 0;
      qu_bindings_t bindings = {};
      qu_render_add_texture_binding(&bindings, texture);
      qu_render_add_uniform_binding(&bindings, 
                                    QU_UNIFORM_TYPE_MATRIX, 
                                    uniform_locations[0], // u_proj
                                    1,
                                    perspective);
      qu_render_add_uniform_binding(&bindings, 
                                    QU_UNIFORM_TYPE_MATRIX, 
                                    uniform_locations[1], // u_view
                                    1,
                                    identity);
      qu_render_add_uniform_binding(&bindings, 
                                    QU_UNIFORM_TYPE_MATRIX, 
                                    uniform_locations[2], // u_world
                                    1,
                                    world);
      qu_render_add_uniform_binding(&bindings, 
                                    QU_UNIFORM_TYPE_SAMPLER, 
                                    uniform_locations[3], // u_diffuse
                                    1,
                                    &diffuse_sampler_index);

      qu_drawcall_t drawcall = {};
      drawcall.primitive_topology = QU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      drawcall.vertex_count = 6;
      drawcall.instance_count = 1;
      drawcall.index_buffer_offset = 0;
      drawcall.vertex_buffer_offset = 0;

      float time = 0.0f;
      while (qu_process())
      {
         time += 1.0f / 60.0f;
         world[12] = 0.0f;
         world[13] = 0.0f;
         world[14] = 10.0f + sinf(time) * 4.0f;

         qu_render_bind_pipeline(&pipeline, &viewport);
         qu_render_bind_assembly(&assembly);
         qu_render_bind_bindings(&bindings);
         qu_render_submit_drawcall(&drawcall);

         world[12] = 0.5f + cosf(time) * 2.0f;
         world[13] = -0.5f;
         world[14] = 10.0f - sinf(time) * 4.0f;
         qu_render_bind_bindings(&bindings);
         qu_render_submit_drawcall(&drawcall);
      }
   }

   return 0;
}
