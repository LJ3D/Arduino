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
    arduinoSerial serial("/dev/ttyACM0"); // Create a serial communication object to talk to the Arduino at /dev/ttyACM0. This doesnt open up the serial port yet.
    serial.begin(B9600); // Start serial communication at 9600 baud
    morph::Visual v(1800, 600, "Graph", {0, 0}, {.1,.1,.1}, 2.0f, 0.01f); // Create a Visual object to display the graph
    v.zNear = 0.001;
    v.backgroundWhite(); // Sets the background colour to white
    v.lightingEffects(); // Enables lighting (shadows etc) to 3D objects

    int rtn = -1;
    try {
        morph::GraphVisual<float>* gv = new morph::GraphVisual<float> (v.shaderprog, v.tshaderprog, {-1,-0.5,0}); // Create a GraphVisual object to display the graph

        // Change the size of the graph and range of the axes
        gv->setsize (2, 1);

        // Change the range of the axes
        int limit = 512;
        gv->setlimits (-1, limit, -1, 64);

        // Set the graphing policy
        gv->policy = morph::stylepolicy::lines; // markers, lines, both, allcolour
        gv->axisstyle = morph::axisstyle::twinax;

        // We 'prepare' a dataset, but won't fill it with data yet. However, we do give the data legend label here.
        gv->prepdata ("Arduino output", morph::axisside::left);
        gv->prepdata ("Arduino output, moving average (last 10 values)", morph::axisside::left);

        // Set the labels up
        gv->ylabel = "Arduino output (CM)";
        gv->ylabel2 = "Moving average (last 10 values)";
        
        gv->finalize(); // "Finalize" the graph (Sets everything up ready for data to be added and drawn)
        
        v.addVisualModel (static_cast<morph::VisualModel*>(gv)); // Add the GraphVisual (as a VisualModel*)
        
        v.render(); // Render the graph
        int i = 0; // Counter for the number of points added to the graph
        int value = 0; // Value read from the serial port
        std::vector<int> last10values; // last 10 values for moving average
        while (v.readyToFinish == false) {
            glfwWaitEventsTimeout (0.018);
            serial.flush(); // Clear the serial buffer
            std::string dataStr = serial.readStringUntil('\n');
            while(dataStr == "\n"){ // Sometimes the first character to hit the buffer is a newline, in that case, read again until we get a value that std::stoi can parse
                dataStr = serial.readStringUntil('\n');
            }
            value = std::stoi(dataStr); // Convert the string to an integer
            gv->append (i, value, 0); // Add a point to the graph
            last10values.push_back(value);
            if(last10values.size() > 10){ // keep last 10 values, remove the oldest
                last10values.erase(last10values.begin());
            }
            gv->append(i, std::accumulate(last10values.begin(), last10values.end(), 0.0) / last10values.size(), 1); // Add the average of the last 10 values to the graph (moving average)
            if(i++ > limit){ // Reset the graph if it gets too big, increment i on the same line too :)
                i = 0;
                gv->clear();
            }
            v.render(); // Render the graph
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        rtn = -1;
    }

    return rtn;
}
