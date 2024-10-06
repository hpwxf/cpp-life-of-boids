FIND_PACKAGE(glfw3 3.3 REQUIRED)
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(fmt REQUIRED)
find_package(PNG REQUIRED)

add_library(libGLX glx.cpp glx.hpp)
target_link_libraries(libGLX PUBLIC glfw glad::glad PNG::PNG)

add_executable(life-of-boids main.cpp)
target_link_libraries(life-of-boids libGLX fmt::fmt)