cmake_minimum_required (VERSION 3.1)

project(DDG_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS_RELEASE "-Ox")
endif()

# disable GLFW docs, examples and tests
# see http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# set path to dependencies
add_definitions(-DGLEW_STATIC)
add_subdirectory(dependency/glfw EXCLUDE_FROM_ALL)
add_subdirectory(dependency/glew-cmake EXCLUDE_FROM_ALL)
add_subdirectory(dependency/assimp EXCLUDE_FROM_ALL)
add_subdirectory(dependency/GeometryTools EXCLUDE_FROM_ALL)
include_directories(dependency/GeometryTools/code)
include_directories(dependency/imgui)
include_directories(dependency/glm)
include_directories(dependency/eigen)

# imgui source files
set(IMGUI_SRC_DIR dependency/imgui)
aux_source_directory(${IMGUI_SRC_DIR} IMGUI_SRC_FILES)

# Add dependencies
find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# ------------------------------------------------------------------------------
set(DEMO DDG_Project)
set(SRC_DIR code)
include_directories(${SRC_DIR})
aux_source_directory(${SRC_DIR} SRC_FILES)
if (WIN32)
	add_executable(${DEMO} 
		${IMGUI_SRC_FILES} 
		${SRC_FILES}
		${SRC_DIR}/camera.hpp
		${SRC_DIR}/mesh.hpp
		${SRC_DIR}/topo_mesh_2D.hpp
		${SRC_DIR}/topo_mesh_3D.hpp
		${SRC_DIR}/shader.hpp
		${SRC_DIR}/stb_image_write.h
		${SRC_DIR}/stb_image.h
		${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.h
		${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.cpp
		${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.h
		${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp
	)
else()
	add_executable(${DEMO} 
		${IMGUI_SRC_FILES} 
		${SRC_FILES}
		${SRC_DIR}/camera.hpp
		${SRC_DIR}/mesh.hpp
		${SRC_DIR}/topo_mesh_2D.hpp
		${SRC_DIR}/topo_mesh_3D.hpp
		${SRC_DIR}/shader.hpp
		${SRC_DIR}/stb_image_write.h
		${SRC_DIR}/stb_image.h
		${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.h
		${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.cpp
		${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.h
		${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp
	)
endif()

# data folder copy on post build
set(DATA_DIR data)
add_custom_command(
	TARGET ${DEMO} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/${DATA_DIR}
			${CMAKE_CURRENT_BINARY_DIR}/${DATA_DIR}
)
target_link_libraries(${DEMO} 
	glfw 
	libglew_static
	assimp
	GeometryTools
)
if (!WIN32)
	target_link_libraries(${DEMO} "stdc++fs")
endif()
target_compile_definitions(
    ${DEMO} PUBLIC
    -DPATH_TO_SRC_DIRECTORY="${CMAKE_SOURCE_DIR}/"
    -DIMGUI_IMPL_OPENGL_LOADER_GLFW
)
