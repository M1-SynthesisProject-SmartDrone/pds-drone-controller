/**
 * @brief Tool class which convey to open a serial port
 * @file serial_port.cpp
 * 
 * @author Sylvain Colomer
 * @date 19/04/19
 * @version 1.1 
 */

#include <iostream>
#include <string>
#include <sys/time.h>
#include <chrono>

using namespace std;

#include <iostream>
#include <fstream>
// extern ofstream MyFilew;
double t_oldw = -1.;

#include "network/Com_Serial.h"

Serial_Port::Serial_Port(std::string uart_name, int baudrate)
{
	initialize_defaults();
	Serial_Port::uart_name = uart_name;
	Serial_Port::baudrate = baudrate;
}

Serial_Port::Serial_Port()
{
	initialize_defaults();
}

Serial_Port::~Serial_Port()
{
	pthread_mutex_destroy(&lock);
}

void Serial_Port::initialize_defaults()
{
	// Initialize attributes
	debug = false;
	fd = -1;
	status = SERIAL_PORT_CLOSED;

	uart_name = (char *)"/dev/ttyUSB0";
	baudrate = 57600;

	// Start mutex
	int result = pthread_mutex_init(&lock, NULL);
	if (result != 0)
	{
		printf("\n mutex init failed\n");
		throw 1;
	}
}

/**
 * Read from Serial
 */
int Serial_Port::read_message(mavlink_message_t &message)
{
	uint8_t cp;
	mavlink_status_t status;
	uint8_t msgReceived = false;

	// --------------------------------------------------------------------------
	//   READ FROM PORT
	// --------------------------------------------------------------------------

	// this function locks the port during read
	int result = _read_port(cp);

	// --------------------------------------------------------------------------
	//   PARSE MESSAGE
	// --------------------------------------------------------------------------
	if (result > 0)
	{

		msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);

		// check for dropped packets
		if ((lastStatus.packet_rx_drop_count != status.packet_rx_drop_count) && debug)
		{
			printf("ERROR: DROPPED %d PACKETS\n", status.packet_rx_drop_count);
			unsigned char v = cp;
			fprintf(stderr, "%02x ", v);
		}
		lastStatus = status;
	}
	else // Couldn't read from port
	{
		//Display_IHM::getInstanceOf().printLog("ERROR: Could'nt read serial");
	}

	// --------------------------------------------------------------------------
	//   DEBUGGING REPORTS
	// --------------------------------------------------------------------------
	if (msgReceived && debug)
	{
		// Report info
		printf("Received message from serial with ID #%d (sys:%d|comp:%d):", message.msgid, message.sysid, message.compid);

		fprintf(stderr, "Received serial data: ");
		unsigned int i;
		uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

		// check message is write length
		unsigned int messageLength = mavlink_msg_to_send_buffer(buffer, &message);

		// message length error
		if (messageLength > MAVLINK_MAX_PACKET_LEN)
		{
			fprintf(stderr, "\nFATAL ERROR: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE\n");
		}

		// print out the buffer
		else
		{
			for (i = 0; i < messageLength; i++)
			{
				unsigned char v = buffer[i];
				fprintf(stderr, "%02x ", v);
			}
			fprintf(stderr, "\n");
		}
	}

	// Done!
	return msgReceived;
}

/**
 * Write to Serial
 */
int Serial_Port::write_message(const mavlink_message_t &message)
{
	char buf[300];

	// Translate message to buffer
	unsigned len = mavlink_msg_to_send_buffer((uint8_t *)buf, &message);
	//  cout<<__FUNCTION__<<" after mavlink_msg_to_send_buffer"<<endl;

	// Write buffer to serial port, locks port while writing
	int bytesWritten = _write_port(buf, len);
	//   cout<<__FUNCTION__<<" writting done"<<endl;

	return bytesWritten;
}

/**
 * Open Serial Port
 */
int Serial_Port::open_serial()
{
	fd = _open_port(uart_name.c_str());

	// Check success
	if (fd == -1)
	{
		//Display_IHM::getInstanceOf().printLog("failure, could not open port");
		return 1;
	}

	bool success = _setup_port(baudrate);

	if (!success)
	{
		//Display_IHM::getInstanceOf().printLog("failure, could not configure port");
		return 1;
	}
	if (fd <= 0)
	{
		//Display_IHM::getInstanceOf().printLog("Connection failed, exiting.\n");
		return 1;
	}

	lastStatus.packet_rx_drop_count = 0;

	status = true;
	return 0;
}

/**
 * Close Serial Port
 */
void Serial_Port::close_serial()
{
	//Display_IHM::getInstanceOf().printLog("CLOSE PORT\n");

	int result = close(fd);
	if (result)
	{
		fprintf(stderr, "WARNING: Error on port close (%i)\n", result);
	}

	status = false;
}

/**
 * Quit Handler
 */
void Serial_Port::handle_quit()
{
	try
	{
		close_serial();
	}
	catch (int error)
	{
		fprintf(stderr, "Warning, could not stop serial port\n");
	}
}

/**
 * Helper Function - Open Serial Port File Descriptor
 */
int Serial_Port::_open_port(const char *port)
{
	// Open serial port
	// O_RDWR - Read and write
	// O_NOCTTY - Ignore special chars like CTRL-C
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

	// Check for Errors
	if (fd == -1)
	{
		/* Could not open the port. */
		return (-1);
	}

	// Finalize
	else
	{
		fcntl(fd, F_SETFL, 0);
	}

	// Done!
	return fd;
}

/**
 * Helper Function - Setup Serial Port
 */
bool Serial_Port::_setup_port(int baud)
{
	// Check file descriptor
	if (!isatty(fd))
	{
		fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
		return false;
	}

	// Read file descritor configuration
	struct termios config;
	if (tcgetattr(fd, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
		return false;
	}

	// Input flags - Turn off input processing
	// convert break to null byte, no CR to NL translation,
	// no NL to CR translation, don't mark parity errors or breaks
	// no input parity check, don't strip high bit off,
	// no XON/XOFF software flow control
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

	// Output flags - Turn off output processing
	// no CR to NL translation, no NL to CR-NL translation,
	// no NL to CR translation, no column 0 CR suppression,
	// no Ctrl-D suppression, no fill characters, no case mapping,
	// no local output processing
	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);

#ifdef OLCUC
	config.c_oflag &= ~OLCUC;
#endif

#ifdef ONOEOT
	config.c_oflag &= ~ONOEOT;
#endif

	// No line processing:
	// echo off, echo newline off, canonical mode off,
	// extended input processing off, signal chars off
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

	// Turn off character processing
	// clear current char size mask, no parity checking,
	// no output processing, force 8 bit input
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;

	// One input byte is enough to return from read()
	// Inter-character timer off
	config.c_cc[VMIN] = 1;
	config.c_cc[VTIME] = 10; // was 0

	// Get the current options for the port

	// Apply baudrate
	switch (baud)
	{
	case 1200:
		if (cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 1800:
		cfsetispeed(&config, B1800);
		cfsetospeed(&config, B1800);
		break;
	case 9600:
		cfsetispeed(&config, B9600);
		cfsetospeed(&config, B9600);
		break;

	case 19200:
		cfsetispeed(&config, B19200);
		cfsetospeed(&config, B19200);
		break;
	case 38400:
		if (cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 57600:
		if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 115200:
		if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;

	// These two non-standard (by the 70'ties ) rates are fully supported on
	// current Debian and Mac OS versions (tested since 2010).
	case 460800:
		if (cfsetispeed(&config, B460800) < 0 || cfsetospeed(&config, B460800) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 921600:
		if (cfsetispeed(&config, B921600) < 0 || cfsetospeed(&config, B921600) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	default:
		fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
		return false;

		break;
	}

	// Finally, apply the configuration
	if (tcsetattr(fd, TCSAFLUSH, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
		return false;
	}

	// Done!
	return true;
}

/**
 * Read Port with Lock
 */
int Serial_Port::_read_port(uint8_t &cp)
{
	// Lock
	pthread_mutex_lock(&lock);
	int result = read(fd, &cp, 1);

	// Unlock
	pthread_mutex_unlock(&lock);

	return result;
}

/**
 *  Write Port with Lock
 */
int Serial_Port::_write_port(char *buf, unsigned len)
{
	struct timeval tw;
	double t_currentw, dtw;
	gettimeofday(&tw, NULL);
	t_currentw = tw.tv_sec + tw.tv_usec * 1e-6;

	// Lock
	pthread_mutex_lock(&lock);

	// Write packet via serial link
	const int bytesWritten = static_cast<int>(write(fd, buf, len));

	using namespace std::chrono;

	// Use auto keyword to avoid typing long
	// type definitions to get the timepoint
	// at this instant use function now()

	auto start = high_resolution_clock::now();
	tcdrain(fd);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	// cout << "Time taken by function: " << duration.count() << " microseconds" << endl;

	pthread_mutex_unlock(&lock);

	return bytesWritten;
}
