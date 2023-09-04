#include <optix_device.h>

#include "Config.h"
#include "RayTracerDevice.cuh"

using namespace owl;

// Selects a random point inside a unit sphere
inline __device__ vec3f randomUnitSphere(LCG<4>& random) {
    vec3f v;
    do {
        v = 2.0f * vec3f(random(), random(), random()) - vec3f(1.0f, 1.0f, 1.0f);
    } while(dot(v, v) >= 1.0f);
    return v;
}

// Tracks a ray through multiple bounces in the world
inline __device__ vec3f tracePath(const RayGenerator& rayGen, Ray& ray, PerRayData& prd) {
    vec3f attenuation = vec3f(1.0f);
    prd.sizeMaterials = 0;
    // Loop as long as we haven't reached the maximum bounce depth
    for(int i = 0; i < 50; i++){
        prd.hitDetected = false;

        // Launch the ray
        traceRay(rayGen.worldHandle, ray, prd);

        attenuation *= prd.color;

        // The ray hit the sky or a light source
        if(!prd.hitDetected) return attenuation;

        // Re-initialize the ray based on collision parameters
        ray = Ray(prd.hitOrigin, prd.bounceDirection, 1e-3f, 1e10f);
    }

    // Max bounces exceeded
    return {0.0f, 0.0f, 0.0f};
}

// Ray generation program
OPTIX_RAYGEN_PROGRAM(rayGenProgram)() {
    const RayGenerator& rayGen = getProgramData<RayGenerator>();
    const vec2i pixel = getLaunchIndex();

    vec3f color = vec3f(0.0f);
    PerRayData prd;

    // Cast rays to fulfill the number of required samples
    for(int i = 0; i < PROGRAM_SAMPLES; i++){
        // Create ray from camera
        Ray ray;
        ray.origin = rayGen.camera.location;

        // Optional: link the ray's random seed to the pixel position. This is good for static images, but makes
        // real-time renders look like there's dirt on the screen.
        // prd.random.init(pixel.x + self.size.x * i,
        //                 pixel.y + self.size.y * i);

        // Set the ray's position and direction based on the current pixel
        const vec2f screen = (vec2f(pixel) + vec2f(prd.random(), prd.random()) + vec2f(0.5f)) / vec2f(rayGen.size);
        ray.direction = normalize(rayGen.camera.originPixel + screen.u * rayGen.camera.dirRight + screen.v * rayGen.camera.dirUp);

        // Trace the ray's path
        vec3f colorOut = tracePath(rayGen, ray, prd);

        // Clamp the output color
        colorOut.x = max(min(colorOut.x, 1.0f), 0.0f);
        colorOut.y = max(min(colorOut.y, 1.0f), 0.0f);
        colorOut.z = max(min(colorOut.z, 1.0f), 0.0f);

        color += colorOut;
    }

    // Assign frame buffer pixel color based on average of all samples
    rayGen.frameBuffer[pixel.x + rayGen.size.x * pixel.y] = make_rgba(color / (float)PROGRAM_SAMPLES);
}

// Ray hit program
OPTIX_CLOSEST_HIT_PROGRAM(TriangleMesh)() {
    PerRayData& prd = getPRD<PerRayData>();
    const WorldGeometry& world = getProgramData<WorldGeometry>();

    // Fetch data about the collision surface
    const unsigned int indexPrimitive = optixGetPrimitiveIndex();
    const vec3i index = world.triangles[indexPrimitive];
    const Material& material = world.materials[indexPrimitive / 12];

    // Calculate the normal of the surface
    const vec3f normalSurface = normalize(cross(world.vertices[index.y] - world.vertices[index.x],
                                                world.vertices[index.z] - world.vertices[index.x]));

    const vec3f ro = optixGetWorldRayOrigin();
    const vec3f rd = optixGetWorldRayDirection();
    const vec3f rdn = normalize(rd);

    // Calculate the position of the collision
    prd.hitOrigin = ro + optixGetRayTmax() * rd;

    // Calculate reflected direction
    vec3f directionReflect = rd - 2.0f * dot(rd, normalSurface) * normalSurface;
#if SHADER_LAMBERTIAN_REFLECTION
    if(prd.random() > material.reflectivity){ // Scattering for lambertians
#if SHADER_SCATTERING
        directionReflect = normalSurface + randomUnitSphere(prd.random);
#else
        directionReflect = normalSurface;
#endif
    }
#endif

    // Transparent objects looked REALLY ugly when multiple boxes of the same material were intersecting each other,
    // so this code was added to ignore interior collisions of the same material (look up 3D modeling "boolean union"
    // operation for more context on what exactly this does). Essentially, nested objects of the same material are
    // now treated as one object without interior faces.
    const Material materialAir = {false, 1.0f, 1.0f, 0.0f, 0.0f, vec3f(1.0f)};

    const bool leavingObject = dot(rdn, normalSurface) > 0.0f;
    Material materialLast = leavingObject ? material : (prd.sizeMaterials > 0 ? prd.materials[prd.sizeMaterials - 1] : materialAir);
    Material materialNext = leavingObject ? (prd.sizeMaterials > 1 ? prd.materials[prd.sizeMaterials - 2] : materialAir) : material;

    const bool continuousObject = prd.sizeMaterials > 0 &&
            materialLast.fullbright == materialNext.fullbright &&
            materialLast.transparency == materialNext.transparency &&
            materialLast.refractiveIndex == materialNext.refractiveIndex &&
            materialLast.reflectivity == materialNext.reflectivity &&
            materialLast.diffuse == materialNext.diffuse &&
            materialLast.color == materialNext.color;

    // This refractive index is what actually matters, OWL's sample code doesn't cover this but it's more accurate.
    float refractiveIndexRelative = materialNext.refractiveIndex / materialLast.refractiveIndex;

    vec3f normalSurfaceOutwards = leavingObject ? -normalSurface : normalSurface;

    // Algorithm source: https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics/refraction
    float refractCosine = min(dot(-rdn, normalSurfaceOutwards), 1.0f);
#if SHADER_REFRACTION
    vec3f rOutPerp = refractiveIndexRelative * (rdn + refractCosine * normalSurfaceOutwards);
    vec3f rOutPara = -sqrtf(abs(1.0f - length(rOutPerp) * length(rOutPerp))) * normalSurfaceOutwards;
    vec3f directionRefract = rOutPerp + rOutPara;
#else
    vec3f directionRefract = rdn;
#endif

    // Schlick's approximation for transparent material reflections instead of refractions
#if SHADER_SCHLICK_REFLECTION
    float r0 = (materialLast.refractiveIndex - materialNext.refractiveIndex) / (materialLast.refractiveIndex + materialNext.refractiveIndex);
    r0 = r0 * r0;
    float pReflectSchlick = r0 + (1.0f - r0) * powf(1.0f - refractCosine, 5.0f);
#else
    float pReflectSchlick = 0.0f;
#endif

    // Assign final ray data based on all the above calculations
#if SHADER_SCATTERING
    if(continuousObject || (prd.random() < materialNext.transparency && prd.random() > pReflectSchlick)){ // Refracted
#else
    if(continuousObject || (0.5f < materialNext.transparency && 0.5f > pReflectSchlick)){ // Refracted
#endif
        if(leavingObject){
            if(prd.sizeMaterials > 0) prd.sizeMaterials--;
        }else{
            if(prd.sizeMaterials < NESTED_MATERIALS_MAX) prd.materials[prd.sizeMaterials++] = material;
        }
        prd.bounceDirection = directionRefract;
    }else prd.bounceDirection = directionReflect;

    // Diffuse material scattering
#if SHADER_SCATTERING
    if(!continuousObject) prd.bounceDirection += material.diffuse * randomUnitSphere(prd.random);
#endif

    prd.color = continuousObject ? vec3f(1.0f) : material.color;

#if SHADER_FULLBRIGHT_MATERIALS
    prd.hitDetected = !material.fullbright;
#else
    prd.hitDetected = true;
#endif
}

// Ray miss program
OPTIX_MISS_PROGRAM(miss)() {
    PerRayData& prd = getPRD<PerRayData>();

    prd.hitDetected = false;

#if SHADER_SKYBOX_DIRECTIONAL
    vec3f rayNormal = normalize(vec3f(optixGetWorldRayDirection()));
    switch(SCENE_SKYBOX){
        case SCENE_SKYBOX_BLACK:
            prd.color = vec3f(0.0f);
            break;
        case SCENE_SKYBOX_WHITE:
            prd.color = vec3f(min(1.0f, 1.0f + rayNormal.y));
            break;
        case SCENE_SKYBOX_RGB:
            prd.color = (1.0f + rayNormal) / 2.0f;
            break;
        case SCENE_SKYBOX_UNDERLIT:
            prd.color = 1.0f - max(rayNormal.y, 0.0f);
            break;
        case SCENE_SKYBOX_NIGHT:
            if(rayNormal.y <= 0.0f) prd.color =  vec3f(0.0f);
            else prd.color = vec3f(0.0f, 0.0f, rayNormal.y / 10.0f);
            break;
    }
#else
    prd.color = vec3f(1.0f);
#endif
}