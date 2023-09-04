cmake_minimum_required(VERSION 3.23)
project(DemoRealTimeLighting LANGUAGES C CXX CUDA)

set(CMAKE_CUDA_STANDARD 14)

set(OptiX_ROOT_DIR "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.6.0")
add_definitions(-DNDEBUG)

add_executable(${PROJECT_NAME} src/Main.cpp)

include_directories(lib)
include_directories(src)

add_subdirectory(src)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED OPTIONAL_COMPONENTS OpenGL)

if(TARGET OpenGL::OpenGL)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::OpenGL)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)
endif()

add_subdirectory(lib/owl EXCLUDE_FROM_ALL)

embed_ptx(
        OUTPUT_TARGET ${PROJECT_NAME}-ptx
        PTX_LINK_LIBRARIES owl::owl
        SOURCES "src/RayTracerDevice.cuh" "src/RayTracerDevice.cu"
)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw owl::owl ${PROJECT_NAME}-ptx)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
