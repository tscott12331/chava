add_executable(chava main.cpp)
add_executable(chava_test test.cpp)

target_link_libraries(chava PRIVATE chava_lib)
target_link_libraries(chava_test PRIVATE chava_test_lib)
