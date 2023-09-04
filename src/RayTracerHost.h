#pragma once

extern "C" char RayTracerDevice_ptx[];

class RayTracerHost {

private:
    OWLRayGen rayGen;
    OWLContext context;

    float timer = 0.0f, timerFreeze = 5.0f;

public:
    RayTracerHost();

    void update(float delta); // Called repeatedly in Main.cpp when running in dynamic mode
    void capture(); // Called once by Main.cpp when running in static mode

};
