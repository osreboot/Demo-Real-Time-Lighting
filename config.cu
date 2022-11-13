#include "scene/scene.cuh"
#include "scene/scene_simple.cu"
#include "scene/scene_demo.cu"
#include "scene/scene_farm.cu"
#include "scene/scene_id.cu"

const SceneSimple sceneSimple;
const SceneDemo sceneDemo;
const SceneFarm sceneFarm;
const SceneId sceneId;

const Scene* SCENE_LIST[] = {&sceneSimple, &sceneDemo, &sceneFarm, &sceneId};