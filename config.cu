#include "scene/scene.cuh"
#include "scene/scene_simple.cu"
#include "scene/scene_demo.cu"
#include "scene/scene_farm.cu"

const SceneSimple sceneSimple;
const SceneDemo sceneDemo;
const SceneFarm sceneFarm;

const Scene* SCENE_LIST[] = {&sceneSimple, &sceneDemo, &sceneFarm};