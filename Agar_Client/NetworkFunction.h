///////////////////////////////////////////////////////////////////////////////////////////////////////
// File name: NetworkFunction.h
// Created: 2045-11-09 18:09:00
// Author: 윤영민
// Description: 네트워크 함수를 정의한 헤더 파일입니다.
///////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Common.h"
#include "../protocol.h"

//////////////////////////////////////////////////////////////////////////
// Network Function

#define SERVERPORT 9000
#define BUFSIZE    512

extern char* SERVERIP;
extern SOCKET clientsocket;


void NetworkInitialize();  // 네트워크 초기화
void NetworkFinalize();    // 네트워크 종료

//DWORD WINAPI RecvThread(LPVOID arg); // 데이터 수신 스레드 -> GameScene에서 구현
//void ProcessPacket(char* buf);		 // 패킷 처리 함수 -> GameScene에서 구현
void SendData(void* buf, int size);			 // 데이터 전송 함수
int RecvData(char buf[]);
PACKET_HEADER* RecvPacket();
