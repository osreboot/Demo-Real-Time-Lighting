
#include <fstream>
#include <vector>

#include <owl/common/math/AffineSpace.h>

#include "Model.h"
#include "ModelObj.h"

using namespace std;
using namespace owl;

ModelObj::ModelObj(const string& path, vec3f scale, vec3f location, float yRotation, Material material) :
        material(std::move(material)) {

    vector<vec3f> verticesVec;
    vector<vec3i> trianglesVec;

    ifstream ifs(path);
    string line;
    while(getline(ifs, line)) {
        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if(prefix == "v") { // Vertex definition
            float x, y, z;
            iss >> x >> y >> z;
            verticesVec.emplace_back(x, y, z);
        }else if(prefix == "f") { // Face definition
            // Space-separated vertices list, with the syntax: vertexIndex/vertexTextureIndex/vertexNormalIndex
            vector<vec3i> faceVertices;

            string sFaceVertex;
            while(iss >> sFaceVertex) {
                vec3i faceVertex = {0, 0, 0};
                int charIndex = 0;
                while(charIndex < sFaceVertex.length() && sFaceVertex[charIndex] != '/'){
                    faceVertex.x = faceVertex.x * 10 + (sFaceVertex[charIndex++] - '0');
                }
                while(charIndex < sFaceVertex.length() && sFaceVertex[charIndex] != '/'){
                    faceVertex.y = faceVertex.y * 10 + (sFaceVertex[charIndex++] - '0');
                }
                while(charIndex < sFaceVertex.length() && sFaceVertex[charIndex] != '/'){
                    faceVertex.z = faceVertex.z * 10 + (sFaceVertex[charIndex++] - '0');
                }
                faceVertices.push_back(faceVertex);
            }
            if(faceVertices.size() == 4) {
                trianglesVec.emplace_back(faceVertices[0].x, faceVertices[1].x, faceVertices[2].x);
                trianglesVec.emplace_back(faceVertices[0].x, faceVertices[2].x, faceVertices[3].x);
            }else if(faceVertices.size() == 3) {
                trianglesVec.emplace_back(faceVertices[0].x, faceVertices[1].x, faceVertices[2].x);
            }else throw runtime_error("Unexpected vertex count in face list!" + to_string(faceVertices.size()));
        }
    }

    affine3f transform(linear3f::scale(scale));
    transform = affine3f(linear3f::rotate(vec3f(0.0f, 1.0f, 0.0f), ((float)M_PI / 180.0f) * yRotation)) * transform;
    transform = affine3f(affine3f::translate(location)) * transform;

    numVertices = (int)verticesVec.size();
    vertices = new vec3f[numVertices];
    for(int i = 0; i < numVertices; i++){
        vertices[i] = xfmPoint(transform, verticesVec[i]);
    }

    numTriangles = (int)trianglesVec.size();
    triangles = new vec3i[numTriangles];
    for(int i = 0; i < numTriangles; i++){
        triangles[i] = trianglesVec[i] - vec3i(1);
    }
}

ModelObj::~ModelObj() {
    delete[] vertices;
    delete[] triangles;
}
