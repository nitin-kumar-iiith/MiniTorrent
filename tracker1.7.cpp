//tracker
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
#include <string>
#include <vector>
//From linux based file transfer for open files
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctime>

//for sleep
#include <unistd.h>

unsigned int microseconds = 1000;

using namespace std;
#define MaxPendingConnection 100
// #define TRACKER1PORTNUM 8787
// #define TRACKER2PORTNUM 8788
int TRACKER1PORTNUM; //1 means port number of self
int MeraPortNumber;
int TRACKER2PORTNUM; //2 means port number of other tracker
int UskaPortNumber;
#define MESSAGELEN 1024
#define deb(x) cout << #x << " : " << x << endl;
// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;
int serverSocket;
void exit()
{
    close(serverSocket);
}
void init()
{
    atexit(exit);
}
// Reader Function
void *connectToOtherThread(void *param)
{
    string str = *reinterpret_cast<string *>(param);
    ;
}

// Writer Function
void *writer(void *param)
{
    ;
}

// Driver Code
int main(int argc, char **argv)
{
    // Initialize variables
    init();
    int choice = atoi(argv[2]);
    if (choice == 1)
    {
        TRACKER1PORTNUM = 8787;
        TRACKER1PORTNUM = 8788;
        MeraPortNumber = 8787;
        UskaPortNumber = 8788;
    }
    else if (choice == 2)
    {
        TRACKER1PORTNUM = 8788;
        TRACKER1PORTNUM = 8787;
        MeraPortNumber = 8788;
        UskaPortNumber = 8787;
    }
    string TrackerInfofilename = argv[1];
    string command;
    cin >> command;
    if (command == "quit")
    {
        //inform other Tracker, Exiting/////////////////////
        UskaPortNumber;
    }
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UskaPortNumber);

    // Bind the socket to the address and port number.
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (listen(serverSocket, MaxPendingConnection) == 0)
    {
        printf("Listening\n");
    }
    else
    {
        printf("Error\n");
    }

    // Array for thread
    pthread_t tid[60];

    int i = 0;
    while (1)
    {
        addr_size = sizeof(serverStorage);

        //string fileName;
        //const char *fileName = (char *)malloc(100);

        //fileName = "IAmOtherTrackerJustStarted";
        //pthread_create(&tid, NULL, connectToOtherThread, &fileName);

        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
        int choice = 0;
        //string choice;
        char choices;
        deb(serverStorage.__ss_align);
        deb(serverStorage.__ss_padding);
        deb(serverStorage.ss_family);
        deb(addr_size);
        //sent one test messages
        string client_message = "abcAbCsbdoclsnfdcklsjdnckvh bsdlcv kl";
        //client_message.resize(MESSAGELEN, ' ');
        write(newSocket, client_message.c_str(), MESSAGELEN);

        char buffer[MESSAGELEN];
        bzero(buffer, MESSAGELEN);
        int n = read(newSocket, buffer, MESSAGELEN);
        if (n < 0)
        {
            cout << "ERROR reading from socket";
            exit(0);
        }

        printf("Here is the message: %s\n", buffer);

        //usleep(microseconds);
        char b[1024];
        int fin, fout, nread;
        fin = open(buffer, O_RDONLY);
        if (fin != 0)
        {
            cout << "File Open Failed";
        }
        while ((nread = read(fin, b, sizeof(b))) > 0)
        {
            //write(fout, b, nread);
            write(newSocket, b, MESSAGELEN); //change write length here
        }
        string eofSignal = "NULLS";
        write(newSocket, eofSignal.c_str(), MESSAGELEN);
        //send(network_socket, &client_request, sizeof(client_request), 0);
        //write(client_sock , client_message , strlen(client_message));
        /*char b[1024];
        int fin, fout, nread;
        fin = open(fname.c_str(), O_RDONLY);
        fout = open((path).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        while ((nread = read(fin, b, sizeof(b))) > 0)
        {
            write(fout, b, nread);
        }
    */
        //return 0;
        if (choice == 1)
        {
            // Creater readers thread
            if (pthread_create(&readerthreads[i++], NULL, connectToOtherThread, &newSocket) != 0)

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
