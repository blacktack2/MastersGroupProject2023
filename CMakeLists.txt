cmake_minimum_required(VERSION 3.17)

project(MastersProject)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")

add_subdirectory(lib/)

set(EXECUTABLE ${CMAKE_PROJECT_NAME})
message("Creating executable: " ${EXECUTABLE})
if(WIN32)
	add_executable(${EXECUTABLE} WIN32 ${SRC})
	target_link_libraries(${EXECUTABLE} glad glm)
else()
	add_executable(${EXECUTABLE} ${SRC} ${GLAD} ${GLM})
	target_link_libraries(${EXECUTABLE} "glad" ${OPENGL_gl_LIBRARY})
	if(APPLE)
		target_link_libraries(${EXECUTABLE} glad glm)
	else()
		target_link_libraries(${EXECUTABLE} ${CMAKE_DL_LIBS} glad glm)
	endif()
endif()