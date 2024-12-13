
typedef unsigned short U16;
typedef int I32;
typedef float F32;
typedef struct { int idx; } GLBuffer;
typedef struct { int idx; } GLShaderProgram;

GLBuffer glBufferF32StaticDraw(F32* data, I32 count);
GLBuffer glBufferU16ElementsStaticDraw(U16* data, I32 count);

GLShaderProgram glShaderProgram(
  const char* vertex_shader_source, I32 vertex_shader_source_length,
  const char* fragment_shader_source, I32 fragment_shader_source_length);

void glBufferBind(GLBuffer buffer);
void glBufferElementsBind(GLBuffer buffer);
void glShaderProgramUse(GLShaderProgram program);

void glClearWithColor(F32 r, F32 g, F32 b, F32 a);
void glDrawElementsU16Triangles(I32 count, I32 offset);

const F32 vertex_data[] = {-0.2, 0.5, 0.0, -0.5, -0.4, 0.0, 0.5, -0.1, 0.0};
const I32 vertex_count = sizeof(vertex_data) / sizeof(F32);

const U16 index_data[] = {0, 1, 2};
const I32 index_count = sizeof(index_data) / sizeof(U16);

const char vertex_shader_source[] = " \
  attribute vec3 vertex_position; \
  void main(void) { \
    gl_Position = vec4(vertex_position, 1.0); \
  }";

const char fragment_shader_source[] = " \
  void main(void) { \
    gl_FragColor = vec4(1.0, 0.5, 0.313, 1.0); \
  }";

int start(int x) {
  GLBuffer vertex_buffer = glBufferF32StaticDraw(vertex_data, vertex_count);
  GLBuffer index_buffer = glBufferU16ElementsStaticDraw(index_data, index_count);
  GLShaderProgram program = glShaderProgram(
    vertex_shader_source, sizeof(vertex_shader_source),
    fragment_shader_source, sizeof(fragment_shader_source));

  glClearWithColor(0.37, 0.31, 0.86, 1.0);

  glBufferBind(vertex_buffer);
  glBufferElementsBind(index_buffer);
  glShaderProgramUse(program);

  glDrawElementsU16Triangles(index_count, 0);

  return x;
}