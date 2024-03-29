#pragma once

#include "Model.h"

class ModelCuboid : public Model {

private:
    owl::vec3f* vertices;
    owl::vec3i* triangles;
    owl::vec2f* textureCoords;
    Material material;

public:
    ModelCuboid(owl::vec3f scale, owl::vec3f location, float rotation, Material material);
    ~ModelCuboid() override;

    int getNumVertices() const override;
    int getNumTriangles() const override;
    owl::vec3f* getVertices() const override { return vertices; }
    owl::vec3i* getTriangles() const override { return triangles; }
    owl::vec2f* getTextureCoords() const override { return textureCoords; }
    Material getMaterial() const override { return material; }

};
