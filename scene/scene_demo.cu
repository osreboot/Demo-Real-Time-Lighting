#include <owl/owl.h>

#include "scene.cuh"

class SceneDemo : public Scene{

private:
    const Material MAT_BASE1 = {false, 0.0f, 0.0f, 0.5f, 0.02f, {0.4f, 0.4f, 0.4f}};
    const Material MAT_BASE2 = {false, 0.0f, 0.0f, 0.5f, 0.2f, {0.8f, 0.8f, 0.8f}};
    const Material MAT_CHROME = {false, 0.0f, 0.0f, 1.0f, 0.01f, {0.95f, 0.95f, 0.95f}};
    const Material MAT_BLUE = {false, 0.0f, 0.0f, 0.5f, 0.01f, {0.4f, 0.4f, 0.8f}};
    const Material MAT_BLUE_GLASS = {false, 0.5f, 1.33f, 0.5f, 0.0f, {0.4f, 0.4f, 0.8f}};
    const Material MAT_GLASS1 = {false, 0.6f, 1.5f, 0.5f, 0.0f, {0.4f, 0.8f, 0.4f}};
    const Material MAT_GLASS2 = {false, 0.99f, 1.05f, 0.1f, 0.0f, {1.0f, 1.0f, 1.0f}};
    const Material MAT_FLAT1 = {false, 0.0f, 0.0f, 1.0f, 0.5f, {0.5f, 0.5f, 0.0f}};
    const Material MAT_FLAT2 = {false, 0.0f, 0.0f, 0.1f, 0.0f, {0.0f, 0.5f, 0.0f}};
    const Material MAT_LIGHT = {true, 0.0f, 0.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}};
    const Material MAT_LIGHT_BLUE = {true, 0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 1.0f}};

public:
    void initialize(const SceneBuildParams &params) const override {
        int indexTile = 0;
        for(float x = -10.0f; x <= 10.0f; x += 2.0f){
            for(float y = -10.0f; y <= 10.0f; y += 2.0f){
                addBox(params, vec3f(1.0f), vec3f(x, -1.0f, y), 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2);
            }
        }

        addBox(params, vec3f(0.75f), vec3f(-2.0f, 0.75f, 0.0f), 0.0f, MAT_CHROME);
        addBox(params, vec3f(0.5f), vec3f(2.0f, 0.5f, 3.0f), 45.0f, MAT_CHROME);

        addBox(params, vec3f(6.0f * 0.2f, 0.1f, 6.0f * 0.2f), vec3f(0.0f, 0.1f, -3.0f), 0.0f, MAT_CHROME);
        for(float x = -5.0f; x <= 5.0f; x++){
            for(float y = -5.0f; y <= 5.0f; y++){
                addBox(params, vec3f(0.08f, random() * 0.2f,0.08f), vec3f(x * 0.2f, 0.2f, -3.0f + y * 0.2f), 0.0f, MAT_CHROME);
            }
        }

        addBox(params, vec3f(0.5f, 0.5f, 0.1f), vec3f(0.0f, 0.5f, 3.0f), 0.0f, MAT_CHROME);
        addBox(params, vec3f(0.5f, 0.5f, 0.1f), vec3f(0.0f, 0.5f, 4.0f), 0.0f, MAT_CHROME);

        addBox(params, vec3f(3.0f, 0.1f, 1.0f), vec3f(6.0f, 0.1f, 0.0f), 0.0f, MAT_BLUE);
        indexTile = 0;
        for(float x = -2.5f; x <= 2.5f; x += 0.5f){
            addBox(params, vec3f(0.04f, 0.2f + (x + 2.5f) / 5.0f, 0.4f), vec3f(6.0f + x, 0.3f, cos(x * 1.5f) * 0.5f), sin(x * 1.5f) * 25.0f,
                   indexTile++ % 2 == 0 ? MAT_BLUE : MAT_BLUE_GLASS);
        }

        addBox(params, vec3f(0.5f, 3.0f, 0.5f), vec3f(-3.0f, 2.8f, -3.0f), 0.0f, MAT_GLASS1);
        addBox(params, vec3f(0.6f, 0.6f, 0.6f), vec3f(0.0f, 0.8f, 0.0f), 0.0f, MAT_GLASS2);
        addBox(params, vec3f(0.4f, 0.4f, 0.4f), vec3f(-3.0f, 0.6f, 3.0f), 0.0f, MAT_GLASS2);
        addBox(params, vec3f(0.3f, 0.3f, 0.3f), vec3f(-2.8f, 0.3f, 3.2f), 0.0f, MAT_CHROME);
        addBox(params, vec3f(0.3f, 0.3f, 0.3f), vec3f(-3.2f, 0.8f, 2.8f), 0.0f, MAT_CHROME);

        addBox(params, vec3f(0.5f, 0.5f, 0.5f), vec3f(3.0f, 0.7f, -3.0f), 45.0f, MAT_FLAT1);
        addBox(params, vec3f(0.3f, 0.3f, 0.3f), vec3f(3.4f, 0.7f, -3.0f), 45.0f, MAT_FLAT1);
        addBox(params, vec3f(0.3f, 0.3f, 0.3f), vec3f(2.6f, 0.7f, -3.0f), 45.0f, MAT_FLAT1);
        addBox(params, vec3f(1.0f, 0.05f, 1.0f), vec3f(3.0f, 0.05f, -3.0f), 45.0f, MAT_FLAT2);

        addBox(params, vec3f(1.0f, 0.05f, 1.0f), vec3f(3.0f, 2.0f, -3.0f), 45.0f, MAT_LIGHT);
        addBox(params, vec3f(1.0f, 0.05f, 1.0f), vec3f(-3.0f, 2.0f, 3.0f), 45.0f, MAT_LIGHT_BLUE);
    }

    vec3f getCameraLocation(const float timer) const override {
        return vec3f(sin(timer) * 10.0f, 4.0f, cos(timer) * 10.0f);
    }

    vec3f getCameraTarget(const float timer) const override {
        return vec3f(0.0f);
    }

};