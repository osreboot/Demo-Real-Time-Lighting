
#include "Scene.h"
#include "ModelCuboid.h"
#include "ModelObj.h"

using namespace owl;

class SceneCustomModel : public Scene {

private:
    const std::string PATH_BASE = R"(C:\Users\osreboot\Desktop\Gecko\)";
    const std::string PATH_MODEL1 = PATH_BASE + "GeckoBody.obj";
    const std::string PATH_MODEL2 = PATH_BASE + "GeckoEyesInner.obj";
    const std::string PATH_MODEL3 = PATH_BASE + "GeckoEyesLens.obj";
    const std::string PATH_TEXTURE1 = PATH_BASE + "GeckoBody.BMP";
    const std::string PATH_TEXTURE2 = PATH_BASE + "GeckoEyes.BMP";

    //const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.6f, 0.2f, {0.2f, 0.2f, 0.2f}, 0.2f, -1};
    //const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.9f, 0.01f, {0.2f, 0.2f, 0.2f}, 0.2f, -1};
    //const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.2f, 0.2f, 0.2f}, 0.0f, -1};
    //const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.0f, 0.0f, {0.3f, 0.3f, 0.3f}, 0.0f, -1};
    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 0.4f, 0.2f, {0.4f, 0.4f, 0.4f}, 0.0f, -1};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 0.4f, 0.2f, {0.5f, 0.5f, 0.5f}, 0.0f, -1};

    const Material MAT_MODEL1 = {false, 0.0f, 1.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}, 0.0f, 0};
    const Material MAT_MODEL2 = {false, 0.0f, 1.0f, 1.0f, 0.02f, {1.0f, 1.0f, 1.0f}, 0.0f, 1};
    const Material MAT_MODEL3 = {false, 0.98f, 1.1f, 1.0f, 0.0f, {1.0f, 1.0f, 1.0f}, 0.7f, -1};

    const Material MAT_LIGHT = {true, 0.0f, 1.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}, 0.0f, -1};
    const Material MAT_LIGHT_R = {true, 0.0f, 1.0f, 0.0f, 0.0f, {10.0f, 0.0f, 0.0f}, 0.0f, -1};
    const Material MAT_LIGHT_Y = {true, 0.0f, 1.0f, 0.0f, 0.0f, {10.0f, 10.0f, 0.0f}, 0.0f, -1};
    const Material MAT_LIGHT_B = {true, 0.0f, 1.0f, 0.0f, 0.0f, {0.0f, 0.0f, 10.0f}, 0.0f, -1};
    const Material MAT_LIGHT_P = {true, 0.0f, 1.0f, 0.0f, 0.0f, {10.0f, 0.0f, 10.0f}, 0.0f, -1};

    const Material MAT_GLASS = {false, 0.9f, 1.4f, 1.0f, 0.0f, {0.9f, 0.9f, 1.0f}, 0.4f, -1};

public:

    // Creates a simple tiled floor and a custom model on top.
    void build(OWLContext context, std::vector<std::shared_ptr<Model>>& models,
               std::vector<OWLTexture>& textures) const override {
        int indexTile = 0;
        for(float x = -7.0f; x <= 7.0f; x += 1.0f){
            for(float z = -7.0f; z <= 7.0f; z += 1.0f){
                models.emplace_back(new ModelCuboid(vec3f(0.5f), {x, -0.5f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
            }
        }

        // The custom model (multiple pieces) & associated textures
        textures.push_back(loadTexture(context, PATH_TEXTURE1));
        textures.push_back(loadTexture(context, PATH_TEXTURE2));
        models.emplace_back(new ModelObj(PATH_MODEL1, vec3f(1.0f), vec3f(0.0f), 0.0f, MAT_GLASS));
        models.emplace_back(new ModelObj(PATH_MODEL2, vec3f(1.0f), vec3f(0.0f), 0.0f, MAT_GLASS));
        models.emplace_back(new ModelObj(PATH_MODEL3, vec3f(1.0f), vec3f(0.0f), 0.0f, MAT_GLASS));

        // Blocks around the model
        //models.emplace_back(new ModelCuboid(vec3f(0.5f), {-1.25f, -0.25f, 1.5f}, 0.0f, MAT_BASE1));
        //models.emplace_back(new ModelCuboid(vec3f(0.25f), {1.0f, 0.0f, 0.0f}, 0.0f, MAT_LIGHT_R));
        //models.emplace_back(new ModelCuboid(vec3f(0.175f), {0.6f, 0.0f, -1.6f}, 0.0f, MAT_LIGHT_Y));
        //models.emplace_back(new ModelCuboid(vec3f(0.1f), {-0.1f, 0.0f, -2.3f}, 0.0f, MAT_LIGHT_B));
        //models.emplace_back(new ModelCuboid(vec3f(0.15f), {0.8f, 0.0f, 1.5f}, 0.0f, MAT_LIGHT_P));

        // Lights above the model
        //models.emplace_back(new ModelCuboid(vec3f(2.0f), {5.0f, 6.0f, 5.0f}, 0.0f, MAT_LIGHT));
        //models.emplace_back(new ModelCuboid(vec3f(6.0f), {0.0f, 10.0f, 0.0f}, 0.0f, MAT_LIGHT));
    }

    vec3f getCameraDynamicLocation(float timer) const override {
        return {sin(timer) * 6.0f, 2.75f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(float timer) const override {
        return {0.0f, 0.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        //return {1.7f, 0.5f, 5.0f}; // View 1
        //return vec3f(1.7f, 2.6f, 5.0f) * 0.8f; // View 2
        //return {4.0f, 3.0f, -3.0f}; // View 3
        return {-3.5f, 4.0f, -0.75f}; // View 4
    }

    vec3f getCameraStaticTarget() const override {
        //return {0.0f, 0.5f, 0.0f}; // View 1
        //return {0.0f, 0.25f, 0.0f}; // View 2/3
        return {-0.5f, 0.0f, -0.75f}; // View 4
    }

};
