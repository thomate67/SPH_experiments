cmake_minimum_required(VERSION 3.10)

project(SPH)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SHADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shaders CACHE PATH "Project specific path. Set manually if it was not found.")

set(RESOURCES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources CACHE PATH "Project specific path. Set manually if it was not found.")

set(DEPENDENCIES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dependencies CACHE PATH "Project specific path. Set manually if it was not found.")

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

set_target_properties(PROPERTIES LINK_FLAGS "$<$<CONFIG:DEBUG>:/NODEFAULTLIB:MSVCRT")

add_definitions(-DSHADERS_PATH="${SHADERS_PATH}")
add_definitions(-DRESOURCES_PATH="${RESOURCES_PATH}")
add_definitions(-DGLFW_INCLUDE_GLCOREARB)
add_definitions(-DGLEW_STATIC)
add_definitions(-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)
add_definitions(-DGLEW_NO_GLU)

## include libraries
find_package(OpenGL REQUIRED)
find_package(GLAD REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(TinyOBJ REQUIRED)
find_package(ASSIMP REQUIRED)
include(${DEPENDENCIES_ROOT}/eigen3/share/eigen3/cmake/Eigen3Targets.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_PATH} ${GLEW_INCLUDE_PATH} ${GLM_INCLUDE_PATH} ${GLFW3_INCLUDE_PATH} ${TinyOBJ_INCLUDE_PATH} ${ASSIMP_INCLUDE_PATH} glad::glad Eigen3::Eigen)
#link_libraries(${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${TinyOBJ_LIBRARY} ${ASSIMP_LIBRARY} glad::glad Eigen3::Eigen)

#"Copying Assimp binaries to '${PROJECT_BINARY_DIR}/projects'"
file(COPY ${DEPENDENCIES_ROOT}/lib/assimp/assimp-vc140-mt.dll DESTINATION ${PROJECT_BINARY_DIR}/projects)

add_subdirectory(src)
add_subdirectory(shaders)
add_subdirectory(projects)
