#include <chrono>
#include <glfw/glfw3.h>

#include "display.cuh"
#include "rt_setup.cuh"

using namespace std;

chrono::high_resolution_clock::time_point timeLastUpdate;

int main(){
    display::initialize(1280, 720, "Test");
    rt_setup::initialize();

    timeLastUpdate = chrono::high_resolution_clock::now();

    while(!display::exiting()){
        float delta = (float)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - timeLastUpdate).count() / 1000.0f;
        delta = min(delta, 0.2f);
        timeLastUpdate = chrono::high_resolution_clock::now();

        display::preUpdate();

        rt_setup::update(delta);

        display::postUpdate();
    }

    glfwTerminate();
    return 0;
}
