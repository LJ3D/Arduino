
/*
	* Using C-style IO for the /dev/ttyACM(X) interface, even though I have to know what an "pointer" is its easier than dealing with C++ stuff
	* Low level my beloved
*/

#include <stdio.h> 
#include <unistd.h>  
#include <fcntl.h>
#include <errno.h>

#include <string>
#include <stdexcept>
#include <iostream>

#define BUFFERSIZE 16 // 16 bytes is double the size of a long long int, so it should be enough for now

#define DEVICE "/dev/ttyACM0"

class arduinoPortIO{
private:
int fd;
std::string ttyName;
unsigned char buffer[BUFFERSIZE];
char requestByte; // The byte sent to the arduino to request data

void readIntoBuffer(){
		int n = read(this->fd, this->buffer, BUFFERSIZE);
	}

public:
	arduinoPortIO(std::string tty, char r = 'r'){
		this->ttyName = tty;
		this->fd = open(this->ttyName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if(this->fd == -1){
			throw std::runtime_error("ERR: Could not open " + tty);
		}
		this->requestByte = r;
	}
	~arduinoPortIO(){
		close(this->fd);
	}	


	/* 
		* Writes a single byte to the arduino to request data, then waits for the arduino to send the data back.
		* What this data exactly is is up to the arduino code, but it should be a single integer in this case.
		* Requesting data instead of the arduino sending data all the time means the values read are more up to date (realtime)
	*/ 
	std::string requestDataBufferString(){
		write(this->fd, &this->requestByte, 1); // Send the request byte
		usleep(1000); // Wait for the arduino to send the data back
		// todo: clear /dev/ttyACM(X) buffer, then wait for the arduino to add something to it before reading it
		this->readIntoBuffer(); // Read the data into the buffer
		std::string bufferString = ""; // Convert the buffer into an std::string (Makes it easier to work with)
		for(int i = 0; i < BUFFERSIZE; i++){
			bufferString += this->buffer[i];
		}
		return bufferString;
	}
	
};
