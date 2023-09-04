
#define CAMERA_COS_FOVY 0.9f // was 0.66f
#define CAMERA_LOOK_UP vec3f{0.0f, 1.0f, 0.0f}

#include <owl/owl.h>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "Config.h"
#include "Display.h"
#include "RayTracerHost.h"
#include "RayTracerDevice.cuh"


RayTracerHost::RayTracerHost(){
    // Fetch scene data
    const Scene& scene = *SCENE_LIST[SCENE_INDEX];
    std::vector<Model*> models;
    scene.build(models);

    // Extract vertex, triangle, and texture/material data from scene models
    // TODO This could be optimized by using primitive arrays instead of vectors
    std::vector<owl::vec3f> vertices;
    std::vector<owl::vec3i> triangles;
    std::vector<Material> materials;
    int triangleOffset = 0;
    for(const Model* model : models){
        for(int i = 0; i < model->numVertices(); i++) vertices.push_back(model->getVertices()[i]);
        for(int i = 0; i < model->numTriangles(); i++) triangles.push_back(model->getTriangles()[i] + triangleOffset);
        materials.push_back(model->getMaterial());
        triangleOffset += model->numVertices();
    }

    // Cleanup scene
    for(Model* model : models) delete model;

    // Initialize OWL data structures and parameters with our world geometry and materials
    context = owlContextCreate(nullptr, 1);
    OWLModule module = owlModuleCreate(context, RayTracerDevice_ptx);

    OWLVarDecl trianglesGeomVars[] = {
            {"index", OWL_BUFPTR, OWL_OFFSETOF(TrianglesGeomData, index)},
            {"vertex", OWL_BUFPTR, OWL_OFFSETOF(TrianglesGeomData, vertex)},
            {"material", OWL_BUFPTR, OWL_OFFSETOF(TrianglesGeomData, material)}
    };
    OWLGeomType trianglesGeomType = owlGeomTypeCreate(context, OWL_TRIANGLES, sizeof(TrianglesGeomData), trianglesGeomVars, 3);
    owlGeomTypeSetClosestHit(trianglesGeomType, 0, module, "TriangleMesh");

    OWLBuffer vertexBuffer = owlDeviceBufferCreate(context, OWL_FLOAT3, vertices.size(), vertices.data());
    OWLBuffer triangleBuffer = owlDeviceBufferCreate(context, OWL_INT3, triangles.size(), triangles.data());
    OWLBuffer materialBuffer = owlDeviceBufferCreate(context, OWL_USER_TYPE(materials[0]), materials.size(), materials.data());

    OWLGeom trianglesGeom = owlGeomCreate(context, trianglesGeomType);
    owlTrianglesSetVertices(trianglesGeom, vertexBuffer, vertices.size(), sizeof(vertices[0]), 0);
    owlTrianglesSetIndices(trianglesGeom, triangleBuffer, triangles.size(), sizeof(triangles[0]), 0);

    owlGeomSetBuffer(trianglesGeom, "vertex", vertexBuffer);
    owlGeomSetBuffer(trianglesGeom, "index", triangleBuffer);
    owlGeomSetBuffer(trianglesGeom, "material", materialBuffer);

    OWLGroup trianglesGroup = owlTrianglesGeomGroupCreate(context, 1, &trianglesGeom);
    owlGroupBuildAccel(trianglesGroup);
    OWLGroup world = owlInstanceGroupCreate(context, 1, &trianglesGroup);
    owlGroupBuildAccel(world);

    owlMissProgCreate(context, module, "miss", 0, nullptr, 0);

    OWLVarDecl rayGenVars[] = {
            {"frameBuffer", OWL_RAW_POINTER, OWL_OFFSETOF(RayGenData, frameBuffer)},
            {"size", OWL_INT2, OWL_OFFSETOF(RayGenData, size)},
            {"world", OWL_GROUP, OWL_OFFSETOF(RayGenData, world)},
            {"camera.pos", OWL_FLOAT3, OWL_OFFSETOF(RayGenData, camera.pos)},
            {"camera.dir_00", OWL_FLOAT3, OWL_OFFSETOF(RayGenData, camera.dir_00)},
            {"camera.dir_du", OWL_FLOAT3, OWL_OFFSETOF(RayGenData, camera.dir_du)},
            {"camera.dir_dv", OWL_FLOAT3, OWL_OFFSETOF(RayGenData, camera.dir_dv)},
            {}
    };
    rayGen = owlRayGenCreate(context, module, "rayGenProgram", sizeof(RayGenData), rayGenVars, -1);
    owlRayGenSetGroup(rayGen, "world", world);

    // Build everything
    owlBuildPrograms(context);
    owlBuildPipeline(context);
}

void RayTracerHost::update(float delta){
    if(timerFreeze > 0.0f) timerFreeze -= delta;
    else timer += delta / (2.0f * 16.0f);

    // Calculate camera parameters
    owl::vec3f camera_pos = SCENE_LIST[SCENE_INDEX]->getCameraDynamicLocation(timer);
    if(timerFreeze > 0.0f) camera_pos += owl::vec3f(0.0f, 10.0f, 0.0f);
    owl::vec3f camera_d00 = normalize(SCENE_LIST[SCENE_INDEX]->getCameraDynamicTarget(timer) - camera_pos);
    float aspect = float(display::getSize().x) / float(display::getSize().y);
    owl::vec3f camera_ddu = CAMERA_COS_FOVY * aspect * normalize(cross(camera_d00, owl::CAMERA_LOOK_UP));
    owl::vec3f camera_ddv = CAMERA_COS_FOVY * normalize(cross(camera_ddu,camera_d00));
    camera_d00 -= 0.5f * camera_ddu;
    camera_d00 -= 0.5f * camera_ddv;

    // Send camera parameters to the ray tracer
    owlRayGenSet1ul(rayGen, "frameBuffer", (uint64_t)display::getFrameBuffer());
    owlRayGenSet2i(rayGen, "size", display::getSize().x, display::getSize().y);
    owlRayGenSet3f(rayGen, "camera.pos", (const owl3f&)camera_pos);
    owlRayGenSet3f(rayGen, "camera.dir_00", (const owl3f&)camera_d00);
    owlRayGenSet3f(rayGen, "camera.dir_du", (const owl3f&)camera_ddu);
    owlRayGenSet3f(rayGen, "camera.dir_dv", (const owl3f&)camera_ddv);

    // Run ray tracer
    owlBuildSBT(context);
    owlRayGenLaunch2D(rayGen, display::getSize().x, display::getSize().y);
}

void RayTracerHost::capture(){
    // Calculate camera parameters
    owl::vec3f camera_pos = SCENE_LIST[SCENE_INDEX]->getCameraStaticLocation();
    owl::vec3f camera_d00 = normalize(SCENE_LIST[SCENE_INDEX]->getCameraStaticTarget() - camera_pos);
    float aspect = float(display::getSize().x) / float(display::getSize().y);
    owl::vec3f camera_ddu = CAMERA_COS_FOVY * aspect * normalize(cross(camera_d00, owl::CAMERA_LOOK_UP));
    owl::vec3f camera_ddv = CAMERA_COS_FOVY * normalize(cross(camera_ddu,camera_d00));
    camera_d00 -= 0.5f * camera_ddu;
    camera_d00 -= 0.5f * camera_ddv;

    // Send camera parameters to the ray tracer
    owlRayGenSet1ul(rayGen, "frameBuffer", (uint64_t)display::getFrameBuffer());
    owlRayGenSet2i(rayGen, "size", display::getSize().x, display::getSize().y);
    owlRayGenSet3f(rayGen, "camera.pos", (const owl3f&)camera_pos);
    owlRayGenSet3f(rayGen, "camera.dir_00", (const owl3f&)camera_d00);
    owlRayGenSet3f(rayGen, "camera.dir_du", (const owl3f&)camera_ddu);
    owlRayGenSet3f(rayGen, "camera.dir_dv", (const owl3f&)camera_ddv);

    // Run ray tracer
    owlBuildSBT(context);
    owlRayGenLaunch2D(rayGen, display::getSize().x, display::getSize().y);

    // Save output image data to a PNG using STB
    uint32_t* frameBuffer = display::getFrameBuffer();
    stbi_flip_vertically_on_write(true);
    stbi_write_png("capture.png", display::getSize().x, display::getSize().y, 4, frameBuffer, display::getSize().x * sizeof(uint32_t));
}