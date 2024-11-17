#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

#include "Common.h"
#include "Game/World.h"
#include "Util.h"
#include "../protocol.h"

using namespace std;
using namespace chrono;


const int SERVERPORT = 9000;
const int PACKETSIZEMAX = 512;

const int MAX_CLIENTS = 8;
mutex socket_mutex;
SOCKET sockets[MAX_CLIENTS];

World world;


void run_game(World& world) {
    auto timer = clock();
    auto elapsed = 0;
    int update_time = fps(30);

    // ���������� ������Ʈ
    while(true) {
        elapsed += clock() - timer;
        timer = clock();
        if(elapsed < update_time) {
            continue;
        }

        elapsed -= update_time;

        world.update();

        auto players = world.getPlayers();

        // World ���� ����
        SC_WORLD_PACKET packet;
        packet.type = SC_WORLD;
        packet.object_num = static_cast<int>(players.size());

        char buf[PACKETSIZEMAX];

        memcpy(buf, &packet, sizeof(SC_WORLD_PACKET));
        int offset = sizeof(SC_WORLD_PACKET);

        for(const auto& p : players) {
            auto player = p.second;

            for(const auto& cell : player.cells) {
                SC_OBJECT obj;
                obj.x = cell->position.x;
                obj.y = cell->position.y;
                obj.radius = cell->getRadius();
                obj.color = cell->color;

                // ���۰� ��ĥ���� �ִ�.
                memcpy(buf + offset, &obj, sizeof(SC_OBJECT));
                offset += sizeof(SC_OBJECT);
            }
        }

        // �� ���Ͽ� ������ ����
        for(auto& sock : sockets) {
            if(sock == INVALID_SOCKET) {
                continue;
            }

            // ������ ����(send()
            int retval = send(sock, buf, sizeof(SC_WORLD_PACKET), 0);
            if(retval == SOCKET_ERROR) {
                err_display("send()");
            }
        }
    }
}


void handle_connection(SOCKET socket, struct sockaddr_in clientaddr, int id) {
    int retval;
    char buf[PACKETSIZEMAX];

    // ������ Ŭ���̾�Ʈ ���� ���
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("%s:%d\n", addr, ntohs(clientaddr.sin_port));

    // World�� �÷��̾� �߰�
    world.addPlayer(id);
    cout << "Players: \n";
    for(const auto& p : world.getPlayers()) {
        printf(" - id: %2d\n", p.first);
    }

    // Ŭ���̾�Ʈ�� ������ ���
    while(true) {
        // Ŭ���̾�Ʈ ���� ����
        retval = recv(socket, buf, PACKETSIZEMAX, 0);
        if(retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if(retval == 0) {		// ���� ����
            break;
        }

        // �÷��̾� ���� ������Ʈ
        // ...
    }

    // World���� �÷��̾� ����
    world.removePlayer(id);
    cout << "Players: \n";
    for(const auto& p : world.getPlayers()) {
        printf(" - id: %2d\n", p.first);
    }

    // ��� ���� �ݱ�
    closesocket(socket);

    socket_mutex.lock();
    sockets[id] = INVALID_SOCKET;
    socket_mutex.unlock();
}


int main() {
    // ------------------------------------- ���� ���� -------------------------------------
    //World world;
    world.setUp();

    thread game_logic { [&]() { run_game(world); } };
    game_logic.detach();


    // ------------------------------------- ��Ʈ��ũ �۾� -------------------------------------
    for(auto& sock : sockets) {
        sock = INVALID_SOCKET;
    }


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
            socket_mutex.lock();
            for(int i=0; i<MAX_CLIENTS; ++i) {
                if(sockets[i] == INVALID_SOCKET) {
                    sockets[i] = client_sock;
                    std::thread new_client_thread { handle_connection, client_sock, clientaddr, i };
                    new_client_thread.detach();

                    wait = false;
                    break;
                }
            }
            socket_mutex.unlock();
        }
    }

    // ��� ���� �ݱ�
    closesocket(listen_sock);

    WSACleanup();
}
