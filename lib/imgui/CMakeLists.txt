cmake_minimum_required(VERSION 3.17)
project(ImGui)

file(GLOB_RECURSE IMGUI_SRC *.cpp)
file(GLOB_RECURSE IMGUI_HEAD *.h)
add_library(imgui ${IMGUI_SRC} ${IMGUI_HEAD})
