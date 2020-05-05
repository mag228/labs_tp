#define _WIN32_WINNT 0x501

#include <iostream>
#include <sstream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <document.h>
#include <prettywriter.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVERADDR "127.0.0.1"
#define PORT "228"
#define BUF_SIZE 1024

using namespace std;
using namespace rapidjson;

enum class cod_responce { def, win, los, draw, disc };
enum class cod_request { def, stone, paper, scis, disc };

struct request {
    cod_request cod;

    request() { cod = cod_request::def; }

    request(int val) { cod = (cod_request)val; }

    static request fromJSON(const char* json) {
        Document doc;
        doc.Parse(json);
        request result(doc["cod"].GetInt());
        return result;
    }
};


struct response {
	cod_responce cod;

	response() { cod = cod_responce::def; }
	response(int val) { cod = cod_responce(val); }
	
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

    static response* game(const request& first, const request& second) {
        response* result = new response[2];
        if (first.cod == cod_request::disc && second.cod == cod_request::disc);
        else if (first.cod == cod_request::disc) result[1].cod = cod_responce::disc;
        else if (second.cod == cod_request::disc) result[0].cod = cod_responce::disc;
        else if (first.cod == cod_request::stone && second.cod == cod_request::paper
            || first.cod == cod_request::scis && second.cod == cod_request::stone
            || first.cod == cod_request::paper && second.cod == cod_request::scis)
            result[0].cod = cod_responce::los, result[1].cod = cod_responce::win;
        else if (first.cod == cod_request::stone && second.cod == cod_request::scis
            || first.cod == cod_request::scis && second.cod == cod_request::paper
            || first.cod == cod_request::paper && second.cod == cod_request::stone)
            result[0].cod = cod_responce::win, result[1].cod = cod_responce::los;
        else result[0].cod = result[1].cod = cod_responce::draw;
        return result;
    }
};




void main()
{
    setlocale(LC_ALL, "rus");
    cout << "Cервер запущен" << endl;
    WSADATA wsaData; 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        cout << "Error WSAStartup" << endl;
        system("pause");
        return;
    }
    struct addrinfo* addr = NULL; 
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_protocol = IPPROTO_TCP; 
    hints.ai_flags = AI_PASSIVE; 
    getaddrinfo(SERVERADDR, PORT, &hints, &addr);

    int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (listen_socket < 0) {
        cout << "Error socket" << endl;
        WSACleanup();
        system("pause");
        return;
    }
    if (bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen)) {
        cout<< "Error bind" <<endl;
        closesocket(listen_socket); 
        WSACleanup();
        return;
    }

    if (listen(listen_socket, 0) == SOCKET_ERROR) {
        cout << "Error listen" << endl;
        closesocket(listen_socket);
        WSACleanup();
        return;
    }
    char buf[BUF_SIZE];
    cout << "ќжидание первого клиента" << endl;
    int first = accept(listen_socket, NULL, NULL);
    cout << "ќжидание второго клиента" << endl;
    int second = accept(listen_socket, NULL, NULL);
    cout << " лиенты подключились" << endl;
    int result;
    string fstr, sstr;
    while (true) {
        result = recv(first, buf, BUF_SIZE, 0);
        buf[result] = '\0';
        request req1 = request::fromJSON(buf);
        result = recv(second, buf, BUF_SIZE, 0);
        buf[result] = '\0';
        request req2 = request::fromJSON(buf);

        response* result = response::game(req1, req2);
        fstr = result[0].toJSON();
        sstr = result[1].toJSON();
        if (result[0].cod != cod_responce::def) send(first, fstr.c_str(), fstr.size(), 0);
        if (result[1].cod != cod_responce::def) send(second, sstr.c_str(), sstr.size(), 0);
        if (result[0].cod == cod_responce::def || result[1].cod == cod_responce::def) break;

    }
   
    WSACleanup();
    system("pause");
    return;
}
