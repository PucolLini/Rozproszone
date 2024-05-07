#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#define PORT 27015
#define BUFLEN 512
#define SERVER_ADDRESS "127.0.0.1"

void SendMessageToServer(SOCKET ClientSocket, const char *message) 
{
    send(ClientSocket, message, strlen(message), 0);
}

void ReceiveMessagesFromServer(SOCKET ClientSocket) 
{
    char buffer[80];
    int bytes_received;

    bytes_received = recv(ClientSocket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        //if (strcmp(buffer, "2") == 0) {
        //    printf("Server is shutting down. Disconnecting...\n");
        //    closesocket(ClientSocket);
        //    WSACleanup();
        //    exit(0);
        //} else {
            printf("Received from server: %s\n", buffer);
        //}
    } 
    else if (bytes_received == 0) {
        printf("Server disconnected\n");
    } 
    else {
        printf("Error in receiving message from server\n");
    }
}

int main(int argc, char *argv[]) {
    //1. Initialize Winsock.

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    char buffer[BUFLEN];

    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    //2. Create a socket.

    ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //3. Connect to the server.

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serverAddr.sin_port = htons(PORT);

    iResult = connect(ConnectSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    printf("Connected to the server.\n");

    //4. Send and receive data.
    int choice;

    while(true){
        printf("Menu:\n");
        printf("1. Find prime numbers.\n");
        printf("2. Shutdown server.\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if(choice == 2) {
            strcpy(buffer, "2");
            send(ConnectSocket, buffer, strlen(buffer), 0);

            printf("Shutting down server...\n");
            break;
        }
        else if (choice == 1){
            int start, end;
            printf("Enter the range - start & end - for prime number calculation: ");
            scanf("%d %d", &start, &end);

            sprintf(buffer, "%d %d", start, end);
            iResult = send(ConnectSocket, buffer, (int) strlen(buffer), 0);
                
            if (iResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            ReceiveMessagesFromServer(ConnectSocket);
        }
        else {
            printf("Invalid choice.\n");
        }
    }

    //5. Disconnect.

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}