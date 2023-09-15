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
target_link_libraries(
  main
  PRIVATE glfw
)