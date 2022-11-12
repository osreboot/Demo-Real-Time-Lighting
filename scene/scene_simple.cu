#include "scene.cuh"

#define INDEX_MATERIAL 5

class SceneSimple : public Scene{

private:
    const Material MAT_BASE1 = {false, 0.0f, 0.0f, 0.2f, 0.02f, {0.4f, 0.4f, 0.4f}};
    const Material MAT_BASE2 = {false, 0.0f, 0.0f, 0.2f, 0.02f, {0.8f, 0.8f, 0.8f}};

    const Material materials[7] = {
            {true, 0.0f, 0.0f, 0.0f, 0.01f, {1.0f, 1.0f, 0.0f}}, // GLOWING
            {false, 0.0f, 0.0f, 1.0f, 0.01f, {0.6f, 0.0f, 0.0f}}, // SHINY
            {false, 0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.6f, 0.0f}}, // LAMBERTIAN
            {false, 0.0f, 0.0f, 1.0f, 0.005f, {0.8f, 0.8f, 0.8f}}, // CHROME
            {false, 0.98f, 1.05f, 1.0f, 0.0f, {0.95f, 0.95f, 1.0f}}, // GLASS
            {false, 0.5f, 1.0f, 1.0f, 0.8f, {0.95f, 1.0f, 1.0f}}, // FROSTED GLASS
            {false, 0.0f, 0.0f, 0.5f, 0.5f, {0.6f, 0.0f, 0.6f}}, // DIELECTRIC
    };

public:
    void initialize(const SceneBuildParams& params) const override {
        int indexTile = 0;
        for(float x = -5.0f; x <= 5.0f; x += 1.0f){
            for(float z = -5.0f; z <= 5.0f; z += 1.0f){
                addBox(params, vec3f(0.5f), {x, -2.5f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2);
            }
        }

        const Material& material = materials[INDEX_MATERIAL];

        addBox(params, vec3f(1.0f), {0.0f, 0.0f, 0.0f}, 0.0f, material);

        for(vec3f m : {vec3f(1.0f, 1.0f, 1.0f), vec3f(-1.0f, -1.0f, 1.0f), vec3f(1.0f, -1.0f, -1.0f), vec3f(-1.0f, 1.0f, -1.0f)}){
            addBox(params, vec3f(0.6f), m * 0.7f, 0.0f, material);
            addBox(params, vec3f(0.4f), -m * 0.7f, 0.0f, material);
            /*for(vec3f m2 : {vec3f(1.15f, 0.7f, 0.7f), vec3f(0.7f, 1.15f, 0.7f), vec3f(0.7f, 0.7f, 1.15f)}) {
                addBox(params, vec3f(0.2f), m * m2, 0.0f, material);
            }*/
        }

    }

    vec3f getCameraDynamicLocation(const float timer) const override {
        return {sin(timer) * 6.0f, 1.75f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(const float timer) const override {
        return {0.0f, 0.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return {5.0f, 1.75f, -3.0f};
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, 0.0f, 0.0f};
    }

};