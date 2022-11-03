#define CAMERA_COS_FOVY 0.66f
#define CAMERA_LOOK_UP vec3f{0.0f, 1.0f, 0.0f}
#define CAMERA_LOOK_AT vec3f{0.0f, 0.0f, 0.0f}

#include <owl/owl.h>
#include <owl/common.h>
#include <vector>
#include <random>

#include "display.cuh"
#include "rt_setup.cuh"
#include "rt_program.cuh"

using namespace std;

extern "C" char rt_program_ptx[];

const int UNIT_NUM_VERTICES = 8;
const vec3f unit_vertices[UNIT_NUM_VERTICES] = {
        {-1.f, -1.f, -1.f},
        {+1.f, -1.f, -1.f},
        {+1.f, +1.f, -1.f},
        {-1.f, +1.f, -1.f},
        {-1.f, +1.f, +1.f},
        {+1.f, +1.f, +1.f},
        {+1.f, -1.f, +1.f},
        {-1.f, -1.f, +1.f}};

const int UNIT_NUM_INDICES = 12;
const vec3i unit_indices[UNIT_NUM_INDICES] = {
        {0, 2, 1}, //face front
        {0, 3, 2},
        {2, 3, 4}, //face top
        {2, 4, 5},
        {1, 2, 5}, //face right
        {1, 5, 6},
        {0, 7, 4}, //face left
        {0, 4, 3},
        {5, 4, 7}, //face back
        {5, 7, 6},
        {0, 6, 7}, //face bottom
        {0, 1, 6}};

OWLRayGen rayGen = {0};
OWLContext context = {0};

vector<vec3f> vertices;
vector<vec3i> indices;
vector<Material> materials;

float timer = 0.0f;

inline float random(){
    static mt19937 gen(0);
    static uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

void addBox(vec3f boxSize, vec3f boxLocation, float rotation, Material material){
    affine3f xfm(linear3f::scale(boxSize));
    xfm = affine3f(linear3f::rotate(vec3f(0.0f, 1.0f, 0.0f), ((float)M_PI / 180.0f) * rotation)) * xfm;
    xfm = affine3f(affine3f::translate(boxLocation)) * xfm;

    const int index = (int)vertices.size();
    for(int i = 0; i < UNIT_NUM_VERTICES; i++){
        vertices.push_back(xfmPoint(xfm, unit_vertices[i]));
    }
    for(int i = 0; i < UNIT_NUM_INDICES; i++){
        indices.push_back(unit_indices[i] + index);
    }
    materials.push_back(material);
}

void rt_setup::initialize(){
    const Material matBase1 = {false, 0.0f, 0.0f, 0.5f, 0.02f, {0.4f, 0.4f, 0.4f}};
    const Material matBase2 = {false, 0.0f, 0.0f, 0.5f, 0.2f, {0.8f, 0.8f, 0.8f}};
    const Material matChrome = {false, 0.0f, 0.0f, 1.0f, 0.01f, {0.95f, 0.95f, 0.95f}};
    const Material matBlue = {false, 0.0f, 0.0f, 0.5f, 0.01f, {0.4f, 0.4f, 0.8f}};
    const Material matBlueGlass = {false, 0.5f, 1.33f, 0.5f, 0.0f, {0.4f, 0.4f, 0.8f}};
    const Material matGlass1 = {false, 0.6f, 1.5f, 0.5f, 0.0f, {0.4f, 0.8f, 0.4f}};
    const Material matGlass2 = {false, 0.99f, 1.05f, 0.1f, 0.0f, {1.0f, 1.0f, 1.0f}};
    const Material matFlat1 = {false, 0.0f, 0.0f, 1.0f, 0.5f, {0.5f, 0.5f, 0.0f}};
    const Material matFlat2 = {false, 0.0f, 0.0f, 0.1f, 0.0f, {0.0f, 0.5f, 0.0f}};
    const Material matLight = {true, 0.0f, 0.0f, 0.0f, 0.0f, {1.0f, 1.0f, 1.0f}};
    const Material matLightBlue = {true, 0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 1.0f}};

    int indexTile = 0;
    for(float x = -10.0f; x <= 10.0f; x += 2.0f){
        for(float y = -10.0f; y <= 10.0f; y += 2.0f){
            addBox(vec3f(1.0f), vec3f(x, -1.0f, y), 0.0f, indexTile++ % 2 == 0 ? matBase1 : matBase2);
        }
    }

    addBox(vec3f(0.75f), vec3f(-2.0f, 0.75f, 0.0f), 0.0f, matChrome);
    addBox(vec3f(0.5f), vec3f(2.0f, 0.5f, 3.0f), 45.0f, matChrome);

    addBox(vec3f(6.0f * 0.2f, 0.1f, 6.0f * 0.2f), vec3f(0.0f, 0.1f, -3.0f), 0.0f, matChrome);
    for(float x = -5.0f; x <= 5.0f; x++){
        for(float y = -5.0f; y <= 5.0f; y++){
            addBox(vec3f(0.08f, random() * 0.2f,0.08f), vec3f(x * 0.2f, 0.2f, -3.0f + y * 0.2f), 0.0f, matChrome);
        }
    }

    addBox(vec3f(0.5f, 0.5f, 0.1f), vec3f(0.0f, 0.5f, 3.0f), 0.0f, matChrome);
    addBox(vec3f(0.5f, 0.5f, 0.1f), vec3f(0.0f, 0.5f, 4.0f), 0.0f, matChrome);

    addBox(vec3f(3.0f, 0.1f, 1.0f), vec3f(6.0f, 0.1f, 0.0f), 0.0f, matBlue);
    indexTile = 0;
    for(float x = -2.5f; x <= 2.5f; x += 0.5f){
        addBox(vec3f(0.04f, 0.2f + (x + 2.5f) / 5.0f, 0.4f), vec3f(6.0f + x, 0.3f, cos(x * 1.5f) * 0.5f), sin(x * 1.5f) * 25.0f,
               indexTile++ % 2 == 0 ? matBlue : matBlueGlass);
    }

    addBox(vec3f(0.5f, 3.0f, 0.5f), vec3f(-3.0f, 2.8f, -3.0f), 0.0f, matGlass1);
    addBox(vec3f(0.6f, 0.6f, 0.6f), vec3f(0.0f, 0.8f, 0.0f), 0.0f, matGlass2);
    addBox(vec3f(0.4f, 0.4f, 0.4f), vec3f(-3.0f, 0.6f, 3.0f), 0.0f, matGlass2);
    addBox(vec3f(0.3f, 0.3f, 0.3f), vec3f(-2.8f, 0.3f, 3.2f), 0.0f, matChrome);
    addBox(vec3f(0.3f, 0.3f, 0.3f), vec3f(-3.2f, 0.8f, 2.8f), 0.0f, matChrome);

    addBox(vec3f(0.5f, 0.5f, 0.5f), vec3f(3.0f, 0.7f, -3.0f), 45.0f, matFlat1);
    addBox(vec3f(0.3f, 0.3f, 0.3f), vec3f(3.4f, 0.7f, -3.0f), 45.0f, matFlat1);
    addBox(vec3f(0.3f, 0.3f, 0.3f), vec3f(2.6f, 0.7f, -3.0f), 45.0f, matFlat1);
    addBox(vec3f(1.0f, 0.05f, 1.0f), vec3f(3.0f, 0.05f, -3.0f), 45.0f, matFlat2);

    addBox(vec3f(1.0f, 0.05f, 1.0f), vec3f(3.0f, 2.0f, -3.0f), 45.0f, matLight);
    addBox(vec3f(1.0f, 0.05f, 1.0f), vec3f(-3.0f, 2.0f, 3.0f), 45.0f, matLightBlue);

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

    vec3f camera_pos = vec3f(sin(timer) * 10.0f, 4.0f, cos(timer) * 10.0f);
    vec3f camera_d00 = normalize(CAMERA_LOOK_AT - camera_pos);
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