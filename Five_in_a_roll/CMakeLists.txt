# ============================================================
# Five_in_a_roll/CMakeLists.txt
# 由根目录 CMakeLists.txt 的 add_subdirectory(Five_in_a_roll) 引入，
# 也可以单独打开本目录构建。
#
#   src/   画面：view（绘制 + 棋盘数组 + 坐标换算）+ main（组装）
#   ai/    算法：eval / minimax / alphabeta / mcts
#   include/ lib/  手动安装的 EasyX 26.9.25（MinGW-w64 x64 MSVCRT）
# ============================================================
cmake_minimum_required(VERSION 3.21)
project(five_in_a_roll CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FIAR_SOURCES
    src/main.cpp
    src/view.cpp

    ai/eval.cpp
    ai/minimax.cpp
    ai/alphabeta.cpp
    ai/mcts.cpp
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
