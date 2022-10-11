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

#include "ArduinoSerialIO/arduinoSerial.h"

int main (int argc, char** argv)
{
    arduinoSerial serial("/dev/ttyACM0");
    serial.begin(B9600);

    int rtn = -1;

    morph::Visual v(1800, 600, "Graph", {0, 0}, {.1,.1,.1}, 2.0f, 0.01f);
    v.zNear = 0.001;
    v.backgroundWhite();
    v.lightingEffects();

    try {
        morph::GraphVisual<float>* gv = new morph::GraphVisual<float> (v.shaderprog, v.tshaderprog, {0,0,0});

        // Optionally change the size of the graph and range of the axes
        gv->setsize (2, 1);
        // Optionally change the range of the axes
        int limit = 512;
        gv->setlimits (-1, limit, -1, 64);

        // Set the graphing policy
        gv->policy = morph::stylepolicy::lines; // markers, lines, both, allcolour
        gv->axisstyle = morph::axisstyle::twinax;
        // We 'prepare' a dataset, but won't fill it with data yet. However, we do give the data legend label here.
        gv->prepdata ("Arduino output", morph::axisside::left);
        gv->prepdata ("Arduino output, moving average (last 10 values)", morph::axisside::left);

        gv->ylabel = "Arduino output (CM)";
        gv->ylabel2 = "Moving average (last 10 values)";
        gv->finalize();

        // Add the GraphVisual (as a VisualModel*)
        v.addVisualModel (static_cast<morph::VisualModel*>(gv));

        std::vector<int> last10values;

        v.render();
        int i = 0;
        while (v.readyToFinish == false) {
            glfwWaitEventsTimeout (0.018);

            // == Arduino stuff:
            serial.flush();
            int value = serial.parseInt();
            //std::cout << value << std::endl;
            if(value != -1){
                gv->append (i, value, 0);
                last10values.push_back(value);
            }
            // Moving average
            if(last10values.size() > 10){
                last10values.erase(last10values.begin());
            }
            gv->append(i, std::accumulate(last10values.begin(), last10values.end(), 0.0) / last10values.size(), 1);
            i++;

            // Experiment with graphing stuff
            if(i > limit){
                i = 0;
                gv->clear();
            }

            v.render();
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        rtn = -1;
    }

    return rtn;
}
