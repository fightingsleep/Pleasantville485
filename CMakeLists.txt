# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (CMPT485)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	external/glfw-3.2/include/GLFW/
	external/glm-0.9.4.0/
	external/glew-1.9.0/include/
	external/libSOIL-master/
	common/
	.
)

find_library(SOIL_LIBRARY libsoil.a HINTS ${CMAKE_CURRENT_SOURCE_DIR}/external/libSOIL-master/)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_190
	${SOIL_LIBRARY}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(part6
	src/part6.cpp
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/texture.cpp
	common/texture.hpp
	common/cubicSpline.cpp
	common/cubicSpline.hpp
	common/init.cpp
	common/init.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/physicsEngine.cpp
	common/physicsEngine.hpp
	common/physicsEntity.cpp
	common/physicsEntity.hpp
	common/quaternion_utils.cpp
	common/quaternion_utils.hpp
	common/render.cpp
	common/render.hpp
	common/tangentspace.cpp
	common/tangentspace.hpp
	common/vboindexer.cpp
	common/vboindexer.hpp
	common/window.cpp
	common/window.hpp
	common/debug.cpp
	common/debug.hpp
	
	src/ShadowMapping.vertexshader
	src/ShadowMapping.fragmentshader
	src/DepthTest.fragmentshader
	src/DepthTest.vertexshader
	src/SimpleTexture.fragmentshader
	src/Passthrough.vertexshader
	src/skybox.frag
	src/skybox.vs
)

target_link_libraries(part6
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(part6 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
create_target_launcher(part6 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION “.*/.*shader$” )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET part6 POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/part6${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

