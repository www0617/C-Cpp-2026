# ============================================================
# Five_in_a_roll/CMakeLists.txt
# 由根目录 CMakeLists.txt 的 add_subdirectory(Five_in_a_roll) 引入，
# 也可以单独打开本目录构建。
#
# 当前只保留：UI（view + main）与棋盘接口（board.h）。
# 以后加了算法，在 FIAR_SOURCES 里补上对应 .cpp 即可。
# ============================================================
cmake_minimum_required(VERSION 3.21)
project(five_in_a_roll CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FIAR_SOURCES
    src/main.cpp
    src/view.cpp
        ai/ai.cpp
)

add_executable(five_in_a_roll ${FIAR_SOURCES})

target_include_directories(five_in_a_roll PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(five_in_a_roll PRIVATE UNICODE _UNICODE)

target_link_libraries(five_in_a_roll PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libeasyxw.a"
    gdi32
)

# -mwindows：GUI 子系统（没有控制台）
target_link_options(five_in_a_roll PRIVATE -mwindows -static-libgcc -static-libstdc++)

target_compile_options(five_in_a_roll PRIVATE -Wno-unknown-pragmas -Wall -Wextra)
