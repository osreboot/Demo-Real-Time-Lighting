#include <chrono>
#include <glfw/glfw3.h>

#include "config.cuh"
#include "display.cuh"
#include "rt_setup.cuh"

using namespace std;

chrono::high_resolution_clock::time_point timeLastUpdate;

int main(){
    display::initialize(1280, 720, "Real-Time Lighting / Photoreal Demo - by Calvin Weaver");
    rt_setup::initialize();

    bool captured = false;
    timeLastUpdate = chrono::high_resolution_clock::now();

    while(!display::exiting()){
        float delta = (float)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - timeLastUpdate).count() / 1000.0f;
        delta = min(delta, 0.2f);
        timeLastUpdate = chrono::high_resolution_clock::now();

        display::preUpdate();

        if(PROGRAM_MODE_DYNAMIC){
            rt_setup::update(delta);
        }else if(!captured){
            captured = true;
            rt_setup::capture();
        }

        display::postUpdate();
    }

    glfwTerminate();
    return 0;
}
