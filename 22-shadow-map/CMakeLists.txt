cmake_minimum_required(VERSION 3.5)
project("gl-init")
add_executable(
  main
  main.cpp
)
include_directories(
  "/opt/homebrew/include"
)
target_link_directories(main PUBLIC "/opt/homebrew/lib")
find_package(OpenGL REQUIRED)
target_link_libraries(
  main
  PRIVATE glfw
  PRIVATE glew
  PRIVATE OpenGL::GL
)

# add_custom_command(
# TARGET main POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy
# ${CMAKE_CURRENT_SOURCE_DIR}/assets/*
# ${CMAKE_CURRENT_BINARY_DIR}/assets/*
# )
