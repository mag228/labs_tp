#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <string>
#include <document.h>
#include <prettywriter.h>

#define _WIN32_WINNT 0x501

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")


#define SERVERADDR "127.0.0.1"
#define PORT 228
#define BUF_SIZE 1024

using namespace std;
using namespace rapidjson;

enum class cod_responce { def, win, los, draw, disc };
enum class cod_request { def, stone, paper, scis, disc };

struct response {
	cod_responce cod;

	response() { cod = cod_responce::def; }
	response(int val) { cod = cod_responce(val); }
	
	static response fromJSON(const char* json) {
		Document doc;
		doc.Parse(json);
		response result(doc["cod"].GetInt());
		return result;
	}
};

struct request {
	cod_request cod;

	request() { cod = cod_request::def; }

	request(int val) { cod = (cod_request)val; }

	string toJSON() {
		Value json_val;
		Document doc;
		auto& allocator = doc.GetAllocator();
		doc.SetObject();
		json_val.SetInt((int)cod);
		doc.AddMember("cod", json_val, allocator);
		
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		doc.Accept(writer);
		return buffer.GetString();
	}
};

void main() {
	setlocale(LC_ALL, "rus");
	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Error WSAStartup" << endl;
		system("pause");
		return;
	}
	SOCKET so = socket(AF_INET, SOCK_STREAM, 0);
	if (so < 0) {
		cout << "Error socket" << endl;
		WSACleanup();
		system("pause");
		return;
	}
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT* hst;
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long*)&dest_addr.sin_addr)[0] =
			((unsigned long**)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(so);
			WSACleanup();
			return;
		}
	if (connect(so, (sockaddr*)&dest_addr, sizeof(dest_addr))) {
		cout << "Connect error" << endl;
		WSACleanup();
		system("pause");
		return;
	}
	
	cout << "Игра камень ножницы бумага" << endl;
	string str;
	char buf[BUF_SIZE];
	response resp;
	request req;
	int result;
	while (true) {
		start:
		cout << "Введите команду (stone, paper, scis, exit): ";
		cin >> str;
		if (str == "exit") req.cod = cod_request::disc;
		else if (str == "paper")req.cod = cod_request::paper;
		else if (str == "stone")req.cod = cod_request::stone;
		else if (str == "scis")req.cod = cod_request::scis;
		else {
			cout << "некорректный ввод" << endl;
			goto start;
		}
		string t = req.toJSON();
		send(so, t.c_str(), t.size(), 0);
		if (req.cod == cod_request::disc) {
			cout << "Прощайте!" << endl;
			system("pause");
			closesocket(so);
			WSACleanup();
			break;
		}
		result = recv(so, buf, BUF_SIZE, 0);
		buf[result] = '\0';
		resp = response::fromJSON(buf);
		switch (resp.cod) {
		case cod_responce::win:
			cout << "Вы победили!" << endl;
			break;
		case cod_responce::los:
			cout << "Вы проиграли(" << endl;
			break;
		case cod_responce::draw:
			cout << "Ничья -(ОО)-" << endl;
			break;
		case cod_responce::disc:
			cout << "Соперник сбежал!" << endl;
			closesocket(so);
			WSACleanup();
			system("pause");
			return;
		}
		
	}
	WSACleanup();
}