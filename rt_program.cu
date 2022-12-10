#include <optix_device.h>

#include "config.cuh"
#include "rt_program.cuh"

using namespace owl;

inline __device__ vec3f randomUnitSphere(LCG<4>& random){
    vec3f v;
    do{
        v = 2.0f * vec3f(random(), random(), random()) - vec3f(1.0f, 1.0f, 1.0f);
    }while(dot(v, v) >= 1.0f);
    return v;
}

inline __device__ vec3f tracePath(const RayGenData& self, Ray& ray, PerRayData& prd){
    vec3f attenuation = vec3f(1.0f);
    prd.sizeMaterials = 0;
    for(int i = 0; i < 50; i++){
        prd.hitDetected = false;

        traceRay(self.world, ray, prd);

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

    for(int i = 0; i < PROGRAM_SAMPLES; i++){
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

    // Calculate normalSurface
    const unsigned int indexPrimitive = optixGetPrimitiveIndex();
    const vec3i index = self.index[indexPrimitive];
    Material& material = self.material[indexPrimitive / 12];

    const vec3f normalSurface = normalize(cross(self.vertex[index.y] - self.vertex[index.x],
                                                self.vertex[index.z] - self.vertex[index.x]));

    const vec3f ro = optixGetWorldRayOrigin();
    const vec3f rd = optixGetWorldRayDirection();
    const vec3f rdn = normalize(rd);

    prd.hitOrigin = ro + optixGetRayTmax() * rd;

    // Calculate reflected direction
    vec3f directionReflect = rd - 2.0f * dot(rd, normalSurface) * normalSurface;
#if SHADER_LAMBERTIAN_REFLECTION
    if(prd.random() > material.reflectivity){
#if SHADER_SCATTERING
        directionReflect = normalSurface + randomUnitSphere(prd.random);
#else
        directionReflect = normalSurface;
#endif
    }
#endif

    const bool leavingObject = dot(rdn, normalSurface) > 0.0f;

    const Material materialAir = {false, 1.0f, 1.0f, 0.0f, 0.0f, vec3f(1.0f)};

    Material materialLast = leavingObject ? material : (prd.sizeMaterials > 0 ? prd.materials[prd.sizeMaterials - 1] : materialAir);
    Material materialNext = leavingObject ? (prd.sizeMaterials > 1 ? prd.materials[prd.sizeMaterials - 2] : materialAir) : material;

    const bool continuousObject = prd.sizeMaterials > 0 &&
            materialLast.fullbright == materialNext.fullbright &&
            materialLast.transparency == materialNext.transparency &&
            materialLast.refractiveIndex == materialNext.refractiveIndex &&
            materialLast.reflectivity == materialNext.reflectivity &&
            materialLast.diffuse == materialNext.diffuse &&
            materialLast.color == materialNext.color;

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

    // Schlick algorithm
#if SHADER_SCHLICK_REFLECTION
    float r0 = (materialLast.refractiveIndex - materialNext.refractiveIndex) / (materialLast.refractiveIndex + materialNext.refractiveIndex);
    r0 = r0 * r0;
    float pReflectSchlick = r0 + (1.0f - r0) * powf(1.0f - refractCosine, 5.0f);
#else
    float pReflectSchlick = 0.0f;
#endif

    if(continuousObject || (prd.random() < materialNext.transparency && prd.random() > pReflectSchlick)){ // Refracted
        if(leavingObject){
            if(prd.sizeMaterials > 0) prd.sizeMaterials--;
        }else{
            if(prd.sizeMaterials < NESTED_MATERIALS_MAX) prd.materials[prd.sizeMaterials++] = material;
        }
        prd.bounceDirection = directionRefract;
    }else prd.bounceDirection = directionReflect;

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

OPTIX_MISS_PROGRAM(miss)(){
    auto &prd = getPRD<PerRayData>();

    prd.hitDetected = false;

#if SHADER_SKYBOX_DIRECTIONAL
    vec3f rdn = normalize(vec3f(optixGetWorldRayDirection()));
    switch(SCENE_SKYBOX){
        case SCENE_SKYBOX_BLACK:
            prd.color = vec3f(0.0f);
            break;
        case SCENE_SKYBOX_WHITE:
            prd.color = vec3f(min(1.0f, 1.0f + rdn.y));
            break;
        case SCENE_SKYBOX_RGB:
            prd.color = (1.0f + rdn) / 2.0f;
            break;
        case SCENE_SKYBOX_UNDERLIT:
            prd.color = 1.0f - max(rdn.y, 0.0f);
            break;
    }
#else
    prd.color = vec3f(1.0f);
#endif
}