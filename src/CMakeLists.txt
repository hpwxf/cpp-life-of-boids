FIND_PACKAGE(glfw3 3.3 REQUIRED)
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(fmt REQUIRED)

add_executable(life-of-boids main.cpp glx.cpp glx.hpp)
target_link_libraries(life-of-boids PRIVATE glfw::glfw glad::glad fmt::fmt)
#target_include_directories(life-of-boids PRIVATE ${glfw_INCLUDE_DIRS} ${glad_INCLUDE_DIRS} ${fmt_INCLUDE_DIRS})
