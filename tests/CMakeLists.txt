include(CTest)

find_package(GTest REQUIRED)
include(GoogleTest)

# Build a list of test binaries
add_executable(first_test first_test.cpp)
target_link_libraries(first_test PRIVATE libGLX)
add_test(NAME FirstTest
        COMMAND first_test # with args 
        )

add_executable(first_gtest first_gtest.cpp)
target_link_libraries(first_gtest PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(first_gtest PRIVATE ${GTest_INCLUDE_DIRS})
gtest_discover_tests(first_gtest) # helps to split one ctest executable into multiple google tests
