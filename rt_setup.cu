#define CAMERA_COS_FOVY 0.66f
#define CAMERA_LOOK_UP vec3f{0.0f, 1.0f, 0.0f}

#include <owl/owl.h>
#include <vector>

#include "config.cuh"
#include "display.cuh"
#include "rt_setup.cuh"
#include "rt_program.cuh"

using namespace std;

extern "C" char rt_program_ptx[];

OWLRayGen rayGen = {0};
OWLContext context = {0};

vector<vec3f> vertices;
vector<vec3i> indices;
vector<Material> materials;

float timer = 0.0f;

void rt_setup::initialize(){
    const Scene &scene = *SCENE_LIST[SCENE_INDEX];
    scene.initialize({&vertices, &indices, &materials});

    context = owlContextCreate(nullptr, 1);
    OWLModule module = owlModuleCreate(context, rt_program_ptx);

    OWLVarDecl trianglesGeomVars[] = {
            {"index", OWL_BUFPTR, OWL_OFFSETOF(TrianglesGeomData, index)},
            {"vertex", OWL_BUFPTR, OWL_OFFSETOF(TrianglesGeomData, vertex)},
            {"material", OWL_BUFPTR, OWL_OFFSETOF(TrianglesGeomData, material)}
    };
    OWLGeomType trianglesGeomType = owlGeomTypeCreate(context, OWL_TRIANGLES, sizeof(TrianglesGeomData), trianglesGeomVars, 3);
    owlGeomTypeSetClosestHit(trianglesGeomType, 0, module, "TriangleMesh");

    OWLBuffer vertexBuffer = owlDeviceBufferCreate(context, OWL_FLOAT3, vertices.size(), vertices.data());
    OWLBuffer indexBuffer = owlDeviceBufferCreate(context, OWL_INT3, indices.size(), indices.data());
    OWLBuffer materialBuffer = owlDeviceBufferCreate(context, OWL_USER_TYPE(materials[0]), materials.size(), materials.data());

    OWLGeom trianglesGeom = owlGeomCreate(context, trianglesGeomType);
    owlTrianglesSetVertices(trianglesGeom, vertexBuffer, vertices.size(), sizeof(vertices[0]), 0);
    owlTrianglesSetIndices(trianglesGeom, indexBuffer, indices.size(), sizeof(indices[0]), 0);

    owlGeomSetBuffer(trianglesGeom, "vertex", vertexBuffer);
    owlGeomSetBuffer(trianglesGeom, "index", indexBuffer);
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

    owlBuildPrograms(context);
    owlBuildPipeline(context);
}

void rt_setup::update(float delta){
    timer += delta / 2.0f;

    vec3f camera_pos = SCENE_LIST[SCENE_INDEX]->getCameraLocation(timer);
    vec3f camera_d00 = normalize(SCENE_LIST[SCENE_INDEX]->getCameraTarget(timer) - camera_pos);
    float aspect = float(display::getSize().x) / float(display::getSize().y);
    vec3f camera_ddu = CAMERA_COS_FOVY * aspect * normalize(cross(camera_d00,CAMERA_LOOK_UP));
    vec3f camera_ddv = CAMERA_COS_FOVY * normalize(cross(camera_ddu,camera_d00));
    camera_d00 -= 0.5f * camera_ddu;
    camera_d00 -= 0.5f * camera_ddv;

    owlRayGenSet1ul(rayGen, "frameBuffer", (uint64_t)display::getFrameBuffer());
    owlRayGenSet2i(rayGen, "size", display::getSize().x, display::getSize().y);
    owlRayGenSet3f(rayGen, "camera.pos", (const owl3f&)camera_pos);
    owlRayGenSet3f(rayGen, "camera.dir_00", (const owl3f&)camera_d00);
    owlRayGenSet3f(rayGen, "camera.dir_du", (const owl3f&)camera_ddu);
    owlRayGenSet3f(rayGen, "camera.dir_dv", (const owl3f&)camera_ddv);

    owlBuildSBT(context);
    owlRayGenLaunch2D(rayGen, display::getSize().x, display::getSize().y);
}