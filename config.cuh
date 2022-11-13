#pragma once

#include "scene/scene.cuh"

#define SCENE_SKYBOX_BLACK 0
#define SCENE_SKYBOX_WHITE 1
#define SCENE_SKYBOX_RGB 2
#define SCENE_SKYBOX_UNDERLIT 3

#define SCENE_INDEX 3
#define SCENE_SKYBOX SCENE_SKYBOX_WHITE

extern const Scene* SCENE_LIST[];

#define PROGRAM_MODE_DYNAMIC 1

#define PROGRAM_SAMPLES 20