# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

include_directories(
	external/AntTweakBar-1.15/include/
	external/glfw-2.7.6/include/
	external/glm-0.9.1/
	external/glew-1.5.8/include/
	external/yaml-cpp-0.3.0/include/
	include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_276
	GLEW_158
	ANTTWEAKBAR_151_OGLCORE_GLFW
	YAMLCPP_030
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

#find_library(YAML libyaml-cpp.a lib/)

add_executable(CustomViewer 
	src/customviewer.cpp
	src/customviewer.h
	src/sphere.cpp
	src/sphere.h
	src/controls.cpp
	src/controls.h
)
target_link_libraries(CustomViewer
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(CustomViewer PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET CustomViewer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CustomViewer${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

add_custom_command(
   TARGET CustomViewer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/src/fragment.fsh" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
)

add_custom_command(
   TARGET CustomViewer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex.vsh" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
)

add_custom_command(
   TARGET CustomViewer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/src/test.yaml" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

