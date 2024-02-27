#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <thread>

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void *arg);
unsigned WINAPI RecvMsg(void *arg);
void ErrorHandling(char * msg);

char nickname[NAME_SIZE] = "[DEFAULT]";
// char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{    
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN servAdr;
    HANDLE hSndThread, hRcvThread;
    if(argc !=3)
    {
        printf("Usage : %s <IP> <port> \n", argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2,2), &wsaData) !=0)
        ErrorHandling((char*)"WSAStartup() error!");    

    hSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if(connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling((char*)"connect() error!");

    std::cout << "닉네임을 입력해주세요." << std::endl;
    std::cin >> nickname;
    std::cin.ignore();
    send(hSock, nickname, strlen(nickname), 0);//닉네임 전송
    //중복결과 받기
    while(1)
    {
        recv(hSock, msg, BUF_SIZE, 0);
        if(std::string (msg) == "re")
        {
            std::cout << "중복된 닉네임이 접속중입니다. 다시 입력해주세요." << std::endl;
            std::cout << "닉네임: ";            
            std::cin >> nickname;
            std::cin.ignore();
            send(hSock, nickname, strlen(nickname), 0);
        }
        else
            break;
    }

    hSndThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
    hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);
    
    WaitForSingleObject(hSndThread, INFINITE);
    WaitForSingleObject(hRcvThread, INFINITE);
    
    closesocket(hSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI SendMsg(void * arg)
{
    SOCKET hSock = *((SOCKET*)arg);
    
    int select;
    std::cout << "====================" << std::endl;
    std::cout << "1. 1:1채팅" << std::endl;
    std::cout << "2. 1:다수 채팅" << std::endl;
    std::cout << "3. 친구추가" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "메뉴선택:";
    std::cin >> select;
    while(select > 4)
    {
        std::cout << "잘못된 입력입니다. 다시 입력해주세요" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << "1. 1:1채팅" << std::endl;
        std::cout << "2. 1:다수 채팅" << std::endl;
        std::cout << "3. 친구추가" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << "메뉴선택:";
        std::cin >> select;
    }
    std::string choice = std::to_string(select);//형변환
    send(hSock, choice.c_str(), strlen(choice.c_str()), 0);//전송
    //수신, 여기에 멈춰있구나
    // recv(hSock, msg, strlen(msg), 0);

    char nameMsg[NAME_SIZE + BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {            
            closesocket(hSock);
            exit(0);
        }
        sprintf(nameMsg, "[%s]: %s", nickname, msg);//name->nickname
        send(hSock, nameMsg, strlen(nameMsg), 0);
    }
    return 0;
}
unsigned WINAPI RecvMsg(void * arg)
{
    //send함수에서 보내고 여기서 받고
    //서버가 보내준거 처리하기
    //서버에서 구분자 붙여서 보낸거 받기
    int hSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen;
    while(1)
    {
        strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE-1, 0);
        if(strLen == -1)
            return -1;
        
        if(std::string(nameMsg) == "1_갠톡")
        {
            //한 명 누구 고를건지
        }
        else if(std::string(nameMsg) == "2_단톡")
        {
            //초대하라고 말하겄지
        }
        else if(std::string(nameMsg) == "3_친구찾기")
        {
            //접속현황
            std::cout << nameMsg << std::endl;
        }
        // nameMsg[strLen] = 0;
        // fputs(nameMsg, stdout);
    }
    return 0;
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}