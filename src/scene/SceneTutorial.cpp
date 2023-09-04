
#include "Scene.h"

class SceneTutorial : public Scene {

private:
    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.3f, 0.05f, {0.4f, 0.4f, 0.4f}};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.3f, 0.05f, {0.6f, 0.6f, 0.6f}};

    const Material MAT_LAMBERT = {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.8f, 0.0f, 0.0f}};
    const Material MAT_LIGHT = {true, 0.0f, 1.0f, 0.0f, 0.01f, {0.5f, 1.0f, 0.5f}};
    const Material MAT_GLASS = {false, 0.98f, 1.5f, 1.0f, 0.0f, {0.95f, 0.95f, 1.0f}};
    const Material MAT_CHROME = {false, 0.0f, 1.0f, 1.0f, 0.01f, {0.95f, 0.95f, 0.95f}};

public:
    // Demonstration scene that is as simple as possible (while still having each type of material).
    void initialize(SceneBuildParams& params) const override {
        int indexTile = 0;
        for(float x = -5.0f; x <= 5.0f; x += 0.5f){
            for(float z = -5.0f; z <= 5.0f; z += 0.5f){
                addBox(params, vec3f(0.25f), {x, -0.25f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2);
            }
        }

        addBox(params, vec3f(0.5f), {-1.8f, 0.6f, 0.0f}, 0.0f, MAT_LAMBERT);
        addBox(params, vec3f(0.5f), {-0.6f, 0.6f, 0.0f}, 0.0f, MAT_LIGHT);
        addBox(params, vec3f(0.5f), {0.6f, 0.6f, 0.0f}, 0.0f, MAT_GLASS);
        addBox(params, vec3f(0.5f), {1.8f, 0.6f, 0.0f}, 0.0f, MAT_CHROME);
    }

    vec3f getCameraDynamicLocation(const float timer) const override {
        return vec3f(sin(timer) * 4.0f, 2.5f, cos(timer) * 4.0f) * 1.2f;
    }

    vec3f getCameraDynamicTarget(const float timer) const override {
        return {0.0f, 0.4f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return vec3f(2.3f, 2.5f, 3.5f) * 1.2f;
    }

    vec3f getCameraStaticTarget() const override {
        return {0.3f, 0.4f, 0.0f};
    }

};
