FIND_PACKAGE(glfw3 3.3 REQUIRED)
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(fmt REQUIRED)
FIND_PACKAGE(easy_profiler)

add_executable(life-of-boids main.cpp glx.cpp glx.hpp)
target_link_libraries(life-of-boids PRIVATE glfw::glfw glad::glad fmt::fmt easy_profiler::easy_profiler)

if (ENABLE_TRACY_PROFILER)
    target_sources(life-of-boids PUBLIC ../tracy/TracyClient.cpp ../tracy/Tracy.hpp)
    target_include_directories(life-of-boids PUBLIC ../tracy)
    ADD_COMPILE_DEFINITIONS(TRACY_ENABLE=1)
endif()

FIND_PACKAGE(OpenCV REQUIRED)
add_executable(main_opencv main_opencv.cpp)
target_link_libraries(main_opencv PRIVATE opencv::opencv)
