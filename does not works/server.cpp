#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <WinSock2.h> //giving error
#pragma comment(lib, "WS2_32.lib")
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <netdb.h>      //hostent
#include <netinet/in.h>
#include <iostream>
#include <math.h>
#include <algorithm>
//#include <winsock.h>
#define PORT 9000
using namespace std;
struct sockaddr_in svr;
int nsocket;
int nArrayClient[5];
fd_set fr, fw, fe;
void ProcessNewMessage(int nClientSocket)
{
    std::cout << "Process the new message for client socket: " << nClientSocket << endl;
    char buffer[256] = {
        0,
    };
    int relIn = recv(nClientSocket, buffer, 256, 0);
    if (relIn < 0)
    {
        std::cout << "Failed to process new message something went wrong" << endl;
        closesocket(nClientSocket);
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (nArrayClient[nIndex] == nClientSocket)
            {
                nArrayClient[nIndex] = 0;
                break;
            }
        }
    }
    else
    {
        std::cout << "Message recieved from client " << buffer << endl;
        send(nClientSocket, "Proceed your request", 23, 0);
        cout << endl
             << "**********************************";
    }
}
void ProcessTheNewRequest()
{
    if (FD_ISSET(nsocket, &fr))
    {
        int nLen = sizeof(struct sockaddr);
        int nClientSocket = accept(nsocket, NULL, &nLen);
        if (nClientSocket > 0)
        {
            int nIndex = 0;
            for (nIndex = 0; nIndex < 5; nIndex++)
            {
                if (nArrayClient[nIndex] == 0)
                {
                    nArrayClient[nIndex] = nClientSocket;
                    std::cout << nClientSocket << endl;
                    send(nClientSocket, "Got the new connection successfully ", 255, 0);
                    break;
                }
            }
            if (nIndex == 5)
            {
                std::cout << "Not Space to new connection" << endl;
            }
        }
    }
    else
    {
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (FD_ISSET(nArrayClient[nIndex], &fr))
            {
                ProcessNewMessage(nArrayClient[nIndex]);
            }
        }
    }
}
int main()
{
    int relInt = 0;
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
    {
        std::cout << "Wsa is not set" << endl;
    }
    else
    {
        std::cout << "WAS sucess" << endl;
    }
    nsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nsocket < 0)
    {
        std::cout << "Socket is not open" << endl;
    }
    else
    {
        std::cout << "Socket is open now" << endl;
    }
    u_long optval = 0;
    relInt = ioctlsocket(nsocket, FIONBIO, &optval);
    if (relInt != 0)
    {
        std::cout << "Failed to block" << endl;
    }
    else
    {
        std::cout << "Passed blocking" << endl;
    }
    int nOptVal = 0;
    int nOptLen = sizeof(nOptVal);
    relInt = setsockopt(nsocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&nOptVal, nOptLen);
    if (!relInt)
    {
        std::cout << "SetSock is called sucessful" << endl;
    }
    else
    {
        std::cout << "SetSock called is failed" << endl;
    }
    svr.sin_family = AF_INET;
    svr.sin_addr.S_un.S_addr = INADDR_ANY;
    svr.sin_port = htons(PORT);
    memset(svr.sin_zero, 0, 8);
    relInt = bind(nsocket, (sockaddr *)&svr, sizeof(sockaddr));
    if (relInt < 0)
    {
        std::cout << "Is not bind" << endl;
    }
    else
    {
        std::cout << "Bind sucess" << endl;
    }
    relInt = listen(nsocket, 5);
    if (relInt < 0)
    {
        std::cout << "Socket is not listing" << endl;
    }
    else
    {
        std::cout << "Is listing" << endl;
    }
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int maxFat = nsocket;

    while (1)
    {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        FD_SET(nsocket, &fr);
        FD_SET(nsocket, &fe);
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (nArrayClient[nIndex] != 0)
            {
                FD_ISSET(nArrayClient[nIndex], &fr);
                FD_ISSET(nArrayClient[nIndex], &fe);
            }
        }
        //std::cout << "Befor start socket connection " << fr.fd_count << endl;
        relInt = select(maxFat + 1, &fr, &fw, &fe, &tv);
        if (relInt > 0)
        {
            std::cout << "Port on processing" << endl;
            ProcessTheNewRequest();
            /*if (FD_ISSET(nsocket, &fe)) {
				std::cout << "Exceptio is , just get away from here" << endl;
			}
			if (FD_ISSET(nsocket, &fw)) {
			std:cout << "Is ready to write someting" << endl;
			}
			if (FD_ISSET(nsocket, &fr)) {
				std::cout << "ready to read something here" << endl;
			}

			break;*/
        }
        else if (relInt == 0)
        {
            //std::cout << "Port is used" << PORT << endl;
        }
        else
        {
            std::cout << "Failed" << endl;
            exit(EXIT_FAILURE);
        }
        //std::cout << "After completion " << fr.fd_count << endl;
    }
    return 0;
}