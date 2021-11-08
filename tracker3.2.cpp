//tracker
// inet_addr
//void *leaveGrpFunction(void *ptr) untested
//line 1086
//pending work is to uplaod file
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
struct groupOwners //used to get authentication details from file
{
    char usrId[64];
    char grpId[64];
};
struct groupJoinRequests //used to get authentication details from file
{
    char grpId[32];
    char usrIds[64];
};
vector<Create_User> auth;
map<int, string> authentication;
map<int, int> mapUseridSendingPort; //checkinh avaliable or not
map<string, string> infoFilessharing;
//download file(if file owner has not left): mapCompletefileNameUserID using this get all
//userIDs(say a) [These users can be in multiple(different also groups)]
//if it is not empty then using mapGroupItsUsers & group ID get the userIDs in that group
//(say b) now,( a intersection b ) are those people who in my group and have my requested
//file .if it is not void then those set of people are in my group and have file also
//so when a user leaves a group just remove his ID from the string given by "mapGroupItsUsers[groupID]"

map<string, int> mapuserIDSharingFiles; /////before sending any one the port address from where to download
//check if he is sharing the file or not
map<string, string> mapCompletefileNameUserID;
map<string, string> mapUserIDGroupID; //this user is owner of group
map<string, string> mapGroupIDUserID; //this group is owned by this user
map<string, string> mapGroupItsUsers; //this group has these many people(excluding admin)
unordered_set<int> activeuser;
map<string, string> groupMembers;
map<string, string> mapPendingReqGroupIDUserIDs;
map<string, string> mapGroupsSharableFiles; // tells group has which sharable files
map<string, string> mapfilesDestination;    //which file is onn which location
map<string, string> mapfileSHA1Complete;    //SHA! of entire file
map<string, vector<string>> mapfilePieceWiseSHA;
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
void *leaveGrpFunction(void *ptr)
{
    //goto <userID,associted grp> and remove entry
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    //socketaddr = buffer;
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is leave_group" << endl;
    int j = i + 12 + 1;
    string grpID;
    string usrID;
    for (; buffer[j] != ' '; j++)
    {
        grpID += buffer[j];
    }
    deb(grpID);
    for (; buffer[j]; j++)
    {
        usrID += buffer[j];
    }
    deb(usrID);
    string allmemberofGrouup;
    if (mapGroupItsUsers.find(grpID) == mapGroupItsUsers.end())
    {
        cout << "user not present in map in first place" << endl;
        cout << "remaining group members are as follows\n";
        for (auto itr = mapGroupItsUsers.begin(); itr != mapGroupItsUsers.end(); itr++)
        {
            cout << itr->first << " " << itr->second << endl;
        }
        pthread_exit(NULL);
        return NULL;
    }
    else
    {
        cout << "User is part of the group" << endl;
        // allmemberofGrouup = mapGroupItsUsers[grpID];
    }
    cout << "I am here, check point1" << endl;
    auto qtr = mapGroupItsUsers.find(grpID);
    cout << "I am here, check point2" << endl;
    cout << qtr->first << " " << qtr->second << endl;
    cout << "I am here, check point3" << endl;
    //string allmemberofGrouup = mapGroupItsUsers[grpID];
    allmemberofGrouup = qtr->second;
    ///////////////////////////////////////////////segment fault here
    cout << "I am here, check point4" << endl;
    //remove "usrID" from allmemberofGrouup
    //string pendingRequestUsrIDs;
    //pendingRequestUsrIDs = mapPendingReqGroupIDUserIDs[grpID];
    //deb(pendingRequestUsrIDs);
    bool found = false;
    cout << "I am here, check point5" << endl;
    // Check if the word is present in string
    // If found, remove it using removeAll()
    cout << "I am here, check point6" << endl;
    cout << "Initially group contains " << allmemberofGrouup;
    cout << "I am here, check point7" << endl;
    //cout << "Initially group contains ";
    if (allmemberofGrouup.find(usrID) != string::npos) //////segment fault
    {
        size_t p = -1;

        // To cover the case
        // if the word is at the
        // beginning of the string
        // or anywhere in the middle
        string tempWord = usrID + " ";
        cout << tempWord << "_" << endl;
        while ((p = allmemberofGrouup.find(usrID)) != string::npos)
        {
            found = true;
            allmemberofGrouup.replace(p, tempWord.length(), "");
        }

        // To cover the edge case
        // if the word is at the
        // end of the string
        tempWord = " " + usrID;
        cout << tempWord << "_" << endl;
        while ((p = allmemberofGrouup.find(usrID)) != string::npos)
        {
            found = true;
            allmemberofGrouup.replace(p, tempWord.length(), "");
        }
    }
    mapGroupItsUsers[grpID] = allmemberofGrouup;
    // Return the resultant string
    string status;
    if (found)
    {
        status = "user left Group";
        write(newSocket, status.c_str(), MESSAGELEN);
        string temps = mapGroupItsUsers[grpID];
        cout << "Now group contains " << temps;
        cout << "I am here, check point8" << endl;
        //mapGroupItsUsers[grpID] = temps + " " + usrID;
        //cout << "Now the group Members are:" << temps + " " + usrID;
    }
    else
    {
        status = "user not part of group in first Place";
        write(newSocket, status.c_str(), MESSAGELEN);
    }
    pthread_exit(NULL);
    return NULL;
}
void *logoutFunction(void *ptr)
{ //user may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    //socketaddr = buffer;
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is logout" << endl;
    int j = i + 7 + 1;
    string usrID;
    for (; buffer[j]; j++)
    {
        usrID += buffer[j];
    }
    deb(usrID);
    int userIDToLogout = stoi(usrID);
    //dszfv;
    auto itr = activeuser.find(userIDToLogout);
    string status;
    if (itr != activeuser.end())
    {
        activeuser.erase(userIDToLogout);
        auto jtr = activeuser.find(userIDToLogout);
        if (jtr == activeuser.end())
        {
            status = "user logged out";
        }
        else
        {
            status = "logout failed";
        }
    }
    else if (itr == activeuser.end())
    {
        status = "user not logined in first Place";
    }
    cout << "List of active user are :";
    for (auto jtr = activeuser.begin(); jtr != activeuser.end(); jtr++)
    {
        cout << *jtr << " ";
    }
    deb(status);
    write(newSocket, status.c_str(), MESSAGELEN);
    pthread_exit(NULL);
    return NULL;
}
void *acceptRequestsFunction(void *ptr)
{ //user may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    //socketaddr = buffer;
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is accept_request" << endl;
    int j = i + 15 + 1;
    string grpID;
    string usrID;
    for (; buffer[j] != ' '; j++)
    {
        grpID += buffer[j];
    }
    deb(grpID);
    for (; buffer[j]; j++)
    {
        usrID += buffer[j];
    }
    deb(usrID);
    string pendingRequestUsrIDs;
    pendingRequestUsrIDs = mapPendingReqGroupIDUserIDs[grpID];
    deb(pendingRequestUsrIDs);
    bool found = false;
    // Check if the word is present in string
    // If found, remove it using removeAll()
    if (pendingRequestUsrIDs.find(usrID) != string::npos)
    {
        size_t p = -1;

        // To cover the case
        // if the word is at the
        // beginning of the string
        // or anywhere in the middle
        string tempWord = usrID + " ";
        while ((p = pendingRequestUsrIDs.find(usrID)) != string::npos)
        {
            found = true;
            pendingRequestUsrIDs.replace(p, tempWord.length(), "");
        }

        // To cover the edge case
        // if the word is at the
        // end of the string
        tempWord = " " + usrID;
        while ((p = pendingRequestUsrIDs.find(usrID)) != string::npos)
        {
            found = true;
            pendingRequestUsrIDs.replace(p, tempWord.length(), "");
        }
    }
    mapPendingReqGroupIDUserIDs[grpID] = pendingRequestUsrIDs;
    // Return the resultant string
    string status;
    if (found)
    {
        status = "accepted";
        //added error here
        write(newSocket, status.c_str(), MESSAGELEN);
        string temps = mapGroupItsUsers[grpID];
        mapGroupItsUsers[grpID] = temps + " " + usrID;
        cout << "Now the group Members are:" << temps + " " + usrID;
    }
    else
    {
        status = "Request Not Found";
        write(newSocket, status.c_str(), MESSAGELEN);
    }
    pthread_exit(NULL);
    return NULL;
}
void *listRequestsFunction(void *ptr)
{ //user may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    //socketaddr = buffer;
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is list_requests" << endl;
    int j = i + 14 + 1;
    string grpID;
    for (; buffer[j]; j++)
    {
        grpID += buffer[j];
    }
    deb(grpID);
    string usrIDsToSend;
    usrIDsToSend = mapPendingReqGroupIDUserIDs[grpID];
    /*for (auto itr = mapGroupIDUserID.begin(); itr != mapGroupIDUserID.end(); itr++)
    {
        grpIDsToSend += itr->first + " ";
    }*/
    deb(usrIDsToSend);
    write(newSocket, usrIDsToSend.c_str(), MESSAGELEN);
    pthread_exit(NULL);
    return NULL;
}
void *listGroupFunction(void *ptr)
{ //user may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    /*for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }*/
    socketaddr = buffer;
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is list_group" << endl;
    string grpIDsToSend;
    for (auto itr = mapGroupIDUserID.begin(); itr != mapGroupIDUserID.end(); itr++)
    {
        grpIDsToSend += itr->first + " ";
    }
    deb(grpIDsToSend);
    write(newSocket, grpIDsToSend.c_str(), MESSAGELEN);
    pthread_exit(NULL);
    return NULL;
}
void *joinGroupFunction(void *ptr)
{ //user may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is join_group" << endl;
    int j = i + 11 + 1;
    string grpID;
    for (; buffer[j] != ' '; j++)
    {
        grpID += buffer[j];
    }
    deb(grpID);
    string userId;
    j++;
    for (; buffer[j]; j++)
    {
        userId += buffer[j];
    }
    deb(userId);
    struct groupJoinRequests groupJoinRequeststemp;
    //is group me iss bande ka request pending hai
    string temp = mapPendingReqGroupIDUserIDs[grpID];
    temp = temp + ' ' + userId;
    //string allrequestuserIds = mapPendingReqGroupIDUserIDs[grpID];
    //strcpy(groupJoinRequeststemp.usrIds, allrequestuserIds.c_str());
    strcpy(groupJoinRequeststemp.usrIds, temp.c_str());
    strcpy(groupJoinRequeststemp.grpId, grpID.c_str());
    FILE *outfile;
    outfile = fopen("groupRequest.dat", "a");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        //exit(1);
    }
    int k = fwrite(&groupJoinRequeststemp, sizeof(struct groupJoinRequests), 1, outfile);
    if (k != 0)
    {
        msg("request Added");
        string client_message = "Group join Request sent";
        //client_message.resize(MESSAGELEN, ' ');
        write(newSocket, client_message.c_str(), MESSAGELEN);
    }
    else
    {
        msg("error writing file !");
    }
    fclose(outfile);
    pthread_exit(NULL);
    return NULL;
}
void *createGroupFunction(void *ptr)
{ //user may already exist
    //string str = *reinterpret_cast<string *>(ptr);
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is create group";
    int j = i + 13 + 1;
    string grpID;
    for (; buffer[j] != ' '; j++)
    {
        grpID += buffer[j];
    }
    deb(grpID);
    string userId;
    j++;
    for (; buffer[j]; j++)
    {
        userId += buffer[j];
    }
    deb(userId);
    struct groupOwners pairuserGrp;
    mapUserIDGroupID[userId] += grpID; //a user can owner of multiple group
    mapGroupIDUserID[grpID] = userId;  //a group can be owned by one user
    strcpy(pairuserGrp.usrId, userId.c_str());
    strcpy(pairuserGrp.grpId, grpID.c_str());
    FILE *outfile;
    outfile = fopen("groupOwner.dat", "a");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    int k = fwrite(&pairuserGrp, sizeof(struct groupOwners), 1, outfile);
    if (k != 0)
    {
        msg("Added Group Sucessfully");
        string client_message = "Group Created";
        //client_message.resize(MESSAGELEN, ' ');
        write(newSocket, client_message.c_str(), MESSAGELEN);
    }
    else
    {
        msg("error writing file !");
    }
    fclose(outfile);
    pthread_exit(NULL);
    return NULL;
}
void *uploadFileFunction(void *ptr)
{ //user may already exist
    /*
    struct groupOwners pairuserGrp;
    mapUserIDGroupID[userId] += grpID; //a user can owner of multiple group
    mapGroupIDUserID[grpID] = userId;  //a group can be owned by one user
    strcpy(pairuserGrp.usrId, userId.c_str());
    strcpy(pairuserGrp.grpId, grpID.c_str());
    FILE *outfile;
    outfile = fopen("groupOwner.dat", "a");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    int k = fwrite(&pairuserGrp, sizeof(struct groupOwners), 1, outfile);
    if (k != 0)
    {
        msg("Added Group Sucessfully");
        string client_message = "Group Created";
        //client_message.resize(MESSAGELEN, ' ');
        write(newSocket, client_message.c_str(), MESSAGELEN);
    }
    else
    {
        msg("error writing file !");
    }
    fclose(outfile);
    pthread_exit(NULL);
    return NULL;
    */
    string str = (char *)ptr;
    deb(str);
    string socketaddr;
    char buffer[MESSAGELEN];
    bzero(buffer, MESSAGELEN);
    strcpy(buffer, str.c_str());
    deb(buffer);
    int i = 0;
    for (; buffer[i] != ' '; i++)
    {
        socketaddr += buffer[i];
    }
    deb(socketaddr);
    int newSocket = stoi(socketaddr);
    /////////////////////////////////////
    cout << endl
         << "command is Upload file \n";
    int j = i + 12 + 1;
    string grpID;
    for (; buffer[j] != ' '; j++)
    {
        grpID += buffer[j];
    }
    deb(grpID);
    string userId;
    j++;
    for (; buffer[j] != ' '; j++)
    {
        userId += buffer[j];
    }
    deb(userId);
    string filekanaam;
    for (; buffer[j]; j++)
    {
        filekanaam += buffer[j];
    }
    deb(filekanaam);
    /////////////////////////////////////////////////////
    vector<string> pieceWise_SHA;
    while (true)
    {
        bzero(buffer, MESSAGELEN);
        int n = read(newSocket, buffer, MESSAGELEN);
        if (n < 0)
        {
            cout << "ERROR reading userID from socket";
            exit(0);
        }
        else
        {
            if (strcmp(buffer, "End_Of_File") == 0)
                break;
            else
            {
                pieceWise_SHA.push_back(buffer);
                //cout << buffer << endl;
            }
        }
    }
    mapfilePieceWiseSHA[filekanaam] = pieceWise_SHA;
    bzero(buffer, MESSAGELEN);
    int n = read(newSocket, buffer, MESSAGELEN);
    if (n < 0)
    {
        cout << "ERROR reading SHA of Entire File from socket";
        exit(0);
    }
    else
    {
        mapfileSHA1Complete[filekanaam] = buffer;
    }
    string temps_S = mapCompletefileNameUserID[filekanaam];
    mapCompletefileNameUserID[filekanaam] = temps_S + ' ' + userId;
    mapGroupsSharableFiles[grpID] = filekanaam; // tells group has which sharable files
    /////////////////////////////////////////////////////
    //degugging all the info recieved
    vector<string> mapstr = mapfilePieceWiseSHA[filekanaam];
    cout << "Piece wise Sha\n";
    for (unsigned int i = 0; i < mapstr.size(); i++)
    {
        //cout << mapstr[i] << endl;
        deb(mapstr[i]);
    }
    cout << "Entire SHA\n"
         << mapfileSHA1Complete[filekanaam] << endl;
    cout << "The Group with groupID:" << grpID << " Has these Files\n";
    cout << mapGroupsSharableFiles[grpID] << endl;
    cout << "The file is present with following USER ID\n";
    cout << mapCompletefileNameUserID[filekanaam] << endl;
    /////////////////////////////////////////////////////////
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
        mapuserIDSharingFiles[usedID] = 1; //////////when ever a user logins he is in sharing mode
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

void ReadGrpOwnerMap()
{
    FILE *infile;
    struct groupOwners mapGroupUsers;
    infile = fopen("groupOwner.dat", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return;
    }
    while (fread(&mapGroupUsers, sizeof(struct groupOwners), 1, infile))
    {
        //cout << "FileName is:" << mapFileUsers.fileName << " is present with " << mapFileUsers.assocaitedUserName;
        //mapCompletefileNameUserID[mapFileUsers.fileName] = mapFileUsers.assocaitedUserName;
        mapUserIDGroupID[mapGroupUsers.usrId] += mapGroupUsers.grpId;
        //this user is owner of these groups
        mapGroupIDUserID[mapGroupUsers.grpId] = mapGroupUsers.usrId;
        //this group is owned by this user
        ///wont it be +=
    }
    for (auto itr = mapUserIDGroupID.begin(); itr != mapUserIDGroupID.end(); itr++)
    {
        cout << "userID:" << itr->first << " is owner of groupID(s):" << itr->second << endl;
    }
    for (auto itr = mapGroupIDUserID.begin(); itr != mapGroupIDUserID.end(); itr++)
    {
        cout << "groupID:" << itr->first << " is ownered by userID:" << itr->second << endl;
    }
    fclose(infile);
}
void ReadGrpJoinRequestMap()
{
    FILE *infile;
    struct groupJoinRequests groupJoinRequeststemp;
    infile = fopen("groupRequest.dat", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return;
    }
    while (fread(&groupJoinRequeststemp, sizeof(struct groupJoinRequests), 1, infile))
    {
        //string temp = mapPendingReqGroupIDUserIDs[groupJoinRequeststemp.grpId];
        //mapPendingReqGroupIDUserIDs[groupJoinRequeststemp.grpId] = temp + ' ' + groupJoinRequeststemp.usrIds;
        mapPendingReqGroupIDUserIDs[groupJoinRequeststemp.grpId] = groupJoinRequeststemp.usrIds;
    }
    for (auto itr = mapPendingReqGroupIDUserIDs.begin(); itr != mapPendingReqGroupIDUserIDs.end(); itr++)
    {
        cout << "groupID:" << itr->first << " is has request Pending from by userIDs:" << itr->second << endl;
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
    ReadGrpOwnerMap();
    ReadGrpJoinRequestMap();

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
    pthread_t handlingCreateGroup;
    pthread_t handlingLoginUser;
    pthread_t handlingDownloadUserFileInfo;
    pthread_t handlingJoinGroup;
    pthread_t handlingListGroup;
    pthread_t handlingListRequests;
    pthread_t handlingacceptRequests;
    pthread_t handlinglogout;
    pthread_t handlingleaveGrp;
    pthread_t handlinguploadFile;
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
        string temp;
        string command_recieved = buffer;
        printf("Here is the message: %s\n", buffer); //identify command and call respective function using thread
        bool createUserFlag = false;
        bool loginUserFlag = false;
        bool creategroupFlag = false;
        bool joingroupFlag = false;
        bool listgroupFlag = false;
        bool listrequestsFlag = false;
        bool acceptrequestsFlag = false;
        bool logoutFlag = false;
        bool leaveGrpFlag = false;
        bool uploadFileFlag = false;

        //bool createUserFlag = false;
        //bool createUserFlag = false;
        //bool createUserFlag = false;
        //bool createUserFlag = false;
        bool downloadFileInfoFlag = false;

        if (buffer[0] == 'c' && buffer[1] == 'r' && buffer[2] == 'e' && buffer[3] == 'a' && buffer[4] == 't' && buffer[5] == 'e' && buffer[6] == '_' && buffer[7] == 'g' && buffer[8] == 'r' && buffer[9] == 'o' && buffer[10] == 'u' && buffer[11] == 'p')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlingCreateGroup, NULL, createGroupFunction, arr);
            creategroupFlag = true;
        }
        else if (buffer[0] == 'j' && buffer[1] == 'o' && buffer[2] == 'i' && buffer[3] == 'n' && buffer[4] == '_' && buffer[5] == 'g' && buffer[6] == 'r' && buffer[7] == 'o' && buffer[8] == 'u' && buffer[9] == 'p')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlingJoinGroup, NULL, joinGroupFunction, arr);
            joingroupFlag = true;
        }
        else if (buffer[0] == 'l' && buffer[1] == 'i' && buffer[2] == 's' && buffer[3] == 't' && buffer[4] == '_' && buffer[5] == 'g' && buffer[6] == 'r' && buffer[7] == 'o' && buffer[8] == 'u' && buffer[9] == 'p')
        {
            temp = to_string(newSocket);
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlingListGroup, NULL, listGroupFunction, arr);
            listgroupFlag = true;
        }
        else if (buffer[0] == 'l' && buffer[1] == 'i' && buffer[2] == 's' && buffer[3] == 't' && buffer[4] == '_' && buffer[5] == 'r' && buffer[6] == 'e' && buffer[7] == 'q' && buffer[8] == 'u' && buffer[9] == 'e' && buffer[10] == 's' && buffer[11] == 't' && buffer[12] == 's')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlingListRequests, NULL, listRequestsFunction, arr);
            listrequestsFlag = true;
        }
        else if (buffer[0] == 'a' && buffer[1] == 'c' && buffer[2] == 'c' && buffer[3] == 'e' && buffer[4] == 'p' && buffer[5] == 't' && buffer[6] == '_' && buffer[7] == 'r' && buffer[8] == 'e' && buffer[9] == 'q' && buffer[10] == 'u' && buffer[11] == 'e' && buffer[12] == 's' && buffer[13] == 't')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlingacceptRequests, NULL, acceptRequestsFunction, arr);
            acceptrequestsFlag = true;
        }
        else if (buffer[0] == 'l' && buffer[1] == 'o' && buffer[2] == 'g' && buffer[3] == 'o' && buffer[4] == 'u' && buffer[5] == 't')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlinglogout, NULL, logoutFunction, arr);
            logoutFlag = true;
        }
        else if (buffer[0] == 'l' && buffer[1] == 'e' && buffer[2] == 'a' && buffer[3] == 'v' && buffer[4] == 'e' && buffer[5] == '_' && buffer[6] == 'g' && buffer[7] == 'r' && buffer[8] == 'o' && buffer[9] == 'u' && buffer[10] == 'p')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            pthread_create(&handlingleaveGrp, NULL, leaveGrpFunction, arr);
            leaveGrpFlag = true;
        }
        else if (buffer[0] == 'u' && buffer[1] == 'p' && buffer[2] == 'l' && buffer[3] == 'o' && buffer[4] == 'a' && buffer[5] == 'd' && buffer[6] == '_' && buffer[7] == 'f' && buffer[8] == 'i' && buffer[9] == 'l' && buffer[10] == 'e')
        {
            temp = to_string(newSocket) + ' ' + buffer;
            //temp += ' ' + buffer;
            char *arr = (char *)malloc(sizeof(char) * temp.length());
            strcpy(arr, temp.c_str());
            deb(temp);
            pthread_create(&handlinguploadFile, NULL, uploadFileFunction, arr);
            uploadFileFlag = true;
        }
        else if (strcmp(buffer, "create_user") == 0)
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
        if (creategroupFlag == true)
            pthread_join(handlingCreateGroup, NULL);
        if (joingroupFlag == true)
            pthread_join(handlingJoinGroup, NULL);
        if (listgroupFlag == true)
            pthread_join(handlingListGroup, NULL);
        if (listrequestsFlag == true)
            pthread_join(handlingListRequests, NULL);
        if (acceptrequestsFlag == true)
            pthread_join(handlingacceptRequests, NULL);
        if (logoutFlag == true)
            pthread_join(handlinglogout, NULL);
        if (leaveGrpFlag == true)
            pthread_join(handlingleaveGrp, NULL);
        if (uploadFileFlag == true)
            pthread_join(handlinguploadFile, NULL);

        /*cout << "Diaplying all the user details\n";
        for (unsigned int i = 0; i < auth.size(); i++)
        {
            cout << auth[i].user << endl;
            cout << auth[i].passwd << endl;
        }*/
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
