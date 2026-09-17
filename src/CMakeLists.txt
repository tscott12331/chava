add_library(chava_lib
    tokenizer/tokenizer.cpp
    parser/class.cpp
    parser/exp.cpp
    parser/parser.cpp
    parser/stmt.cpp
    parser/type.cpp
    typechecker/typechecker.cpp
    typechecker/scope.cpp
    typechecker/typemap.cpp
    typechecker/classtype.cpp
    typechecker/methodsignature.cpp
)

file(GLOB TEST_FILES test/*.cpp)
add_library(chava_test_lib ${TEST_FILES})
target_link_libraries(chava_test_lib PRIVATE chava_lib)

target_include_directories(chava_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(chava_test_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
