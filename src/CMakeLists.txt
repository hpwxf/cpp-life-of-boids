FIND_PACKAGE(glfw3 3.3 REQUIRED)
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(fmt REQUIRED)

add_executable(life-of-bois main.cpp glx.cpp glx.hpp)
target_link_libraries(life-of-bois PRIVATE glfw::glfw glad::glad fmt::fmt)
#target_include_directories(life-of-bois PRIVATE ${glfw_INCLUDE_DIRS} ${glad_INCLUDE_DIRS} ${fmt_INCLUDE_DIRS})
