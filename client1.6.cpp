//send file name
//recieve one mesasge
//recieve file content
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

//for sleep
#include <unistd.h>

//for file handling
#include <fstream>

unsigned int microseconds = 1000;

using namespace std;
#define MaxPendingConnection 100
#define PORTNUM 8989
#define MESSAGELEN 1024
#define deb(x) cout << #x << " : " << x << endl
#define msg(x) cout << x << endl

/*void exit()
{
    close(serverSocket);
}
void init()
{
    atexit(exit);
}*/
// Function to send data to
// server socket.
void *FileDownload(void *ptr)
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
        //puts("Error\n");
        msg("Error");
        return 0;
    }
    else
    {
        //printf("Connection estabilished\n");
        msg("Connection estabilished");
    }
    deb(network_socket);
    deb(str);

    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN + 10];
    bzero(buffer, MESSAGELEN);
    //fgets(buffer, 255, stdin);
    //buffer = str.c_str();
    strcpy(buffer, str.c_str());
    int n = write(network_socket, buffer, strlen(buffer));
    if (n < 0)
    {
        //cout << "ERROR writing to socket" << endl;
        msg("ERROR writing to socket");
        exit(0);
    }
    //cout <<
    msg("before first read");
    char server_reply[MESSAGELEN + 10];
    //Receive a reply from the server
    if (recv(network_socket, server_reply, MESSAGELEN, 0) < 0)
    {
        puts("recv failed");
        msg("recv failed");
    }
    else
    {
        //cout << "Recieved msg";
        msg("Recieved msg");
        deb(server_reply);
    }
    //cout << "outside msg";
    msg("outside msg");
    string destn = "PQR.TXT";
    // Close the connection
    //int fin, fout, nread;
    //fout = open((destn).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    ofstream myfile;
    myfile.open("example.txt", ios::out);
    //myfile << "Writing this to a file.\n";
    //cout << "outside while true";
    msg("outside while true");
    while (true)
    {
        //sleep(3000);
        //cout << "Inside while true";
        msg("Inside while true");
        int len = recv(network_socket, server_reply, MESSAGELEN, 0);
        //cout << "after read";
        msg("after read");
        //if (recv(network_socket, server_reply, 2000, 0) < 0)
        if (len < 0)
        {
            //puts("recv failed");
            msg("recv failed");
            break;
        }
        else if (len == 0)
        {
            msg("Time to leave");
            //cout << "Time to leave";
            break;
        }
        else if (len > 0)
        {
            string resposne = server_reply;
            if (resposne == "NULLS")
            {
                break;
            }
            else
            {
                myfile << server_reply;
                //write(fout, server_reply, MESSAGELEN);
                deb(server_reply);
                //cout << "I am Here";
                msg("I am Here");
            }
        }
    }
    myfile.close();
    sleep(2000);
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
    pthread_create(&tid, NULL, FileDownload, &fileName);

    // Suspend execution of calling thread
    pthread_join(tid, NULL);
}
