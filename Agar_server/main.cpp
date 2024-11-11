#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

#include "Common.h"
#include "Game/World.h"
#include "Util.h"

using namespace std;
using namespace chrono;


const int SERVERPORT = 9000;
const int PACKETSIZEMAX = 512;

const int MAX_CLIENTS = 8;
atomic_bool slot[MAX_CLIENTS];


void run_game(World& world) {
    auto timer = clock();
    auto elapsed = 0;
    int update_time = fps(60);

    // ���������� ������Ʈ
    while(true) {
        elapsed += clock() - timer;
        timer = clock();
        if(elapsed < update_time) {
            continue;
        }

        elapsed -= update_time;

        world.update();
    }
}


void handle_connection(SOCKET socket, struct sockaddr_in clientaddr, int thread_id) {
    int retval;
    char buf[PACKETSIZEMAX];

    // ������ Ŭ���̾�Ʈ ���� ���
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("%s:%d\n", addr, ntohs(clientaddr.sin_port));

    // Ŭ���̾�Ʈ�� ������ ���
    while(true) {
        // ������ �ޱ�
        retval = recv(socket, buf, PACKETSIZEMAX, 0);
        if(retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if(retval == 0) {		// ���� ����
            break;
        }

        struct Packet {
            LONG x;
            LONG y;
        }* p;
        p = reinterpret_cast<Packet*>(buf);
        cout << p->x << ", " << p->y << endl;

        // ������ ������
        retval = send(socket, buf, retval, 0);
        if(retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
    }

    // ��� ���� �ݱ�
    closesocket(socket);

    slot[thread_id] = false;
}


int main() {
    // ------------------------------------- ���� ���� -------------------------------------
    World world;
    world.setUp();

    thread game_logic { [&]() { run_game(world); } };
    game_logic.detach();


    // ------------------------------------- ��Ʈ��ũ �۾� -------------------------------------
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "���� �ʱ�ȭ ����" << endl;
        return 1;
    }

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock == INVALID_SOCKET) {
        err_quit("socket()");
    }

    // bind()
    int retval;

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == SOCKET_ERROR) {
        err_quit("bind()");
    }

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if(retval == SOCKET_ERROR) {
        err_quit("listen()");
    }

    // ������ ��ſ� ����� ��Ĺ
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;

    while(true) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if(client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // ������ �������� ���
        bool wait = true;
        while(wait) {
            for(int i=0; i<MAX_CLIENTS; ++i) {
                // ������ ��������� �ٿ�ε� ����
                if(slot[i] == false) {
                    slot[i] = true;
                    std::thread new_client_thread { handle_connection, client_sock, clientaddr, i };
                    new_client_thread.detach();

                    wait = false;
                    break;
                }
            }
        }
    }

    // ��� ���� �ݱ�
    closesocket(listen_sock);

    WSACleanup();
}
