#include "scene/scene.cuh"
#include "scene/scene_simple.cu"
#include "scene/scene_demo.cu"
#include "scene/scene_tutorial.cu"
#include "scene/scene_farm.cu"
#include "scene/scene_id.cu"
#include "scene/scene_composite.cu"
#include "scene/scene_background.cu"

const SceneSimple sceneSimple;
const SceneDemo sceneDemo;
const SceneTutorial sceneTutorial;
const SceneFarm sceneFarm;
const SceneId sceneId;
const SceneComposite sceneComposite;
const SceneBackground sceneBackground;

// List of available scenes
const Scene* SCENE_LIST[] = {&sceneSimple, &sceneDemo, &sceneTutorial, &sceneFarm, &sceneId, &sceneComposite,
                             &sceneBackground};