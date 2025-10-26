#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

/*
	initialise winsock
	create socket
	connect to server
	send/recieve
	close the socket
*/

static bool Initialise() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s) {
	cout << "enter your chat name :";
	string name;
	getline(cin, name);
	string message;

	while (1) {
		getline(cin, message);
		string msg = name + ":" + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);

		if (bytesent == SOCKET_ERROR) {
			cout << "error sending message" << endl;
			break;
		}

		if (message == "__quit__") {
			cout << "stopping the application" << endl;
		}
	}

	closesocket(s);
	WSACleanup();
}

void ReceiveMsg(SOCKET s) {
	char buffer[4096];
	int recvlength;
	while (1) {
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "disconnected from the server" << endl;
			break;
		}
	}

	closesocket(s);
	WSACleanup();
}

int main() {
	if (!Initialise()) {
		cout << "iniliatising winsock failed" << endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket created" << endl;
		return 1;
	}

	int port = 5000;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	// binding
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "unable to connect to server" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "successfully connected to server" << endl;

	// senderthread
	thread senderthread(SendMsg, s);

	//reciever thread
	thread recieverthread(ReceiveMsg, s);

	senderthread.join();
	recieverthread.join();

	return 0;
}