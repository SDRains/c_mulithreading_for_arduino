#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

/********************************************/
/*********** Function Declarations **********/
/********************************************/
void port_main(void);						// The main function for port communication
HANDLE openComPort(void);						// Attempts to open com port
void setStandards(HANDLE hComm);			// Set comm initializers (baud rate, ByteSize, etc.)
void setTimeouts();							// Set how long each timeout is initialized to
void writeToPort(HANDLE hComm);				// Write data to the com port opened by openComPort()

void port_main() {
	char KeyInfo[10] = "";
	HANDLE hComm; // Create handle to serial port

	printf("\nYOU ARE NOW INSIDE PORT COMMUNICATION\n");
	printf("Enter 'open' to open the port.\n");
	printf("After you have opened the port, enter 'write' to start communicating to the Arduino.\n");

	hComm = openComPort();

	do {
		scanf_s("%s", KeyInfo, (unsigned)_countof(KeyInfo));

		if ((strcmp(KeyInfo, "write")) == 0) {
			printf("Writing data port...\n");
			writeToPort(hComm);
		}
	} while ((strcmp(KeyInfo, "quit")) != 0);

	printf("Closing port and thread\n");
	CloseHandle(hComm);		// Closing port
	CloseThread();			// Closing thread
}

HANDLE openComPort() {
	HANDLE hComm; // Create handle to serial port

	hComm = CreateFile("COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	// Port name, read/write, no sharing, no security, open existing port, non-overlapped I/O, Null for comm devices
	// For COM devices between 1-9, no "\\\\.\\" is needed

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("Error in opening serial port\n");
	}
	else {
		printf("Opened serial port successfully!\n");
	}

	setStandards(hComm);
	return(hComm);
}

void setStandards(HANDLE hComm) {
	int Status;
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	FILE *fp;

	printf("Setting standards for Com Port...\n");

	Status = GetCommState(hComm, &dcbSerialParams);

	dcbSerialParams.BaudRate = CBR_9600;	// Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;			// Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;	// Setting StopBits = 0
	dcbSerialParams.Parity = NOPARITY;		// Setting Parity = None

	SetCommState(hComm, &dcbSerialParams);

	Sleep(1000);
	printf("Standards set successfully!\n");

	fp = fopen("\ComStat.txt", "w");
	fprintf(fp, "BaudRate : %d\nByteSize : %d\nStopBits : %d\nParity : %d", dcbSerialParams.BaudRate, dcbSerialParams.ByteSize, dcbSerialParams.StopBits, dcbSerialParams.Parity);
	fclose(fp);

	printf("Logged to ComStat.txt\n");
	setTimeouts();
}

void setTimeouts() {
	COMMTIMEOUTS timeouts = { 0 };

	printf("Setting timeouts for Com Port...\n");
	// All in milliseconds
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 50;
}

void writeToPort(HANDLE hComm) {
	char lpBuffer[] = "A";
	DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
	DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port
	dNoOFBytestoWrite = sizeof(lpBuffer);

	WriteFile(hComm,        // Handle to the Serial port
		lpBuffer,     // Data to be written to the port
		dNoOFBytestoWrite,  //No of bytes to write
		&dNoOfBytesWritten, //Bytes written
		NULL);
}