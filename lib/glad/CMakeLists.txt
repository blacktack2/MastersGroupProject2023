cmake_minimum_required(VERSION 3.17)
project(Glad)

add_library(glad
	include/glad/glad.h
	include/glad/gl.h
	include/glad/wgl.h
	include/KHR/khrplatform.h
	include/KHR/wglext.h
	src/gl.c
	src/wgl.c
)
target_include_directories(glad PUBLIC include/)
