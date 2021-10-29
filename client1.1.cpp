//send file name
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
using namespace std;
#define MaxPendingConnection 100
#define PORTNUM 8989
#define deb(x) cout << #x << " : " << x << endl;
// Function to send data to
// server socket.
void *SendFileName(void *ptr)
{
    string str = *reinterpret_cast<string *>(ptr);
    //deb(str);

    int network_socket;

    // Create a stream socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORTNUM);

    // Initiate a socket connection
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        puts("Error\n");
        return 0;
    }
    else
    {
        printf("Connection estabilished\n");
    }
    deb(network_socket);
    deb(str);

    // Send data to the socket
    send(network_socket, &str, sizeof(str), 0);
    char server_reply[20];
    //Receive a reply from the server
    if (recv(network_socket, server_reply, 2000, 0) < 0)
    {
        puts("recv failed");
    }
    else
    {
        deb(server_reply);
    }
    // Close the connection
    close(network_socket);
    pthread_exit(NULL);

    return NULL;
}
void *clienthread(void *args)
{

    int client_request = *((int *)args);
    int network_socket;

    // Create a stream socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORTNUM);

    // Initiate a socket connection
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        puts("Error\n");
        return 0;
    }
    else
    {
        printf("Connection estabilished\n");
    }
    cout << "network_socket:" << network_socket << endl;
    cout << "client_request:" << client_request << endl;
    // Send data to the socket
    send(network_socket, &client_request, sizeof(client_request), 0);
    char server_reply[20];
    //Receive a reply from the server
    if (recv(network_socket, server_reply, 2000, 0) < 0)
    {
        puts("recv failed");
    }
    else
    {
        deb(server_reply);
    }
    // Close the connection
    close(network_socket);
    pthread_exit(NULL);

    return 0;
}

// Driver Code
int main()
{
    //printf("1. Read\n");
    //printf("2. Write\n");

    // Input
    int choice;
    //scanf("%d", &choice);
    pthread_t tid;
    string fileName;
    //const char *fileName = (char *)malloc(100);

    fileName = "ABC.TXT";
    //pthread_create(&tid, NULL, SendFileName, &fileName);
    pthread_create(&tid, NULL, SendFileName, &fileName);
    // Create connection
    // depending on the input
    /*switch (choice)
    {
    case 1:
    {
        int client_request = 1;

        // Create thread
        pthread_create(&tid, NULL, clienthread, &client_request);
        sleep(20);
        break;
    }
    case 2:
    {
        int client_request = 2;

        // Create thread
        pthread_create(&tid, NULL, clienthread, &client_request);
        sleep(20);
        break;
    }
    default:
        printf("Invalid Input\n");
        break;
    }*/

    // Suspend execution of
    // calling thread
    pthread_join(tid, NULL);
}
