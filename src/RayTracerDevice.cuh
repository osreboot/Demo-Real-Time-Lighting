#pragma once

#include <owl/owl.h>
#include <owl/common/math/random.h>

#define NESTED_MATERIALS_MAX 50

// Material properties
struct Material {
    bool fullbright; // Is it a light source?
    float transparency; // Probability to allow rays to pass through instead of reflect
    float refractiveIndex; // Refractive index
    float reflectivity; // Probability to reflect instead of absorb
    float diffuse; // Scattering magnitude
    owl::vec3f color; // Surface color
};

// Raw geometry data
struct WorldGeometry {
    owl::vec3f* vertices;
    owl::vec3i* triangles;
    Material* materials;
};

// Data used by the ray generator
struct RayGenerator {
    uint32_t *frameBuffer;
    owl::vec2i size;

    OptixTraversableHandle worldHandle;

    // The ray tracer projects rays from the camera's 'location' point through the plane specified by:
    //   originPixel + dirRight * x + dirUp * y
    // This effectively creates a perspective projection, with zNear and zFar planes determined by the limits of the
    // ray tracer itself.
    struct {
        owl::vec3f location;
        owl::vec3f originPixel;
        owl::vec3f dirRight;
        owl::vec3f dirUp;
    } camera;
};

// Data associated with each ray
struct PerRayData {
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