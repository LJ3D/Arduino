#include "arduinoInterface.hpp"

int main(){
    arduinoPortIO arduino("/dev/ttyACM0");
    usleep(1000000);
    
    while(1){
        arduino.clearBuffer();
        std::cout << arduino.readGetBuffer() << std::endl;
    }
    return 0;
}