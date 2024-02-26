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

    std::cout << "�г����� �Է����ּ���." << std::endl;
    std::cin.getline(nickname, NAME_SIZE);
    // std::cin >> nickname;
    send(hSock, nickname, strlen(nickname), 0);//�г��� ����
    //�ߺ���� �ޱ�
    while(1)
    {
        recv(hSock, msg, BUF_SIZE, 0);
        // recv(hSock, msg, strlen(msg), 0); //�� ���� �����ִ°ž�
        std::cout << "��";
        if(msg == "re")
        {
            std::cout << "�ٽ�";
            std::cin.getline(nickname, NAME_SIZE);
            std::cin >> nickname;
        }
        else
            break;
    }

    hSndThread =
        (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
    hRcvThread =
        (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);
    
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
    std::cout << "1. 1:1ä��" << std::endl;
    std::cout << "2. 1:�ټ� ä��" << std::endl;
    std::cout << "3. ģ���߰�" << std::endl;
    std::cout << "4. ���ӻ��� Ȯ��" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "�޴�����:";
    std::cin >> select;
    while(select > 4)
    {
        std::cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << "1. 1:1ä��" << std::endl;
        std::cout << "2. 1:�ټ� ä��" << std::endl;
        std::cout << "3. ģ���߰�" << std::endl;
        std::cout << "4. ���ӻ��� Ȯ��" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << "�޴�����:";
        std::cin >> select;
    }
    std::string choice = std::to_string(select);//����ȯ
    send(hSock, choice.c_str(), strlen(choice.c_str()), 0);//����
    //����, ���⿡ �����ֱ��� �� ������
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
    int hSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen;
    while(1)
    {
        strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE-1, 0);
        if(strLen == -1)
            return -1;
        nameMsg[strLen] = 0;
        fputs(nameMsg, stdout);
    }
    return 0;
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}