
#include "Scene.h"
#include "ModelCuboid.h"
#include "ModelObj.h"

using namespace owl;

class SceneCustomModel : public Scene {

private:
    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.6f, 0.06f, {0.2f, 0.2f, 0.2f}, 0.2f};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.8f, 0.02f, {0.2f, 0.2f, 0.2f}, 0.2f};

    const Material MAT_MODEL1 = {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.2f, 0.8f, 0.2f}};
    const Material MAT_MODEL2 = {false, 0.0f, 1.0f, 1.0f, 0.02f, {0.0f, 0.0f, 0.0f}, 0.6f};

public:

    // Creates a simple tiled floor and a custom model on top.
    void build(std::vector<std::shared_ptr<Model>>& models) const override {
        int indexTile = 0;
        for(float x = -5.0f; x <= 5.0f; x += 1.0f){
            for(float z = -5.0f; z <= 5.0f; z += 1.0f){
                models.emplace_back(new ModelCuboid(vec3f(0.5f), {x, -0.5f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
            }
        }

        models.emplace_back(new ModelObj(R"(C:\Users\osreboot\Desktop\Gecko_m_body.obj)",
                                         vec3f(1.0f), vec3f(0.0f), 0.0f, MAT_MODEL1));
        models.emplace_back(new ModelObj(R"(C:\Users\osreboot\Desktop\Gecko_m_eyes.obj)",
                                         vec3f(1.0f), vec3f(0.0f), 0.0f, MAT_MODEL2));

        models.emplace_back(new ModelCuboid(vec3f(2.0f), {5.0f, 6.0f, 5.0f}, 0.0f, {true, 0.0f, 1.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}}));
    }

    vec3f getCameraDynamicLocation(float timer) const override {
        return {sin(timer) * 6.0f, 2.75f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(float timer) const override {
        return {0.0f, 0.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return {5.5f, 1.0f, -2.0f};
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, -0.3f, 0.0f};
    }

};
