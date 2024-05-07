#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <vector>
#include <string>

#define SHARED_FILE "shared_results.txt"
#define PORT 27015
#define BUFLEN 512
#define MAX_CLIENTS 10

typedef struct 
{
    SOCKET socket;
    int id;
} ClientThreadArgs;

CRITICAL_SECTION criticalSection;

//std::vector<SOCKET> connectedClients;

void clearFile(){
    FILE *file = fopen(SHARED_FILE, "w");

    if (file == NULL) 
    {
        printf("Error opening file\n");
        return;
    }
    fclose(file);
}

void saveToFile(int clientID, int result){
    EnterCriticalSection(&criticalSection);

    FILE *file = fopen(SHARED_FILE, "a");
    if (file == NULL) 
    {
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "Client ID: %d, Result: %d\n", clientID, result);
    fclose(file);

    LeaveCriticalSection(&criticalSection);
}

std::vector<int> findPrimeNumbers(int begin, int end, int clientID) {
    std::vector<int> primes;
    for (int num = begin; num <= end; num++) {
        bool isPrime = true;
        for (int i = 2; i*i <= num; i++) {
            if (num % i == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime && num > 1) {
            primes.push_back(num);
        }
        saveToFile(clientID, num);
        Sleep(100);
    }
    return primes;
}

std::string convertToString(const std::vector<int>& primes) {
    std::string primeStr;
    for (size_t i = 0; i < primes.size(); i++) {
        primeStr += std::to_string(primes[i]) + " ";
    }
    return primeStr;
}

void handleClient(void* args){
    ClientThreadArgs *clientArgs = (ClientThreadArgs *)args;
    SOCKET clientSocket = clientArgs->socket;
    int clientID = clientArgs->id;

    char recBuffer[BUFLEN];
    int recBufferlen = BUFLEN;
    int iResult;

    printf("Client %d connected.\n", clientID);
    
    while(true){
        iResult = recv(clientSocket, recBuffer, recBufferlen, 0);
        if (iResult > 0) {
            recBuffer[iResult] = '\0';
            
            if(strcmp(recBuffer, "2") == 0) {
                printf("Received from client %d signal to close the server.\n", clientID);
                printf("Closing server...\n");

                //for (SOCKET client : connectedClients) {
                //    if (client != clientSocket) {
                //        send(client, "2", strlen("2"), 0);
                //    }
                //}   

                closesocket(clientSocket);
                WSACleanup();
                DeleteCriticalSection(&criticalSection);
                _endthread();
            }
            else{
                printf("Received from client %d: %s\n", clientID, recBuffer);
                printf("Server has started calculations.\n");

                int begin, end;
                sscanf(recBuffer, "%d %d", &begin, &end);
                    
                std::vector<int> primeNumbers = findPrimeNumbers(begin, end, clientID);

                //sending back numbers
                std::string primeStr = convertToString(primeNumbers);

                iResult = send(clientSocket, primeStr.c_str(), primeStr.length(), 0);

                if (iResult == SOCKET_ERROR) {
                    printf("send failed: %d\n", WSAGetLastError());
                    break;
                }

                printf("Success!\n");
            }
        }
        else if (iResult == 0) {
            printf("Client %d: Connection closing...\n", clientID);
            break;
        }
        else {
            printf("Client %d: recv failed: %d\n", clientID, WSAGetLastError());
            break;
        }
    }

    closesocket(clientSocket);
    printf("Client disconnected.\n");
    _endthread();
}

int main(int argc, char *argv[]) {
    //1. Initialize Winsock.

    WSADATA wsaData;
    SOCKET ServerSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr, clientAddr;

    int clientAddrLen = sizeof(clientAddr);
    int clientID = 0;

    clearFile();

    InitializeCriticalSection(&criticalSection);

    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    //2. Create a socket.
    
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(ServerSocket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // 3. Binding a socket

    iResult = bind(ServerSocket, (struct sockaddr *)&serverAddr, (int) sizeof(serverAddr));

    if(iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    // 4. Listening on a socket

    if(listen(ServerSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf( "Listen failed with error: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    printf("Server initialized. Waiting for connections on port %d...\n", PORT);

    //5. Accepting a connection
    
    while(true){
        ClientSocket = accept(ServerSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

        if(ClientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(ServerSocket);
            WSACleanup();
            return 1;
        }

        //6. Receive and send data
        ClientThreadArgs *args = (ClientThreadArgs *)malloc(sizeof(ClientThreadArgs));
        args->socket = ClientSocket;
        args->id = clientID;
        //connectedClients.push_back(ClientSocket);
        _beginthread(handleClient, 0, (void *)args);
        
        clientID++;
    }

    //7. Disconnect server

    iResult = shutdown(ClientSocket, SD_SEND);
    if(iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}