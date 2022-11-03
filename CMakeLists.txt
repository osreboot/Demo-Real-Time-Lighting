cmake_minimum_required(VERSION 3.23)
project(DemoRealTimeLighting LANGUAGES C CXX CUDA)

set(CMAKE_CUDA_STANDARD 14)

set(OptiX_ROOT_DIR "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.6.0")
add_definitions(-DNDEBUG)

set(SOURCE_FILES
        "config.cuh"
        "main.cu"
        "display.cuh"
        "display.cu"
        "rt_setup.cuh"
        "rt_setup.cu")

add_subdirectory(scene)

add_executable(DemoRealTimeLighting ${SOURCE_FILES})

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED OPTIONAL_COMPONENTS OpenGL)

if(TARGET OpenGL::OpenGL)
    target_link_libraries(DemoRealTimeLighting PUBLIC OpenGL::OpenGL)
else()
    target_link_libraries(DemoRealTimeLighting PUBLIC OpenGL::GL)
endif()

set(owl_dir ${PROJECT_SOURCE_DIR}/owl)
add_subdirectory(${owl_dir} EXCLUDE_FROM_ALL)

embed_ptx(
        OUTPUT_TARGET DemoRealTimeLighting-ptx
        PTX_LINK_LIBRARIES owl::owl
        SOURCES "rt_program.cuh" "rt_program.cu"
)
target_link_libraries(DemoRealTimeLighting PRIVATE glfw owl::owl DemoRealTimeLighting-ptx)

set_target_properties(DemoRealTimeLighting PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
