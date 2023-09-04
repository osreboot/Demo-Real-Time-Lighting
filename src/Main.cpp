#include <chrono>
#include <glfw/glfw3.h>

#include "Config.h"
#include "Display.h"
#include "RayTracerHost.h"

using namespace std;

chrono::high_resolution_clock::time_point timeLastUpdate, timeNow;

int main(){
    // Create the display and initialize the ray tracing program
    display::initialize(1920, 1080, "Real-Time Lighting / Photoreal Demo - by Calvin Weaver");
    rt_setup::initialize();

    bool captured = false;
    timeLastUpdate = chrono::high_resolution_clock::now();

    // Loop while the user hasn't clicked the window close button
    while(!display::exiting()){
        // Delta is the time in seconds since the last update. This value is used to synchronize scene timing elements
        // that need to run at a consistent speed, even through inconsistent rendering speeds / update timings (lag).

        timeNow = chrono::high_resolution_clock::now();
        float delta = (float)chrono::duration_cast<chrono::nanoseconds>(timeNow - timeLastUpdate).count() / 1000000000.0f;
        //delta = min(delta, 0.2f);
        timeLastUpdate = timeNow;

        // Prepare the display for rendering
        display::preUpdate();

        // Run the ray tracing program
        if(PROGRAM_MODE_DYNAMIC){
            rt_setup::update(delta);
        }else if(!captured){
            captured = true;
            rt_setup::capture();
            cout << "Rendered in ";
            cout << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - timeLastUpdate).count();
            cout << " microseconds." << endl;
        }

        // Render the final image to the display
        display::postUpdate();
    }

    glfwTerminate();
    return 0;
}
