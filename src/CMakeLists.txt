FIND_PACKAGE(glfw3 3.3 REQUIRED) # REQUIRED means : "fatal if not found"
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)

add_library(libGLX glx.cpp glx.hpp)
target_link_libraries(libGLX PUBLIC glfw::glfw glad::glad opencv::opencv)

add_executable(life-of-boids main.cpp)
target_link_libraries(life-of-boids libGLX)

add_executable(main_opencv main_opencv.cpp)
target_link_libraries(main_opencv PRIVATE opencv::opencv)
