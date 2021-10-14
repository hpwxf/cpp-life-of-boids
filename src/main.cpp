#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using vec2 = std::array<float, 2>;
using vec3 = std::array<float, 3>;
using vec4 = std::array<float, 4>;
using mat3x3 = std::array<vec3, 3>;
using mat4x4 = std::array<vec4, 4>;

#include "glx.hpp"
#include "shaders/lines.hpp"
#include "shaders/points.hpp"
#include "shaders/triangle.hpp"

static void error_callback(int error, const char* description) {
  std::cerr << "Error[" << error << "]: " << description << "\n";
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    std::puts("Export current display");
    char export_filename[] = "export.png";
    saveImage(export_filename, window);
  }
}

int main() {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  // FIXME: OpenGL error checks have been omitted for brevity

  // triangle part
  // new
  ShaderProgram triangle_shaderProgram
      = ShaderProgram_new(triangle::vertex_shader_text, triangle::fragment_shader_text);
  VertexArray triangle_vertexArray = VertexArray_new();
  Buffer triangle_buffer = Buffer_new();
  // init
  VertexArray_bind(triangle_vertexArray);
  Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
  ShaderProgram_activate(triangle_shaderProgram);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle::vertices), triangle::vertices.data(), GL_STATIC_DRAW);

  const GLint mvp_location = ShaderProgram_getUniformLocation(triangle_shaderProgram, "MVP");
  const GLint vpos_location = ShaderProgram_getAttribLocation(triangle_shaderProgram, "vPos");
  const GLint vcol_location = ShaderProgram_getAttribLocation(triangle_shaderProgram, "vCol");

  glVertexAttribPointer(
      vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
  glVertexAttribPointer(
      vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));
  glEnableVertexAttribArray(vpos_location);
  glEnableVertexAttribArray(vcol_location);

  // point part
  // new
  ShaderProgram points_shaderProgram = ShaderProgram_new(points::vertex_shader_text, points::fragment_shader_text);
  VertexArray points_vertexArray = VertexArray_new();
  Buffer points_buffer = Buffer_new();
  // init
  VertexArray_bind(points_vertexArray);
  Buffer_bind(points_buffer, GL_ARRAY_BUFFER);
  ShaderProgram_activate(points_shaderProgram);

  const GLint transform_location = ShaderProgram_getUniformLocation(points_shaderProgram, "transform");
  const GLint pointSize_location = ShaderProgram_getUniformLocation(points_shaderProgram, "pointSize");
  const GLint maxSpeedSquared_location = ShaderProgram_getUniformLocation(points_shaderProgram, "maxSpeedSquared");
  const GLint position_location = ShaderProgram_getAttribLocation(points_shaderProgram, "position");
  const GLint velocity_location = ShaderProgram_getAttribLocation(points_shaderProgram, "velocity");

  glVertexAttribPointer(
      position_location, 2, GL_FLOAT, GL_FALSE, sizeof(points::Point), (void*)offsetof(points::Point, position));
  glVertexAttribPointer(
      velocity_location, 2, GL_FLOAT, GL_FALSE, sizeof(points::Point), (void*)offsetof(points::Point, velocity));

  glEnableVertexAttribArray(position_location);
  glEnableVertexAttribArray(velocity_location);

  glEnable(GL_PROGRAM_POINT_SIZE);

  // lines
  // new
  ShaderProgram lines_shaderProgram = ShaderProgram_new(lines::vertex_shader_text, lines::fragment_shader_text);
  VertexArray lines_vertexArray = VertexArray_new();
  Buffer lines_buffer = Buffer_new();
  // init
  VertexArray_bind(lines_vertexArray);
  Buffer_bind(lines_buffer, GL_ARRAY_BUFFER);
  ShaderProgram_activate(lines_shaderProgram);

  const GLint transform_location2 = ShaderProgram_getUniformLocation(lines_shaderProgram, "transform");
  const GLint vpos_location2 = ShaderProgram_getAttribLocation(lines_shaderProgram, "vPos");
  const GLint vcol_location2 = ShaderProgram_getAttribLocation(lines_shaderProgram, "vCol");

  glVertexAttribPointer(
      vpos_location2, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
  glVertexAttribPointer(
      vcol_location2, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));

  glEnableVertexAttribArray(vpos_location2);
  glEnableVertexAttribArray(vcol_location2);

  // points pre-processing

  int width{}, height{};
  glfwGetFramebufferSize(window, &width, &height);

  std::vector<points::Point> points(10'000);
  auto get_pos = [=](float t) {
    return vec2{(float)(width * (0.5 + 0.4 * std::cos(t))), (float)(height * (0.5 + 0.4 * std::sin(t)))};
  };

  float v = 0;
  for (auto& p : points) {
    v += 1.0;
    p = points::Point{get_pos(v), vec2{}};
  }

  // global loop
  float t = 0;
  while (!glfwWindowShouldClose(window)) {
    t += 1.;

    int width{}, height{};
    glfwGetFramebufferSize(window, &width, &height);
    // std::cout << "W x H = " << width << " x " << height << "\n";
    const float ratio = (float)width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    {  // triangle
      mat4x4 m = triangle::mat4x4_identity();
      m = triangle::mat4x4_rotate_Z(m, (float)glfwGetTime());
      mat4x4 p = triangle::mat4x4_ortho(-ratio, ratio, -1., 1., 1., -1.);
      mat4x4 mvp = triangle::mat4x4_mul(p, m);

      VertexArray_bind(triangle_vertexArray);
      Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
      ShaderProgram_activate(triangle_shaderProgram);

      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
      glBindVertexArray(triangle_vertexArray.vertex_array);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    {  // points
      mat3x3 transform = points::vertex_transform_2d(width, height);
      float pointSize = 3.0;
      float max_speed = 10.0;

      float v = 0;
      for (auto& p : points) {
        v += 1.0;
        p.velocity = vec2{20*std::cos(t/10) * (std::cos(v)-std::cos(v+1)), 20*std::cos(t/10) * (std::sin(v)-std::sin(v+1))};
        p.position[0] += p.velocity[0];
        p.position[1] += p.velocity[1];
      }

      VertexArray_bind(points_vertexArray);
      Buffer_bind(points_buffer, GL_ARRAY_BUFFER);
      ShaderProgram_activate(points_shaderProgram);

      glUniformMatrix3fv(transform_location, 1, GL_FALSE, (const GLfloat*)&transform);
      glUniform1f(pointSize_location, pointSize);
      glUniform1f(maxSpeedSquared_location, max_speed);
      glBindVertexArray(points_vertexArray.vertex_array);

      glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
      glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(points::Point), points.data(), GL_STREAM_DRAW);
      glDrawArrays(GL_POINTS, 0, points.size());
    }

    {  // lines
      mat3x3 transform = points::vertex_transform_2d(width, height);

      VertexArray_bind(lines_vertexArray);
      Buffer_bind(lines_buffer, GL_ARRAY_BUFFER);
      ShaderProgram_activate(lines_shaderProgram);

      glUniformMatrix3fv(transform_location2, 1, GL_FALSE, (const GLfloat*)&transform);
      glBindVertexArray(lines_vertexArray.vertex_array);

      std::vector<triangle::Vertex> vertex_data;

      auto h = static_cast<float>(height);
      auto w = static_cast<float>(width);

      vertex_data.push_back(triangle::Vertex{{w, h / 2 + h * 0.002f}, {1.0, 1.0, 1.0}});  // Vertex A |
      vertex_data.push_back(triangle::Vertex{{w, h / 2 - h * 0.002f}, {1.0, 1.0, 1.0}});  // Vertex B | Triangle ABC
      vertex_data.push_back(triangle::Vertex{{0, h / 2 + h * 0.002f}, {1.0, 1.0, 1.0}});  // Vertex C |
      vertex_data.push_back(triangle::Vertex{{w, h / 2 - h * 0.002f}, {1.0, 1.0, 1.0}});  // Vertex B   |
      vertex_data.push_back(triangle::Vertex{{0, h / 2 + h * 0.002f}, {1.0, 1.0, 1.0}});  // Vertex C   | Triangle BCD
      vertex_data.push_back(triangle::Vertex{{0, h / 2 - h * 0.002f}, {1.0, 1.0, 1.0}});  // Vertex D   |
      glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(triangle::Vertex), vertex_data.data(), GL_STREAM_DRAW);
      glDrawArrays(GL_TRIANGLES, 0, vertex_data.size());  // Should use GL_TRIANGLE_SPAN by setting only Vertices ABCD
    }

    // Measure FPS
    glfwSetWindowTitle(window, "FPS: to be defined");

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}