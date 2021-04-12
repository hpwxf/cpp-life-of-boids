#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>

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
  fmt::print(stderr, "Error[{}]: {}\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if (key == GLFW_KEY_S  && action == GLFW_PRESS) {
    std::puts("Export current display");
    saveImage("export.png", window);
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

  const GLint transform_location2 = ShaderProgram_getUniformLocation(points_shaderProgram, "transform");
  const GLint vpos_location2 = ShaderProgram_getAttribLocation(triangle_shaderProgram, "vPos");
  const GLint vcol_location2 = ShaderProgram_getAttribLocation(triangle_shaderProgram, "vCol");

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
    return vec2{(float)(width * (0.5 + 0.4 * cos(t))), (float)(height * (0.5 + 0.4 * sin(t)))};
  };

  for (float v = 0; auto& p : points) {
    v += 1.0;
    p = points::Point{get_pos(v), vec2{}};
  }

  std::random_device random_device;
  std::default_random_engine eng{random_device()};
  std::uniform_real_distribution<float> velocity_space{0, 10};
  std::uniform_real_distribution<float> angle_space{0, 6.28};

  // global pre-processing

  auto last_time = std::chrono::steady_clock::now();
  long count = 0;
  double accumulated_time = 0;

  // global loop
  while (!glfwWindowShouldClose(window)) {
    int width{}, height{};
    glfwGetFramebufferSize(window, &width, &height);
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

      for (auto& p : points) {
        auto a = angle_space(eng);
        auto m = velocity_space(eng);
        p.velocity = vec2{m * cos(a), m * sin(a)};
        p.position[0] += p.velocity[0] / 5.0;
        p.position[1] += p.velocity[1] / 5.0;
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
      vertex_data.push_back(triangle::Vertex{{0, static_cast<float>(height) / 2}, {1.0, 1.0, 1.0}});
      vertex_data.push_back(
          triangle::Vertex{{static_cast<float>(width), static_cast<float>(height) / 2}, {1.0, 1.0, 1.0}});
      vertex_data.push_back(triangle::Vertex{{static_cast<float>(width) / 2, 0}, {1.0, 1.0, 1.0}});
      vertex_data.push_back(
          triangle::Vertex{{static_cast<float>(width) / 2, static_cast<float>(height)/2}, {1.0, 1.0, 1.0}});

      glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(triangle::Vertex), vertex_data.data(), GL_STREAM_DRAW);
      glDrawArrays(GL_LINES, 0, vertex_data.size());
    }

    // Measure FPS
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(now - last_time).count();
    last_time = now;
    accumulated_time += duration;
    count += 1;

    if (accumulated_time > 1) {
      auto title = fmt::format("FPS: {:.2}", static_cast<double>(count) / accumulated_time);
      glfwSetWindowTitle(window, title.data());
      count = 0;
      accumulated_time = 0;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}