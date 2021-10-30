// C program for the Server Side

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

// client code
void *downloadFile(void *param)
{
    int choice = 1;
    cin >> choice;
    string whichfile = "ABC.txt";
    cin >> whichFile;

    int n = write(serverID, choice, strlen(choice));

    int n = write(serverID, whichFile, strlen(whichFile));
    if (n < 0)
        error("ERROR writing to socket");

    fout = open("ABC.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    while (true)
    {

        n = read(serverAddr, buffer, 255); //reading from server
        if (buffer == "NULLS")
        {
            break;
        }
        write(fout, buffer, 255); //stored in ABC.txt
    }

    if (n < 0)
        error("ERROR reading from socket");

    printf("%s\n", buffer);
}

//inside server
void *downloadFileFunction(void *param)
{

    whichFile == "abc.txt;";
    char buff[1024];
    int fin, fout, nread;

    fin = open(whichFile, O_RDONLY);

    while ((nread = read(fin, buff, sizeof(buff))) > 0)
    {
        write(clientAddr, buff, nread); //server is sending
    }
    string abc = "NULLS";

    write(clientAddr, abc, nread); //server is sending
    bzero(buffer, 256);

    printf("%s\n", buffer);
}
// Driver Code
int main()
{
    // Initialize variables
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    // Bind the socket to the
    // address and port number.
    bind(serverSocket,
         (struct sockaddr *)&serverAddr,
         sizeof(serverAddr));

    // Listen on the socket,
    // with 40 max connection
    // requests queued
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");

    // Array for thread
    pthread_t tid[60];

    int i = 0;

    while (1)
    {

        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);

        int choice = 0;
        read(newSocket, &choice, sizeof(choice));

        if (choice == 1)
        {
            // Creater readers thread
            string whichFile;
            read(newSocket, &whichFile, sizeof(whichFile));
            if (pthread_create(&readerthreads[i++], NULL, downloadFileFunction, &newSocket) != 0)

                // Error in creating thread
                printf("Failed to create thread\n");
        }

        if (i >= 50)
        {
            // Update i
            i = 0;

            while (i < 50)
            {
                // Suspend execution of
                // the calling thread
                // until the target
                // thread terminates
                pthread_join(writerthreads[i++],
                             NULL);
                pthread_join(readerthreads[i++],
                             NULL);
            }

            // Update i
            i = 0;
        }
    }

    return 0;
}
