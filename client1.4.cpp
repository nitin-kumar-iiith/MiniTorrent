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

using namespace std;
#define MaxPendingConnection 100
#define PORTNUM 8989
#define deb(x) cout << #x << " : " << x << endl;
// Function to send data to
// server socket.
void *SendFileName(void *ptr)
{
    string str = *reinterpret_cast<string *>(ptr);
    deb(str);

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
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[256];
    bzero(buffer, 256);
    //fgets(buffer, 255, stdin);
    //buffer = str.c_str();
    strcpy(buffer, str.c_str());
    int n = write(network_socket, buffer, strlen(buffer));
    if (n < 0)
    {
        cout << "ERROR writing to socket" << endl;
        exit(0);
    }

    char server_reply[200];
    //Receive a reply from the server
    if (recv(network_socket, server_reply, 2000, 0) < 0)
    {
        puts("recv failed");
    }
    else
    {
        deb(server_reply);
    }
    string destn = "PQR.TXT";
    // Close the connection
    int fin, fout, nread;
    fout = open((destn).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //ofstream myfile;
    //myfile.open("example.txt");
    //myfile << "Writing this to a file.\n";

    while (true)
    {
        //int len = recv(network_socket, server_reply, 2000, 0);
        if (recv(network_socket, server_reply, 2000, 0) < 0)
        {
            puts("recv failed");
            break;
        }
        /*else if (len == 0)
        {
            break;
        }*/
        else
        {
            //myfile << server_reply;
            write(fout, server_reply, 200);
            deb(server_reply);
            cout << "I am Here";
        }
    }
    //myfile.close();
    close(network_socket);
    pthread_exit(NULL);
    return NULL;
}

// Driver Code
int main()
{
    pthread_t tid;
    string fileName;
    //const char *fileName = (char *)malloc(100);

    fileName = "ABC.TXT";
    //pthread_create(&tid, NULL, SendFileName, &fileName);
    pthread_create(&tid, NULL, SendFileName, &fileName);

    // Suspend execution of calling thread
    pthread_join(tid, NULL);
}
