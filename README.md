# What is this?
This project demonstrates the application of NVIDIA Ray-Tracing Core hardware acceleration in real-time lighting simulation and photorealistic rendering. This project depends on [OWL](https://github.com/owl-project/owl), [stb](https://github.com/nothings/stb), and the standard CUDA libraries.

# Usage
There are several parts to this project:

`main.cu` contains the main program function and the runtime loop for the continuous execution mode.

`config.cuh`, `config.cu` contain the settings that dictate all aspects of the program, such as the current scene, the sky color, whether to run in snapshot or continuous mode, how many samples to capture per pixel, and which lighting features to enable.

`rt_setup.cuh`, `rt_setup.cu` handle OptiX initialization and shader program invocation.

`rt_program.cuh`, `rt_program.cu` contain everything related to the actual shader program; data structures, light calculations, and the shader programs themselves.

`display.cuh`, `display.cu` handle GLFW and OpenGL interactions such as initializing the system window, allocating the FBO, updating the system window, and rendering the FBO to a quad.

The `scene` folder contains a number of scene definitions that, when selected in the configuration file, each generate unique arrangements of world geometries and materials.

# Examples
*Tutorial* scene rendered at 20,000 samples per pixel.
<p align="center">
  <img src="https://github.com/osreboot/Demo-Real-Time-Lighting/blob/main/examples/exampleSceneTutorial.png" alt="Tutorial Scene">
</p>
