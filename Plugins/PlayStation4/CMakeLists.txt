set(PROJECT_NAME Playstation4)

################################################################################
# Source groups
################################################################################

set(Header_Files
    "InputBase.h"
    "Ps4AudioEmitter.h"
    "PS4AudioSource.h"
    "Ps4AudioSystem.h"
    "PS4Camera.h"
	"PS4ComputeShader.h"
    "PS4Frame.h"
    "PS4Input.h"
    "PS4InputManager.h"
    "PS4MemoryAware.h"
    "PS4Mesh.h"
    "PS4RendererBase.h"
    "PS4Shader.h"
    "PS4Sound.h"
    "PS4Texture.h"
    "PS4Window.h"
   "PS4Renderer.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "InputBase.cpp"
    "Ps4AudioEmitter.cpp"
    "PS4AudioSource.cpp"
    "Ps4AudioSystem.cpp"
    "PS4Camera.cpp"
	"PS4ComputeShader.cpp"
    "PS4Frame.cpp"
    "PS4Input.cpp"
    "PS4InputManager.cpp"
    "PS4MemoryAware.cpp"
    "PS4Mesh.cpp"
    "PS4RendererBase.cpp"
    "PS4Shader.cpp"
    "PS4Sound.cpp"
    "PS4Texture.cpp"
    "PS4Window.cpp"
   "PS4Renderer.cpp"
)

source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE PS4)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Compile definitions
################################################################################
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "UNICODE;"
        "_UNICODE" 
        "WIN32_LEAN_AND_MEAN"
        "_WINSOCKAPI_"   
        "_WINSOCK2API_"
        "_WINSOCK_DEPRECATED_NO_WARNINGS"
    )
	else()
	target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            -Oi;
            -Gy
        >
		$<$<CONFIG:Debug>:
		-O0
		>
		-frtti;
		-fexceptions;
        -std:c++17;
        #${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        #${DEFAULT_CXX_EXCEPTION_HANDLING};
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            -OPT:REF;
            -OPT:ICF
        >
    )
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <vector>
    <map>
    <stack>
    <std::string>
    <list>
    <thread>
    <atomic>
    <functional>
    <iostream>
    <set>
    "../../NCLCoreClasses/Vector2.h"
    "../../NCLCoreClasses/Vector3.h"
    "../../NCLCoreClasses/Vector4.h"
    "../../NCLCoreClasses/Quaternion.h"
    "../../NCLCoreClasses/Plane.h"
    "../../NCLCoreClasses/Matrix2.h"
    "../../NCLCoreClasses/Matrix3.h"
    "../../NCLCoreClasses/Matrix4.h"
    
)
 
if(USE_VULKAN)
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "../VulkanRendering/SmartTypes.h"

    "../VulkanRendering/VulkanRenderer.h"
    "../VulkanRendering/VulkanShader.h"
    "../VulkanRendering/VulkanShaderBuilder.h"
    "../VulkanRendering/VulkanTexture.h"
    "../VulkanRendering/VulkanMesh.h"
    "../VulkanRendering/VulkanPipelineBuilder.h"
    "../VulkanRendering/VulkanDynamicRenderBuilder.h"

    "../VulkanRendering/VulkanDescriptorSetLayoutBuilder.h"
    "../VulkanRendering/VulkanRenderPassBuilder.h"

    "../VulkanRendering/VulkanCompute.h"
    "../VulkanRendering/VulkanComputePipelineBuilder.h"
)
endif()

################################################################################
# Compile and link options
################################################################################

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "NCLCoreClasses"
    "CSC8503CoreClasses"
)

include_directories("../../NCLCoreClasses/")
include_directories("../../CSC8503CoreClasses/")
include_directories("./")
include_directories("${SCE_ORBIS_SDK_DIR}/target/samples/")

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC NCLCoreClasses)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC CSC8503CoreClasses)

################################################################################
# PS4 config
################################################################################
if(USE_ORBIS)
 set_property(SOURCE ${ALL_FILES} PROPERTY COMPILE_OPTIONS "-Wall" "-fno-implicit-templates")
 set_property(TARGET ${PROJECT_NAME} PROPERTY VS_SOURCE_SETTINGS_WavePsslc "FileType=Document")
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
) 
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" FORCE)
endif()