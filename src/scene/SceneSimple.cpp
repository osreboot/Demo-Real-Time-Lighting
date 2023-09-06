
#include "Scene.h"
#include "ModelCuboid.h"

using namespace owl;

class SceneSimple : public Scene {

private:
    const int INDEX_MATERIAL0 = 3;
    const int INDEX_MATERIAL1 = 4;
    const int INDEX_MATERIAL2 = 4;

    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.2f, 0.02f, {0.6, 0.6f, 0.6f}};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.2f, 0.02f, {0.8f, 0.8f, 0.8f}};

    const Material materials[7] = {
            {true, 0.0f, 1.0f, 0.0f, 0.01f, {0.2f, 0.2f, 1.0f}}, // GLOWING
            {false, 0.0f, 1.0f, 1.0f, 0.01f, {0.0f, 0.0f, 0.6f}}, // SHINY
            {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.0f, 0.6f, 0.0f}}, // LAMBERTIAN
            {false, 0.0f, 1.0f, 1.0f, 0.005f, {0.8f, 0.8f, 0.8f}}, // CHROME
            {false, 0.98f, 1.2f, 1.0f, 0.0f, {0.95f, 0.95f, 0.95f}}, // GLASS
            {false, 0.9f, 1.04f, 1.0f, 0.2f, {0.9f, 0.9f, 0.9f}}, // FROSTED GLASS
            {false, 0.0f, 1.0f, 0.9f, 0.4f, {0.6f, 0.0f, 0.6f}}, // DIELECTRIC
    };

public:

    // Creates a cube with a bunch of interconnected side cubes and a cube at the core. Change the macro numbers to swap between different materials!
    void build(OWLContext context, std::vector<std::shared_ptr<Model>>& models,
               std::vector<OWLTexture>& textures) const override {
        int indexTile = 0;
        for(float x = -5.0f; x <= 5.0f; x += 1.0f){
            for(float z = -5.0f; z <= 5.0f; z += 1.0f){
                models.emplace_back(new ModelCuboid(vec3f(0.5f), {x, -2.5f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
            }
        }

        models.emplace_back(new ModelCuboid(vec3f(0.2f), {0.0f, 0.0f, 0.0f}, 0.0f, materials[INDEX_MATERIAL0]));
        models.emplace_back(new ModelCuboid(vec3f(1.0f), {0.0f, 0.0f, 0.0f}, 0.0f, materials[INDEX_MATERIAL1]));

        for(vec3f m : {vec3f(1.0f, 1.0f, 1.0f), vec3f(-1.0f, -1.0f, 1.0f), vec3f(1.0f, -1.0f, -1.0f), vec3f(-1.0f, 1.0f, -1.0f)}){
            models.emplace_back(new ModelCuboid(vec3f(0.6f), m * 0.7f, 0.0f, materials[INDEX_MATERIAL2]));
            models.emplace_back(new ModelCuboid(vec3f(0.4f), -m * 0.7f, 0.0f, materials[INDEX_MATERIAL2]));
        }

    }

    vec3f getCameraDynamicLocation(float timer) const override {
        return {sin(timer) * 6.0f, 2.75f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(float timer) const override {
        return {0.0f, 0.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return vec3f(5.5f, 2.0f, -2.0f) * 1.0f;
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, -0.3f, 0.0f};
    }

};
