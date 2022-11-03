#define COUNT_SAMPLES 10.0f

#include <optix_device.h>

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
    //float attenuation = 1.0f;
    vec3f attenuation = vec3f(1.0f);
    //vec3f color = {1.0f, 1.0f, 1.0f};
    for(int i = 0; i < 50; i++){
        prd.hitDetected = false;

        traceRay(self.world, ray, prd);

        //float mixNew = attenuation * (1.0f - prd.lastTransparency);
        //color = ((1.0f - mixNew) * color) + (mixNew * prd.color);
        //attenuation = prd.lastTransparency * attenuation + (1.0f - prd.lastTransparency) * attenuation * prd.lastReflectivity;

        if(prd.hitCancelled) return vec3f(0.0f);

        attenuation *= prd.color;

        if(!prd.hitDetected) return attenuation;
        ray = Ray(prd.hitOrigin, prd.bounceDirection, 1e-3f, 1e10f);

        /*
        float mixNew = attenuation * (1.0f - prd.lastTransparency);
        color = ((1.0f - mixNew) * color) + (mixNew * prd.color);
        attenuation = prd.lastTransparency * attenuation + (1.0f - prd.lastTransparency) * attenuation * prd.lastReflectivity;

        if(!prd.hitDetected){
            return color;
        }
        if(prd.hitCancelled) return vec3f(0.0f);
        ray = Ray(prd.hitOrigin, prd.bounceDirection, 1e-3f, 1e10f);*/
    }
    return vec3f(0.0f, 0.0f, 0.0f);
}

OPTIX_RAYGEN_PROGRAM(rayGenProgram)(){
    const auto &self = getProgramData<RayGenData>();
    const vec2i pixel = getLaunchIndex();

    // Trace ray
    vec3f color = vec3f(0.0f);
    PerRayData prd;
    //prd.random.init(pixel.x + self.size.x * COUNT_SAMPLES,
    //                pixel.y + self.size.y * COUNT_SAMPLES);

    for(float i = 0; i < COUNT_SAMPLES; i++){
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
    self.frameBuffer[indexPixel] = make_rgba(color / COUNT_SAMPLES);
}

OPTIX_CLOSEST_HIT_PROGRAM(TriangleMesh)(){
    auto &prd = getPRD<PerRayData>();
    auto &self = getProgramData<TrianglesGeomData>();

    // Calculate normal
    const int indexPrimitive = optixGetPrimitiveIndex();
    const vec3i index = self.index[indexPrimitive];
    const vec3f &a = self.vertex[index.x];
    const vec3f &b = self.vertex[index.y];
    const vec3f &c = self.vertex[index.z];
    const vec3f normal = normalize(cross(b - a, c - a));

    const vec3f ro = optixGetWorldRayOrigin();
    const vec3f rd = optixGetWorldRayDirection();
    const float ht = optixGetRayTmax();

    Material &material = self.material[indexPrimitive / 12];

    bool refracted = false;

    prd.hitDetected = true;
    prd.hitOrigin = ro + ht * rd;

    // Calculate reflected direction
    vec3f directionReflect = rd - 2.0f * dot(rd, normal) * normal;
    if(prd.random() < material.reflectivity){
        directionReflect = directionReflect + material.diffuse * randomUnitSphere(prd.random);
    }else directionReflect = normal + randomUnitSphere(prd.random);

    // Calculate refracted direction
    vec3f v = normalize(rd);
    float cosine;

    // TODO more complicated reflect/refract selection (schlick)
    vec3f outward_normal;
    float ni;
    if(dot(v, normal) > 0.0f){
        outward_normal = -normal;
        ni = material.refractiveIndex;
        cosine = dot(v, normal);
        cosine = sqrtf(1.0f - material.refractiveIndex * material.refractiveIndex * (1.0f - cosine * cosine));
    }else{
        outward_normal = normal;
        ni = 1.0f / material.refractiveIndex;
        cosine = -dot(v, normal);
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
        //pReflect = 1.0f - material.transparency;
        // Schlick algorithm
        float r0 = (1.0f - material.refractiveIndex) / (1.0f + material.refractiveIndex);
        r0 = r0 * r0;
        pReflect = r0 + (1.0f - r0) * powf(1.0f - cosine, 5.0f);
    }

    prd.bounceDirection = directionReflect;

    if(prd.random() < pReflect){ // Reflected
        prd.bounceDirection = directionReflect;
    }else if(prd.random() < material.transparency){ // Refracted
        prd.bounceDirection = directionRefract;
    }

    prd.color = material.color;
    prd.hitDetected = !material.fullbright;

    /*
    prd.hitDetected = true;
    prd.hitOrigin = ro + ht * rd;
    if(material.transparency <= 0.0f){
        prd.bounceDirection = rd - 2.0f * dot(rd, normal) * normal;
    }else{
        vec3f v = normalize(rd);

        vec3f outward_normal;
        float ni;
        if(dot(v, normal) > 0.0f){
            outward_normal = -normal;
            ni = material.refractiveIndex;
        }else{
            outward_normal = normal;
            ni = 1.0f / material.refractiveIndex;
        }

        float dt = dot(v, outward_normal);
        float disc = 1.0f - ni * ni * (1.0f - dt * dt);
        if(disc > 0.0f) {
            prd.bounceDirection = ni * (v - outward_normal * dt) - outward_normal * sqrtf(disc);
        }else prd.bounceDirection = rd;
    }

    prd.color = material.color;
    prd.lastReflectivity = material.reflectivity;
    prd.lastTransparency = material.transparency;*/
}

OPTIX_MISS_PROGRAM(miss)(){
    const vec2i pixel = getLaunchIndex();
    auto &prd = getPRD<PerRayData>();

    vec3f rd = normalize(vec3f(optixGetWorldRayDirection()));
    //vec3f colorSkybox = (1.0f + rd) / 2.0f/* + rd.y*/;
    vec3f colorSkybox = 1.0f - max(rd.y, 0.0f);
    //colorSkybox.x = round(colorSkybox.x + 0.3f);
    //colorSkybox.y = round(colorSkybox.y);
    //colorSkybox.z = round(colorSkybox.z + 0.3f);

    prd.hitDetected = false;
    prd.hitCancelled = false;
    //prd.color = vec3f(0.0f);
    prd.color = colorSkybox;
}