#include "scene.cuh"

#define INDEX_MATERIAL0 3
#define INDEX_MATERIAL1 4
#define INDEX_MATERIAL2 4

class SceneSimple : public Scene{

private:
    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.2f, 0.02f, {0.4f, 0.4f, 0.4f}};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.2f, 0.02f, {0.8f, 0.8f, 0.8f}};

    const Material materials[7] = {
            {true, 0.0f, 1.0f, 0.0f, 0.01f, {1.0f, 1.0f, 0.5f}}, // GLOWING
            {false, 0.0f, 1.0f, 1.0f, 0.01f, {0.6f, 0.0f, 0.0f}}, // SHINY
            {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.0f, 0.6f, 0.0f}}, // LAMBERTIAN
            {false, 0.0f, 1.0f, 1.0f, 0.005f, {0.8f, 0.8f, 0.8f}}, // CHROME
            {false, 0.98f, 1.5f, 1.0f, 0.0f, {0.95f, 0.95f, 1.0f}}, // GLASS
            {false, 0.9f, 1.04f, 1.0f, 0.2f, {0.9f, 0.9f, 0.9f}}, // FROSTED GLASS
            {false, 0.0f, 1.0f, 0.5f, 0.5f, {0.6f, 0.0f, 0.6f}}, // DIELECTRIC
    };

public:

    // Creates a cube with a bunch of interconnected side cubes and a cube at the core. Change the macro numbers to swap between different materials!
    void initialize(const SceneBuildParams& params) const override {
        int indexTile = 0;
        for(float x = -5.0f; x <= 5.0f; x += 1.0f){
            for(float z = -5.0f; z <= 5.0f; z += 1.0f){
                addBox(params, vec3f(0.5f), {x, -2.5f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2);
            }
        }

        addBox(params, vec3f(0.2f), {0.0f, 0.0f, 0.0f}, 0.0f, materials[INDEX_MATERIAL0]);
        addBox(params, vec3f(1.0f), {0.0f, 0.0f, 0.0f}, 0.0f, materials[INDEX_MATERIAL1]);

        for(vec3f m : {vec3f(1.0f, 1.0f, 1.0f), vec3f(-1.0f, -1.0f, 1.0f), vec3f(1.0f, -1.0f, -1.0f), vec3f(-1.0f, 1.0f, -1.0f)}){
            addBox(params, vec3f(0.6f), m * 0.7f, 0.0f, materials[INDEX_MATERIAL2]);
            addBox(params, vec3f(0.4f), -m * 0.7f, 0.0f, materials[INDEX_MATERIAL2]);
        }

    }

    vec3f getCameraDynamicLocation(const float timer) const override {
        return {sin(timer) * 6.0f, 2.75f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(const float timer) const override {
        return {0.0f, 0.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return vec3f(5.5f, 2.0f, -2.0f) * 0.8f;
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, -0.5f, 0.0f};
    }

};