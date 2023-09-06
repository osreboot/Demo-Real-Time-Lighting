
#include "Scene.h"
#include "ModelCuboid.h"

using namespace owl;

class SceneBackground : public Scene {

private:
    const float value1 = /*0.95f;*/0.7f;
    const float value2 = /*0.9f;*/0.5f;
    const bool close = false;

    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.0f, 0.0f, vec3f(value1)};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.0f, 0.0f, vec3f(value2)};
    const Material MAT_LIGHT1 = {true, 0.0f, 1.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}};
    const Material MAT_GLASS1 = {false, 0.95f, 1.3f, 1.0f, 0.25f, {0.95f, 0.95f, 0.95f}};
    const Material MAT_GLASS2 = {false, 0.95f, 1.3f, 1.0f, 0.5f, {0.9f, 0.9f, 0.9f}};

public:

    // Creates a hallway with decorative lights on the walls and ceiling
    void build(OWLContext context, std::vector<std::shared_ptr<Model>>& models,
               std::vector<OWLTexture>& textures) const override {
        int indexTile = 0;
        for(float z = -2.0f; z <= 2.0f; z += 0.5f){
            models.emplace_back(new ModelCuboid(vec3f(0.25f, 1.0f, 0.25f), {-2.0f, 0.0f, z}, 0.0f, indexTile % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
            models.emplace_back(new ModelCuboid(vec3f(0.25f, 1.0f, 0.25f), {2.0f, 0.0f, z}, 0.0f, indexTile % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
            if(indexTile % 2 == 1){
                models.emplace_back(new ModelCuboid(vec3f(0.25f, 1.0f, 0.1f), {1.95f, 0.0f, z}, 0.0f, MAT_LIGHT1));
                models.emplace_back(new ModelCuboid(vec3f(0.25f, 1.0f, 0.1f), {-1.95f, 0.0f, z}, 0.0f, MAT_LIGHT1));
                models.emplace_back(new ModelCuboid(vec3f(0.29f, 1.0f, 0.14f), {1.95f, 0.0f, z}, 0.0f, MAT_GLASS2));
                models.emplace_back(new ModelCuboid(vec3f(0.29f, 1.0f, 0.14f), {-1.95f, 0.0f, z}, 0.0f, MAT_GLASS2));
                models.emplace_back(new ModelCuboid(vec3f(0.30f, 0.2f, 0.15f), {-1.95f, -0.75f, z}, 0.0f, MAT_BASE2));
                models.emplace_back(new ModelCuboid(vec3f(0.30f, 0.1f, 0.15f), {-1.95f, 0.75f, z}, 0.0f, MAT_BASE2));
                models.emplace_back(new ModelCuboid(vec3f(0.30f, 0.2f, 0.15f), {1.95f, -0.75f, z}, 0.0f, MAT_BASE2));
                models.emplace_back(new ModelCuboid(vec3f(0.30f, 0.1f, 0.15f), {1.95f, 0.75f, z}, 0.0f, MAT_BASE2));
            }
            for(float x = -2.0f; x <= 2.0f; x += 0.5f){
                models.emplace_back(new ModelCuboid(vec3f(0.25f), {x, -1.0f + (float)(rand() % 100) / 1000.0f, z}, 0.0f, indexTile % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
                if(indexTile % 2 == 0) models.emplace_back(new ModelCuboid(vec3f(0.25f), {x, 1.0f, z}, 0.0f, indexTile % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
                indexTile++;
            }
        }

        models.emplace_back(new ModelCuboid(vec3f(2.0f, 1.0f, 0.25f), {0.0f, 0.0f, 2.5f}, 0.0f, MAT_BASE1));
        models.emplace_back(new ModelCuboid(vec3f(2.0f, 0.1f, 2.5f), {0.0f, 0.85f + 0.25f, 0.0f}, 0.0f, MAT_LIGHT1));
        models.emplace_back(new ModelCuboid(vec3f(2.0f, 0.02f, 2.5f), {0.0f, 0.79f, 0.0f}, 0.0f, MAT_GLASS1));
    }

    vec3f getCameraDynamicLocation(float timer) const override {
        return getCameraStaticLocation(); // Not supported for this scene
    }

    vec3f getCameraDynamicTarget(float timer) const override {
        return getCameraStaticTarget(); // Not supported for this scene
    }

    vec3f getCameraStaticLocation() const override {
        return vec3f(0.0f, 0.0f, close ? -0.25f : -3.5f);
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, 0.0f, close ? 5.0f : 0.0f};
    }

};
