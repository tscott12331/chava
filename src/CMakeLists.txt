add_library(chava_lib
    tokenizer.cpp
)

target_include_directories(chava_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
