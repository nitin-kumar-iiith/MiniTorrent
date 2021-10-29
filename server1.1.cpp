//send File Content

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
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
using namespace std;
#define MaxPendingConnection 100
#define PORTNUM 8989
#define deb(x) cout << #x << " : " << x << endl;
// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

// Reader Function
void *reader(void *param)
{
    // Lock the semaphore
    sem_wait(&x);
    readercount++;

    if (readercount == 1)
        sem_wait(&y);

    // Unlock the semaphore
    sem_post(&x);

    printf("\n%d reader is inside", readercount);

    sleep(5);

    // Lock the semaphore
    sem_wait(&x);
    readercount--;

    if (readercount == 0)
    {
        sem_post(&y);
    }

    // Lock the semaphore
    sem_post(&x);

    printf("\n%d Reader is leaving", readercount + 1);
    pthread_exit(NULL);
}

// Writer Function
void *writer(void *param)
{
    printf("\nWriter is trying to enter");

    // Lock the semaphore
    sem_wait(&y);

    printf("\nWriter has entered");

    // Unlock the semaphore
    sem_post(&y);

    printf("\nWriter is leaving");
    pthread_exit(NULL);
}

// Driver Code
int main()
{
    // Initialize variables
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORTNUM);

    // Bind the socket to the
    // address and port number.
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Listen on the socket,
    // with 40 max connection
    // requests queued
    //if (listen(serverSocket, 50) == 0)
    if (listen(serverSocket, MaxPendingConnection) == 0)
        printf("Listening\n");
    else
        printf("Error\n");

    // Array for thread
    pthread_t tid[60];

    int i = 0;

    while (1)
    {
        addr_size = sizeof(serverStorage);

        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
        int choice = 0;
        //string choice;
        char choices;
        //cout << serverStorage.ss_family << endl;
        //cout << serverStorage.__ss_padding << endl;
        //cout << serverStorage.__ss_align << endl;
        //cout << addr_size << endl;
        deb(serverStorage.__ss_align);
        deb(serverStorage.__ss_padding);
        deb(serverStorage.ss_family);
        deb(addr_size);

        /*recv(newSocket, &choices, 1000, 0);
        deb(newSocket);
        deb(choices);
        string str = *reinterpret_cast<string *>(choices);
        deb(str);*/
        //giving segment fault

        ///////////////////
        ////////////////////
        ///////////////////
        /////////////////
        // create the buffer with space for the data
        const unsigned int MAX_BUF_LENGTH = 4096;
        vector<char> buffer(MAX_BUF_LENGTH);
        string rcv;
        rcv.clear();
        int bytesReceived = 0;
        do
        {
            bytesReceived = recv(newSocket, &buffer[0], buffer.size(), 0);
            // append string from buffer.
            if (bytesReceived == -1)
            {
                // error
            }
            else
            {
                //rcv.append(buffer.cbegin(), buffer.cend());
                rcv.append(buffer.cbegin(), buffer.cend());
            }
        } while (bytesReceived == MAX_BUF_LENGTH);
        // At this point we have the available data (which may not be a complete
        // application level message).
        deb(rcv);
        ///////////////////
        ////////////////////
        ///////////////////
        /////////////////

        //send(network_socket, &client_request, sizeof(client_request), 0);
        //write(client_sock , client_message , strlen(client_message));
        string client_message = "abc";
        write(newSocket, client_message.c_str(), client_message.length());
        /*char b[1024];
        int fin, fout, nread;
        fin = open(fname.c_str(), O_RDONLY);
        fout = open((path).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        while ((nread = read(fin, b, sizeof(b))) > 0)
        {
            write(fout, b, nread);
        }
    */
        if (choice == 1)
        {
            // Creater readers thread
            if (pthread_create(&readerthreads[i++], NULL, reader, &newSocket) != 0)

                // Error in creating thread
                printf("Failed to create thread\n");
        }
        else if (choice == 2)
        {
            // Create writers thread
            if (pthread_create(&writerthreads[i++], NULL, writer, &newSocket) != 0)

                // Error in creating thread
                printf("Failed to create thread\n");
        }

        //if (i >= 50)//original
        if (i >= MaxPendingConnection) //don't know why i added this
        {
            // Update i
            i = 0;

            //while (i < 50)//original
            while (i < MaxPendingConnection) //don't know why i added this
            {
                // Suspend execution of
                // the calling thread
                // until the target
                // thread terminates
                pthread_join(writerthreads[i++], NULL);
                pthread_join(readerthreads[i++], NULL);
            }

            // Update i
            i = 0;
        }
    }

    return 0;
}
