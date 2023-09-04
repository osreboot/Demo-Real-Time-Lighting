# What is this?
This project demonstrates the application of NVIDIA Ray-Tracing Core hardware acceleration in real-time lighting simulation and photorealistic rendering. The program renders scenes to a frame buffer object, which can then be sent to either: 1) an image file or 2) a real-time OpenGL system window. This project depends on [OWL](https://github.com/owl-project/owl), [stb](https://github.com/nothings/stb), and the standard CUDA libraries.

# Requirements
This project must be run on a system with a NVIDIA graphics card with the Volta microarchitecture or newer (e.g. RTX 2070).

The [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads) and [OptiX](https://developer.nvidia.com/rtx/ray-tracing/optix) must be installed. `OptiX_ROOT_DIR` needs to be assigned in the top-level *CMakeLists.txt*.

# Usage
There are several parts to this project:

`Main.cu` contains the main program function and the runtime loop for the continuous execution mode.

`Config.h`, `Config.cpp` contain the settings that dictate all aspects of the program, such as the current scene, the sky color, whether to run in snapshot or continuous mode, how many samples to capture per pixel, and which lighting features to enable.

`RayTracerHost.cpp` handles OptiX initialization and shader program invocation.

`RayTracerDevice.cuh`, `RayTracerDevice.cu` contain everything related to the actual shader program; data structures, light calculations, and the shader programs themselves.

`Display.cpp` handles GLFW and OpenGL interactions such as initializing the system window, allocating the FBO, updating the system window, and rendering the FBO to a quad.

The `scene` folder contains a number of scene definitions that, when selected in the configuration file, each generate unique arrangements of world geometries and materials.

# Examples
*Tutorial* scene rendered at 20,000 samples per pixel.
<p align="center">
  <img src="https://github.com/osreboot/Demo-Real-Time-Lighting/blob/main/examples/exampleSceneTutorial.png" alt="Tutorial Scene">
</p>

*Farm* scene rendered at 20,000 samples per pixel.
<p align="center">
  <img src="https://github.com/osreboot/Demo-Real-Time-Lighting/blob/main/examples/exampleSceneFarm.png" alt="Farm Scene">
</p>

*Background* scene rendered at 20,000 samples per pixel.
<p align="center">
  <img src="https://github.com/osreboot/Demo-Real-Time-Lighting/blob/main/examples/exampleSceneBackground.png" alt="Background Scene">
</p>
