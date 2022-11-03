#pragma once

#include <owl/owl.h>

using namespace owl;

namespace display{

    void initialize(int widthArg, int heightArg, const char* titleArg);

    void preUpdate();
    void postUpdate();

    bool exiting();

    uint32_t* getFrameBuffer();
    vec2i getSize();

}