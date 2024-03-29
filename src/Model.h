#pragma once

#include <owl/owl.h>
#include <owl/common/math/vec.h>

#include "RayTracerDevice.cuh"

class Model {

public:
    virtual ~Model() = default;

    virtual int getNumVertices() const = 0;
    virtual int getNumTriangles() const = 0;
    virtual owl::vec3f* getVertices() const = 0;
    virtual owl::vec3i* getTriangles() const = 0;
    virtual owl::vec2f* getTextureCoords() const = 0;
    virtual Material getMaterial() const = 0;

};