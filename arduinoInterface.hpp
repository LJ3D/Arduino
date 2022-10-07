
/*
	* Using C-style IO for the /dev/ttyACM(X) interface, even though I have to know what an "pointer" is its easier than dealing with C++ stuff
	* Low level my beloved
	* This class is not very good right now, but later i will add a lot of functionality to it (maybe)
*/

#include <stdio.h> 
#include <unistd.h>  
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

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

	int open_port(std::string tty){
		int fd = open(tty.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if(fd == -1){
			fprintf(stderr, "Failed to open port\n");
			return -1;
		}
		fcntl(fd, F_SETFL, 0); // Blocking mode - waits for data in input buffer
		struct termios options; // Port options
		tcgetattr(fd, &options); // Get the current options for the port
		cfsetispeed(&options, B19200); // Set baud rates
		cfsetospeed(&options, B19200);
		options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode
		tcsetattr(fd, TCSANOW, &options); // Set the new options for the port
		options.c_cflag &= ~CSIZE; // Mask the character size bits
		options.c_cflag |= CS8; // Select 8 data bits
		options.c_cflag &= ~PARENB; // No parity
		options.c_cflag &= ~CSTOPB; // 1 Stop bit
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8; // 8 bits
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input mode
		return fd;
	}

public:
	arduinoPortIO(std::string tty){
		this->ttyName = tty;
		this->fd = open_port(ttyName);
		if(this->fd == -1){
			throw std::runtime_error("ERR: Could not open port " + tty);
		}
		tcflush(this->fd, TCIOFLUSH);
	}
	~arduinoPortIO(){
		close(this->fd);
	}

	void writeByte(char c){
		write(this->fd, &c, 1);
	}

	void clearBuffer(){
		for(int i = 0; i < BUFFERSIZE; i++){
			this->buffer[i] = 0;
		}
	}

	int readIntoBuffer(){
		return read(this->fd, this->buffer, BUFFERSIZE); // Returns number of bytes read
	}

	std::string getBuffer(){
		std::string bufferString;
		for(int i=0; i<BUFFERSIZE; i++){
			bufferString += this->buffer[i];
		}
		return bufferString;
	}
	
	std::string readGetBuffer(){
		this->readIntoBuffer();
		return this->getBuffer();
	}
};
