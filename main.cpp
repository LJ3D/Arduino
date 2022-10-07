/*
 * Visualize a graph on which points are added with time.
 */
#include <morph/Visual.h>
#include <morph/ColourMap.h>
#include <morph/GraphVisual.h>
#include <morph/Scale.h>
#include <morph/Vector.h>
#include <morph/vVector.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>

#include "arduinoInterface.hpp"

int main (int argc, char** argv)
{
    arduinoPortIO arduino("/dev/ttyACM0");
    usleep(1000000);

    int rtn = -1;

    morph::Visual v(1800, 600, "Graph", {0, 0}, {.1,.1,.1}, 2.0f, 0.01f);
    v.zNear = 0.001;
    v.backgroundWhite();
    v.lightingEffects();

    try {
        morph::GraphVisual<float>* gv = new morph::GraphVisual<float> (v.shaderprog, v.tshaderprog, {-1,-0.5,0});

        // Optionally change the size of the graph and range of the axes
        gv->setsize (2, 1);
        // Optionally change the range of the axes
        int limit = 512;
        gv->setlimits (-1, limit, -1, 64);

        // Set the graphing policy
        gv->policy = morph::stylepolicy::lines; // markers, lines, both, allcolour
        // We 'prepare' a dataset, but won't fill it with data yet. However, we do give the data legend label here.
        gv->prepdata ("Arduino output", morph::axisside::left);

        gv->ylabel = "Arduino output (CM)";
        gv->finalize();

        // Add the GraphVisual (as a VisualModel*)
        v.addVisualModel (static_cast<morph::VisualModel*>(gv));

        v.render();
        int i = 0;
        while (v.readyToFinish == false) {
            glfwWaitEventsTimeout (0.018);

            // Arduino stuff
            arduino.clearBuffer();
            std::string data = arduino.readGetBuffer();
            int dataSTOI;
            try{
                dataSTOI = std::stoi(data);
                gv->append (i, dataSTOI, 0);
            }catch(...){
                //std::cout << "STOI error\n";
            }
            v.render();
            i++;

            // Experiment with graphing stuff
            if(i > limit){
                i = 0;
                gv->clear();
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        rtn = -1;
    }

    return rtn;
}
