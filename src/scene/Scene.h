#pragma once

#include <vector>
#include <random>
#include <owl/owl.h>
#include <owl/common.h>

#include "RayTracerDevice.cuh"

using namespace std;

const int UNIT_BOX_NUM_VERTICES = 8;
const vec3f UNIT_BOX_VERTICES[UNIT_BOX_NUM_VERTICES] = {
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f}
};
const int UNIT_BOX_NUM_INDICES = 12;
const vec3i UNIT_BOX_INDICES[UNIT_BOX_NUM_INDICES] = {
        {0, 2, 1}, {0, 3, 2}, // Front
        {2, 3, 4}, {2, 4, 5}, // Top
        {1, 2, 5}, {1, 5, 6}, // Right
        {0, 7, 4}, {0, 4, 3}, // Left
        {5, 4, 7}, {5, 7, 6}, // Back
        {0, 6, 7}, {0, 1, 6} // Bottom
};

struct SceneBuildParams{
    vector<vec3f> vertices;
    vector<vec3i> indices;
    vector<Material> materials;
};

// Easy uniform random number access
inline float random(){
    static mt19937 gen(0);
    static uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

// Creates a box in the scene
inline void addBox(SceneBuildParams& params, vec3f boxSize, vec3f boxLocation, float rotation, Material material){
    affine3f xfm(linear3f::scale(boxSize));
    xfm = affine3f(linear3f::rotate(vec3f(0.0f, 1.0f, 0.0f), ((float)M_PI / 180.0f) * rotation)) * xfm;
    xfm = affine3f(affine3f::translate(boxLocation)) * xfm;

    const int index = (int)params.vertices.size();
    for(int i = 0; i < UNIT_BOX_NUM_VERTICES; i++){
        params.vertices.push_back(xfmPoint(xfm, UNIT_BOX_VERTICES[i]));
    }
    for(int i = 0; i < UNIT_BOX_NUM_INDICES; i++){
        params.indices.push_back(UNIT_BOX_INDICES[i] + index);
    }
    params.materials.push_back(material);
}

class Scene {

public:
    virtual void initialize(SceneBuildParams& params) const = 0;
    virtual vec3f getCameraDynamicLocation(const float timer) const { return vec3f(0.0f); }
    virtual vec3f getCameraDynamicTarget(const float timer) const { return vec3f(0.0f); }
    virtual vec3f getCameraStaticLocation() const { return vec3f(0.0f); }
    virtual vec3f getCameraStaticTarget() const { return vec3f(0.0f); }

};