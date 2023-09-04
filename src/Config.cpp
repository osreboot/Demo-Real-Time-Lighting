
#include "scene/Scene.h"
#include "scene/SceneSimple.cpp"
#include "scene/SceneDemo.cpp"
#include "scene/SceneTutorial.cpp"
#include "scene/SceneFarm.cpp"
#include "scene/SceneId.cpp"
#include "scene/SceneComposite.cpp"
#include "scene/SceneBackground.cpp"

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