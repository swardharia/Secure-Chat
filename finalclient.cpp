#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Rijndael.h"
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

SOCKET Socket;
WSADATA Winsock;
sockaddr_in Addr;
int Addrlen = sizeof(Addr);

int main()
{
    WSAStartup(MAKEWORD(2, 2), &Winsock);    // Start Winsock

	CRijndael oRijndael;
	oRijndael.MakeKey("abcdefghabcdefghabcdefghabcdefghabcdefghabcdefghabcdefghabcdefgh", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, 32);

    if(LOBYTE(Winsock.wVersion) != 2 || HIBYTE(Winsock.wVersion) != 2)    // Check version
    {
        WSACleanup();
        return 0;
    }

     Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ZeroMemory(&Addr, sizeof(Addr));    // clear the struct
    Addr.sin_family = AF_INET;    // set the address family
    Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    Addr.sin_port = htons(6000);    // set the port

    if(connect(Socket, (sockaddr*)&Addr, sizeof(Addr)) < 0)
    {
        printf("Connection failed !\n");
        getchar();
        return 0;
    }

    printf("Connection successful !\n");

    int Size,ig=0;
    char *Filesize = new char[1024];
	int iResult,end;
	char szDataOut[33] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	
	do {

    iResult = recv(Socket, Filesize, 1024, 0);
	cout << "\nEncrypted : " << Filesize << endl;
	oRijndael.DecryptBlock(Filesize, szDataOut);

    if ( iResult > 0 )
	     {
	      string st(szDataOut);
 		 char last=st[st.length()-1];
	      if(last=='.')
	      {
			  printf("\nReceived Text: %s", szDataOut);
			}
			else
			{
				printf("\nReceived File: %s", szDataOut);
			}
	      if(iResult > 5)
	      {
		  ofstream myfile;
		  myfile.open ("FromServer.txt",ios::binary|ios::out);
		  myfile << Filesize;
		  myfile.close();
		 }
	}
    else if ( iResult == 0 )
        printf("Connection closed\n");
    else
        printf("error");

	} while( iResult > 0 );

    getchar();
    closesocket(Socket);
    WSACleanup();
    return 0;
}
