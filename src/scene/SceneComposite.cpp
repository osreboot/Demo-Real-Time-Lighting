
#include "Scene.h"

class SceneComposite : public Scene {

private:
    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.3f, 0.05f, {0.4f, 0.4f, 0.4f}};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.3f, 0.05f, {0.3f, 0.3f, 0.3f}};

    const Material MAT_BASE3 = {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.5f, 0.5f, 0.5f}};

    const Material MAT_CUBE1 = {false, 0.95f, 1.3f, 1.0f, 0.0f, {1.0f, 1.0f, 0.95f}};
    const Material MAT_CUBE2 = {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.2f, 0.7f, 0.2f}};
    const Material MAT_CUBE3 = {false, 0.0f, 1.0f, 1.0f, 0.7f, {0.8f, 0.3f, 0.3f}};
    const Material MAT_CUBE4 = {false, 0.95f, 1.0f, 0.0f, 0.3f, {1.0f, 0.7f, 1.0f}};
    const Material MAT_CUBE5 = {false, 0.0f, 1.0f, 1.0f, 0.01f, {0.3f, 0.8f, 0.8f}};
    const Material MAT_CUBE6 = {true, 0.0f, 1.0f, 0.0f, 0.01f, {0.5f, 0.5f, 1.0f}};

public:

    // Creates a cube with interconnected sides of varying materials
    void initialize(SceneBuildParams& params) const override {
        int indexTile = 0;
        for(float x = -2.0f; x <= 2.0f; x += 0.5f){
            for(float z = -2.0f; z <= 2.0f; z += 0.5f){
                addBox(params, vec3f(0.25f), {x, -1.15f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2);
            }
        }

        addBox(params, vec3f(0.5f), {0.0f, 0.0f, 0.0f}, 0.0f, MAT_BASE3);

        addBox(params, vec3f(0.5f, 0.05f, 0.5f), {0.0f, 0.5f, 0.0f}, 0.0f, MAT_CUBE1);
        addBox(params, vec3f(0.2f), {0.0f, 0.5f, 0.0f}, 0.0f, MAT_CUBE1);
        addBox(params, vec3f(0.5f, 0.025f, 0.5f), {0.0f, -0.525f, 0.0f}, 0.0f, MAT_CUBE6);
        addBox(params, vec3f(0.2f), {0.0f, -0.5f, 0.0f}, 0.0f, MAT_CUBE6);

        addBox(params, vec3f(0.5f, 0.5f, 0.025f), {0.0f, 0.0f, 0.525f}, 0.0f, MAT_CUBE2);
        addBox(params, vec3f(0.2f), {0.0f, 0.0f, 0.5f}, 0.0f, MAT_CUBE2);
        addBox(params, vec3f(0.5f, 0.5f, 0.025f), {0.0f, 0.0f, -0.525f}, 0.0f, MAT_CUBE4);
        addBox(params, vec3f(0.2f), {0.0f, 0.0f, -0.5f}, 0.0f, MAT_CUBE4);

        addBox(params, vec3f(0.025f, 0.5f, 0.5f), {0.525f, 0.0f, 0.0f}, 0.0f, MAT_CUBE3);
        addBox(params, vec3f(0.2f), {0.5f, 0.0f, 0.0f}, 0.0f, MAT_CUBE3);
        addBox(params, vec3f(0.025f, 0.5f, 0.5f), {-0.525f, 0.0f, 0.0f}, 0.0f, MAT_CUBE5);
        addBox(params, vec3f(0.2f), {-0.5f, 0.0f, 0.0f}, 0.0f, MAT_CUBE5);

        addBox(params, vec3f(0.05f, 0.05f, 0.1f), {0.525f, 0.525f, 0.0f}, 0.0f, MAT_CUBE1);
        addBox(params, vec3f(0.05f, 0.1f, 0.05f), {0.525f, 0.0f, 0.525f}, 0.0f, MAT_CUBE3);
        addBox(params, vec3f(0.1f, 0.05f, 0.05f), {0.0f, 0.525f, 0.525f}, 0.0f, MAT_CUBE2);
        addBox(params, vec3f(0.05f, 0.05f, 0.1f), {0.525f, -0.525f, 0.0f}, 0.0f, MAT_CUBE6);
        addBox(params, vec3f(0.05f, 0.1f, 0.05f), {0.525f, 0.0f, -0.525f}, 0.0f, MAT_CUBE3);
        addBox(params, vec3f(0.1f, 0.05f, 0.05f), {0.0f, 0.525f, -0.525f}, 0.0f, MAT_CUBE4);
        addBox(params, vec3f(0.05f, 0.05f, 0.1f), {-0.525f, -0.525f, 0.0f}, 0.0f, MAT_CUBE6);
        addBox(params, vec3f(0.05f, 0.1f, 0.05f), {-0.525f, 0.0f, -0.525f}, 0.0f, MAT_CUBE5);
        addBox(params, vec3f(0.1f, 0.05f, 0.05f), {0.0f, -0.525f, -0.525f}, 0.0f, MAT_CUBE4);
        addBox(params, vec3f(0.05f, 0.05f, 0.1f), {-0.525f, 0.525f, 0.0f}, 0.0f, MAT_CUBE1);
        addBox(params, vec3f(0.05f, 0.1f, 0.05f), {-0.525f, 0.0f, 0.525f}, 0.0f, MAT_CUBE5);
        addBox(params, vec3f(0.1f, 0.05f, 0.05f), {0.0f, -0.525f, 0.525f}, 0.0f, MAT_CUBE2);
    }

    vec3f getCameraDynamicLocation(const float timer) const override {
        return {sin(timer) * 2.5f, 1.7f, cos(timer) * 2.5f};
    }

    vec3f getCameraDynamicTarget(const float timer) const override {
        return {0.0f, -0.2f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return vec3f(-2.5f, 1.7f, -2.5f) * 0.7f;
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, -0.2f, 0.0f};
    }

};
