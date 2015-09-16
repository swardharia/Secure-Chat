#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include<string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <commctrl.h>
#include "Rijndael.h"
#include "md5.h"
#include "ssl.h"
#include "z.h"
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Secur32.lib")

#define Port 6000

SOCKET Socket, Sub;
WSADATA Winsock;
//SSL_SOCKET* sx = 0;
sockaddr_in Addr;
sockaddr_in IncomingAddress;
int AddressLen = sizeof(IncomingAddress);
string user = "", pass = "";
int a = 0, choice = 0, ex = 4;
string passhash = md5("dharia"), userhash;

/*
void r(void*)
{
	// Receiving Thread
	// Uncomment the commented lines to allow logging

	//   FILE* fp = fopen("log.txt","a+b");
	for (;;)
	{
		char c;
		int rval = 0;

		if (SSL)
			rval = sx->s_recv(&c, 1);
		else
			rval = recv(s, &c, 1, 0);
		if (rval == 0 || rval == -1)
		{
			printf("--- Disconnected !\r\n\r\n");
			//         fclose(fp);
			exit(0);
		}
		putc(c, stdout);
		//      fputc(c,fp);
	}
}

void Loop()
{
	getpeername(s, (sockaddr*)&aa, &slen);

	printf("OK , connected with %s:%u...\r\n\r\n", inet_ntoa(aa.sin_addr), ntohs(aa.sin_port));
	if (SSL)
	{
		if (__argc == 2)
		{
			sx = new SSL_SOCKET(s, 1);
			sx->ServerInit();
		}
		else
		{
			sx = new SSL_SOCKET(s, 0);
			sx->ClientInit();
		}
	}

	_beginthread(r, 4096, 0);
	for (;;)
	{
		char c = getc(stdin);
		int rval = 0;
		if (SSL)
			rval = sx->s_ssend(&c, 1);
		else
			rval = send(s, &c, 1, 0);
		if (rval == 0 || rval == -1)
		{
			printf("--- Disconnected !\r\n\r\n");
			exit(0);
		}
	}
}
*/


int main()
{
	try{
		char szHex[65];
		//One block testing
		char szDataOut[33] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		CRijndael oRijndael;
		oRijndael.MakeKey("abcdefghabcdefghabcdefghabcdefghabcdefghabcdefghabcdefghabcdefgh", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, 32);
	
		WSAStartup(MAKEWORD(2, 2), &Winsock);    // Start Winsock

		if (LOBYTE(Winsock.wVersion) != 2 || HIBYTE(Winsock.wVersion) != 2)    // Check version
		{
			WSACleanup();
			return 0;
		}

		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		ZeroMemory(&Addr, sizeof(Addr));
		Addr.sin_family = AF_INET;
		Addr.sin_port = htons(Port);
		bind(Socket, (sockaddr*)&Addr, sizeof(Addr));



		if (listen(Socket, 1) == SOCKET_ERROR)
		{
			printf("\nlistening error\n");
		}

		else
		{
			printf("\nSign in To Connect to client :\n");
		}

		if (Sub = accept(Socket, (sockaddr*)&IncomingAddress, &AddressLen))
		{
			char *ClientIP = inet_ntoa(IncomingAddress.sin_addr);
			int ClientPort = ntohs(IncomingAddress.sin_port);


			do{

				if (a == 1){
					cout << "\n-------Invalid username or password----------\n";
				}

				cout << "\nEnter Username : ";
				getline(cin, user);

				cout << "\nEnter Password : ";
				getline(cin, pass);
				a = 1;
				userhash = md5(pass);

			} while (!(user == "swar" && (passhash == userhash)));

			if ((user == "swar") && (passhash == userhash))
			{

				printf("\nClient conncted!\n");
				printf("IP: %s:%d\n", ClientIP, ClientPort);

			again:

				cout << "\n1. Send Text  2. Send File  3. Exit" << endl;
				
				choice = 0;
				cin >> choice;
				char hey[1000] = { 0 };
				int i = 0, c;
				cout << endl;

				if (choice == 1)
				{
					cout << "Enter Text :";
					do {
						c = getchar();
						hey[i] = c;
						i++;
					} while (c != '.');

					oRijndael.EncryptBlock(hey, szDataOut);

					send(Sub, szDataOut, 1024, 0);

					
					

					while (!strcmp(hey, "exit"))
					{

						//cin.clear();
					}

					goto again;
				}



				if (choice == 2)
				{

					printf("\nSending file .. \n");

					FILE *File;
					char *Buffer;
					unsigned long Size;
					string fpath = "";

					cout << "\nEnter file name : ";
					cin >> fpath;
					const char* fname = fpath.c_str();
					File = fopen(fname, "rb");
					//send(Sub, fname, 1024, 0); // File size
					if (!File)
					{
						printf("\nError while reading the file\n");
						getchar();
						return 0;
					}

					fseek(File, 0, SEEK_END);
					Size = ftell(File);
					fseek(File, 0, SEEK_SET);
					//cout << Size;
					Buffer = new char[Size+1000];

					fread(Buffer, Size, 1, File);
					Buffer[Size] = '\0';
					oRijndael.EncryptBlock(Buffer, szDataOut);
					
					cout << "\nencrypted: " << szDataOut;
					fclose(File);
					send(Sub, szDataOut, 1024, 0); // File size
					
					free(Buffer);
					// send(Sub, (const char*)&ex, sizeof(int), 0);

					goto again;
				}

				else if (choice == 3)
				{
					send(Sub, (const char *)&choice, sizeof(int), 0);
					goto exit;
				}

				else
				{
					cin.clear();
					goto again;
				}

			exit:  closesocket(Sub);
				closesocket(Socket);
				WSACleanup();
			}

		}
		else
		{
			cout << "invallid";
		}
	}

	catch (exception& e)
	{
		cout << e.what() << endl;
	}

	getchar();
	return 0;
}
