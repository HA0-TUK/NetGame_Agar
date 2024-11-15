///////////////////////////////////////////////////////////////////////////////////////////////////////
// File name: NetworkFunction.h
// Created: 2045-11-09 18:09:00
// Author: ������
// Description: ��Ʈ��ũ �Լ��� ������ ��� �����Դϴ�.
///////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Common.h"

//////////////////////////////////////////////////////////////////////////
// Network Function

#define SERVERPORT 9000
#define BUFSIZE    512

void NetworkInitialize();  // ��Ʈ��ũ �ʱ�ȭ
void NetworkFinalize();    // ��Ʈ��ũ ����

DWORD WINAPI RecvThread(LPVOID arg); // ������ ���� ������
void ProcessPacket(char* buf);		 // ��Ŷ ó�� �Լ�
void SendData(char* buf);			 // ������ ���� �Լ�

