#pragma once

#include "Model.h"

class ModelObj : public Model {

private:
    int numVertices = 0;
    int numTriangles = 0;
    owl::vec3f* vertices = nullptr;
    owl::vec3i* triangles = nullptr;
    Material material;

public:
    ModelObj(const std::string& path, owl::vec3f scale, owl::vec3f location, float rotation, Material material);
    ~ModelObj() override;

    int getNumVertices() const override { return numVertices; }
    int getNumTriangles() const override { return numTriangles; }
    owl::vec3f* getVertices() const override { return vertices; }
    owl::vec3i* getTriangles() const override { return triangles; }
    Material getMaterial() const override { return material; }

};
