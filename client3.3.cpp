//creating file chunks & SHA
//Pending check are we able to dowanload
//512KB of data (524288B)
//untested void *leaveGroup(void *ptr)
//line 1149
//worked of uplaod file
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
//for SHA
#include <openssl/sha.h>

//for random Number
#include <time.h>
unsigned int microseconds = 1000;
int user_id; //to keep track of who am i
using namespace std;
#define MaxPendingConnection 100
#define TRACKER1PORTNUM 8787
#define TRACKER2PORTNUM 8788
#define PORTNUM 8989
#define MESSAGELEN 524288
#define deb(x) cout << #x << " : " << x << endl;
#define msg(x) cout << x << endl;
void get_Downloaded_list();
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;
int serverSocket;
unsigned int mySendingPortAddr;
vector<string> sha1ofFile;
vector<string> fileContentsToCalculateShaOfEntireFileAtOnce;
vector<string> sha1ofFilePieceWise;
unordered_map<string, string> mapGroupIDactive_Download; //whenevr downlaod starts add the file name here
//& when download completes remove it from here and keep it in completed_Download
//whenever download completes "save it in file "$user_id.downlaodedFile.dat"
unordered_map<string, string> mapGroupIDcompleted_Download;
struct downloadedFiles
{
    char SourceGroupId[32];
    char DownloadedFileName[64];
};
void exit()
{
    close(serverSocket);
}
void init()
{
    atexit(exit);
}

void *reader(void *param)
{
    return NULL;
}

void *writer(void *param)
{
    return NULL;
}

bool loginSucess = false;
int portofTracker1;
int portofTracker2;
unordered_set<string> mygroups;
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
void *FileToDownload(void *ptr)
{
    string str = *reinterpret_cast<string *>(ptr);
    msg("File Name Recieved For Downloading:");
    msg(str);
    //deb(str);
    int network_socket;
    // Create a stream socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORTNUM); //recieve from this person
    // Initiate a socket connection
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        //puts("Error\n");
        msg("Error: connect system call Failed");
        return 0;
    }
    else
    {
        //printf("Connection estabilished\n");
        msg("Connection estabilished");
    }
    deb(network_socket);
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    //tell other peer which file to download
    strcpy(buffer, str.c_str());
    int n = write(network_socket, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR: writing to socket failed");
        exit(0);
    }
    //msg("Check Point:before first read");
    char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //if (recv(network_socket, server_reply, MESSAGELEN, 0) < 0)
    //{
    //msg("Recv failed");
    //}
    //else
    //{
    //msg("Recieved msg");
    //deb(server_reply);
    //}
    //cout << "outside msg";
    msg("Check Point:outside msg");
    string destn = "PQR.TXT";
    str += "sdfgv";
    // Close the connection
    //int fin, fout, nread;
    //fout = open((destn).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    ofstream myfile;
    myfile.open(str.c_str(), ios::out);
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
                //deb(server_reply);
                //cout << "I am Here";
                //msg("I am Here");
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
struct Create_User
{
    int user;
    string passwd;
};
void *createUser(void *ptr)
{ //user may already exist
    //cout << "aaya mai idhar aaya re";
    string str = *reinterpret_cast<string *>(ptr);
    deb(str);
    string word = "";
    string usedId, password;
    for (auto x : str)
    {
        if (x == ' ')
        {
            cout << word << endl;
            usedId = word;
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    password = word;
    cout << word << endl;
    ////////////////////////////////////////////////////
    int network_socketForCreateUser;

    // Create a stream socket
    network_socketForCreateUser = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number

    // Initiate a socket connection
    int connection_status = connect(network_socketForCreateUser, (struct sockaddr *)&server_address, sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        //msg("Connection estabilished");
    }
    deb(network_socketForCreateUser);
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN + 10];
    bzero(buffer, MESSAGELEN);
    string commands = "create_user";
    strcpy(buffer, commands.c_str());
    int n = write(network_socketForCreateUser, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing CreateUser command to socket");
        exit(0);
    }
    //msg("before first read");
    char server_reply[MESSAGELEN + 10];
    //Receive a reply from the server
    string reply = server_reply;
    if (recv(network_socketForCreateUser, server_reply, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        //msg("Recieved msg");
        //deb(server_reply);
    }
    if (strcmp(server_reply, "ok!") == 0)
    {
        strcpy(buffer, usedId.c_str());
        n = write(network_socketForCreateUser, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing UserID in CreateUser command to socket");
            exit(0);
        }
        strcpy(buffer, password.c_str());
        n = write(network_socketForCreateUser, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing password in CreateUser command to socket");
            exit(0);
        }

        if (recv(network_socketForCreateUser, server_reply, MESSAGELEN, 0) < 0)
        {
            msg("recv failed");
        }
        else
        {
            msg(server_reply); //informs sucess or failure
        }
    }
    else
    {
        //cout << "reply == ok! is a false" << endl;
        //deb(reply);
        //deb(server_reply);
        ;
    }
    //msg("outside msg");

    // Close the connection
    sleep(2000);
    close(network_socketForCreateUser);
    //////////////////////////////////////////////////////
    pthread_exit(NULL);
    return NULL;
}

void *leaveGroup(void *ptr)
{ //user may already exist
    //cout << "aaya mai idhar aaya re";
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForlogout;
    // Create a stream socket
    network_socketForlogout = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForlogout, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);

    //string commands = str;
    //strcpy(buffer, commands.c_str());
    int n = write(network_socketForlogout, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing leave_group command to socket");
        exit(0);
    }
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;

    if (recv(network_socketForlogout, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(buffer);
    }
    if (strcmp(buffer, "user left Group") == 0)
    {
        //unorderedset;
        cout << "user left Group Sucessfully" << endl;
        //cout << "Pending Requests for given group ID are:" << buffer << endl;
    }
    else if (strcmp(buffer, "user not part of group in first Place"))
    {
        cout << buffer << endl;
    }
    else if (strcmp(buffer, "leave grp failed"))
    {
        cout << buffer << endl;
    }
    else
    {
        cout << "Error occured" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForlogout);
    pthread_exit(NULL);
    return NULL;
}

void *logOut(void *ptr)
{ //user may already exist
    //cout << "aaya mai idhar aaya re";
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForlogout;
    // Create a stream socket
    network_socketForlogout = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForlogout, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);

    //string commands = str;
    //strcpy(buffer, commands.c_str());
    int n = write(network_socketForlogout, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing logout command to socket");
        exit(0);
    }
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;

    if (recv(network_socketForlogout, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(buffer);
    }
    if (strcmp(buffer, "user logged out") == 0)
    {
        //unorderedset;
        cout << "Command accepted" << endl;
        //clear_Downloaded_list(); ///////////////////////////////////////////////////add this here
        //cout << "Pending Requests for given group ID are:" << buffer << endl;
    }
    else if (strcmp(buffer, "user not logined in first Place"))
    {
        cout << buffer << endl;
    }
    else if (strcmp(buffer, "logout failed"))
    {
        cout << buffer << endl;
    }
    else
    {
        cout << "Error occured" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForlogout);
    pthread_exit(NULL);
    return NULL;
}
void *loginUser(void *ptr)
{ //user may already exist
    //cout << "aaya mai idhar aaya re";
    string str = *reinterpret_cast<string *>(ptr);
    deb(str);
    string word = "";
    string usedId, password;
    for (auto x : str)
    {
        if (x == ' ')
        {
            cout << word << endl;
            usedId = word;
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    password = word;
    cout << word << endl;
    ////////////////////////////////////////////////////
    int network_socketForCreateUser;

    // Create a stream socket
    network_socketForCreateUser = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number

    // Initiate a socket connection
    int connection_status = connect(network_socketForCreateUser, (struct sockaddr *)&server_address, sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        //msg("Connection estabilished");
        ;
    }
    deb(network_socketForCreateUser);
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN + 10];
    bzero(buffer, MESSAGELEN);
    string commands = "login";
    string portAddresstosend;
    strcpy(buffer, commands.c_str());
    int n = write(network_socketForCreateUser, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    //msg("before first read");
    char server_reply[MESSAGELEN + 10];
    //Receive a reply from the server
    string reply = server_reply;
    if (recv(network_socketForCreateUser, server_reply, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        //msg("Recieved msg");
        deb(server_reply);
    }
    if (strcmp(server_reply, "ok! send credentails") == 0)
    {
        cout << "I came inside send credentials" << endl;
        strcpy(buffer, usedId.c_str());
        sleep(1);
        n = write(network_socketForCreateUser, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing UserID in CreateUser command to socket");
            exit(0);
        }
        strcpy(buffer, password.c_str());
        n = write(network_socketForCreateUser, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing password in CreateUser command to socket");
            exit(0);
        }
        if (recv(network_socketForCreateUser, server_reply, MESSAGELEN, 0) < 0)
        {
            msg("unable to get login Message");
        }
        else
        {
            if (strcmp(server_reply, "true") == 0)
            {
                loginSucess = true;
                //send this
                //mySendingPortAddr
                portAddresstosend = to_string(mySendingPortAddr);
                strcpy(buffer, portAddresstosend.c_str());
                get_Downloaded_list();
                n = write(network_socketForCreateUser, buffer, strlen(buffer));
                if (n < 0)
                {
                    msg("ERROR writing portAddress to tracker");
                    exit(0);
                }
            }
            else
            {
                loginSucess = false;
                msg("server_reply");
            }
        }
        deb(loginSucess);
    }
    else
    {
        cout << "reply == ok! send credentails is a false" << endl;
        deb(reply);
        deb(server_reply);
    }
    //msg("outside msg");

    // Close the connection
    //sleep(2000);
    close(network_socketForCreateUser);
    //////////////////////////////////////////////////////
    pthread_exit(NULL);
    return NULL;
}

void *listGroups(void *ptr)
{
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForlistGroup;
    // Create a stream socket
    network_socketForlistGroup = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForlistGroup, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    bzero(buffer, MESSAGELEN);
    string commands = str;
    strcpy(buffer, commands.c_str());
    int n = write(network_socketForlistGroup, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;

    if (recv(network_socketForlistGroup, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(buffer);
    }
    if (buffer != "")
    {
        //unorderedset;
        cout << "list of group ID are:" << buffer << endl;
    }
    else
    {
        cout << "Unable to get group List or no group exists" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForlistGroup);
    pthread_exit(NULL);
    return NULL;
}
void *acceptRequests(void *ptr)
{
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForlistRequests;
    // Create a stream socket
    network_socketForlistRequests = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForlistRequests, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);

    //string commands = str;
    //strcpy(buffer, commands.c_str());
    int n = write(network_socketForlistRequests, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing accept Requests command to socket");
        exit(0);
    }
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;

    if (recv(network_socketForlistRequests, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(buffer);
    }
    if (strcmp(buffer, "accepted") == 0)
    {
        //unorderedset;
        cout << "Command accepted" << endl;
        //cout << "Pending Requests for given group ID are:" << buffer << endl;
    }
    else if (strcmp(buffer, "Request Not Found") == 0)
    {
        cout << "No pending request from user" << endl;
    }
    else
    {
        cout << "Error occured" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForlistRequests);
    pthread_exit(NULL);
    return NULL;
}
void *listRequests(void *ptr)
{
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForlistRequests;
    // Create a stream socket
    network_socketForlistRequests = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForlistRequests, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);

    //string commands = str;
    //strcpy(buffer, commands.c_str());
    int n = write(network_socketForlistRequests, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;

    if (recv(network_socketForlistRequests, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(buffer);
    }
    if (buffer != "")
    {
        //unorderedset;
        cout << "Pending Requests for given group ID are:" << buffer << endl;
    }
    else
    {
        cout << "Unable to get group List or no group exists" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForlistRequests);
    pthread_exit(NULL);
    return NULL;
}
void *joinGroup(void *ptr)
{
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForJoinGroup;
    // Create a stream socket
    network_socketForJoinGroup = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForJoinGroup, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    bzero(buffer, MESSAGELEN);
    string commands = str;
    strcpy(buffer, commands.c_str());
    int n = write(network_socketForJoinGroup, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    char server_reply[MESSAGELEN];
    //Receive a reply from the server
    string reply = server_reply;
    if (recv(network_socketForJoinGroup, server_reply, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(server_reply);
    }
    if (strcmp(server_reply, "Group join Request sent") == 0)
    {
        //unorderedset;
        cout << "sent join_group request" << endl;
    }
    else
    {
        cout << "Unable to send join_group request" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForJoinGroup);
    pthread_exit(NULL);
    return NULL;
}
void *uploadFiletoTracker(void *ptr)
{ // k. Upload File: upload_file <file_path> <group_id>
    //upload_file + " " + file_path + " " + to_string(group_id);
    //group may already exist
    //cout << "function was called" << endl;
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForuploadFiletoTracker;
    // Create a stream socket
    network_socketForuploadFiletoTracker = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForuploadFiletoTracker, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    //int i=0;
    int i = 12;
    string path;
    while (buffer[i] != ' ')
    {
        path += buffer[i++];
    }
    deb(path);
    i++;
    string grpID;
    while (buffer[i] != ' ')
    {
        grpID += buffer[i++];
    }
    deb(grpID);
    i++;
    string usrID;
    while (buffer[i])
    {
        usrID += buffer[i++];
    }
    deb(usrID);

    auto fileNameStart = path.find_last_of('/');
    auto fileName = fileNameStart == std::string::npos ? path : path.substr(fileNameStart + 1);
    deb(fileName);
    string Fname = fileName;
    string temp_s;
    temp_s.clear();
    ///////////////////////////////////////////////////////////
    //issue is here (string temp_s is getting garbage vale)
    ///////////////////////////////////////////////////////////
    // temp_s.append("upload_file ");
    // temp_s.append(grpID);
    // temp_s.append(" ");
    // temp_s.append(usrID);
    // temp_s.append(" ");
    // temp_s.append(fileName);
    temp_s = "upload_file " + grpID + ' ' + usrID + ' ' + fileName;
    //temp_s = "upload_file" + " " + grpID + " " + usrID + " " + fileName;

    bzero(buffer, MESSAGELEN);
    strcpy(buffer, temp_s.c_str());
    deb(temp_s);
    //cout << "Message to be sent:" << endl;
    deb(buffer);
    int n = write(network_socketForuploadFiletoTracker, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing upload_file command to socket");
        exit(0);
    }
    /////////////////////////
    sha1ofFile.clear();
    fileContentsToCalculateShaOfEntireFileAtOnce.clear();
    sha1ofFilePieceWise.clear();
    fileContentsToCalculateShaOfEntireFileAtOnce.push_back(""); //used vector since it si thread SAFE
    ////////////////////////////////////////////////////
    string strrs;
    char fileContents[524289];
    bzero(fileContents, 524288);
    char c[21];
    char data[524288];
    ofstream outfile;
    int filedescriptor, size;
    unsigned int read_byte;
    //filedescriptor = open("tracker_info.txt", O_RDONLY);
    //filedescriptor = open("test.pdf", O_RDONLY);
    filedescriptor = open(path.c_str(), O_RDONLY);
    if (filedescriptor < 0)
    {
        perror("r1");
        exit(1);
    }
    char abc[2];
    int k = 0;
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    //while (read(filedescriptor, fileContents, 524288))
    /*
shaes1 : 5b267b273fd45add3d3e30d2bd05ee910512aca4
shaes1 : 68e2d96aa456b5c4095b51ac6364fab6dd80d684
shaes1 : 8741502381a6bc3f9a94c46b264c2201972ea5fe
shaes1 : 18c1c4b702a97a4352c11d8b8a9dd456d027e957
shaes1 : 6bf68830ce7b9ae7d50aad966cf78003a8ec68da
shaes1 : 68369e41a1ba9451d69f9d4edf8092b46e3c73c2
shaes1 : 59ebb5798bff4e8140410f658e62ca17debaa618
shaes1 : 07984e8dd18ec50f11f671cde74bd9b7647ef773
shaes1 : 99dcc54b25b7eb11d111eb1820baf996cbb845b8
shaes1 : 71c910b2872115fa50ef64414fa6620956ad9435
shaes1 : da39a3ee5e6b4b0d3255bfef95601890afd80709
shaes1 : 6567b1087a59f2100911fab09194c74ce70cd6b3
shaes1 : 69eed4abe6d44967b1bf694ad7b744128983b465
shaes1 : 3a9058c79ed1f0bf41886b538b15019b6af29b82
shaes1 : d6b0c5e6277d1d2e54937ae7b3543de3298aee34
shaes1 : bc22852481651e026df9e0e8cde79d565823559b
shaes1 : 0a637958ce4fc6be80a3315f9bd7f36fe87f3f0d
shaes1 : 2af68c552bae89ceb649b14370ed66caece3d178
shaes1 : dd4fc5cc607fbd601f284cd7f883c97fe861f2ce
shaes1 : 7b1d1ab6e18293496adcb1728108261eff644120
shaes1 : 2d857700f8a6a9799d9e1b1e2c3af384f9889e52
shaes1 : c4595d8f743731cbc1ca0bb34be79a40d771ddf0
shaes1 : d598403286669dadd8efc0aafb95602a7175b322
shaes1 : b1fcc4eb7a437c37609ba8e34a3b5f4fb679ac4b
shaes1 : 8e58b8db39de3bf81bce9d54662f5d210ca42bfd
shaes1 : fadeec466336919864539b508b4d5cdaca863266
shaes1 : 5d49bf26f40b8893f8600f51c2b902296ab8b297
shaes1 : 7f2cd820a3c58137abe7f71e39f25837b6b10a67
shaes1 : bda6d28df51638227800cb2b785e1d4c89a76749
shaes1 : f3572bb7812875981685de005ebaaf2222644b93
shaes1 : fba229b5b034c5a0e488369885e861fd6c5e389b
shaes1 : a13613a8ba7730b355e2db6c482b79a2a850ee10
shaes1 : d203c9757fa905eff645935165fc81bd0930d8a3
shaes1 : 5f0890b5b85e614b4bf3ce81f3d850d925a46cc9
shaes1 : 8bcb11a28a47d643340db3bac555acac6ba1fd88
shaes1 : da39a3ee5e6b4b0d3255bfef95601890afd80709
shaes1 : aa18b65fe8d51dd68996e11f6521793404dfe182
shaes1 : c17a26c802359d916b487dd6d75647e00515a09c
    */
    while ((read_byte = read(filedescriptor, fileContents, 524288)) > 0)
    {
        deb(read_byte);
        k++;
        deb(k);
        strrs = string(fileContents);
        //fileContentsToCalculateShaOfEntireFileAtOnce[0].append(strrs);
        //SHA1((const unsigned char *)strrs.c_str(), strrs.length(), (unsigned char *)c);
        //SHA1((const unsigned char *)strrs.c_str(), read_byte, (unsigned char *)c);
        SHA1((const unsigned char *)fileContents, read_byte, (unsigned char *)c);
        SHA1_Update(&ctx, fileContents, read_byte);
        //SHA1_Update(&ctx, strrs.c_str(), read_byte);

        string shaes1;
        char sha1[SHA_DIGEST_LENGTH * 2 + 1];
        int j = 0;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            stringstream ss;
            string temp;
            ss << hex << (short)c[i];
            temp = ss.str();
            if (temp.length() == 1)
            {

                shaes1 += '0';
                shaes1 += temp[0];
            }
            else if (temp.length() > 2)
            {
                shaes1 += temp[2];
                shaes1 += temp[3];
            }
            else
            {
                shaes1 += temp[0];
                shaes1 += temp[1];
            }
        }
        deb(shaes1);
        sha1ofFilePieceWise.push_back(shaes1);
        /*if (read_byte < 524288)
        {
            break;
        }*/
        bzero(fileContents, 524288); ///test it once commented
    }
    char hash[21];
    SHA1_Final((unsigned char *)hash, &ctx);
    string shaofFile;
    char sha1[SHA_DIGEST_LENGTH * 2 + 1];
    int j = 0;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        stringstream ss;
        string temp;
        ss << hex << (short)hash[i];
        temp = ss.str();
        if (temp.length() == 1)
        {
            shaofFile += '0';
            shaofFile += temp[0];
        }
        else if (temp.length() > 2)
        {
            shaofFile += temp[2];
            shaofFile += temp[3];
        }
        else
        {
            shaofFile += temp[0];
            shaofFile += temp[1];
        }
    }
    //cout << "I came here\n";
    deb(shaofFile);
    //cout << shaofFile;
    //cout << "I came here\n";
    ////////////////////////////////
    close(filedescriptor);
    for (unsigned int i = 0; i < sha1ofFilePieceWise.size(); i++)
    {
        bzero(buffer, MESSAGELEN);
        strcpy(buffer, sha1ofFilePieceWise[i].c_str());
        int n = write(network_socketForuploadFiletoTracker, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing upload_file command to socket");
            exit(0);
        }
    }
    bzero(buffer, MESSAGELEN);
    string commands = "End_Of_File";
    strcpy(buffer, commands.c_str());
    n = write(network_socketForuploadFiletoTracker, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing upload_file command to socket");
        exit(0);
    }
    ///////////////////////////////////////////////////////////
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, shaofFile.c_str());
    n = write(network_socketForuploadFiletoTracker, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing upload_file command to socket");
        exit(0);
    }

    ////////////////////////////////////////////////////////////
    // Close the connection
    close(network_socketForuploadFiletoTracker);
    pthread_exit(NULL);
    return NULL;
}
void *createGroup(void *ptr)
{
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForCreateGroup;
    // Create a stream socket
    network_socketForCreateGroup = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForCreateGroup, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        msg("Error");
        return 0;
    }
    else
    {
        msg("Connection estabilished");
    }
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    bzero(buffer, MESSAGELEN);
    string commands = str;
    strcpy(buffer, commands.c_str());
    int n = write(network_socketForCreateGroup, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    char server_reply[MESSAGELEN];
    //Receive a reply from the server
    string reply = server_reply;
    if (recv(network_socketForCreateGroup, server_reply, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        msg("Recieved msg");
        deb(server_reply);
    }
    if (strcmp(server_reply, "Group Created") == 0)
    {
        buffer; ///////////////////////
        string word = "";
        string groupId;
        int i = 13;
        for (; buffer[i] != ' '; i++)
        {
            word += buffer[i];
        }
        groupId = word;
        cout << "group id extracted is:" << word << endl;
        mygroups.insert(groupId);
        //unorderedset;
        cout << "group created" << endl;
    }
    else
    {
        cout << "Unable to create Group" << endl;
    }
    msg("outside msg");
    // Close the connection
    close(network_socketForCreateGroup);
    pthread_exit(NULL);
    return NULL;
}

void *downloadFileHelper(void *ptr)
{
    //cout << *reinterpret_cast<string *>(ptr);
    //cout << (char *)ptr;
    //string str1 = *reinterpret_cast<string *>(ptr);
    string str1 = (char *)ptr;
    string str = str1;
    msg("File Name Recieved For Downloading:");
    deb(str);
    string filePart;
    string fileName;
    int portNumber;
    int numberOfParts;
    string word;
    string temp;
    int i = 0;
    for (auto x : str)
    {
        if (x == ' ')
        {
            i++;
            if (word != "")
            {
                if (i == 1)
                {
                    filePart = word;
                }
                else if (i == 2)
                {
                    fileName = word;
                }
                else if (i == 3)
                {
                    portNumber = stoi(word);
                }
            }
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    if (word != "")
        numberOfParts = stoi(word);
    //FILE *fp;
    //fp = fopen(fileName.c_str(), "r");
    //fseek(fp, 0L, SEEK_END);
    //unsigned long long sz = ftell(fp);
    //fseek(fp, 0L, SEEK_SET);
    msg(str);
    //deb(str);
    int network_socket;
    // Create a stream socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portNumber); //recieve from this person
    // Initiate a socket connection
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        //puts("Error\n");
        msg("Error: connect system call Failed");
        return 0;
    }
    else
    {
        //printf("Connection estabilished\n");
        msg("Connection estabilished");
    }
    deb(network_socket);
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);

    string strs = filePart + " " + fileName + " " + to_string(numberOfParts);
    //tell other peer which file to download
    strcpy(buffer, strs.c_str());
    int n = write(network_socket, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR: writing to socket failed");
        exit(0);
    }
    //msg("Check Point:before first read");
    char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //if (recv(network_socket, server_reply, MESSAGELEN, 0) < 0)
    //{
    //msg("Recv failed");
    //}
    //else
    //{
    //msg("Recieved msg");
    //deb(server_reply);
    //}
    //cout << "outside msg";
    msg("Check Point:outside msg");
    string destn = "PQR.TXT";
    str += "sdfgv";

    str = "Part" + filePart + "__" + fileName;
    // Close the connection
    //int fin, fout, nread;
    //fout = open((str).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    FILE *fp;
    fp = fopen(str.c_str(), "wb");

    ofstream myfile;
    str = "Parts" + filePart + "__" + fileName;
    myfile.open(str.c_str(), ios::binary | ios::out);

    //myfile.open(str.c_str(), ios::out);
    //string filePart;
    //string fileName;
    //int portNumber;

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
            //cout << resposne << endl;
            //if (resposne == "NULLS")
            if (strcmp(resposne.c_str(), "NULLS") == 0)
            {
                cout << resposne << endl;
                break;
            }
            else
            {
                myfile << server_reply;
                fwrite(resposne.c_str(), resposne.length(), 1, fp);

                //deb(server_reply);
                //cout << "I am Here";
                //msg("I am Here");
            }
        }
    }
    myfile.close();
    fclose(fp);

    //sleep(2000);
    sleep(2);
    cout << "///////////////////////";
    cout << "File Part recieving complete" << endl;
    close(network_socket);
    pthread_exit(NULL);
    return NULL;
}
void *downloadFile(void *ptr)
{
    string str = *reinterpret_cast<string *>(ptr);
    msg("File Name Recieved For Downloading:");
    //seperate groupID, fileName, Destin Paths
    deb(str);
    int groupID;
    string word = "";
    string fileName, DestinPaths;
    int i = 0;
    for (auto x : str)
    {
        if (x == ' ')
        {
            i++;
            if (i == 1)
            {
                groupID = stoi(word);
            }
            else if (i == 2)
            {
                fileName = word;
            }
            //cout << word << endl;
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    DestinPaths = word;
    //msg(str);
    //deb(str);
    int network_socketForDownloadFileInfo;
    // Create a stream socket
    network_socketForDownloadFileInfo = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    //server_address.sin_port = htons(PORTNUM); //connect to tracker to get peer info
    server_address.sin_port = htons(TRACKER1PORTNUM); //connect to tracker to get peer info

    // Initiate a socket connection
    int connection_status = connect(network_socketForDownloadFileInfo, (struct sockaddr *)&server_address, sizeof(server_address));
    // Check for connection error
    if (connection_status < 0)
    {
        //puts("Error\n");
        msg("Error: connect system call Failed");
        return 0;
    }
    else
    {
        //printf("Connection estabilished\n");
        msg("Connection estabilished");
    }
    deb(network_socketForDownloadFileInfo);
    // Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    //tell other peer which file to download
    string commands = "downloadFileInfo";
    strcpy(buffer, commands.c_str());
    //strcpy(buffer, str.c_str());
    int n = write(network_socketForDownloadFileInfo, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR: writing to socket failed");
        exit(0);
    }
    //msg("Check Point:before first read");
    char server_reply[MESSAGELEN];
    //Receive a reply from the server
    string reply = server_reply;
    string portNumberFRomWhereToDownload;
    if (recv(network_socketForDownloadFileInfo, server_reply, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        //msg("Recieved msg");
        deb(server_reply);
    }
    vector<string> portAddressOfPeopleHavingMyFile;
    if (strcmp(server_reply, "ok! send fileName") == 0)
    {
        strcpy(buffer, fileName.c_str());
        n = write(network_socketForDownloadFileInfo, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing filename to socket for download");
            exit(0);
        }
        if (recv(network_socketForDownloadFileInfo, server_reply, MESSAGELEN, 0) < 0)
        {
            msg("unable to get port number of people who have my file");
        }
        else
        {
            //strcmp(portNumberFRomWhereToDownload, server_reply);
            portNumberFRomWhereToDownload = server_reply;
        }
        deb(portNumberFRomWhereToDownload);

        string word;
        int possibleportNum;
        for (auto x : portNumberFRomWhereToDownload)
        {
            if (x == ' ')
            {

                if (word != "" && stoi(word) != 0 && stoi(word) != mySendingPortAddr)
                {
                    portAddressOfPeopleHavingMyFile.push_back(word);
                }
                word = "";
            }
            else
            {
                word = word + x;
            }
        }
        if (word != "" && stoi(word) != 0 && stoi(word) != mySendingPortAddr)
            portAddressOfPeopleHavingMyFile.push_back(word);
    }
    else
    {
        cout << "reply == ok! send fileName is a false" << endl;
        deb(reply);
        deb(server_reply);
    }
    //msg("outside msg");
    // Close the connection with tracker
    close(network_socketForDownloadFileInfo);
    unsigned int numberOfDestnPorts = portAddressOfPeopleHavingMyFile.size();

    pthread_t threads[numberOfDestnPorts];
    // Creating 4 threads
    string temps;
    //vector<string> arr;
    vector<char *> arr;
    char *ptrs;
    for (unsigned int i = 0; i < numberOfDestnPorts; i++)
    {
        temps = to_string(i + 1) + " " + fileName + " " + portAddressOfPeopleHavingMyFile[i] + " " + to_string(numberOfDestnPorts);
        //cout << temps << endl;
        //arr.push_back(temps);
        ptrs = (char *)malloc(MESSAGELEN * sizeof(char));
        arr.push_back(ptrs);
        strcpy(ptrs, temps.c_str());
        //pthread_create(&threads[i], NULL, downloadFileHelper, &temps);
        pthread_create(&threads[i], NULL, downloadFileHelper, ptrs);
        temps = "";
        usleep(100);
    }

    // joining 4 threads i.e. waiting for all 4 threads to complete
    for (int i = 0; i < numberOfDestnPorts; i++)
        pthread_join(threads[i], NULL);
    cout << "Did I ever came here?" << endl;
    //Merge parts
    pthread_exit(NULL);
    return NULL;
}
void getInput()
{

    pthread_t handlingInputTid;
    pthread_t handlingDownloadFile;
    bool commandwasDownload = false;
    bool commandwasLogin = false;
    //char *arr = NULL;
    string imput;
    string temp;
    cout << "Enter Command";
    cin >> imput;
    //imput = "create_user";
    //imput = "login";
    //imput = "create_group";
    if (imput == "create_user")
    {                            //b. Create User Account: create_user <user_id> <passwd>
        if (loginSucess == true) //////////////////////////////////you  dont have be loginned in order to create user
        {
            int user_id; //this must not be too long
            string passwd;
            //cin >> user_id >> passwd;
            user_id = 12;
            passwd = "sdasdfgf";
            string temp = to_string(user_id) + " " + passwd;
            cout << temp << endl;
            /////allocate space for string from heap then send
            pthread_create(&handlingInputTid, NULL, createUser, &temp);
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "login")
    { // c. Login: login <user_id> <passwd>
        commandwasLogin = true;
        //int user_id;
        string passwd;
        cout << "User ID  & Password";
        cin >> user_id >> passwd;
        //user_id = 12323;
        //passwd = "sdgf";
        string temp = to_string(user_id) + " " + passwd;
        //cout << temp;
        /////allocate space for string from heap then send
        pthread_create(&handlingInputTid, NULL, loginUser, &temp);
    }
    else if (imput == "create_group")
    { // d. Create Group: create_group <group_id>
        char *arr;
        if (loginSucess == true)
        {
            int group_id;
            cin >> group_id;
            //group_id = 12323;
            temp = imput + " " + to_string(group_id) + " " + to_string(user_id);
            //cout << temp;
            arr = (char *)malloc(temp.length() * sizeof(char)); //////untested
            //arr = (char *)malloc(sizeof(temp.length() * sizeof(char))); //////untested
            strcpy(arr, temp.c_str());
            /////allocate space for string from heap then send
            //pthread_create(&handlingInputTid, NULL, createGroup, &temp);
            pthread_create(&handlingInputTid, NULL, createGroup, arr);
            //free(arr);
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "join_group")
    { // e. Join Group: join_group <group_id>
        char *arr;
        if (loginSucess == true)
        {

            /////allocate space for string from heap then send
            //pthread_create(&handlingInputTid, NULL, joinGroup, &temp);

            //////////////////////////
            int group_id_to_join;
            //cin >> group_id;
            group_id_to_join = 12323;
            temp = imput + " " + to_string(group_id_to_join) + " " + to_string(user_id);
            //cout << temp;
            arr = (char *)malloc(temp.length() * sizeof(char)); ////////////untested
            strcpy(arr, temp.c_str());
            /////allocate space for string from heap then send
            //pthread_create(&handlingInputTid, NULL, createGroup, &temp);
            pthread_create(&handlingInputTid, NULL, joinGroup, arr);
            //free(arr);

            ///////////////////////////
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "leave_group")
    { // f. Leave Group: leave_group <group_id>
        char *arr;
        if (loginSucess == true)
        {

            ///////////////////

            int group_id_to_leave;
            //cin >> group_id_to_leave;
            group_id_to_leave = 12323;
            temp = imput + " " + to_string(group_id_to_leave) + " " + to_string(user_id) + '\0';
            //cout << temp;
            arr = (char *)malloc(temp.length() * sizeof(char)); ///untested
            strcpy(arr, temp.c_str());
            /////////////////
            //cout << temp;
            /////allocate space for string from heap then send
            pthread_create(&handlingInputTid, NULL, leaveGroup, arr);
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "list_requests")
    { // g. List pending join: list_requests <group_id>
        char *arr = NULL;
        if (loginSucess == true)
        {
            int group_id_to_list;
            cout << "Enter <groupId>" << endl;
            cin >> group_id_to_list;
            //group_id_to_list = 12323;
            temp = imput + " " + to_string(group_id_to_list);
            //cout << temp;
            arr = (char *)malloc(temp.length() * sizeof(char)); ////////////untested
            strcpy(arr, temp.c_str());
            /////allocate space for string from heap then send
            //pthread_create(&handlingInputTid, NULL, createGroup, &temp);
            pthread_create(&handlingInputTid, NULL, listRequests, arr);
            //free(arr);

            /////////////////////
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "accept_request")
    { // h. Accept Group Joining Request: accept_request <group_id> <user_id>
        char *arr = NULL;
        if (loginSucess == true)
        {
            ///////user may be owner of multiple groups
            int user_id;
            int group_id;
            cout << "Enter <GroupID> <UserID>" << endl;
            cin >> group_id >> user_id;
            //user_id = 234;
            //group_id = 12323;
            if (arr != NULL)
                free(arr);
            temp.clear();
            //temp = imput + " " + to_string(group_id) + " " + to_string(user_id) + " q";
            temp = imput + " " + to_string(group_id) + " " + to_string(user_id) + '\0';
            //cout << temp;
            string temp2 = temp;
            deb(temp2);
            arr = (char *)malloc(temp2.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient memory, Unable to allocate\n";
            }
            else
            {
                strcpy(arr, temp2.c_str());
                /////allocate space for string from heap then send
                //pthread_create(&handlingInputTid, NULL, createGroup, &temp);
                deb(arr);
                pthread_create(&handlingInputTid, NULL, acceptRequests, arr);
            }
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "list_groups")
    { // i. List All Group In Network: list_groups
        char *arr;
        if (loginSucess == true)
        {
            ///////user may be owner of multiple groups
            //string temp = " ";
            //cout << temp;
            /////allocate space for string from heap then send

            temp = imput;
            //cout << temp;
            //arr = (char *)malloc(sizeof(temp.length() * sizeof(char)));
            arr = (char *)malloc(temp.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient memory, Unable to allocate\n";
            }
            else
            {
                strcpy(arr, temp.c_str());
                pthread_create(&handlingInputTid, NULL, listGroups, arr);
            }
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "list_files")
    { // j. List All sharable Files In Group: list_files <group_id>
        char *arr;
        if (loginSucess == true)
        {
            int group_id;
            //cout << "Enter Group ID:";
            //cin >> user_id;
            group_id = 12323;
            string temp;
            //cout << temp;
            temp = imput + ' ' + to_string(group_id);
            //arr = (char *)malloc(sizeof(temp.length() * sizeof(char)));
            arr = (char *)malloc(temp.length() * sizeof(char));
            strcpy(arr, temp.c_str());
            /////allocate space for string from heap then send
            //pthread_create(&handlingInputTid, NULL, listFiles, &temp);
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "upload_file")
    { // k. Upload File: upload_file <file_path> <group_id>
        char *arr;
        if (loginSucess == true)
        {
            int group_id;
            string file_path;
            cout << "Enter <file_path> <group_id>\n";
            cin >> file_path >> group_id;
            //file_path = "./test.pdf";
            //file_path = "./../../../IIIT HYD/Resources/test.pdf";
            //file_path = "./../../../AOS3TEST/Ressadfsources/test.pdf"; //destination must be space seperated
            //group_id = 12323;
            //temp = imput + " " + file_path + " " + to_string(group_id);
            temp = imput + ' ' + file_path + ' ' + to_string(group_id) + ' ' + to_string(user_id);
            string temp2 = temp;
            arr = (char *)malloc(temp2.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient memory, Unable to allocate\n";
            }
            else
            {
                strcpy(arr, temp2.c_str());
                /////allocate space for string from heap then send
                //pthread_create(&handlingInputTid, NULL, createGroup, &temp);
                deb(arr);
                pthread_create(&handlingInputTid, NULL, uploadFiletoTracker, arr);
            }
            //cout << temp;
            /////allocate space for string from heap then send
            //pthread_create(&handlingInputTid, NULL, uploadFile, &temp);

            /*
            temp = imput;
            //cout << temp;
            //arr = (char *)malloc(sizeof(temp.length() * sizeof(char)));
            arr = (char *)malloc(temp.length() * sizeof(char));
            strcpy(arr, temp.c_str());
            pthread_create(&handlingInputTid, NULL, listGroups, arr);
            */
            /*
            int user_id;
            int group_id;
            cout << "Enter <GroupID> <UserID>" << endl;
            cin >> group_id >> user_id;
            //user_id = 234;
            //group_id = 12323;
            if (arr != NULL)
                free(arr);
            temp.clear();
            //temp = imput + " " + to_string(group_id) + " " + to_string(user_id) + " q";
            temp = imput + " " + to_string(group_id) + " " + to_string(user_id) + '\0';
            //cout << temp;
            string temp2 = temp;
            deb(temp2);
            arr = (char *)malloc(temp2.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient memory, Unable to allocate\n";
            }
            else
            {
                strcpy(arr, temp2.c_str());
                /////allocate space for string from heap then send
                //pthread_create(&handlingInputTid, NULL, createGroup, &temp);
                deb(arr);
                pthread_create(&handlingInputTid, NULL, acceptRequests, arr);
            }

           */
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "download_file")
    { // l. Download File: download_file <group_id> <file_name> <destination_path>
        loginSucess = true;
        if (loginSucess == true)
        {
            commandwasDownload = true;
            int group_id;
            string fileName, dest_path;
            //cout << "Tell GroupID, FileName, Destination" << endl;
            //cin >> group_id>> file_name >>destination_path;
            group_id = 12323;
            fileName = "test.pdf";
            cout << fileName << endl;
            dest_path = "sdgf/ds";
            temp = to_string(group_id) + " " + fileName + " " + dest_path;
            //cout << temp;
            /////allocate space for string from heap then send
            pthread_create(&handlingDownloadFile, NULL, downloadFile, &temp);
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "logout")
    { // m. Logout: logout
        char *arr;
        if (loginSucess == true)
        {
            //can a client login to ultiple groups?
            loginSucess = false;
            deb(loginSucess);
            //inform tracker Thta I am not active

            //int currentGroupID = 12323;
            //temp=
            temp.clear();
            temp = imput + " " + to_string(user_id) + '\0';
            deb(temp);
            //cout << temp;
            // arr = (char *)malloc(sizeof(temp.length() * sizeof(char)));
            arr = (char *)malloc(temp.length() * sizeof(char));
            strcpy(arr, temp.c_str());

            //string temp = to_string(currentGroupID);
            pthread_create(&handlingInputTid, NULL, logOut, arr);
            //inform tracker that user in not available now
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "show_downloads") ////////////////////////////////////////////untested
    {
        // n. Show_downloads: show_downloads
        // Output format:
        // [D] [grp_id] filename
        // [C] [grp_id] filename
        // D(Downloading), C(Complete)
        if (loginSucess == true)
        {
            string temp = " ";
            //pthread_create(&handlingInputTid, NULL, showDownload, &temp);
            //thread needed because we want to continue previous download
            //meanwhile show the current download
            for (auto itr = mapGroupIDactive_Download.begin(); itr != mapGroupIDactive_Download.end(); itr++)
            {
                cout << "[C] [" << itr->first << "] " << (itr->second) << endl;
            }
            for (auto itr = mapGroupIDcompleted_Download.begin(); itr != mapGroupIDcompleted_Download.end(); itr++)
            {
                cout << "[D] [" << itr->first << "] " << (itr->second) << endl;
            }
            cout << "D(Downloading), C(Complete)\n";
        }
        else
        {
            msg("you Need to login first");
        }
    }
    else if (imput == "stop_share")
    { // o. Stop sharing: stop_share <group_id> <file_name>
        if (loginSucess == true)
        {
            int currentGroupID = 12323;
            string temp = to_string(currentGroupID);
            //pthread_create(&handlingInputTid, NULL, stopShare, &temp);
            //inform tracker that user in not sharing now
        }
        else
        {
            msg("you Need to login first");
        }
    }
    cout << "I came here" << endl;
    if (commandwasDownload)
        pthread_join(handlingDownloadFile, NULL);
    if (commandwasLogin)
        pthread_join(handlingInputTid, NULL);
    cout << "But I didnt came here" << endl;
    /*if (arr != NULL)
        free(arr);*/
}

void *FileSendToPeer(void *ptr)
{
    init();
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    int opt = 1;
    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Forcefully attaching socket to the port 8080
    /*if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    } */
    ////mayinterfere with our randomly generated port

    ////////////////////////////////
    /*int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }*/

    /////////////////////////////////////

    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_port = htons(PORTNUM); //send from this port to peer
    //this port number can be obtained from tracker
    //tracker will be holding pool of port number and from there it will
    //send one to us for use during login

    // Bind the socket to the
    // address and port number.
    srand(time(0));
    int lower = 2000, upper = 9999;
    int port;
    while (true)
    {
        port = (rand() % (upper - lower + 1)) + lower;
        //serverAddr.sin_port = htons(PORTNUM);
        serverAddr.sin_port = htons(port);
        int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (n < 0)
        {
            continue;
        }
        else
        {
            mySendingPortAddr = port;
            break;
        }
    }
    //int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    // Listen on the socket,
    // with 40 max connection
    // requests queued
    //if (listen(serverSocket, 50) == 0)
    if (listen(serverSocket, MaxPendingConnection) == 0)
    {
        msg("Listening");
    }
    else
    {
        msg("Error: listen Failed");
    }

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
        deb(serverStorage.__ss_align);
        deb(serverStorage.__ss_padding);
        deb(serverStorage.ss_family);
        deb(addr_size);
        //sent one test messages
        //string client_message = "abcAbCsbdoclsnfdcklsjdnckvh bsdlcv kl";
        //client_message.resize(MESSAGELEN, ' ');
        //write(newSocket, client_message.c_str(), MESSAGELEN);

        char buffer[MESSAGELEN];
        bzero(buffer, MESSAGELEN);
        int n = read(newSocket, buffer, MESSAGELEN);
        if (n < 0)
        {
            cout << "ERROR reading(File Name for Download) from socket";
            exit(0);
        }

        printf("Requested File for download: %s\n", buffer);
        ////////////////////////////////////////
        ////////////////////////////////////////
        string str = buffer;
        int filePart;
        string fileName;
        int numberOfParts;
        string word;
        string temp;
        int i = 0;
        for (auto x : str)
        {
            if (x == ' ')
            {
                i++;
                if (word != "")
                {
                    if (i == 1)
                    {
                        filePart = stoi(word);
                    }
                    else if (i == 2)
                    {
                        fileName = word;
                    }
                }
                word = "";
            }
            else
            {
                word = word + x;
            }
        }
        numberOfParts = stoi(word);
        FILE *fp;
        fp = fopen(fileName.c_str(), "rb");
        if (fp == NULL)
        {
            cout << "File Open Failed\n";
        }
        fseek(fp, 0L, SEEK_END);
        unsigned long long sz = ftell(fp);
        unsigned long long readleanght = 0;
        cout << "file size acc to " << filePart << " is " << sz << endl;
        fseek(fp, 0L, SEEK_SET);
        //open file read (sz/numberOfParts) amount of data
        //from (sz/numberOfParts)*(filePart-1)
        //and send it to recieveer
        //////////////////////////////////////////
        if (0 != fseek(fp, (sz / numberOfParts) * (filePart - 1), SEEK_CUR))
        {
            printf("\n fseek() failed\n");
            return NULL;
        }
        else
        {
            cout << filePart << " is reading from location:" << ftell(fp) << endl;
        }
        unsigned long long numberOfReadOperationReqd = ((sz / numberOfParts) / MESSAGELEN);
        unsigned long long chunkoflastSegment;
        //chunkoflastSegment = ((sz / numberOfParts) % MESSAGELEN);
        chunkoflastSegment = ((sz * filePart) / numberOfParts) % MESSAGELEN;
        ////problem in above stmt
        if (chunkoflastSegment != 0)
        {
            numberOfReadOperationReqd++;
        }
        char b[MESSAGELEN];
        bzero(b, MESSAGELEN);
        int fin, fout, nread;
        //while (fread(b, sizeof(b), 1, fp) > 0)
        //while (fread(b, (sz / numberOfParts), 1, fp) > 0)
        n = 0;
        //if (n+=fread(b, MESSAGELEN, 1, fp) > 0)
        while (true)
        {
            if (n == (numberOfReadOperationReqd - 1))
            {

                long long k = fread(b, chunkoflastSegment, 1, fp);
                readleanght += k * chunkoflastSegment;
                if (0 < k)
                {

                    n++;
                    cout << "for last Chunk";
                    cout << filePart << " read " << n << " times";
                    write(newSocket, b, chunkoflastSegment);
                }

                //add error check for write
                break;
            }
            else
            {
                //n += fread(b, MESSAGELEN, 1, fp);
                long long k = fread(b, MESSAGELEN, 1, fp);
                readleanght += k * MESSAGELEN;
                if (0 < k)
                {
                    n++;
                    cout << filePart << " read " << n << " times" << endl;
                    write(newSocket, b, MESSAGELEN);
                }
                //add error check for write
            }
            //if (n == (numberOfReadOperationReqd - 1))
            //write(fout, b, nread);
            // write(newSocket, b, MESSAGELEN); //change write length here
        }
        string eofSignal = "NULLS";
        strcpy(b, eofSignal.c_str());
        //write(newSocket, eofSignal.c_str(), MESSAGELEN);
        if (0 > write(newSocket, b, 5))
        {
            cout << "last write (NULLS) failed";
        }
        fclose(fp);
        cout << "send my part" << endl;
        cout << "total read len:" << readleanght << endl;
        cout << "expected raed len:" << chunkoflastSegment + (sz / numberOfParts) << endl;
        //FILE *fd = NULL;
        //char buff[100];
        //memset(buff, 0, sizeof(buff));
        //fd = fopen("test.txt", "r");

        //printf("\n File opened successfully through fopen()\n");

        //int sizeOfoneReadOperation = 10;
        //int howManyReadopeartionBeforeFreadREturns = 5;
        //int howManyReadopeartionBeforeFreadREturns = 5;
        //if (SIZE * NUMELEM != fread(buff, sizeOfoneReadOperation, howManyReadopeartionBeforeFreadREturns, fd))
        //if (SIZE * NUMELEM != fread(buff, sizeof(buff), howManyReadopeartionBeforeFreadREturns, fd))
        /*if (0 > fread(buff, sizeof(buff), howManyReadopeartionBeforeFreadREturns, fd))
        {
            printf("\n fread() failed\n");
            return 1;
        }
        cout << buff << endl;*/
        //////////////////////////////////////////

        //usleep(microseconds);
        /*char b[MESSAGELEN];
        bzero(b, MESSAGELEN);
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
        write(newSocket, eofSignal.c_str(), MESSAGELEN);*/
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
        /*
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
        }*/
    }
}
void get_Downloaded_list()
{

    FILE *infile;
    string fileName = to_string(user_id) + "_downloadLog.dat";
    struct downloadedFiles compl_Download;
    char DownloadedFileName[64];
    //struct groupJoinRequests groupJoinRequeststemp;
    infile = fopen(fileName.c_str(), "r");
    if (infile == NULL)
    {
        //fprintf(stderr, "\nError opening file\n");
        cout << "User:" << user_id << " has not downloaded any files" << endl;
        return;
    }
    while (fread(&compl_Download, sizeof(struct downloadedFiles), 1, infile))
    {
        //string temp = mapPendingReqGroupIDUserIDs[groupJoinRequeststemp.grpId];
        //mapPendingReqGroupIDUserIDs[groupJoinRequeststemp.grpId] = temp + ' ' + groupJoinRequeststemp.usrIds;
        //mapPendingReqGroupIDUserIDs[groupJoinRequeststemp.grpId] = groupJoinRequeststemp.usrIds;
        mapGroupIDcompleted_Download.insert(make_pair(compl_Download.DownloadedFileName, compl_Download.SourceGroupId));
    }
    fclose(infile);
    cout << "downaloded files of user(" << user_id << ")"
         << "are in :\n";
    for (auto itr = mapGroupIDcompleted_Download.begin(); itr != mapGroupIDcompleted_Download.end(); itr++)
    {
        cout << "Group ID " << itr->first << " FileName" << itr->second;
    }
}
// Driver Code
int main(int argc, char **argv)
{

    // a. ./client <IP>:<PORT> tracker_info.txt
    //getTrackerDetails(argc, argv);
    string fileName;
    pthread_t Sendertid;
    pthread_create(&Sendertid, NULL, FileSendToPeer, &fileName);
    //run server for sending

    while (true)
        getInput(); //this too has to be thread so that we can download from
    //this source and also type input from here

    pthread_t Recievertid;

    //recieve file only when needed(and later change it to download)
    //int choice = 1;
    int choice;
    msg("want to receive files also?");
    cin >> choice;
    if (choice == 1)
    {
        //listFiles();
        cout << "Enter File Name to Download:" << endl;
        cin >> fileName;
        //fileName = "ABC.TXT";
        //downnloads file in one go
        pthread_create(&Recievertid, NULL, FileToDownload, &fileName);
    }
    else
    {

        //fileName = "ABC.TXT";
        //pthread_create(&Sendertid, NULL, FileSendToPeer, &fileName);
    }
    pthread_join(Sendertid, NULL);
    pthread_join(Recievertid, NULL);

    // Suspend execution of calling thread

    pthread_join(tid, NULL);
}
