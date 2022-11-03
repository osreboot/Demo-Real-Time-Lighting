#include <optix_device.h>

#include "config.cuh"
#include "rt_program.cuh"

using namespace owl;

inline __device__ vec3f randomUnitSphere(LCG<4> &random){
    vec3f v;
    do{
        v = 2.0f * vec3f(random(), random(), random()) - vec3f(1.0f, 1.0f, 1.0f);
    }while(dot(v, v) >= 1.0f);
    return v;
}

inline __device__ vec3f tracePath(const RayGenData &self, Ray &ray, PerRayData &prd){
    vec3f attenuation = vec3f(1.0f);
    for(int i = 0; i < 50; i++){
        prd.hitDetected = false;

        traceRay(self.world, ray, prd);

        if(prd.hitCancelled) return vec3f(0.0f);

        attenuation *= prd.color;

        if(!prd.hitDetected) return attenuation;
        ray = Ray(prd.hitOrigin, prd.bounceDirection, 1e-3f, 1e10f);
    }
    return vec3f(0.0f);
}

OPTIX_RAYGEN_PROGRAM(rayGenProgram)(){
    const auto &self = getProgramData<RayGenData>();
    const vec2i pixel = getLaunchIndex();

    // Trace ray
    vec3f color = vec3f(0.0f);
    PerRayData prd;

    for(float i = 0; i < PROGRAM_SAMPLES; i++){
        // Create ray from camera
        Ray ray;
        ray.origin = self.camera.pos;

        const vec2f screen = (vec2f(pixel) + vec2f(prd.random(), prd.random()) + vec2f(0.5f)) / vec2f(self.size);
        ray.direction = normalize(self.camera.dir_00 + screen.u * self.camera.dir_du + screen.v * self.camera.dir_dv);

        vec3f colorOut = tracePath(self, ray, prd);
        colorOut.x = max(min(colorOut.x, 1.0f), 0.0f);
        colorOut.y = max(min(colorOut.y, 1.0f), 0.0f);
        colorOut.z = max(min(colorOut.z, 1.0f), 0.0f);

        color += colorOut;
    }

    // Assign frame buffer pixel color based on ray
    const int indexPixel = pixel.x + self.size.x * pixel.y;
    self.frameBuffer[indexPixel] = make_rgba(color / (float)PROGRAM_SAMPLES);
}

OPTIX_CLOSEST_HIT_PROGRAM(TriangleMesh)(){
    auto &prd = getPRD<PerRayData>();
    auto &self = getProgramData<TrianglesGeomData>();

    // Calculate hitNormal
    const int indexPrimitive = optixGetPrimitiveIndex();
    const vec3i index = self.index[indexPrimitive];
    const vec3f hitNormal = normalize(cross(self.vertex[index.y] - self.vertex[index.x],
                                         self.vertex[index.z] - self.vertex[index.x]));

    const vec3f ro = optixGetWorldRayOrigin();
    const vec3f rd = optixGetWorldRayDirection();
    const float ht = optixGetRayTmax();

    Material &material = self.material[indexPrimitive / 12];

    bool refracted = false;

    prd.hitOrigin = ro + ht * rd;

    // Calculate reflected direction
    vec3f directionReflect = rd - 2.0f * dot(rd, hitNormal) * hitNormal;
    if(prd.random() < material.reflectivity){
        directionReflect = directionReflect + material.diffuse * randomUnitSphere(prd.random);
    }else directionReflect = hitNormal + randomUnitSphere(prd.random);

    // Calculate refracted direction
    vec3f v = normalize(rd);
    float cosine;

    vec3f outward_normal;
    float ni;
    if(dot(v, hitNormal) > 0.0f){
        outward_normal = -hitNormal;
        ni = material.refractiveIndex;
        cosine = dot(v, hitNormal);
        cosine = sqrtf(1.0f - material.refractiveIndex * material.refractiveIndex * (1.0f - cosine * cosine));
    }else{
        outward_normal = hitNormal;
        ni = 1.0f / material.refractiveIndex;
        cosine = -dot(v, hitNormal);
    }

    float dt = dot(v, outward_normal);
    float disc = 1.0f - ni * ni * (1.0f - dt * dt);

    vec3f directionRefract = rd;
    if(disc > 0.0f){
        refracted = true;
        directionRefract = ni * (v - outward_normal * dt) - outward_normal * sqrtf(disc);
    }

    float pReflect = 1.0f;
    if(!refracted){
        pReflect = 1.0f;
    }else{
        // Schlick algorithm
        float r0 = (1.0f - material.refractiveIndex) / (1.0f + material.refractiveIndex);
        r0 = r0 * r0;
        pReflect = r0 + (1.0f - r0) * powf(1.0f - cosine, 5.0f);
    }

    // TODO this isn't actually doing anything
    prd.bounceDirection = directionReflect;
    if(prd.random() < pReflect){ // Reflected
        prd.bounceDirection = directionReflect;
    }else if(prd.random() < material.transparency){ // Refracted
        prd.bounceDirection = directionRefract;
    }

    prd.color = material.color;
    prd.hitDetected = !material.fullbright;
}

OPTIX_MISS_PROGRAM(miss)(){
    auto &prd = getPRD<PerRayData>();

    prd.hitDetected = false;
    prd.hitCancelled = false;

    vec3f rdn = normalize(vec3f(optixGetWorldRayDirection()));
    switch(SCENE_SKYBOX){
        case SCENE_SKYBOX_BLACK:
            prd.color = vec3f(0.0f);
            break;
        case SCENE_SKYBOX_RGB:
            prd.color = (1.0f + rdn) / 2.0f;
            break;
        case SCENE_SKYBOX_UNDERLIT:
            prd.color = 1.0f - max(rdn.y, 0.0f);
            break;
    }
}