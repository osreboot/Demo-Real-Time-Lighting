#include "scene/scene.cuh"
#include "scene/scene_demo.cu"
#include "scene/scene_farm.cu"

#define SCENE_SKYBOX_BLACK 0
#define SCENE_SKYBOX_RGB 1
#define SCENE_SKYBOX_UNDERLIT 2

#define SCENE_INDEX 1
#define SCENE_SKYBOX SCENE_SKYBOX_BLACK

const SceneDemo sceneDemo;
const SceneFarm sceneFarm;

const Scene* SCENE_LIST[] = {&sceneDemo, &sceneFarm};

#define PROGRAM_SAMPLES 10