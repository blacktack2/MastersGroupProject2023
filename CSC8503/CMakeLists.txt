set(PROJECT_NAME CSC8503)

################################################################################
# Source groups
################################################################################
set(Header_Files
	"Bullet.h"
	"Bonus.h"
	"EnemyObject.h"
	"Maze.h"
    "NetworkedGame.h"
    "NetworkPlayer.h"
	"NPCObject.h"
    "StateGameObject.h"
    "TutorialGame.h"
    "DebugViewPoint.h"
    "GameGrid.h"
    "GameGridManager.h"
    "GameStateManager.h"
    "Obstacle.h"
    "GameLevel.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
	"Bullet.cpp"
	"Bonus.cpp"
	"EnemyObject.cpp"
    "Main.cpp"
	"Maze.cpp"
    "NetworkedGame.cpp"
    "NetworkPlayer.cpp"
	"NPCObject.cpp"
    "StateGameObject.cpp"
    "TutorialGame.cpp"
    "DebugViewPoint.cpp"
    "GameGrid.cpp"
    "GameGridManager.cpp"
    "GameStateManager.cpp"
    "GameLevel.cpp"
)

set(Render_Files
    "GameTechRenderer.h"
    "GameTechRenderer.cpp"
    "SkyboxRPass.h"
    "SkyboxRPass.cpp"
    "ModelRPass.h"
    "ModelRPass.cpp"
    "LightingRPass.h"
    "LightingRPass.cpp"
    "SSAORPass.h"
    "SSAORPass.cpp"
    "CombineRPass.h"
    "CombineRPass.cpp"
    "BloomRPass.h"
    "BloomRPass.cpp"
    "HDRRPass.h"
    "HDRRPass.cpp"
    "PresentRPass.h"
    "PresentRPass.cpp"
    "DebugRPass.h"
    "DebugRPass.cpp"
    "PresentRPass.h"
    "PresentRPass.cpp"
    "PaintRenderObject.h"
    "PaintRenderObject.cpp"
    "PaintingRPass.cpp"
    "PaintingRPass.h"
    "ModelRPass.h"
    "ModelRPass.cpp"
)

if(USE_VULKAN)
    list(APPEND Header_Files "GameTechVulkanRenderer.h")
    list(APPEND Source_Files "GameTechVulkanRenderer.cpp")

    file(GLOB SHADER_FILES ${ASSET_ROOT}/Shaders/VK/*.*)
endif()

source_group("Source Files" FILES ${Source_Files})
source_group("Rendering" FILES ${Render_Files})

set(Audio_Files 
    "dr_wav_impl.cpp"
    "SoundErrorReport.h"
    "SoundErrorReport.cpp"
    "SoundSystem.h"
    "SoundSystem.cpp"
    "Sound.h"
    "Sound.cpp"
    "SoundObject.h"
    "SoundObject.cpp"
    "SoundManager.h"
    "SoundManager.cpp"
    "SoundSource.h"
    "SoundSource.cpp"
    "SoundDevice.h"
    "SoundDevice.cpp"
    "TestAudio.h"
    "TestAudio.cpp"
)
source_group("Audio Files" FILES ${Audio_Files})

set(Ink_Files 
    "InkEffectManager.h"
    "InkEffectManager.cpp"
    "InkEffect.h"
	"InkEffect.cpp"
    "InkEffectOverTime.h"
    "InkEffectOverTime.cpp"
)
source_group("Ink Files" FILES ${Ink_Files})

set(Player_Files 
    "health.h"
	"health.cpp"
    "PlayerObject.h"
    "PlayerObject.cpp"
	
)
source_group("Player Files" FILES ${Player_Files})

set(Enemy_Files 
    "Boss.h"
    "BossBullet.h"
	"BossBullet.cpp"
)
source_group("Enemy Files" FILES ${Enemy_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${Audio_Files}
    ${Render_Files}
    ${Ink_Files}
	${Player_Files}
    ${Enemy_Files}
)

if(USE_VULKAN)
    set(ALL_FILES 
        ${Header_Files}
        ${Source_Files}
        ${SHADER_FILES}
        ${Audio_Files}
        ${Ink_Files}
	    ${Player_Files}
        ${Enemy_Files}
    )

foreach (file ${SHADER_FILES})
    get_filename_component(file_name ${file} NAME)
	get_filename_component(file_ext ${file} EXT)
	
	if(file_ext STREQUAL ".h" OR file_ext STREQUAL ".cpp")
		continue()
	endif()
		
	if( file_ext STREQUAL  ".vert" OR
		file_ext STREQUAL  ".frag" OR
		file_ext STREQUAL  ".comp" OR
		file_ext STREQUAL  ".geom" OR
		file_ext STREQUAL  ".tesc" OR
		file_ext STREQUAL  ".tese" OR
		file_ext STREQUAL  ".rgen" OR
		file_ext STREQUAL  ".rint" OR
		file_ext STREQUAL  ".rahit" OR
		file_ext STREQUAL  ".rchit" OR
		file_ext STREQUAL  ".rmiss" OR
		file_ext STREQUAL  ".rcall" OR
		file_ext STREQUAL  ".task" OR
		file_ext STREQUAL  ".mesh"
	)
		message("Adding custom command to ${file}")
		get_filename_component(file_dir ${file} ABSOLUTE)
        set(SPIRV_OUTPUT ${file_name}.spv)
		set(SPIRV_ABS_INPUT ${file_dir})
		set(SPIRV_ABS_OUTPUT ${file_dir}.spv)
		
		add_custom_command(
			OUTPUT ${SPIRV_ABS_OUTPUT}
			
			COMMENT "Compiling GLSL shader:"
			COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V  ${SPIRV_ABS_INPUT} -o ${SPIRV_ABS_OUTPUT}
            DEPENDS ${file}
			VERBATIM
		)
        list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUTPUT})   
	endif()
endforeach()

endif()

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME}  ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE CSC8503)

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
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <vector>
    <map>
    <stack>
    <string>
    <list>
    <thread>
    <atomic>
    <functional>
    <iostream>
    <set>
    "../NCLCoreClasses/Vector2.h"
    "../NCLCoreClasses/Vector3.h"
    "../NCLCoreClasses/Vector4.h"
    "../NCLCoreClasses/Quaternion.h"
    "../NCLCoreClasses/Plane.h"
    "../NCLCoreClasses/Matrix2.h"
    "../NCLCoreClasses/Matrix3.h"
    "../NCLCoreClasses/Matrix4.h"
    
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
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /Oi;
            /Gy
        >
        /permissive-;
        /std:c++latest;
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
    )
endif()

################################################################################
# Dependencies
################################################################################
if(MSVC)
    target_link_libraries(${PROJECT_NAME} LINK_PUBLIC  "Winmm.lib")
endif()

include_directories("../OpenGLRendering/")
include_directories("../NCLCoreClasses/")
include_directories("../CSC8503CoreClasses/")

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC NCLCoreClasses)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC CSC8503CoreClasses)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC OpenGLRendering)

if(USE_VULKAN)
include_directories("../VulkanRendering/")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC VulkanRendering)


add_custom_target(
    SHADER_FILES
    DEPENDS ON ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} SHADER_FILES)

endif()

################################################################################
# OpenAL config
################################################################################
include_directories("../include/")
link_directories("../lib/Debug/OpenAL/")

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC  ${CMAKE_SOURCE_DIR}/lib/Debug/OpenAL/OpenAL32.lib)