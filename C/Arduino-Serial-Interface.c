#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

volatile int dX, dY;

int main()
{   
    char errorMessage[256], inputBuffer[20], byte; // byte and input buffer are the ones that take the serial input.
    HANDLE hSerial;                                // handle to the serial Com port

    DWORD numberOfBytesRead, eventMask;

    DCB serial = {0}; // setting for the  serial port
    serial.DCBlength = sizeof(serial);

    COMMTIMEOUTS time = {0};

    hSerial = CreateFile("COM6", // this might be different for you mine appeared at COM6
                         GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    // Just some basic error checking and exiting the program if an error has occured.
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, errorMessage, sizeof(errorMessage), NULL);
        printf("Error: %s", errorMessage);
        return 0;
    }
    else
    {
        printf("Port sucessfully opened\n");
    }

    // Setting the DCB settings
    BOOL getStatus = GetCommState(hSerial, &serial);
    if (getStatus) // again error checking
    {
        printf("Current: Baud Rate: %lu, ByteSize: %lu\n", serial.BaudRate, serial.ByteSize);
    }
    else
    {
        printf("Error: Failed to get the Comm State.\n");
        CloseHandle(hSerial);
        return 0;
    }

    // Serial Port settings for arduino
    serial.BaudRate = CBR_9600;
    serial.fParity = FALSE;
    serial.ByteSize = 8;
    serial.StopBits = ONESTOPBIT;

    BOOL setStatus = SetCommState(hSerial, &serial);
    if (setStatus)
    {
        printf("Sucessfully set port settings.\n");
    }
    else
    {
        printf("Error occured while implementing port settings.\n");
        CloseHandle(hSerial);
        return 0;
    }

    // clearing the RX(receving buffer) so if there is any value present in it, it doesn't corrupt the incoming data
    PurgeComm(hSerial, PURGE_RXCLEAR);

    time.ReadIntervalTimeout = 20;
    time.ReadTotalTimeoutConstant = 50;
    time.ReadTotalTimeoutMultiplier = 1;
    SetCommTimeouts(hSerial, &time);

    SetCommMask(hSerial, EV_RXCHAR);

    BOOL condition = WaitCommEvent(hSerial, &eventMask, NULL);
    int lineIndex = 0;
    while (condition)
    {
        BOOL readStatus;
        if (eventMask & EV_RXCHAR)
        {
            readStatus = ReadFile(hSerial, &byte, 1, &numberOfBytesRead, NULL);
            // reading indiviual bytes of the incoming data since it easy to parse this way
        }

        if (readStatus && numberOfBytesRead > 0) // checking if there are any bytes read and the com port was read successfully
        {
            /* Data sent by arduino by Serial.println() is in the following form "blahblah\r\n" so when we encounter the \n
            we can know that the data that we have encountered the end of the line*/
            if (byte == '\n')
            {
                inputBuffer[lineIndex] = '\0';
                // printf("%s\n", inputBuffer);
                char X[sizeof(inputBuffer)], Y[sizeof(inputBuffer)];

                /*Additional note about strncpy you need to leave an additional space for string termination so that is why
                5 is used instead of 4 in the size*/
                // setting the values of X
                strncpy(X, inputBuffer, 5);
                dX = atoi(X);

                // setting the values of Y
                strncpy(Y, inputBuffer + 5, 5);
                dY = atoi(Y);

                printf("X: %d, Y: %d\n", dX, dY);

                lineIndex = 0;
            }
            // disregard \r nobody loves it (jk)
            if (byte != '\r')
            {
                inputBuffer[lineIndex] = byte;
                lineIndex++;
                // no one is buffer overflowing my scripts
                if (lineIndex > (sizeof(inputBuffer) - 1))
                {
                    // exiting if such an event is detected
                    printf("Buffer overflow detected!");
                    return 0;
                }
            }
        }

        if (!readStatus)
        {
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, errorMessage, sizeof(errorMessage), NULL);
            printf("Error: 0x%x, %s", GetLastError(), errorMessage);
            CloseHandle(hSerial);
            return -1;
        }
    }

    printf("Closing handle");
    CloseHandle(hSerial);
    return 0;
}
