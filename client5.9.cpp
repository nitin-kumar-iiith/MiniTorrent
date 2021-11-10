//single peer downlaod
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
int portofTracker1;
int portofTracker2;
bool loginSucess = false;
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

unordered_set<string> mygroups;
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

    msg("Check Point:outside msg");
    string destn = "PQR.TXT";
    str += "sdfgv";
    ofstream myfile;
    myfile.open(str.c_str(), ios::out);
    msg("outside while true");
    while (true)
    {
        msg("Inside while true");
        int len = recv(network_socket, server_reply, MESSAGELEN, 0);
        msg("after read");
        if (len < 0)
        {
            msg("recv failed");
            break;
        }
        else if (len == 0)
        {
            msg("Time to leave");
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
            }
        }
    }
    myfile.close();
    sleep(2);
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
    string t, q, filename;
    //string trackerFileName = "tracker_info.txt";
    string trackerFileName = argv[2];
    string Ip_Port = argv[1];
    string IP_ADDRESS;
    string PORT_NUM;
    string word = "";
    for (unsigned int i = 0; i < Ip_Port.length(); i++)
    {
        if (Ip_Port[i] == ':')
        {
            IP_ADDRESS = word;
            word = "";
        }
        else
        {
            word += Ip_Port[i];
        }
    }
    if (word != "")
    {
        PORT_NUM = word;
        mySendingPortAddr = stoi(PORT_NUM);
    }
    deb(IP_ADDRESS);
    deb(PORT_NUM);
    //deb(Ip_Port);
    deb(trackerFileName);
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
    file.close();
}
struct Create_User
{
    int user;
    string passwd;
};
void *createUser(void *ptr)
{ //user may already exist
    string str = (char *)ptr;
    deb(str);
    string word = "";
    string usedId, password;
    for (auto x : str)
    {
        if (x == ' ')
        {
            //cout << word << endl;
            usedId = word;
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    if (word != "")
        password = word;
    //cout << word << endl;

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
    //deb(network_socketForCreateUser);
    //Send data to the socket
    //send(network_socket, &str, sizeof(str), 0);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    int n = write(network_socketForCreateUser, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing CreateUser command to socket");
        exit(0);
    }
    //Receive a reply from the server
    bzero(buffer, MESSAGELEN);
    if (recv(network_socketForCreateUser, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        if (strcmp(buffer, "Added user Sucessfully") == 0)
        {
            cout << "Account Created\n";
        }
        else
        {
            cout << "Unable to create Acconut at the moment" << endl;
        }
    }
    // Close the connection
    usleep(200);
    close(network_socketForCreateUser);
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
    //msg("outside msg");
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
    //msg("outside msg");
    // Close the connection
    close(network_socketForlogout);
    pthread_exit(NULL);
    return NULL;
}
void *loginUser(void *ptr)
{ //user may already exist
    string str = (char *)ptr;
    deb(str);

    int network_socketForLoginUser;

    // Create a stream socket
    network_socketForLoginUser = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number

    // Initiate a socket connection
    int connection_status = connect(network_socketForLoginUser, (struct sockaddr *)&server_address, sizeof(server_address));

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
    // Send data to the socket
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    string portAddresstosend;
    strcpy(buffer, str.c_str());
    int n = write(network_socketForLoginUser, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    bzero(buffer, MESSAGELEN);
    if (recv(network_socketForLoginUser, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        //msg("Recieved msg");
        deb(buffer);
    }
    if (strcmp(buffer, "true") == 0)
    {
        loginSucess = true;
        //send mySendingPortAddr
        portAddresstosend = to_string(mySendingPortAddr);
        bzero(buffer, MESSAGELEN);
        strcpy(buffer, portAddresstosend.c_str());
        get_Downloaded_list();
        n = write(network_socketForLoginUser, buffer, strlen(buffer));
        if (n < 0)
        {
            msg("ERROR writing portAddress to tracker");
            exit(0);
        }
    }
    else
    {
        loginSucess = false;
    }
    deb(loginSucess);
    // Close the connection
    usleep(200);
    close(network_socketForLoginUser);
    //////////////////////////////////////////////////////
    pthread_exit(NULL);
    return NULL;
}
void *listFiles(void *ptr)
{
    //group may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    strcpy(buffer, (char *)ptr);
    deb(buffer);
    int network_socketForlistFiles;
    // Create a stream socket
    network_socketForlistFiles = socket(AF_INET, SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(TRACKER1PORTNUM); //tracker's port number
    // Initiate a socket connection
    int connection_status = connect(network_socketForlistFiles, (struct sockaddr *)&server_address, sizeof(server_address));
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
    strcpy(buffer, str.c_str());
    int n = write(network_socketForlistFiles, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing listFiles command to socket");
        exit(0);
    }
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;

    if (recv(network_socketForlistFiles, buffer, MESSAGELEN, 0) < 0)
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
        cout << "list of files in given group ID are:" << buffer << endl;
    }
    else
    {
        cout << "Unable to get files in given group List or no group exists" << endl;
    }
    //msg("outside msg");
    // Close the connection
    close(network_socketForlistFiles);
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
    //msg("outside msg");
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
    //msg("outside msg");
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
        //msg("Connection estabilished");
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
    //msg("outside msg");
    // Close the connection
    close(network_socketForlistRequests);
    pthread_exit(NULL);
    return NULL;
}
void *joinGroup(void *ptr)
{
    //user may be already member of group
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    //deb(buffer);
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
        //msg("Connection estabilished");
    }
    // Send data to the Tracker
    int n = write(network_socketForJoinGroup, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing login command to socket");
        exit(0);
    }
    bzero(buffer, MESSAGELEN);
    if (recv(network_socketForJoinGroup, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        deb(buffer);
    }
    if (strcmp(buffer, "Group join Request sent") == 0)
    {
        //unorderedset;
        cout << "Sent join_group Request" << endl;
    }
    else
    {
        cout << "Unable to Send join_group Request" << endl;
    }
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
    string str = (char *)ptr;
    deb(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, (char *)ptr);
    //deb(buffer);

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
    //bzero(buffer, MESSAGELEN);
    //string commands = str;
    //strcpy(buffer, str.c_str());
    string groupId;
    string word;
    int i = 13;
    for (; buffer[i] != ' '; i++)
    {
        word += buffer[i];
    }
    groupId = word;
    int n = write(network_socketForCreateGroup, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR writing Create_group command to Tracker");
        exit(0);
    }
    bzero(buffer, MESSAGELEN);
    //char server_reply[MESSAGELEN];
    //Receive a reply from the server
    //string reply = server_reply;
    if (recv(network_socketForCreateGroup, buffer, MESSAGELEN, 0) < 0)
    {
        msg("recv failed");
    }
    else
    {
        deb(buffer);
    }
    if (strcmp(buffer, "Group Created") == 0)
    {
        //buffer; ///////////////////////
        //string word = "";
        //string groupId;
        //int i = 13;
        //for (; buffer[i] != ' '; i++)
        //{
        //    word += buffer[i];
        //}
        //groupId = word;
        //cout << "group id extracted is:" << word << endl;
        mygroups.insert(groupId);
        //unorderedset;
        cout << "Group created: " << groupId << endl;
    }
    else
    {
        cout << "Unable to create Group" << endl;
    }
    //msg("outside msg");
    // Close the connection
    close(network_socketForCreateGroup);
    pthread_exit(NULL);
    return NULL;
}

void *downloadFileHelper_Single(void *ptr)
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
    //char server_reply[MESSAGELEN];
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
    //string destn = "PQR.TXT";
    //str += "sdfgv";
    //unsigned long long offset;
    str = "Part" + filePart + "__" + fileName;
    // Close the connection
    //int fin, fout, nread;
    //fout = open((str).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //FILE *fp;
    //fp = fopen(str.c_str(), "wb");
    //ofstream myfile;
    str = "Parts" + filePart + "__" + fileName;
    //myfile.open(str.c_str(), ios::binary | ios::out);
    //char write_fileName[24]; //="downloaded_file.pdf";
    //strcpy(write_fileName, "downloaded_file.pdf");
    char b[512 * 1024];
    unsigned long long offset_pos = 0;
    unsigned long long r;

    int fd_write;
    fd_write = open(str.c_str(), O_CREAT | O_WRONLY, 0777);
    if (fd_write == -1)
    {
        cout << "Error Occured opening file" << endl;
    }

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
        //msg("Inside while true");
        bzero(b, MESSAGELEN);
        int len = recv(network_socket, b, MESSAGELEN, 0);
        //appraoch 2 send length also
        if (filePart == "1")
        {
            cout << len << endl;
        }
        //cout << "after read";
        //msg("after read");
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
            string resposne = b;
            //cout << resposne << endl;
            //if (resposne == "NULLS")
            if (strcmp(resposne.c_str(), "NULLS") == 0)
            {
                cout << resposne << endl;
                break;
            }
            else
            {
                //myfile << server_reply;
                //len
                //fwrite(resposne.c_str(), resposne.length(), 1, fp);
                //fwrite(resposne.c_str(), len, 1, fp);
                unsigned long long rs = pwrite(fd_write, b, len, offset_pos);
                //unsigned long long rs = pwrite(fd_write, resposne.c_str(), len, offset_pos);
                offset_pos += rs;
                //deb(server_reply);
                //cout << "I am Here";
                //msg("I am Here");
            }
        }
    }
    //myfile.close();
    //fclose(fp);
    close(fd_write);
    //sleep(2000);
    sleep(2);
    cout << "///////////////////////";
    cout << "File Part recieving complete" << endl;
    close(network_socket);
    pthread_exit(NULL);
    return NULL;
}
void *downloadFileHelper_Multi(void *ptr)
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
    int start_Chunk;
    int end_Chunk;
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
                else if (i == 4)
                {
                    numberOfParts = stoi(word);
                }
                else if (i == 5)
                {
                    start_Chunk = stoi(word);
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
        end_Chunk = stoi(word);
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

    string strs = filePart + " " + fileName + " " + to_string(numberOfParts) + " " + to_string(start_Chunk) + " " + to_string(end_Chunk);
    //string strs = fileName + "_" + to_string(numberOfParts);
    //tell other peer which file to download
    strcpy(buffer, strs.c_str());
    int n = write(network_socket, buffer, strlen(buffer));
    if (n < 0)
    {
        msg("ERROR: writing to socket failed");
        exit(0);
    }
    //msg("Check Point:before first read");
    //char server_reply[MESSAGELEN];
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
    //string destn = "PQR.TXT";
    //str += "sdfgv";
    //unsigned long long offset;
    str = "Part" + filePart + "__" + fileName;
    //str = "Part__" + fileName;
    // Close the connection
    //int fin, fout, nread;
    //fout = open((str).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //FILE *fp;
    //fp = fopen(str.c_str(), "wb");
    //ofstream myfile;
    //str = "Parts" + filePart + "__" + fileName;
    str = "Parts__" + fileName;
    //myfile.open(str.c_str(), ios::binary | ios::out);
    //char write_fileName[24]; //="downloaded_file.pdf";
    //strcpy(write_fileName, "downloaded_file.pdf");
    char b[512 * 1024];
    bzero(b, MESSAGELEN);
    //unsigned long long offset_pos = 0;
    int len = recv(network_socket, b, MESSAGELEN, 0);
    string Fsize = b;
    unsigned long long filesz = stoi(Fsize);
    bzero(b, MESSAGELEN);
    cout << "Recieved File Size is:" << filesz << endl;
    unsigned long long offset_pos = start_Chunk * 1024 * 512;
    //offset_pos = ceil((filesz * ((double)stoi(filePart) - 1)) / (double)numberOfParts);
    //offset_pos = ceil((filesz * ((double)stoi(filePart) - 1)) / ((double)numberOfParts * 512 * 1024));
    unsigned long long r;
    int fd_write;
    fd_write = open(str.c_str(), O_CREAT | O_WRONLY, 0777);
    if (fd_write == -1)
    {
        cout << "Error Occured opening file" << endl;
    }

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
        //msg("Inside while true");
        bzero(b, MESSAGELEN);
        int len = recv(network_socket, b, MESSAGELEN, 0);
        //appraoch 2 send length also
        //if (filePart == "1")
        //{
        cout << filePart << " Sent :" << len << endl;
        //}

        //cout << "after read";
        //msg("after read");
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
            string resposne = b;
            //cout << resposne << endl;
            //if (resposne == "NULLS")
            if (strcmp(resposne.c_str(), "NULLS") == 0)
            {
                cout << resposne << endl;
                break;
            }
            else
            {
                //myfile << server_reply;
                //len
                //fwrite(resposne.c_str(), resposne.length(), 1, fp);
                //fwrite(resposne.c_str(), len, 1, fp);
                unsigned long long rs;
                //if (offset_pos < (end_Chunk * 1024 * 512))
                //if ((offset_pos + len) < ((end_Chunk + 1) * 1024 * 512))
                rs = pwrite(fd_write, b, len, offset_pos);
                //else
                //{
                //    break;
                //}

                //unsigned long long rs = pwrite(fd_write, resposne.c_str(), len, offset_pos);
                offset_pos += rs;
                //deb(server_reply);
                //cout << "I am Here";
                //msg("I am Here");
                if ((offset_pos >= (end_Chunk + 1) * 1024 * 512) || (offset_pos >= filesz))
                    break;
            }
        }
    }
    //myfile.close();
    //fclose(fp);
    close(fd_write);
    //sleep(2000);
    sleep(2);
    cout << "///////////////////////";
    cout << "File Part recieving complete" << endl;
    close(network_socket);
    pthread_exit(NULL);
    return NULL;
}
void *downloadFileSingle(void *ptr)
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
    if (word != "")
        DestinPaths = word;
    deb(DestinPaths);
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
        pthread_create(&threads[i], NULL, downloadFileHelper_Single, ptrs);
        temps = "";
        usleep(10000);
    }

    // joining 4 threads i.e. waiting for all 4 threads to complete
    for (int i = 0; i < numberOfDestnPorts; i++)
        pthread_join(threads[i], NULL);
    cout << "Did I ever came here?" << endl;
    //Merge parts
    pthread_exit(NULL);
    return NULL;
}
void *downloadFileMulti(void *ptr)
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
    if (word != "")
        DestinPaths = word;
    deb(DestinPaths);
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
    string file_sizesese;
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

        if (recv(network_socketForDownloadFileInfo, server_reply, MESSAGELEN, 0) < 0)
        {
            msg("unable to get port number of people who have my file");
        }
        else
        {
            //strcmp(portNumberFRomWhereToDownload, server_reply);
            file_sizesese = server_reply;
        }
        deb(file_sizesese);
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
    int how_manyChunk_each_peer_will_upload;
    how_manyChunk_each_peer_will_upload = ceil((double)stod(file_sizesese) / (double)(numberOfDestnPorts * 1024 * 512));
    deb(how_manyChunk_each_peer_will_upload);
    int start_chunk = 0;
    int end_chunk = how_manyChunk_each_peer_will_upload - 1;
    pthread_t threads[numberOfDestnPorts];
    // Creating 4 threads
    string temps;
    //vector<string> arr;
    vector<char *> arr;
    char *ptrs;
    int max_number_ofChunks = ceil((double)stod(file_sizesese) / (double)(1024 * 512));
    if (stol(file_sizesese) % (512 * 1024) != 0)
    {
        max_number_ofChunks++;
    }
    int j = 0;
    for (unsigned int i = 0; i < numberOfDestnPorts; i++)
    {
        temps = to_string(i + 1) + " " + fileName + " " + portAddressOfPeopleHavingMyFile[i] + " " + to_string(numberOfDestnPorts) + ' ' + to_string(start_chunk) + ' ' + to_string(end_chunk);
        //cout << temps << endl;
        //arr.push_back(temps);
        ptrs = (char *)malloc(MESSAGELEN * sizeof(char));
        arr.push_back(ptrs);
        strcpy(ptrs, temps.c_str());
        //pthread_create(&threads[i], NULL, downloadFileHelper_Multi, &temps);
        //pthread_create(&threads[i], NULL, downloadFileHelper_Multi, ptrs);
        pthread_create(&threads[i], NULL, downloadFileHelper_Single, ptrs);
        temps = "";
        usleep(10000);
        start_chunk = end_chunk + 1;
        end_chunk = end_chunk + how_manyChunk_each_peer_will_upload;
        if (end_chunk > max_number_ofChunks)
        {
            //end_chunk = max_number_ofChunks;
            end_chunk = max_number_ofChunks - 1;
        } //handling for last chunk
        j++;
        if (start_chunk >= max_number_ofChunks)
            break;
    }

    // joining 4 threads i.e. waiting for all 4 threads to complete
    for (int i = 0; i < j; i++)
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
    string imput;
    string temp;
    cout << "Enter Command";
    cin >> imput;
    if (imput == "create_user")
    { //b. Create User Account: create_user <user_id> <passwd>
        char *arr = NULL;
        string user_id_create; //this must not be too long
        string passwd;
        cout << "Enter <user_id> <passwd>" << endl;
        cin >> user_id_create >> passwd;
        temp = imput + ' ' + user_id_create + ' ' + passwd + '\0';
        //deb(temp);
        arr = (char *)malloc(temp.length() * sizeof(char));
        if (arr == NULL)
        {
            cout << "Insufficient Memory, Unable to allocate\n";
        }
        else
        {
        }
        strcpy(arr, temp.c_str());
        //deb((char *)arr);
        pthread_create(&handlingInputTid, NULL, createUser, arr);
        usleep(200);
    }
    else if (imput == "login")
    { // c. Login: login <user_id> <passwd>
        char *arr = NULL;
        commandwasLogin = true;
        string passwd;
        cout << "Enter <user_id> <passwd>" << endl;
        cin >> user_id >> passwd;
        temp = imput + ' ' + to_string(user_id) + ' ' + passwd + '\0';
        //deb(temp);
        arr = (char *)malloc(temp.length() * sizeof(char));
        if (arr == NULL)
        {
            cout << "Insufficient Memory, Unable to allocate\n";
        }
        else
        {
        }
        strcpy(arr, temp.c_str());
        //deb((char *)arr);
        pthread_create(&handlingInputTid, NULL, loginUser, arr);
        usleep(200);
    }
    else if (imput == "create_group")
    { // d. Create Group: create_group <group_id>
        if (loginSucess == true)
        {
            int group_id;
            char *arr = NULL;
            cout << "Enter <group_id>\n";
            cin >> group_id;
            temp = imput + ' ' + to_string(group_id) + ' ' + to_string(user_id) + '\0';
            deb(temp);
            arr = (char *)malloc(temp.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            strcpy(arr, temp.c_str());
            deb((char *)arr);
            pthread_create(&handlingInputTid, NULL, createGroup, arr);
            usleep(200);
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "join_group")
    { // e. Join Group: join_group <group_id>
        if (loginSucess == true)
        {
            char *arr = NULL;
            int group_id_to_join;
            cout << "Enter <group_id>\n";
            cin >> group_id_to_join;
            temp = imput + ' ' + to_string(group_id_to_join) + ' ' + to_string(user_id) + '\0';
            arr = (char *)malloc(temp.length() * sizeof(char)); ////////////untested
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            strcpy(arr, temp.c_str());
            //deb((char *)arr);
            pthread_create(&handlingInputTid, NULL, joinGroup, arr);
            usleep(200);
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "leave_group")
    { // f. Leave Group: leave_group <group_id>
        if (loginSucess == true)
        {
            ///////////////////
            char *arr = NULL;
            int group_id_to_leave;
            //cin >> group_id_to_leave;
            group_id_to_leave = 12323;
            temp = imput + " " + to_string(group_id_to_leave) + " " + to_string(user_id) + '\0';
            //cout << temp;
            arr = (char *)malloc(temp.length() * sizeof(char)); ///untested
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            strcpy(arr, temp.c_str());
            /////////////////
            //cout << temp;
            /////allocate space for string from heap then send
            pthread_create(&handlingInputTid, NULL, leaveGroup, arr);
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "list_requests")
    { // g. List pending join: list_requests <group_id>
        if (loginSucess == true)
        {
            char *arr = NULL;
            int group_id_to_list;
            cout << "Enter <groupId>" << endl;
            cin >> group_id_to_list;
            temp = imput + ' ' + to_string(group_id_to_list) + '\0';
            arr = (char *)malloc(temp.length() * sizeof(char)); ////////////untested
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            strcpy(arr, temp.c_str());
            pthread_create(&handlingInputTid, NULL, listRequests, arr);
            usleep(200);
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "accept_request")
    { // h. Accept Group Joining Request: accept_request <group_id> <user_id>
        if (loginSucess == true)
        {
            ///////user may be owner of multiple groups
            char *arr = NULL;
            int user_id_to_accept;
            int group_id;
            cout << "Enter <GroupID> <UserID>" << endl;
            cin >> group_id >> user_id_to_accept;
            //user_id = 234;
            //group_id = 12323;
            if (arr != NULL)
                free(arr);
            temp.clear();
            //temp = imput + " " + to_string(group_id) + " " + to_string(user_id) + " q";
            temp = imput + ' ' + to_string(group_id) + ' ' + to_string(user_id_to_accept) + '\0';
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
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "list_groups")
    { // i. List All Group In Network: list_groups
        if (loginSucess == true)
        {
            ///////user may be owner of multiple groups
            //string temp = " ";
            //cout << temp;
            /////allocate space for string from heap then send
            char *arr = NULL;
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
        if (loginSucess == true)
        {
            int group_id;
            char *arr = NULL;
            //cout << "Enter Group ID:";
            //cin >> group_id;
            group_id = 12323;
            //string temp;
            //cout << temp;
            temp = imput + ' ' + to_string(group_id);
            deb(temp);
            //arr = (char *)malloc(sizeof(temp.length() * sizeof(char)));
            arr = (char *)malloc(temp.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            strcpy(arr, temp.c_str());
            /////allocate space for string from heap then send
            pthread_create(&handlingInputTid, NULL, listFiles, arr);
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "upload_file")
    { // k. Upload File: upload_file <file_path> <group_id>
        if (loginSucess == true)
        {
            int group_id;
            char *arr = NULL;
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
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "download_file")
    { // l. Download File: download_file <group_id> <file_name> <destination_path>
        //loginSucess = true;
        if (loginSucess == true)
        {
            commandwasDownload = true;
            int group_id;
            char *arr = NULL;
            string fileName, dest_path;
            //cout << "Tell GroupID, FileName, Destination" << endl;
            //cin >> group_id>> file_name >>destination_path;
            /*
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            */
            group_id = 12323;
            fileName = "test.pdf";
            cout << fileName << endl;
            dest_path = "sdgf/ds";
            temp = to_string(group_id) + ' ' + fileName + ' ' + dest_path;
            //cout << temp;
            /////allocate space for string from heap then send
            //pthread_create(&handlingDownloadFile, NULL, downloadFileMulti, &temp);
            pthread_create(&handlingDownloadFile, NULL, downloadFileSingle, &temp);
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "logout")
    { // m. Logout: logout
        //char *arr;
        if (loginSucess == true)
        {
            //can a client login to ultiple groups?
            loginSucess = false;
            deb(loginSucess);
            char *arr = NULL;
            //inform tracker Thta I am not active

            //int currentGroupID = 12323;
            //temp=
            temp.clear();
            temp = imput + ' ' + to_string(user_id) + '\0';
            deb(temp);
            //cout << temp;
            // arr = (char *)malloc(sizeof(temp.length() * sizeof(char)));
            arr = (char *)malloc(temp.length() * sizeof(char));
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            strcpy(arr, temp.c_str());

            //string temp = to_string(currentGroupID);
            pthread_create(&handlingInputTid, NULL, logOut, arr);
            //inform tracker that user in not available now
        }
        else
        {
            msg("You Need to Login First\n");
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
            msg("You Need to Login First\n");
        }
    }
    else if (imput == "stop_share")
    { // o. Stop sharing: stop_share <group_id> <file_name>
        if (loginSucess == true)
        {
            char *arr = NULL;
            if (arr == NULL)
            {
                cout << "Insufficient Memory, Unable to allocate\n";
            }
            else
            {
            }
            int currentGroupID = 12323;
            string temp = to_string(currentGroupID);
            //pthread_create(&handlingInputTid, NULL, stopShare, &temp);
            //inform tracker that user in not sharing now
        }
        else
        {
            msg("You Need to Login First\n");
        }
    }
    else
    {
        msg("Command Not Found");
    }
    //cout << "I came here" << endl;
    if (commandwasDownload)
        pthread_join(handlingDownloadFile, NULL);
    if (commandwasLogin)
        pthread_join(handlingInputTid, NULL);
    //cout << "But I didnt came here" << endl;
    /*if (arr != NULL)
        free(arr);*/
}
void *FileSendToPeerSingle(void *ptr)
{
    init();
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    int opt = 1;
    socklen_t addr_size;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_port = htons(PORTNUM);
    //send from this port to peer
    //this port number can be obtained from tracker
    //tracker will be holding port number of Other Peers
    //Bind the socket to the
    //address and port number.
    serverAddr.sin_port = htons(mySendingPortAddr);
    int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (n < 0)
    {
        cout << "\nPort is not Available , trying a random port#" << endl;
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
                deb(mySendingPortAddr);
                break;
            }
        }
    }
    else
    {
        cout << "\nPort was Available using now#" << endl;
    }

    //////////////////////////////////
    //int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    // Listen on the socket,
    // with 100 max connection
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
    pthread_t tid[60]; //At max 60 threads possible
    int i = 0;
    while (1)
    {
        addr_size = sizeof(serverStorage);
        // Extract the first connection in the queue
        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
        int choice = 0;
        char choices;
        deb(serverStorage.__ss_align);
        deb(serverStorage.__ss_padding);
        deb(serverStorage.ss_family);
        deb(addr_size);
        char buffer[MESSAGELEN];
        bzero(buffer, MESSAGELEN);
        int n = read(newSocket, buffer, MESSAGELEN);
        if (n < 0)
        {
            cout << "ERROR reading(File Name for Download) from socket";
            exit(0);
        }
        printf("Requested File Chunk for download: %s\n", buffer);
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
        fclose(fp);
        char b[MESSAGELEN];
        bzero(b, MESSAGELEN);

        //////////////////////////////////////
        int fd_read;
        unsigned long long offset_pos = 0;
        fd_read = open(fileName.c_str(), O_RDONLY);
        cout << "readfd" << fd_read << endl;
        if ((fd_read != -1))
        {
            while (true)
            {
                bzero(b, MESSAGELEN);
                unsigned long long r = pread(fd_read, b, 512 * 1024, offset_pos);
                if (r <= 0)
                {
                    cout << r << endl;
                    break;
                }
                cout << r << endl;
                write(newSocket, b, r);
                offset_pos += r;
            }
        }
        close(fd_read);
        string eofSignal = "NULLS";
        strcpy(b, eofSignal.c_str());
        //write(newSocket, eofSignal.c_str(), MESSAGELEN);
        if (0 > write(newSocket, b, 5))
        {
            cout << "last write (NULLS) failed";
        }
    }
}
void *FileSendToPeerMulti(void *ptr)
{
    init();
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    int opt = 1;
    socklen_t addr_size;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

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
    /////////////////////////

    // serverAddr.sin_port = htons(mySendingPortAddr);
    // int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    // if (n < 0)
    // {
    //     cout << "Port is not Available , trying a random port" << endl;
    //     while (true)
    //     {
    //         port = (rand() % (upper - lower + 1)) + lower;
    //         //serverAddr.sin_port = htons(PORTNUM);
    //         serverAddr.sin_port = htons(port);
    //         int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    //         if (n < 0)
    //         {
    //             continue;
    //         }
    //         else
    //         {
    //             mySendingPortAddr = port;
    //             deb(mySendingPortAddr);
    //             break;
    //         }
    //     }
    // }
    // else
    // {
    //     cout << "Port was Available using now" << endl;
    // }
    ////////////////////////

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
        string str = buffer;
        int filePart;
        string fileName;
        int numberOfParts;
        int startChunk;
        int endChunk;
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
                    else if (i == 3)
                    {
                        numberOfParts = stoi(word);
                    }
                    else if (i == 4)
                    {
                        startChunk = stoi(word);
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
            endChunk = stoi(word);
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
        fclose(fp);
        char b[MESSAGELEN];
        bzero(b, MESSAGELEN);
        string file_lengths = to_string(sz);
        strcpy(b, file_lengths.c_str());
        write(newSocket, b, MESSAGELEN);
        cout << "sent File Size" << endl;
        //////////////////////////////////////
        int fd_read;
        //from file Name find path
        fd_read = open(fileName.c_str(), O_RDONLY);
        cout << "readfd" << fd_read << endl;
        if ((fd_read != -1))
        {
            unsigned long long offset_pos = startChunk * (512 * 1024);
            unsigned long long end_offset_pos = endChunk * (512 * 1024);
            while (offset_pos <= end_offset_pos && (offset_pos < sz))
            //while (offset_pos <= end_offset_pos || (offset_pos < sz))
            //unsigned long long end_offset_pos = (endChunk + 1) * (512 * 1024);
            //while (offset_pos < end_offset_pos && (offset_pos < sz)) ////////////////////////////////////////changes here
            { //above may give issue
                bzero(b, MESSAGELEN);
                unsigned long long r = pread(fd_read, b, 512 * 1024, offset_pos);
                offset_pos += r;
                if (r <= 0)
                { //r <= 0 handles if end of file has occured without reading the entire block
                    cout << r << endl;
                    break;
                }
                write(newSocket, b, r);
            }
        }
        close(fd_read);
        cout << "Check point after read" << endl;
        string eofSignal = "NULLS";
        bzero(b, MESSAGELEN);
        strcpy(b, eofSignal.c_str());
        //write(newSocket, eofSignal.c_str(), MESSAGELEN);
        if (0 > write(newSocket, b, 5))
        {
            cout << "last write (NULLS) failed";
        }
    }
}
void get_Downloaded_list() /////////////////////////////////////////////////////////pending
{
    FILE *infile;
    string fileName = to_string(user_id) + "_downloadLog.dat";
    struct downloadedFiles compl_Download;
    char DownloadedFileName[64];
    //struct groupJoinRequests groupJoinRequeststemp;
    infile = fopen(fileName.c_str(), "r");
    if (infile == NULL)
    {
        //cout << "User:" << user_id << " has not downloaded any files" << endl;
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
int main(int argc, char **argv)
{
    // a. ./client <IP>:<PORT> tracker_info.txt
    getTrackerDetails(argc, argv);
    string fileName;
    pthread_t Sendertid;
    //pthread_create(&Sendertid, NULL, FileSendToPeerMulti, &fileName);
    pthread_create(&Sendertid, NULL, FileSendToPeerSingle, &fileName);
    //run server for sending
    while (true)
        getInput();
}