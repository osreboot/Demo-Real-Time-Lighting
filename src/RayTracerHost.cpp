
#define CAMERA_COS_FOVY 0.8f // wide = 0.9 | original = 0.66 | narrow = 0.5
#define CAMERA_LOOK_UP vec3f{0.0f, 1.0f, 0.0f}

#include <vector>

#include <owl/owl.h>
#include <stb/stb_image_write.h>

#include "Config.h"
#include "Display.h"
#include "RayTracerHost.h"
#include "RayTracerDevice.cuh"

using namespace owl;

RayTracerHost::RayTracerHost() {
    // Initialize OWL context so we can start loading resources
    context = owlContextCreate(nullptr, 1);
    OWLModule module = owlModuleCreate(context, RayTracerDevice_ptx);

    // Fetch scene data
    std::cout << "Building scene..." << std::endl;
    const Scene& scene = *SCENE_LIST[SCENE_INDEX];
    std::vector<std::shared_ptr<Model>> models;
    std::vector<OWLTexture> textures;
    scene.build(context, models, textures);

    // Extract vertex, triangle, and texture/material data from scene models
    // TODO This could be optimized by using primitive arrays instead of vectors
    std::cout << "Initializing geometry..." << std::endl;
    std::vector<vec3f> vertices;
    std::vector<vec3i> triangles;
    std::vector<vec2f> textureCoords;
    std::vector<int> materialIndices;
    std::vector<Material> materials;
    int triangleOffset = 0;
    for(std::shared_ptr<Model> model : models){
        for(int i = 0; i < model->getNumVertices(); i++) vertices.push_back(model->getVertices()[i]);
        for(int i = 0; i < model->getNumTriangles(); i++){
            triangles.push_back(model->getTriangles()[i] + triangleOffset);
            textureCoords.push_back(model->getTextureCoords()[i * 3]);
            textureCoords.push_back(model->getTextureCoords()[i * 3 + 1]);
            textureCoords.push_back(model->getTextureCoords()[i * 3 + 2]);
            materialIndices.push_back((int)materials.size());
        }
        materials.push_back(model->getMaterial());
        triangleOffset += model->getNumVertices();
    }

    // Initialize OWL data structures and parameters with our world geometry and materials
    OWLVarDecl trianglesGeomVars[] = {
            {"vertices", OWL_BUFPTR, OWL_OFFSETOF(WorldGeometry, vertices)},
            {"triangles", OWL_BUFPTR, OWL_OFFSETOF(WorldGeometry, triangles)},
            {"textureCoords", OWL_BUFPTR, OWL_OFFSETOF(WorldGeometry, textureCoords)},
            {"materialIndices", OWL_BUFPTR, OWL_OFFSETOF(WorldGeometry, materialIndices)},
            {"materials", OWL_BUFPTR, OWL_OFFSETOF(WorldGeometry, materials)},
            {"textures", OWL_BUFPTR, OWL_OFFSETOF(WorldGeometry, textures)},
            {nullptr}
    };
    OWLGeomType trianglesGeomType = owlGeomTypeCreate(context, OWL_TRIANGLES, sizeof(WorldGeometry), trianglesGeomVars, -1);
    owlGeomTypeSetClosestHit(trianglesGeomType, 0, module, "TriangleMesh");

    OWLBuffer vertexBuffer = owlDeviceBufferCreate(context, OWL_FLOAT3, vertices.size(), vertices.data());
    OWLBuffer triangleBuffer = owlDeviceBufferCreate(context, OWL_INT3, triangles.size(), triangles.data());
    OWLBuffer textureCoordBuffer = owlDeviceBufferCreate(context, OWL_FLOAT2, textureCoords.size(), textureCoords.data());
    OWLBuffer materialIndicesBuffer = owlDeviceBufferCreate(context, OWL_INT, materialIndices.size(), materialIndices.data());
    OWLBuffer materialBuffer = owlDeviceBufferCreate(context, OWL_USER_TYPE(materials[0]), materials.size(), materials.data());
    OWLBuffer textureBuffer = owlDeviceBufferCreate(context, OWL_TEXTURE, textures.size(), textures.data());

    OWLGeom worldGeometry = owlGeomCreate(context, trianglesGeomType);
    owlTrianglesSetVertices(worldGeometry, vertexBuffer, vertices.size(), sizeof(vertices[0]), 0);
    owlTrianglesSetIndices(worldGeometry, triangleBuffer, triangles.size(), sizeof(triangles[0]), 0);

    owlGeomSetBuffer(worldGeometry, "vertices", vertexBuffer);
    owlGeomSetBuffer(worldGeometry, "triangles", triangleBuffer);
    owlGeomSetBuffer(worldGeometry, "textureCoords", textureCoordBuffer);
    owlGeomSetBuffer(worldGeometry, "materialIndices", materialIndicesBuffer);
    owlGeomSetBuffer(worldGeometry, "materials", materialBuffer);
    owlGeomSetBuffer(worldGeometry, "textures", textureBuffer);

    OWLGroup trianglesGroup = owlTrianglesGeomGroupCreate(context, 1, &worldGeometry);
    owlGroupBuildAccel(trianglesGroup);
    OWLGroup worldGroup = owlInstanceGroupCreate(context, 1, &trianglesGroup);
    owlGroupBuildAccel(worldGroup);

    owlMissProgCreate(context, module, "miss", 0, nullptr, 0);

    OWLVarDecl rayGenVars[] = {
            {"frameBuffer", OWL_RAW_POINTER, OWL_OFFSETOF(RayGenerator, frameBuffer)},
            {"size", OWL_INT2, OWL_OFFSETOF(RayGenerator, size)},
            {"worldHandle", OWL_GROUP, OWL_OFFSETOF(RayGenerator, worldHandle)},
            {"camera.location", OWL_FLOAT3, OWL_OFFSETOF(RayGenerator, camera.location)},
            {"camera.originPixel", OWL_FLOAT3, OWL_OFFSETOF(RayGenerator, camera.originPixel)},
            {"camera.dirRight", OWL_FLOAT3, OWL_OFFSETOF(RayGenerator, camera.dirRight)},
            {"camera.dirUp", OWL_FLOAT3, OWL_OFFSETOF(RayGenerator, camera.dirUp)},
            {}
    };
    rayGen = owlRayGenCreate(context, module, "rayGenProgram", sizeof(RayGenerator), rayGenVars, -1);
    owlRayGenSetGroup(rayGen, "worldHandle", worldGroup);

    // Build everything
    owlBuildPrograms(context);
    owlBuildPipeline(context);

    std::cout << "Launching program..." << std::endl;
}

void RayTracerHost::updateCamera(vec3f cameraLocation, vec3f cameraTarget) {
    // Calculate camera parameters
    vec3f cameraDir = normalize(cameraTarget - cameraLocation);
    float aspect = (float)display::getSize().x / (float)display::getSize().y;
    vec3f cameraDirRight = CAMERA_COS_FOVY * aspect * normalize(cross(cameraDir, CAMERA_LOOK_UP));
    vec3f cameraDirUp = CAMERA_COS_FOVY * normalize(cross(cameraDirRight, cameraDir));
    vec3f cameraOriginPixel = cameraDir - 0.5f * cameraDirRight - 0.5f * cameraDirUp;

    // Send camera parameters to the ray tracer
    owlRayGenSet1ul(rayGen, "frameBuffer", (uint64_t)display::getFrameBuffer());
    owlRayGenSet2i(rayGen, "size", display::getSize().x, display::getSize().y);
    owlRayGenSet3f(rayGen, "camera.location", (const owl3f&)cameraLocation);
    owlRayGenSet3f(rayGen, "camera.originPixel", (const owl3f&)cameraOriginPixel);
    owlRayGenSet3f(rayGen, "camera.dirRight", (const owl3f&)cameraDirRight);
    owlRayGenSet3f(rayGen, "camera.dirUp", (const owl3f&)cameraDirUp);
}

void RayTracerHost::update(float delta) {
    if(timerFreeze > 0.0f) timerFreeze -= delta;
    else timer += delta / (2.0f * 4.0f);

    updateCamera(SCENE_LIST[SCENE_INDEX]->getCameraDynamicLocation(timer) + (timerFreeze > 0.0f ? vec3f(0.0f, 10.0f, 0.0f) : vec3f(0.0f)),
                 SCENE_LIST[SCENE_INDEX]->getCameraDynamicTarget(timer));

    // Run ray tracer
    owlBuildSBT(context);
    owlRayGenLaunch2D(rayGen, display::getSize().x, display::getSize().y);
}

void RayTracerHost::capture() {
    updateCamera(SCENE_LIST[SCENE_INDEX]->getCameraStaticLocation(),
                 SCENE_LIST[SCENE_INDEX]->getCameraStaticTarget());

    // Run ray tracer
    owlBuildSBT(context);
    owlRayGenLaunch2D(rayGen, display::getSize().x, display::getSize().y);

    // Save output image data to a PNG using STB
    uint32_t* frameBuffer = display::getFrameBuffer();
    stbi_flip_vertically_on_write(true);
    stbi_write_png("capture.png", display::getSize().x, display::getSize().y, 4, frameBuffer, display::getSize().x * sizeof(uint32_t));
}