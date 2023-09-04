#pragma once

#include <vector>
#include <random>

#include <owl/owl.h>
#include <owl/common.h>

#include "Model.h"
#include "RayTracerDevice.cuh"

// Easy uniform random number access
inline float random(){
    static std::mt19937 gen(0);
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

class Scene {

public:
    virtual void build(std::vector<Model*>& models) const = 0;
    virtual owl::vec3f getCameraDynamicLocation(float timer) const { return {0.0f, 0.0f, 0.0f}; }
    virtual owl::vec3f getCameraDynamicTarget(float timer) const { return {0.0f, 0.0f, 0.0f}; }
    virtual owl::vec3f getCameraStaticLocation() const { return {0.0f, 0.0f, 0.0f}; }
    virtual owl::vec3f getCameraStaticTarget() const { return {0.0f, 0.0f, 0.0f}; }

};