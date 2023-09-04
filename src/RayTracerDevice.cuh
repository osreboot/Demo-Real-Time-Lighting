#pragma once

#include <owl/owl.h>
#include <owl/common/math/random.h>

#define NESTED_MATERIALS_MAX 50

// Material properties
struct Material{
    bool fullbright; // Is it a light source?
    float transparency; // Probability to allow rays to pass through instead of reflect
    float refractiveIndex; // Refractive index
    float reflectivity; // Probability to reflect instead of absorb
    float diffuse; // Scattering magnitude
    owl::vec3f color; // Surface color
};

// Raw geometry data
struct TrianglesGeomData{
    owl::vec3i *index;
    owl::vec3f *vertex;
    Material *material;
};

// Data used by the ray generator
struct RayGenData{
    uint32_t *frameBuffer;
    owl::vec2i size;
    OptixTraversableHandle world;

    struct {
        owl::vec3f pos;
        owl::vec3f dir_00;
        owl::vec3f dir_du;
        owl::vec3f dir_dv;
    } camera;
};

// Data associated with each ray
struct PerRayData{
    owl::LCG<4> random; // A random object
    bool hitDetected; // Did the ray hit something?
    owl::vec3f hitOrigin; // Collision location
    owl::vec3f bounceDirection; // New ray direction (post-collision)
    owl::vec3f color; // New ray color (post-collision)

    // Material stack, used to track the material that the ray is currently inside. Also used to calculate the relative
    // refractive index when transitioning between materials.
    int sizeMaterials;
    Material materials[NESTED_MATERIALS_MAX];
};