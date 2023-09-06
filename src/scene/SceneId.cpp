
#include "Scene.h"
#include "ModelCuboid.h"

using namespace owl;

class SceneId : public Scene {

private:
    const int INDEX_MATERIAL1 = 1;
    const int INDEX_MATERIAL2 = 1;

    const Material MAT_BASE1 = {false, 0.0f, 1.0f, 1.0f, 0.01f, {0.5f, 0.5f, 0.8f}};
    const Material MAT_BASE2 = {false, 0.0f, 1.0f, 1.0f, 0.4f, {0.5f, 0.5f, 0.8f}};
    const Material MAT_NUMBERS = {false, 0.0f, 1.0f, 1.0f, 0.005f, {0.8f, 0.8f, 0.8f}};

public:

    // This scene renders my (Calvin Weaver) student ID in big reflective letters with an interesting dual material tiled floor.
    void build(OWLContext context, std::vector<std::shared_ptr<Model>>& models,
               std::vector<OWLTexture>& textures) const override {
        int indexTile = 0;
        for(float x = -5.0f; x <= 5.0f; x += 1.0f){
            for(float z = -5.0f; z <= 5.0f; z += 1.0f){
                models.emplace_back(new ModelCuboid(vec3f(0.5f), {x, -0.5f - 0.45f - 0.02f, z}, 0.0f, indexTile++ % 2 == 0 ? MAT_BASE1 : MAT_BASE2));
            }
        }

        const std::string id = "20007737";
        float x = -((float)id.length() / 2.0f) * 0.7f + 0.35f;
        for(char c : id){
            models.emplace_back(new ModelCuboid({0.2f, 0.05f, 0.05f}, {x, 0.4f, 0.0f}, 0.0f, MAT_NUMBERS));
            if(c == '2' || c == '3'){
                models.emplace_back(new ModelCuboid({0.2f, 0.05f, 0.05f}, {x, 0.0f, 0.0f}, 0.0f, MAT_NUMBERS));
            }
            if(c == '0' || c == '2' || c == '3'){
                models.emplace_back(new ModelCuboid({0.2f, 0.05f, 0.05f}, {x, -0.4f, 0.0f}, 0.0f, MAT_NUMBERS));
            }
            if(c == '0'){
                models.emplace_back(new ModelCuboid({0.05f, 0.2f, 0.05f}, {x - 0.25f, 0.25f, 0.0f}, 0.0f, MAT_NUMBERS));
            }
            if(c == '0' || c == '2'){
                models.emplace_back(new ModelCuboid({0.05f, 0.25f, 0.05f}, {x - 0.25f, -0.2f, 0.0f}, 0.0f, MAT_NUMBERS));
            }
            if(c == '0' || c == '2' || c == '7' || c == '3'){
                models.emplace_back(new ModelCuboid({0.05f, 0.25f, 0.05f}, {x + 0.25f, 0.2f, 0.0f}, 0.0f, MAT_NUMBERS));
            }
            if(c == '0' || c == '7' || c == '3'){
                models.emplace_back(new ModelCuboid({0.05f, 0.2f, 0.05f}, {x + 0.25f, -0.25f, 0.0f}, 0.0f, MAT_NUMBERS));
            }
            x += 0.7f;
        }
    }

    vec3f getCameraDynamicLocation(float timer) const override {
        return {sin(timer) * 6.0f, 2.75f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(float timer) const override {
        return {0.0f, 0.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return vec3f(3.0f + 0.7f, 2.75f - 0.1f, 5.0f) * 0.6f;
    }

    vec3f getCameraStaticTarget() const override {
        return {0.7f, -0.1f, 0.0f};
    }

};
