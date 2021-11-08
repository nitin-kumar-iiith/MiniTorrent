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
#define msg(x) cout << x << endl;
// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;
int serverSocket;
struct person //used to get authentication details from file
{
    int user;
    char passwd[32];
};
struct Create_User
{
    int user;
    string passwd;
};
vector<Create_User> auth;
map<int, string> authentication;
map<int, int> mapUseridSendingPort;
map<string, string> mapCompletefileNameUserID;
unordered_set<int> activeuser;
void exit()
{
    close(serverSocket);
}
void init(int choice)
{
    if (choice == 1)
    {
        MeraPortNumber = 8787;
        UskaPortNumber = 8788;
    }
    else if (choice == 2)
    {
        MeraPortNumber = 8788;
        UskaPortNumber = 8787;
    }
    atexit(exit);
}
// Reader Function
void *connectToOtherThread(void *param)
{
    string str = *reinterpret_cast<string *>(param);
    return NULL;
}

// Writer Function
void *writer(void *param)
{
    return NULL;
}
void getTrackerDetails(int argc, char **argv)
{
    //IP Format: ./client tracker_info.txt
    if (argc < 3)
    {
        msg("Insuffiecient Amount of parameter given, exiting");
        exit(1);
    }
    fstream file;
    string word, t, q, filename;
    //string trackerFileName = "tracker_info.txt";
    string trackerFileName = argv[1];
    // opening file
    file.open(trackerFileName.c_str());
    if (file >> word)
    {
        UskaPortNumber = stoi(word);
        deb(UskaPortNumber);
    } //write your port number at the end
}
void inputTake()
{
    string command;
    //cin >> command;
    command = "Chalo chale mitwa in bhegi bhegi raho me";
    if (command == "quit")
    {
        //inform other Tracker, Exiting/////////////////////
        UskaPortNumber;
    }
}
void *createUserFunction(void *ptr)
{ //user may already exist
    string str = *reinterpret_cast<string *>(ptr);
    int newSocket = stoi(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    cout << "command accepted waitng for ramining input\n";
    string client_message = "ok!";
    //client_message.resize(MESSAGELEN, ' ');
    write(newSocket, client_message.c_str(), MESSAGELEN);
    int n = read(newSocket, buffer, MESSAGELEN);
    if (n < 0)
    {
        cout << "ERROR reading from socket";
        exit(0);
    }
    string usedID = buffer;
    n = read(newSocket, buffer, MESSAGELEN);
    if (n < 0)
    {
        cout << "ERROR reading from socket";
        exit(0);
    }
    string passwd = buffer;
    Create_User newUser;
    newUser.user = stoi(usedID);
    newUser.passwd = passwd;
    auth.push_back(newUser);
    //user may already exist
    deb(usedID);
    deb(passwd);
    FILE *outfile;
    // open file for writing
    outfile = fopen("person.dat", "a");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    struct person input;
    // write struct to file
    input.user = newUser.user;
    strcpy(input.passwd, newUser.passwd.c_str());
    fwrite(&input, sizeof(struct person), 1, outfile);
    client_message = "Adding user UnSucessful";
    if (fwrite != 0)
    {
        msg("Added user Sucessfully");
        authentication[input.user] = input.passwd;
        client_message = "Added user Sucessfully";
    }
    else
    {
        msg("error writing file !");
    }

    //////////////////////////inform user about status
    write(newSocket, client_message.c_str(), MESSAGELEN);
    //////////////////////////
    fclose(outfile);
    pthread_exit(NULL);
    return NULL;
}
void *loginUserFunction(void *ptr)
{ //user may already exist
    string str = *reinterpret_cast<string *>(ptr);
    int newSocket = stoi(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    cout << "command accepted waitng for ramining input\n";
    string client_message = "ok! send credentails";
    //client_message.resize(MESSAGELEN, ' ');
    write(newSocket, client_message.c_str(), MESSAGELEN);
    int n = read(newSocket, buffer, MESSAGELEN);
    if (n < 0)
    {
        cout << "ERROR reading userID from socket";
        exit(0);
    }
    string usedID = buffer;
    n = read(newSocket, buffer, MESSAGELEN);
    if (n < 0)
    {
        cout << "ERROR reading Password from socket";
        exit(0);
    }
    string passwd = buffer;
    Create_User newUser;
    newUser.user = stoi(usedID);
    newUser.passwd = passwd;
    //active.push_back(user);//keep it in unordered set
    //user may already exist
    deb(usedID);
    deb(passwd);
    bool loginSucess = false;
    auto itr = authentication.find(newUser.user);
    if (itr != authentication.end() && authentication[newUser.user] == passwd)
    {
        loginSucess = true;
        msg("Login Sucess");
        activeuser.insert(newUser.user);
    }
    else if (itr == authentication.end())
    {
        msg("user not created");
        client_message = "user not created";
        write(newSocket, client_message.c_str(), MESSAGELEN);
    }
    else if (itr != authentication.end())
    {
        msg("wrong password");
        client_message = "wrong password";
        write(newSocket, client_message.c_str(), MESSAGELEN);
    }
    if (loginSucess)
    {
        client_message = "true";
        write(newSocket, client_message.c_str(), MESSAGELEN);
        n = read(newSocket, buffer, MESSAGELEN);
        if (n < 0)
        {
            cout << "ERROR reading sending Port Addr from socket";
            exit(0);
        }
        cout << "\nDuring login recieved userID:" << newUser.user << "On port#" << buffer << endl;
        //we mapped userID on the port on which he is available
        mapUseridSendingPort[newUser.user] = stoi(buffer);
        ////////////////////////////////////////////////last changes were made here, so mistake can be here
    }

    pthread_exit(NULL);
    return NULL;
}
void getAuthDetails()
{
    FILE *infile;
    struct person input;
    struct Create_User temp;
    infile = fopen("person.dat", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return;
    }
    while (fread(&input, sizeof(struct person), 1, infile))
    {
        temp.passwd = input.passwd;
        temp.user = input.user;
        auth.push_back(temp);
    }
    for (unsigned int i = 0; i < auth.size(); i++)
    {
        //cout << auth[i].user << endl
        // << auth[i].passwd << endl;
        authentication[auth[i].user] = auth[i].passwd;
    }
    fclose(infile);
} /*
void ReadFileOwnerMap()
{
    FILE *infile;
    struct person input;
    struct Create_User temp;
    infile = fopen("fileOwner.dat", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return;
    }
    while (fread(&input, sizeof(struct person), 1, infile))
    {
        temp.passwd = input.passwd;
        temp.user = input.user;
        auth.push_back(temp);
    }
    for (unsigned int i = 0; i < auth.size(); i++)
    {
        //cout << auth[i].user << endl
        //     << auth[i].passwd << endl;
        authentication[auth[i].user] = auth[i].passwd;
    }
    fclose(infile);
}*/
void *sendFileInfoForDownload(void *ptr)
{ //user may already exist
    string str = *reinterpret_cast<string *>(ptr);
    int newSocket = stoi(str);
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    cout << "command accepted waitng for ramining input\n";
    string client_message = "ok! send fileName";
    //client_message.resize(MESSAGELEN, ' ');
    write(newSocket, client_message.c_str(), MESSAGELEN);
    int n = read(newSocket, buffer, MESSAGELEN);
    if (n < 0)
    {
        cout << "ERROR reading filename from socket";
        exit(0);
    }
    string filename = buffer;
    deb(filename);
    string userID = mapCompletefileNameUserID[filename];
    char userids[120];
    strcpy(userids, mapCompletefileNameUserID[filename].c_str());
    deb(userids);
    deb(userID);
    string portAddr;
    string word;
    string temp;
    for (auto x : userID)
    {
        if (x == ' ')
        {
            if (word != "")
            {
                temp = to_string(mapUseridSendingPort[stoi(word)]);
                if (temp != "")
                    //portAddr += temp; //pert address must be space seperated
                    portAddr = portAddr + " " + temp; //pert address must be space seperated
            }
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    if (word != "")
    {
        temp = to_string(mapUseridSendingPort[stoi(word)]);
        if (temp != "")
        {
            portAddr = portAddr + " " + temp;
        }
        //portAddr += temp;
    }
    if (portAddr != "")
    {
        write(newSocket, portAddr.c_str(), MESSAGELEN);
    }
    else
    {
        cout << "UserID is null, i.e no port has complete file";
    }
    ////////////////////////////////////////////////last changes were made here, so mistake can be here
    pthread_exit(NULL);
    return NULL;
}
struct FileUserID
{
    char fileName[32];
    char assocaitedUserName[200];
};

void ReadFileOwnerMap()
{
    FILE *infile;
    struct FileUserID mapFileUsers;
    infile = fopen("fileOwner.dat", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return;
    }
    while (fread(&mapFileUsers, sizeof(struct FileUserID), 1, infile))
    {
        //cout << "FileName is:" << mapFileUsers.fileName << " is present with " << mapFileUsers.assocaitedUserName;
        mapCompletefileNameUserID[mapFileUsers.fileName] = mapFileUsers.assocaitedUserName;
    }
    for (auto itr = mapCompletefileNameUserID.begin(); itr != mapCompletefileNameUserID.end(); itr++)
    {
        cout << itr->first << " is present with " << itr->second << endl;
    }
    fclose(infile);
}
// Driver Code
int main(int argc, char **argv)
{
    // Initialize variables
    //init(stoi(argv[2]));
    init(1);
    //getTrackerDetails(argc, argv);
    getAuthDetails();
    //inputTake();
    ReadFileOwnerMap();

    //////////////////////////setup connection
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    serverAddr.sin_port = htons(MeraPortNumber); //start server at this port

    // Bind the socket to the address and port number.
    int n = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (n < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
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
    pthread_t handlingCreateUser;
    pthread_t handlingLoginUser;
    pthread_t handlingDownloadUserFileInfo;
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
        //string client_message = "abcAbCsbdoclsnfdcklsjdnckvh bsdlcv kl";
        //client_message.resize(MESSAGELEN, ' ');
        //write(newSocket, client_message.c_str(), MESSAGELEN);

        char buffer[MESSAGELEN];
        bzero(buffer, MESSAGELEN);
        int n = read(newSocket, buffer, MESSAGELEN);
        if (n < 0)
        {
            cout << "ERROR reading from socket";
            exit(0);
        }
        string command_recieved = buffer;
        printf("Here is the message: %s\n", buffer); //identify command and call respective function using thread
        bool createUserFlag = false;
        bool loginUserFlag = false;
        //bool createUserFlag = false;
        //bool createUserFlag = false;
        //bool createUserFlag = false;
        //bool createUserFlag = false;
        //bool createUserFlag = false;
        bool downloadFileInfoFlag = false;
        if (strcmp(buffer, "create_user") == 0)
        {
            string temp = to_string(newSocket);
            pthread_create(&handlingCreateUser, NULL, createUserFunction, &temp);
            createUserFlag = true;
            //deb(auth[0].passwd);
            //deb(auth[0].user);
        }
        else if (strcmp(buffer, "login") == 0)
        {
            loginUserFlag = true;
            string temp = to_string(newSocket);
            pthread_create(&handlingLoginUser, NULL, loginUserFunction, &temp);
        }
        else if (strcmp(buffer, "downloadFileInfo") == 0)
        {
            downloadFileInfoFlag = true;
            string temp = to_string(newSocket);
            pthread_create(&handlingDownloadUserFileInfo, NULL, sendFileInfoForDownload, &temp);
        }
        else
        {
            msg("bhai tera command match nhi hua khi pe");
        }
        if (loginUserFlag == true)
            pthread_join(handlingLoginUser, NULL);
        if (createUserFlag == true)
            pthread_join(handlingCreateUser, NULL);
        if (downloadFileInfoFlag == true)
            pthread_join(handlingDownloadUserFileInfo, NULL);

        cout << "Diaplying all the user details\n";
        for (unsigned int i = 0; i < auth.size(); i++)
        {
            cout << auth[i].user << endl;
            cout << auth[i].passwd << endl;
        }
        //usleep(microseconds);
        // char b[1024];
        // int fin, fout, nread;
        // fin = open(buffer, O_RDONLY);
        // if (fin != 0)
        // {
        //     cout << "File Open Failed";
        // }
        // while ((nread = read(fin, b, sizeof(b))) > 0)
        // {
        //     //write(fout, b, nread);
        //     write(newSocket, b, MESSAGELEN); //change write length here
        // }
        // string eofSignal = "NULLS";
        // write(newSocket, eofSignal.c_str(), MESSAGELEN);
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
            cout << "glt jagah aa gya mai bro";
            if (pthread_create(&readerthreads[i++], NULL, connectToOtherThread, &newSocket) != 0)

                // Error in creating thread
                printf("Failed to create thread\n");
        }
        else if (choice == 2)
        {
            // Create writers thread
            cout << "glt jagah aa gya mai bro";
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
