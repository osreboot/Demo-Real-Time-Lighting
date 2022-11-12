#pragma once

#include "scene/scene.cuh"
#include "scene/scene_demo.cu"
#include "scene/scene_farm.cu"
#include "scene/scene_simple.cu"

#define SCENE_SKYBOX_BLACK 0
#define SCENE_SKYBOX_WHITE 1
#define SCENE_SKYBOX_RGB 2
#define SCENE_SKYBOX_UNDERLIT 3

#define SCENE_INDEX 1
#define SCENE_SKYBOX SCENE_SKYBOX_WHITE

extern const Scene* SCENE_LIST[];

#define PROGRAM_MODE_DYNAMIC 0

#define PROGRAM_SAMPLES 2000