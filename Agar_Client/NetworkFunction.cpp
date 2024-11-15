///////////////////////////////////////////////////////////////////////////////////////////////////////
// File name: NetworkFunction.cpp
// Created: 2045-11-09 18:09:00
// Author: ������
// Description: ��Ʈ��ũ �Լ��� ������ �ҽ� �����Դϴ�.
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "NetworkFunction.h"

#include <iostream>
#include <string>

char* SERVERIP = (char*)"127.0.0.1";

WSADATA wsadata;
SOCKET clientsocket;
struct sockaddr_in serveraddr;

void NetworkInitialize()
{
	int retval;

	// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		err_quit("WSAStartup()");

	// ���� ����
	clientsocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientsocket == INVALID_SOCKET)
		err_quit("socket()");

	// ���� �ּ� ����
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);

	// ������ ����
	retval = connect(clientsocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

}

void NetworkFinalize()
{
	// ���� �ݱ�
	closesocket(clientsocket);

	// ���� ����
	WSACleanup();
}


void SendData(char* buf)
{
	int retval;

	// ������ ����
	retval = send(clientsocket, buf, strlen(buf), 0);
	if (retval == SOCKET_ERROR) err_quit("send()");
}


DWORD __stdcall RecvThread(LPVOID arg)
{
	int retval;

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE];
	size_t len;

	while (1)
	{
		// ������ ����
		retval = recv(clientsocket, buf, BUFSIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) err_quit("recv()");
		else if (retval == 0) break;

		// ������ ó��
		ProcessPacket(buf);
	}

	return 0;
}

void ProcessPacket(char* buf)
{
	// ��Ŷ �м�
	
	// ��Ŷ ó��
}
