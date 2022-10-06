#include <stdio.h> 
#include <string.h>
#include <unistd.h>  
#include <fcntl.h>
#include <errno.h>
//#include <termios.h> // POSIX terminal control definitions, not needed?

#define DEVTTY "/dev/ttyACM0"

int open_port(){
	int fd = open(DEVTTY, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd == -1){
		fprintf(stderr, "Failed to open port\n");
		return -1;
	}
	/*
	fcntl(fd, F_SETFL, 0); // Blocking mode - waits for data in input buffer
	struct termios options; // Port options
	tcgetattr(fd, &options); // Get the current options for the port
	cfsetispeed(&options, B9600); // Set baud rates
	cfsetospeed(&options, B9600);
	options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode
	tcsetattr(fd, TCSANOW, &options); // Set the new options for the port
	options.c_cflag &= ~CSIZE; // Mask the character size bits
	options.c_cflag |= CS8; // Select 8 data bits
	options.c_cflag &= ~PARENB; // No parity
	options.c_cflag &= ~CSTOPB; // 1 Stop bit
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8; // 8 bits
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input mode
	*/
	return fd;
}

// Sleep the program for n milliseconds
void sleep_ms(int milliseconds){
	usleep(milliseconds * 1000);
}

#define BUFFERSIZE 16

int main(){
	int port = open_port();
	if(port == -1){
		return -1;
	}
	unsigned char buffer[BUFFERSIZE];
	sleep_ms(2500); // Give arduino a second to start
	while(1){
		int n = read(port, buffer, BUFFERSIZE);
		if(n > 0){
			printf("Read %d bytes: ", n);
		}
		for(int i=0; i<n; i++){
			printf("%c", buffer[i]);
		}
	}
	return 0;
}
