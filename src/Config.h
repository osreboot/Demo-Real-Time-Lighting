#pragma once

#include "scene/Scene.h"

#define SCENE_SKYBOX_BLACK 0
#define SCENE_SKYBOX_WHITE 1
#define SCENE_SKYBOX_RGB 2
#define SCENE_SKYBOX_UNDERLIT 3
#define SCENE_SKYBOX_NIGHT 4

// Select the current scene and skybox
#define SCENE_INDEX 0
#define SCENE_SKYBOX SCENE_SKYBOX_UNDERLIT

extern const Scene* SCENE_LIST[];

// 0 = render scene to png, 1 = render scene in real-time
#define PROGRAM_MODE_DYNAMIC 1

// Number of raycasts per pixel
#define PROGRAM_SAMPLES 20

// Disable specific shader features for demonstration purposes
#define SHADER_SKYBOX_DIRECTIONAL 1
#define SHADER_SCATTERING 1
#define SHADER_LAMBERTIAN_REFLECTION 1
#define SHADER_REFRACTION 1
#define SHADER_SCHLICK_REFLECTION 1
#define SHADER_FULLBRIGHT_MATERIALS 1