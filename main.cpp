#include "arduinoInterface.hpp"

int main(){
    arduinoPortIO arduino("/dev/ttyACM0");
    while(true){
        std::cout << arduino.requestDataBufferString();
        usleep(100000);
    }
    return 0;
}