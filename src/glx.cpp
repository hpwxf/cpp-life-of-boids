#include "glx.hpp"
#include <glad/glad.h>

VertexArray VertexArray_new() {
  GLuint vertex_array{};
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);

  return VertexArray{ vertex_array };
}

void VertexArray_bind(VertexArray& vertexArray) {
  glBindVertexArray(vertexArray.vertex_array);
}

Buffer Buffer_new() {
  GLuint vertex_buffer{};
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

  return Buffer { vertex_buffer };
}

void Buffer_bind(Buffer& buffer, GLenum target) {
  glBindBuffer(target, buffer.vertex_buffer);
}

ShaderProgram ShaderProgram_new(const char* vertex_shader_text, const char* fragment_shader_text) {
  // TODO Manage errors
  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);

  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  return ShaderProgram{ program };
}

void ShaderProgram_activate(ShaderProgram& shaderProgram) {
  glUseProgram(shaderProgram.program);
}

GLint ShaderProgram_getAttribLocation(ShaderProgram & shaderProgram, const char* name) {
  const GLint loc = glGetAttribLocation(shaderProgram.program, name);
  return loc;
}

GLint ShaderProgram_getUniformLocation(ShaderProgram & shaderProgram, const char* name) {
  const GLint loc = glGetUniformLocation(shaderProgram.program, name);
  return loc;
}
