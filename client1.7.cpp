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
#define TRACKER1PORTNUM 8787
#define TRACKER2PORTNUM 8788
#define PORTNUM 8989
#define MESSAGELEN 1024
#define deb(x) cout << #x << " : " << x << endl
#define msg(x) cout << x << endl
int portofTracker1;
int portofTracker2;
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
void getTrackerDetails(int argc, char **argv)
{
    //IP Format: ./client <IP>:<PORT> tracker_info.txt
    if (argc < 3)
    {
        msg("Insuffiecient Amount of parameter given, exiting");
        exit(1);
    }
    fstream file;
    string word, t, q, filename;
    //string trackerFileName = "tracker_info.txt";
    string trackerFileName = argv[2];
    // opening file
    file.open(trackerFileName.c_str());
    if (file >> word)
    {
        portofTracker1 = stoi(word);
        deb(portofTracker1);
    }
    if (file >> word)
    {
        portofTracker2 = stoi(word);
        deb(portofTracker2);
    }
}
void getInput()
{

    string imput;
    cin >> imput;
    if (imput == "create_user")
    { //b. Create User Account: create_user <user_id> <passwd>
        ;
    }
    else if (imput == "login")
    { // c. Login: login <user_id> <passwd>
        ;
    }
    else if (imput == "create_group")
    { // d. Create Group: create_group <group_id>
        ;
    }
    else if (imput == "join_group")
    { // e. Join Group: join_group <group_id>
        ;
    }
    else if (imput == "leave_group")
    { // f. Leave Group: leave_group <group_id>
        ;
    }
    else if (imput == "list_requests")
    { // g. List pending join: requests list_requests <group_id>
        ;
    }
    else if (imput == "accept_request")
    { // h. Accept Group Joining Request: accept_request <group_id> <user_id>
        ;
    }
    else if (imput == "list_groups")
    { // i. List All Group In Network: list_groups
        ;
    }
    else if (imput == "list_files")
    { // j. List All sharable Files In Group: list_files <group_id>

        ;
    }
    else if (imput == "upload_file")
    { // k. Upload File: upload_file <file_path> <group_id>
        ;
    }
    else if (imput == "download_file")
    { // l. Download File: download_file <group_id> <file_name> <destination_path>

        ;
    }
    else if (imput == "logout")
    { // m. Logout: logout
        ;
    }
    else if (imput == "show_downloads")
    {
        // n. Show_downloads: show_downloads
        // Output format:
        // [D] [grp_id] filename
        // [C] [grp_id] filename
        // D(Downloading), C(Complete)
        ;
    }
    else if (imput == "stop_share")
    { // o. Stop sharing: stop_share <group_id> <file_name>
        ;
    }
}
// Driver Code
int main(int argc, char **argv)
{

    // a. ./client <IP>:<PORT> tracker_info.txt
    getTrackerDetails(argc, argv);
    //getInput();

    pthread_t tid;
    string fileName;
    int choice;
    //const char *fileName = (char *)malloc(100);
    cin >> choice;
    if (choice == 1)
    { //download file
        cin >> fileName;
        fileName = "ABC.TXT";
        //pthread_create(&tid, NULL, SendFileName, &fileName);
        pthread_create(&tid, NULL, FileDownload, &fileName);
    }

    // Suspend execution of calling thread
    pthread_join(tid, NULL);
}
