FIND_PACKAGE(glfw3 3.3 REQUIRED)
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(fmt REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)

add_executable(life-of-boids main.cpp glx.cpp glx.hpp)
target_link_libraries(life-of-boids PRIVATE glfw::glfw glad::glad fmt::fmt opencv::opencv)

add_executable(main_opencv main_opencv.cpp)
target_link_libraries(main_opencv PRIVATE opencv::opencv)
