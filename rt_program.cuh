#pragma once

#include <owl/owl.h>
#include <owl/common/math/random.h>

using namespace owl;

struct Material{
    bool fullbright;
    float transparency;
    float refractiveIndex;
    float reflectivity;
    float diffuse;
    vec3f color;
};

struct TrianglesGeomData{
    vec3i *index;
    vec3f *vertex;
    Material *material;
};

struct RayGenData{
    uint32_t *frameBuffer;
    vec2i size;
    OptixTraversableHandle world;

    struct {
        vec3f pos;
        vec3f dir_00;
        vec3f dir_du;
        vec3f dir_dv;
    } camera;
};

struct PerRayData{
    LCG<4> random;
    bool hitDetected, hitCancelled;
    vec3f hitOrigin;
    vec3f bounceDirection;
    vec3f color;
};