
#include <owl/common/math/AffineSpace.h>

#include "Model.h"
#include "ModelCuboid.h"

using namespace owl;

const int UNIT_BOX_NUM_VERTICES = 8;
const vec3f UNIT_BOX_VERTICES[UNIT_BOX_NUM_VERTICES] = {
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f}
};
const int UNIT_BOX_NUM_TRIANGLES = 12;
const vec3i UNIT_BOX_TRIANGLES[UNIT_BOX_NUM_TRIANGLES] = {
        {0, 2, 1}, {0, 3, 2}, // Front
        {2, 3, 4}, {2, 4, 5}, // Top
        {1, 2, 5}, {1, 5, 6}, // Right
        {0, 7, 4}, {0, 4, 3}, // Left
        {5, 4, 7}, {5, 7, 6}, // Back
        {0, 6, 7}, {0, 1, 6} // Bottom
};

ModelCuboid::ModelCuboid(vec3f scale, vec3f location, float yRotation, Material material) :
    vertices(new vec3f[UNIT_BOX_NUM_VERTICES]),
    triangles(new vec3i[UNIT_BOX_NUM_TRIANGLES]),
    material(std::move(material)) {

    affine3f transform(linear3f::scale(scale));
    transform = affine3f(linear3f::rotate(vec3f(0.0f, 1.0f, 0.0f), ((float)M_PI / 180.0f) * yRotation)) * transform;
    transform = affine3f(affine3f::translate(location)) * transform;

    for(int i = 0; i < UNIT_BOX_NUM_VERTICES; i++){
        vertices[i] = xfmPoint(transform, UNIT_BOX_VERTICES[i]);
    }
    for(int i = 0; i < UNIT_BOX_NUM_TRIANGLES; i++){
        triangles[i] = UNIT_BOX_TRIANGLES[i];
    }
}

ModelCuboid::~ModelCuboid() {
    delete[] vertices;
    delete[] triangles;
}

int ModelCuboid::getNumVertices() const {
    return UNIT_BOX_NUM_VERTICES;
}

int ModelCuboid::getNumTriangles() const {
    return UNIT_BOX_NUM_TRIANGLES;
}
