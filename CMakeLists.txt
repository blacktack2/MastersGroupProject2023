cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(CSC8507 CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64") AND SCE_ROOT_DIR AND  (NOT("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ORBIS")))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
#set(CMAKE_CXX_STANDARD 20)
if(SCE_ROOT_DIR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSION OFF)

else()
set(CMAKE_CXX_STANDARD 23)
endif()
################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#find_package(Vulkan REQUIRED)

set(ASSET_ROOT "${CMAKE_SOURCE_DIR}/Assets/" CACHE STRING "" FORCE)
add_compile_definitions(ASSETROOTLOCATION="${ASSET_ROOT}") 

set(USE_VULKAN CACHE BOOL false)
if(USE_VULKAN)
    add_compile_definitions("USEVULKAN") 
endif() 
add_compile_definitions(NOMINMAX)
set(USE_ORBIS BOOL false FORCE)
if(SCE_ROOT_DIR)
set(USE_ORBIS BOOL true FORCE)
add_compile_definitions(_ORBIS)
message("ORBIS in use")
else()
add_compile_definitions("x64")  
endif()
################################################################################
# Sub-projects
################################################################################
add_subdirectory(NCLCoreClasses)
add_subdirectory(CSC8503CoreClasses)
add_subdirectory(OpenGLRendering)
add_subdirectory(CSC8503)

if(USE_ORBIS)
add_subdirectory(Plugins/PlayStation4)
endif()

set(OPENAL_LIBRARY "${CMAKE_SOURCE_DIR}/lib/Debug/OpenAL/" CACHE STRING "" FORCE)
set(OPENAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include" CACHE STRING "" FORCE)
find_package(OpenAL REQUIRED)

if(USE_VULKAN)
    add_subdirectory(VulkanRendering)
endif()
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CSC8503)