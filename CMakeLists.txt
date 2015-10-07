cmake_minimum_required(VERSION 3.2)
project(othello-battler CXX)

# ソースとビルドのディレクトリーが一緒の場合はエラーで落とす
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif()

# C++er 鉄の掟
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# C++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# 環境によるちょいちょい
if(CYGWIN)
  add_definitions(-D_WIN32_WINNT=0x0501 -D__USE_W32_SOCKETS)
endif()

# Clang と GCC では色付きの翻訳診断表示を行う
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fcolor-diagnostics")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9.2 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9.2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
  endif()
endif()

# Emscripten でも PTHREAD をサポート
if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1")
  set(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} -s USE_PTHREADS=1")
  set(THREADS_PTHREAD_ARG 2)
endif()

# bin, lib 等へ出力
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# プロジェクトルートを起点にインクルードの探索パスを設定
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

# ビルドディレクトリーの lib をリンクの探索パスを設定
link_directories(${CMAKE_BINARY_DIR}/lib)

# サンプル
add_executable(othello_sample othello_sample.cpp)
# ランナー
add_executable(othello_runner othello_runner.cpp)
