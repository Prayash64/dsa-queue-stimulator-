 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define PIPE_NAME "\\\\.\\pipe\\VehicleQueue"
#define MAX_TEXT 100

// Generate a random vehicle number
void generateVehicleNumber(char* buffer) {
    buffer[0] = 'A' + rand() % 26;
    buffer[1] = 'A' + rand() % 26;
    buffer[2] = '0' + rand() % 10;
    buffer[3] = 'A' + rand() % 26;
    buffer[4] = 'A' + rand() % 26;
    buffer[5] = '0' + rand() % 10;
    buffer[6] = '0' + rand() % 10;
    buffer[7] = '0' + rand() % 10;
    buffer[8] = '\0';
}

// Generate a random lane
char generateLane() {
    char lanes[] = {'A', 'B', 'C', 'D'};
    return lanes[rand() % 4];
}

int main() {
    srand((unsigned int)time(NULL));
    char vehicle[9];
    char message[MAX_TEXT];

    // Create named pipe
    HANDLE hPipe = CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,           // max instances
        MAX_TEXT,    // out buffer size
        MAX_TEXT,    // in buffer size
        0,           // default timeout
        NULL         // default security
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Failed to create pipe. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("Waiting for client to connect to the pipe...\n");
    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected) {
        printf("Failed to connect to pipe. Error: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    printf("Client connected! Sending vehicle data...\n");

    while (1) {
        generateVehicleNumber(vehicle);
        char lane = generateLane();
        snprintf(message, MAX_TEXT, "%s:%c", vehicle, lane);

        DWORD bytesWritten;
        BOOL success = WriteFile(hPipe, message, (DWORD)strlen(message) + 1, &bytesWritten, NULL);
        if (!success) {
            printf("Failed to write to pipe. Error: %lu\n", GetLastError());
            break;
        }

        printf("New vehicle added: %s\n", message);
        Sleep(1000); // 1 second delay
    }

    CloseHandle(hPipe);
    return 0;
}

