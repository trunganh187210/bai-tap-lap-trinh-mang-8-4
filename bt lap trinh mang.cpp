#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "ws2_32")
#include <stdlib.h>
#include <string.h>

DWORD WINAPI ClientThread(LPVOID);

// xoa client
void RemoveClient(SOCKET);


SOCKET clients[64];
int numClients;

int main() {
	//Khoi tao thu vien Winsock
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	
	// Tao doi tuong socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// Khai bao dia chi server
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);
	
	//Chuyen sang trang thai cho ket noi
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	numClients = 0;

	while (true) {
		
		SOCKET client = accept(listener, NULL, NULL); //chap nhan ket noi
		printf("Client accepted: %d\n", client);
		while (true) {
			const char* name_asking = "What is your id?";
			send(client, name_asking, strlen(name_asking), 0); // gui client cau hoi
			char buf[256];
			int ret = recv(client, buf, sizeof(buf), 0); // nhan cau tra loi tu client
			int right_form = 1;
			if (right_form) break;
		}
		clients[numClients] = client;
		numClients++;
		
		CreateThread(0, 0, ClientThread, &client, 0, 0); // Tao luong client moi
	}
	closesocket(listener);
	WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET*)lpParam;
	char buf[256], sender[512];
	int ret;
	struct tm* timeinfo;
	while (true) {
		ret = recv(client, buf, sizeof(buf), 0); // nhan du lieu tu client
		if (ret <= 0) {
			RemoveClient(client); // neu du lieu tra ve nho hon hoac bang 0 bit thi loai bo client do
			break;
		}

		buf[ret] = 0;
		printf("%d: %s\n", client, buf); // in ra id client va tin client do gui
		_itoa_s(client, sender, 10);
		strcat_s(sender, ": ");
		strcat_s(sender, buf);
		strcat_s(sender, " ");
		
		// gui tin cho tat ca client khac
		for (int i = 0; i < numClients; i++) {
			if (clients[i] != client) {
				send(clients[i], sender, strlen(sender), 0);
			}
		}
		
	}
	closesocket(client);
}

void RemoveClient(SOCKET client) {
	int i = 0;
	while (clients[i] != client) i++;
	if (i < numClients - 1) clients[i] = clients[numClients - 1];
	numClients--;
}
