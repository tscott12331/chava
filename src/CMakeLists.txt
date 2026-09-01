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
)

target_include_directories(chava_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
