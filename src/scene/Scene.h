#pragma once

#include <vector>
#include <random>

#include <owl/owl.h>
#include <owl/common.h>
#include <stb/stb_image.h>

#include "Model.h"
#include "RayTracerDevice.cuh"

// Easy uniform random number access
inline float random(){
    static std::mt19937 gen(0);
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

class Scene {

protected:
    static OWLTexture loadTexture(OWLContext context, const std::string& path) {
        int width, height;
        unsigned char* image = stbi_load(path.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
        OWLTexture texture = owlTexture2DCreate(context, OWL_TEXEL_FORMAT_RGBA8, width, height, image,
                                                OWL_TEXTURE_NEAREST, OWL_TEXTURE_WRAP);
        stbi_image_free(image);
        return texture;
    }

public:
    virtual void build(OWLContext context, std::vector<std::shared_ptr<Model>>& models, std::vector<OWLTexture>& textures) const = 0;
    virtual owl::vec3f getCameraDynamicLocation(float timer) const { return {0.0f, 0.0f, 0.0f}; }
    virtual owl::vec3f getCameraDynamicTarget(float timer) const { return {0.0f, 0.0f, 0.0f}; }
    virtual owl::vec3f getCameraStaticLocation() const { return {0.0f, 0.0f, 0.0f}; }
    virtual owl::vec3f getCameraStaticTarget() const { return {0.0f, 0.0f, 0.0f}; }

};