cmake_minimum_required(VERSION 3.10)

include_directories(${OPENGL_INCLUDE_PATH} ${GLEW_INCLUDE_PATH} ${GLM_INCLUDE_PATH} ${GLFW3_INCLUDE_PATH} ${TinyOBJ_INCLUDE_PATH} ${ASSIMP_INCLUDE_PATH} glad::glad Eigen3::Eigen)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADER *.h)

add_library(libvisco ${SOURCES} ${HEADER})
					
					
target_include_directories(libvisco
						PUBLIC $<BUILD_INTERFACE:${SRC_PATH}>
						PUBLIC ${OPENGL_INCLUDE_PATH} 
						PUBLIC ${GLEW_INCLUDE_PATH} 
						PUBLIC ${GLM_INCLUDE_PATH} 
						PUBLIC ${GLFW3_INCLUDE_PATH} 
						PUBLIC ${TinyOBJ_INCLUDE_PATH} 
						PUBLIC ${ASSIMP_INCLUDE_PATH} 
						PUBLIC glad::glad 
						PUBLIC Eigen3::Eigen)
		
target_compile_definitions(libvisco
    PUBLIC 
        GLFW_INCLUDE_NONE
        GLM_FORCE_RADIANS
        GLM_ENABLE_EXPERIMENTAL)
		
target_compile_features(libvisco PUBLIC cxx_std_17)
set_target_properties(libvisco PROPERTIES
			LINKER_LANGUAGE CXX
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
			CXX_STANDARD_REQUIRED ON)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set_target_properties(libvisco PROPERTIES
			LINK_FLAGS "/NODEFAULTLIB:MSVCRT")
endif()

target_link_libraries(libvisco PUBLIC ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${TinyOBJ_LIBRARY} ${ASSIMP_LIBRARY} glad::glad Eigen3::Eigen)