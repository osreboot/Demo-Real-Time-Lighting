#pragma once

#include <owl/owl.h>

namespace display{

    void initialize(int widthArg, int heightArg, const char* titleArg);

    void preUpdate();
    void postUpdate();

    bool exiting();

    uint32_t* getFrameBuffer();
    owl::vec2i getSize();

}