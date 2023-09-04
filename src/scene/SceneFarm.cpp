
#include "Scene.h"

class SceneFarm : public Scene {

private:
    const float SIZE_FARM_BLOCK = 0.5f;
    const float SIZE_FARM_BLOCK2 = SIZE_FARM_BLOCK * 0.3f;

    const Material MAT_CHROME = {false, 0.0f, 1.0f, 1.0f, 0.01f, {0.95f, 0.95f, 0.95f}};
    const Material MAT_LIGHT_PRIMARY = {true, 0.0f, 1.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}};

    // Returns a material based on a cube's location in the scene
    Material getMaterial(vec2f location, bool fullbright) const {
        if(location.y > cos(location.x) + location.x / 2.0f + 3.0f){
            return {fullbright, 0.0f, 1.0f, 1.0f, 0.1f, {1.0f, 0.5f, 0.5f}};
        }else if(location.y > cos(location.x + 1.5f) * 2.0f + 3.0f){
            return {fullbright, 0.0f, 1.0f, 1.0f, 0.02f, {0.5f, 0.5f, 1.0f}};
        }else if(location.y > cos(location.x * 0.5f)){
            return {fullbright, 0.0f, 1.0f, 1.0f, 0.0f, {1.0f, 0.5f, 1.0f}};
        }else if(location.y > cos(location.x * 0.5f) - location.x / 4.0f - 2.0f){
            return {fullbright, 0.0f, 1.0f, 0.0f, 0.0f, {0.5f, 1.0f, 1.0f}};
        }else if(location.y > cos(location.x) * 1.5f - 4.0f){
            return {fullbright, 0.0f, 1.0f, 0.5f, 0.0f, {0.5f, 1.0f, 0.5f}};
        }else return {fullbright, 0.0f, 1.0f, 1.0f, 0.0f, {1.0f, 1.0f, 0.5f}};
    }

    // Creates a "farm" plot on the floor with a grid of cubes
    void addFarm(const SceneBuildParams &params, vec3f location) const {
        addBox(params, {SIZE_FARM_BLOCK * 3.5f, 0.05f, SIZE_FARM_BLOCK * 3.5f},
               {location.x, location.y - 0.02f, location.z}, 0.0f,
               getMaterial({location.x, location.z}, false));
        for(float x = -SIZE_FARM_BLOCK * 3.0f; x <= SIZE_FARM_BLOCK * 3.0f; x += SIZE_FARM_BLOCK){
            for(float z = -SIZE_FARM_BLOCK * 3.0f; z <= SIZE_FARM_BLOCK * 3.0f; z += SIZE_FARM_BLOCK){
                const float height = random() * SIZE_FARM_BLOCK / 4.0f;
                addBox(params, {SIZE_FARM_BLOCK2, height, SIZE_FARM_BLOCK2},
                       {location.x + x, location.y + height, location.z + z}, 0.0f,
                       getMaterial({location.x + x, location.z + z}, random() < 0.1f));
            }
        }
    }

public:

    // This scene has a bunch of cube "farm" plots on the ground and reflective walls. Best used with a dark sky!
    void initialize(SceneBuildParams &params) const override {
        addBox(params, {9.0f, 0.05f, 7.0f}, {0.0f, -0.1f, 0.0f}, 0.0f, MAT_LIGHT_PRIMARY);

        addBox(params, {9.0f, 3.0f, 0.05f}, {0.0f, 3.0f, 7.05f}, 0.0f, MAT_CHROME);
        addBox(params, {9.0f, 3.0f, 0.05f}, {0.0f, 3.0f, -7.05f}, 0.0f, MAT_CHROME);
        addBox(params, {0.05f, 3.0f, 7.0f}, {9.05f, 3.0f, 0.0f}, 0.0f, MAT_CHROME);
        addBox(params, {0.05f, 3.0f, 7.0f}, {-9.05f, 3.0f, 0.0f}, 0.0f, MAT_CHROME);

        for(float x = -1.0f; x <= 1.0f; x += 1.0f){
            for(float z = -1.0f; z <= 1.0f; z += 1.0f){
                addFarm(params, {x * 4.0f, 0.0f, z * 4.0f});
            }
        }
        for(float x = -5.5f; x <= 5.5f; x += SIZE_FARM_BLOCK){
            const float size = random() * SIZE_FARM_BLOCK;
            addBox(params, {SIZE_FARM_BLOCK2, 3.0f, size}, {x, 3.0f, 7.0f - size}, 0.0f, MAT_CHROME);
            addBox(params, {SIZE_FARM_BLOCK2, 3.0f, size}, {x, 3.0f, -7.0f + size}, 0.0f, MAT_CHROME);
        }

        addBox(params, {1.0f, 1.0f, 1.0f}, {0.0f, 5.0f, 0.0f}, 0.0f, MAT_LIGHT_PRIMARY);
    }

    vec3f getCameraDynamicLocation(const float timer) const override {
        return {sin(timer) * 6.0f, 2.0f, cos(timer) * 6.0f};
    }

    vec3f getCameraDynamicTarget(const float timer) const override {
        return {0.0f, sin(timer) + 1.0f, 0.0f};
    }

    vec3f getCameraStaticLocation() const override {
        return {-7.9f, 2.0f, -3.5f};
    }

    vec3f getCameraStaticTarget() const override {
        return {0.0f, 1.0f, 0.0f};
    }

};
