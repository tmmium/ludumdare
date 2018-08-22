// cucumber_render_api.c

#define ENABLE_OPENGL_ERROR_CHECK 1

#if ENABLE_OPENGL_ERROR_CHECK
typedef struct
{
   const char *string;
   const GLenum code;
} qu_opengl_error_t;

static qu_opengl_error_t qu_opengl_error_strings[] =
{
   { "GL_NO_ERROR", GL_NO_ERROR }, 
   { "GL_INVALID_ENUM", GL_INVALID_ENUM }, 
   { "GL_INVALID_VALUE", GL_INVALID_VALUE }, 
   { "GL_INVALID_OPERATION", GL_INVALID_OPERATION }, 
   { "GL_OUT_OF_MEMORY", GL_OUT_OF_MEMORY }, 
   { "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT },
   { "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT },
   { "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER },
   { "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER },
   { "GL_FRAMEBUFFER_UNSUPPORTED", GL_FRAMEBUFFER_UNSUPPORTED },
};

static const char *qu_opengl_error_string(GLenum err)
{
   for (int i = 0; i < ARRAYCOUNT(qu_opengl_error_strings); i++)
   {
      if (qu_opengl_error_strings[i].code == err)
      {
         return qu_opengl_error_strings[i].string;
      }
   }

   return "GL_UNKNOWN_ERROR";
}

static void qu_opengl_error_check()
{
   GLenum err = glGetError();
   if (err != GL_NO_ERROR) 
   {
      const char *error_type = qu_opengl_error_string(err); 
      ASSERT(!"opengl error code!");
   }
}
#else
#define qu_opengl_error_check() 
#endif

static const GLenum gl_texture_format_internal[] = 
{ 
   GL_RGB, 
   GL_RGBA, 
};

static const GLenum gl_texture_format[] = 
{ 
   GL_RGB, 
   GL_RGBA, 
};

static const GLenum gl_texture_filter[] = 
{ 
   GL_NEAREST,
   GL_LINEAR,
   GL_NEAREST_MIPMAP_NEAREST,
   GL_NEAREST_MIPMAP_LINEAR,
   GL_LINEAR_MIPMAP_NEAREST,
   GL_LINEAR_MIPMAP_LINEAR,
};

static const GLenum gl_texture_address[] = 
{ 
   GL_CLAMP_TO_EDGE, 
   GL_REPEAT, 
   GL_MIRRORED_REPEAT 
};

static const GLenum gl_blend_eq[] = 
{ 
   GL_FUNC_ADD, 
   GL_FUNC_SUBTRACT, 
   GL_FUNC_REVERSE_SUBTRACT, 
   GL_MIN, 
   GL_MAX, 
};

static const GLenum gl_blend_ft[] = 
{ 
   GL_ZERO,
   GL_ONE,
   GL_SRC_COLOR,
   GL_ONE_MINUS_SRC_COLOR,
   GL_DST_COLOR,
   GL_ONE_MINUS_DST_COLOR,
   GL_SRC_ALPHA,
   GL_ONE_MINUS_SRC_ALPHA,
   GL_DST_ALPHA,
   GL_ONE_MINUS_DST_ALPHA,
   GL_CONSTANT_COLOR,
   GL_ONE_MINUS_CONSTANT_COLOR,
   GL_CONSTANT_ALPHA,
   GL_ONE_MINUS_CONSTANT_ALPHA,
   GL_SRC_ALPHA_SATURATE,
};

static const GLenum gl_compare_func[] = 
{
   GL_NEVER,
   GL_LESS,
   GL_EQUAL,
   GL_LEQUAL,
   GL_GREATER,
   GL_NOTEQUAL,
   GL_GEQUAL,
   GL_ALWAYS,
};

static const GLenum gl_stencil_op[] = 
{
   GL_KEEP,
   GL_ZERO,
   GL_REPLACE,
   GL_INCR,
   GL_INCR_WRAP,
   GL_DECR,
   GL_DECR_WRAP,
   GL_INVERT,
};

static const GLenum gl_cull_mode[] =
{
   GL_NONE,
   GL_BACK,
   GL_FRONT,
   GL_FRONT_AND_BACK,
};

static const GLenum gl_front_face[] =
{
   GL_CCW,
   GL_CW,
};

static const GLenum gl_primitive_topology[] = 
{
   GL_POINTS,
   GL_LINES,
   GL_TRIANGLES,
};

static const GLenum gl_attribute_type[] = 
{
   GL_FLOAT,
   GL_UNSIGNED_BYTE,
};

static const GLuint gl_attribute_size[] =
{
   sizeof(float),
   sizeof(char),
};

static const GLenum gl_uniform_type[] = 
{
   GL_FLOAT,
   GL_INT,
   GL_BOOL,
   GL_SAMPLER_2D,
   GL_FLOAT_MAT4,
};

static const GLuint gl_uniform_size[] =
{
   sizeof(float),
   sizeof(int),
   sizeof(int),
   sizeof(int),
   sizeof(float) * 16,
};

const GLenum gl_buffer_type[] = 
{ 
   GL_ARRAY_BUFFER, 
   GL_ELEMENT_ARRAY_BUFFER, 
};

const GLenum gl_buffer_access[] = 
{ 
   GL_STATIC_DRAW, 
   GL_STREAM_DRAW, 
};

void 
qu_render_create_shader(qu_shader_t *shader, 
   const char *vertex_shader_source, 
   const char *fragment_shader_source)
{
   GLuint vid = glCreateShader(GL_VERTEX_SHADER);  
   glShaderSource(vid, 1, &vertex_shader_source, NULL);
   glCompileShader(vid);

   GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);  
   glShaderSource(fid, 1, &fragment_shader_source, NULL);
   glCompileShader(fid);

   GLuint pid = glCreateProgram();
   glAttachShader(pid, vid);
   glAttachShader(pid, fid);
   glLinkProgram(pid);

   GLint link_status = 0;
   glGetProgramiv(pid, GL_LINK_STATUS, &link_status);
   if (link_status == GL_FALSE)
   {
      GLchar vertex_error[1024];
      GLchar fragment_error[1024];
      GLchar program_error[1024];

      glGetShaderInfoLog(vid, sizeof(vertex_error), NULL, vertex_error);
      glGetShaderInfoLog(fid, sizeof(fragment_error), NULL, fragment_error);
      glGetProgramInfoLog(pid, sizeof(program_error), NULL, program_error);
   
      ASSERT(!"shader program error");

      glDetachShader(pid, vid);   
      glDetachShader(pid, fid);   
      glDeleteProgram(pid);
   }
   else 
   {
      shader->id = pid;
   }

   glDeleteShader(vid);
   glDeleteShader(fid);

   qu_opengl_error_check();
}

void 
qu_render_destroy_shader(qu_shader_t shader) 
{
   glDeleteProgram(shader.id);
   qu_opengl_error_check();
}

void
qu_render_get_attribute_locations(qu_shader_t shader, int count,
   const char **names, int *locations)
{
   glUseProgram(shader.id);
   for (int i = 0; i < count; i++)
      locations[i] = glGetAttribLocation(shader.id, names[i]);
   
   qu_opengl_error_check();
}

void
qu_render_get_uniform_locations(qu_shader_t shader, int count, 
   const char **names, int *locations)
{
   glUseProgram(shader.id);
   for (int i = 0; i < count; i++)
      locations[i] = glGetUniformLocation(shader.id, names[i]);
   
   qu_opengl_error_check();
}

void
qu_render_create_texture(qu_texture_t *texture, 
   qu_texture_format_t format, qu_texture_filter_t filter, 
   qu_texture_address_t addr_u, qu_texture_address_t addr_v,
   int width, int height, const void *data)
{
   GLuint id = 0;
   glGenTextures(1, &id);
   glBindTexture(GL_TEXTURE_2D, id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   gl_texture_filter[filter]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   filter == QU_TEXTURE_FILTER_NEAREST ? GL_NEAREST : GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_texture_address[addr_u]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_texture_address[addr_v]);
   glTexImage2D(GL_TEXTURE_2D, 
                0, // mip level
                gl_texture_format_internal[format],
                width, 
                height, 
                0,
                gl_texture_format[format],
                GL_UNSIGNED_BYTE, data);

   texture->id = id;

   qu_opengl_error_check();
}

void
qu_render_destroy_texture(qu_texture_t texture) 
{
   glDeleteTextures(1, &texture.id);
   qu_opengl_error_check();
}

void
qu_render_update_texture(qu_texture_t texture, int width, 
                         qu_texture_format_t format, 
                         int height, const void *data)
{
   GLuint id = texture.id;
   const GLenum formats[]   = { GL_RGB, GL_RGBA, };

   glBindTexture(GL_TEXTURE_2D, id);
   glTexImage2D(GL_TEXTURE_2D, 0, formats[format], width, height, 
      0, formats[format], GL_UNSIGNED_BYTE, data);
   
   qu_opengl_error_check();
}

void
qu_render_create_buffer(qu_buffer_t *buffer, 
                        qu_buffer_type_t type, qu_buffer_access_t access,
                        int size, const void *data)
{

   GLuint id = 0;
   glGenBuffers(1, &id);
   glBindBuffer(gl_buffer_type[type], id);
   glBufferData(gl_buffer_type[type], size, data, gl_buffer_access[access]);

   buffer->id = id;

   qu_opengl_error_check();
}

void
qu_render_destroy_buffer(qu_buffer_t buffer) 
{
   glDeleteBuffers(1, &buffer.id);
   qu_opengl_error_check();
}

void
qu_render_update_buffer(qu_buffer_t buffer, qu_buffer_type_t type, 
   int size, const void *data) 
{
   const GLenum types[]    = { GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, };

   glBindBuffer(types[type], buffer.id);
   glBufferSubData(types[type], 0, size, data);
   qu_opengl_error_check();
}

void
qu_render_add_attribute(qu_vertex_format_t *format, int index, int size,
                        qu_attribute_type_t type, qu_bool32_t normalized)
{
   ASSERT(format->attribute_count < QU_MAX_VERTEX_ATTRIBUTES);

   qu_vertex_attribute_t *attribute = format->attributes + format->attribute_count++;
   attribute->index = index;
   attribute->size = size;
   attribute->offset = format->stride;
   attribute->type = type;
   attribute->normalized = normalized;

   format->stride += gl_attribute_size[type] * size;
}

void
qu_render_create_framebuffer(qu_framebuffer_t *framebuffer, int width, int height)
{
   ASSERT(!"impl");
}

void
qu_render_destroy_framebuffer(qu_framebuffer_t *framebuffer)
{
   ASSERT(!"impl");
}

void
qu_render_bind_framebuffer(qu_framebuffer_t *framebuffer)
{
   ASSERT(!"impl");
}

void
qu_render_add_texture_binding(qu_bindings_t *bindings, qu_texture_t texture)
{
   ASSERT(bindings->texture_count < QU_MAX_TEXTURE_BINDINGS);

   bindings->textures[bindings->texture_count++] = texture;
}

//#pragma warning(push)
//#pragma warning(disable: 4090) // : '=': different 'const' qualifiers
void
qu_render_add_uniform_binding(qu_bindings_t *bindings, qu_uniform_type_t type,
                              int location, int size, const void *data)
{
   ASSERT(bindings->uniform_count < QU_MAX_UNIFORM_BINGINGS);

   qu_uniform_t *uniform = bindings->uniforms + bindings->uniform_count++;
   uniform->type = type;
   uniform->location = location;
   uniform->size = size;
   uniform->data = data;
}
//#pragma warning(pop)

void
qu_render_bind_pipeline(qu_pipeline_t *pipeline, qu_viewport_t *viewport)
{
   glViewport(viewport->x, viewport->y, viewport->width, viewport->height);
   glUseProgram(pipeline->program.id);
   
   qu_blend_state_t *bs = &pipeline->blend_state;
   if (bs->enabled)
   {
      glEnable(GL_BLEND);
      glBlendFuncSeparate(gl_blend_ft[bs->src_rgb],
                          gl_blend_ft[bs->dst_rgb],
                          gl_blend_ft[bs->src_alpha],
                          gl_blend_ft[bs->dst_alpha]);
      glBlendEquationSeparate(gl_blend_eq[bs->eq_rgb],
                              gl_blend_eq[bs->eq_alpha]);
   }
   else 
   {
      glDisable(GL_BLEND);
   }

   qu_depth_stencil_state_t *dss = &pipeline->depth_stencil_state;
   if (dss->depth_write_enabled)
      glDepthMask(GL_TRUE);
   else 
      glDepthMask(GL_FALSE);

   if (dss->depth_read_enabled)
      glEnable(GL_DEPTH_TEST);
   else 
      glDisable(GL_DEPTH_TEST);

   glDepthRange(dss->depth_range_near, dss->depth_range_far);
   glDepthFunc(gl_compare_func[dss->depth_func]);
   
   // todo: stencil stuff...

   qu_rasterizer_state_t *rs = &pipeline->rasterizer_state;
   if (rs->cull_mode != QU_CULL_MODE_NONE)
   {
      glEnable(GL_CULL_FACE);
      glCullFace(gl_cull_mode[rs->cull_mode]);
   }
   else 
   {
      glDisable(GL_CULL_FACE);
   }

   glFrontFace(gl_front_face[rs->front_face]);

   qu_opengl_error_check();
}

void
qu_render_bind_assembly(qu_assembly_t *assembly)
{
   // vertex_buffer
   qu_buffer_t *vb = &assembly->vertex_buffer;
   glBindBuffer(GL_ARRAY_BUFFER, vb->id);

   qu_buffer_t *ib = &assembly->index_buffer;
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);

   qu_vertex_format_t *vf = &assembly->vertex_format;
   int stride = vf->stride;
   int count = vf->attribute_count;
   for (int i = 0; i < count; i++)
   {
      qu_vertex_attribute_t *va = vf->attributes + i;

      glEnableVertexAttribArray(va->index);
      glVertexAttribPointer(va->index,
                            va->size, 
                            gl_attribute_type[va->type],
                            va->normalized ? GL_TRUE : GL_FALSE,
                            stride,
                            (const GLvoid *)(uintptr_t)va->offset);
   }

   qu_opengl_error_check();
}

void
qu_render_bind_bindings(qu_bindings_t *bindings)
{
   qu_texture_t *textures = bindings->textures;
   for (int i = 0; i < 4; i++)
   {
      qu_texture_t *texture = textures + i;

      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, texture->id);
   }

   int uniform_count = bindings->uniform_count;
   qu_uniform_t *uniforms = bindings->uniforms;
   for (int i = 0; i < uniform_count; i++)
   {
      qu_uniform_t *uniform = uniforms + i;
      
      switch (uniform->type)
      {
         case QU_UNIFORM_TYPE_FLOAT:
         {
            glUniform1fv(uniform->location, uniform->size, (const GLfloat *)uniform->data);
         } break;
         case QU_UNIFORM_TYPE_INT:
         case QU_UNIFORM_TYPE_BOOL:
         case QU_UNIFORM_TYPE_SAMPLER:
         {
            glUniform1iv(uniform->location, uniform->size, (const GLint *)uniform->data);
         } break;
         case QU_UNIFORM_TYPE_MATRIX:
         {
            glUniformMatrix4fv(uniform->location,
                               uniform->size, 
                               GL_FALSE, 
                               (const GLfloat *)uniform->data);
         } break;
      }
   }

   qu_opengl_error_check();
}

void
qu_render_submit_drawcall(qu_drawcall_t *drawcall)
{
   qu_primitive_topology_t primitive_topology = drawcall->primitive_topology;
   
   int vertex_count = drawcall->vertex_count;
   int instance_count = drawcall->instance_count; // note: always 1 for now
   int index_buffer_offset = drawcall->index_buffer_offset;
   int vertex_buffer_offset = drawcall->vertex_buffer_offset;

   if (index_buffer_offset >= 0)
   {
      glDrawElements(gl_primitive_topology[primitive_topology],
                     vertex_count,
                     GL_UNSIGNED_SHORT,
                     (const GLvoid *)(uintptr_t)index_buffer_offset);
   }
   else 
   {
      glDrawArrays(gl_primitive_topology[primitive_topology],
                   vertex_buffer_offset,
                   vertex_count);
   }

   qu_opengl_error_check();
}
