/**
 * @brief Tool class which convey to open a serial port
 * @file serial_port.cpp
 * 
 * @author Sylvain Colomer
 * @date 19/04/19
 * @version 1.1 
 */

#ifndef COMM_WIFI_H
#define COMM_WIFI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>

#include "../../lib/mavlink/common/mavlink.h"

#define BUFFER_LENGTH 2041

/*
 * Wifi port class
 *
 */
class Wifi_Port
{

private: 

	int sock;
	struct sockaddr_in targetAddr;				// The socket of the target address
	struct sockaddr_in locAddr;					// The socket of the local address
	uint16_t len;
	uint8_t buf[BUFFER_LENGTH];
	int bytes_sent;
	
	
	char target_ip[100]= "192.168.1.36"; // default simulation ip
	int local_port = 14550; //default simulation port

public:

	Wifi_Port();
	~Wifi_Port();

    int initialize_defaults(int *sock, struct sockaddr_in *locAddr, struct sockaddr_in *targetAddr, int local_port, char *target_ip);

    int read_message(mavlink_message_t *message);
	int write_message(const mavlink_message_t *message);

private:

};

#endif 