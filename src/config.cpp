#include "scene/scene.h"
#include "scene/scene_simple.cpp"
#include "scene/scene_demo.cpp"
#include "scene/scene_tutorial.cpp"
#include "scene/scene_farm.cpp"
#include "scene/scene_id.cpp"
#include "scene/scene_composite.cpp"
#include "scene/scene_background.cpp"

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