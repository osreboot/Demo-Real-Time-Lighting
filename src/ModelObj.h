#pragma once

#include "Model.h"

class ModelObj : public Model {

private:
    int numVertices = 0;
    int numTriangles = 0;
    owl::vec3f* vertices;
    owl::vec3i* triangles;
    owl::vec2f* textureCoords;
    Material material;

public:
    ModelObj(const std::string& path, owl::vec3f scale, owl::vec3f location, float rotation, Material material);
    ~ModelObj() override;

    int getNumVertices() const override { return numVertices; }
    int getNumTriangles() const override { return numTriangles; }
    owl::vec3f* getVertices() const override { return vertices; }
    owl::vec3i* getTriangles() const override { return triangles; }
    owl::vec2f* getTextureCoords() const override { return textureCoords; }
    Material getMaterial() const override { return material; }

};
