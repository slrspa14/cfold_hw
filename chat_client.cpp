#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <sstream>
#include <string>
#include <sstream>
#include <list>

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
    std::cin >> nickname;
    std::cin.ignore();
    send(hSock, nickname, strlen(nickname), 0);//�г��� ����
    //�ߺ���� �ޱ�
    while(1)
    {
        recv(hSock, msg, BUF_SIZE, 0);
        if(std::string (msg) == "re")
        {
            std::cout << "�ߺ��� �г����� �������Դϴ�. �ٽ� �Է����ּ���." << std::endl;
            std::cout << "�г���: ";
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
    std::cout << "1. 1:1ä��" << std::endl;
    std::cout << "2. 1:�ټ� ä��" << std::endl;
    std::cout << "3. ģ���߰�" << std::endl;
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
        std::cout << "====================" << std::endl;
        std::cout << "�޴�����:";
        std::cin >> select;
    }
    std::string choice = std::to_string(select);//����ȯ
    send(hSock, choice.c_str(), strlen(choice.c_str()), 0);//����

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
    //send�Լ����� ������ ���⼭ �ް�
    //������ �����ذ� ó���ϱ�
    //�������� ������ �ٿ��� ������ �ޱ�
    // std::cout << "����";//�����µ�
    int hSock = *((SOCKET*)arg);
    // std::cout << hSock  << "����" << std::endl;
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen, i = 0;
    while(1)
    {
        std::cout << "Ȯ�ο�";
        // recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE-1, 0);
        //�� 5���� ����
        strLen = recv(hSock,nameMsg,sizeof(NAME_SIZE + BUF_SIZE-1),0);
        nameMsg[strLen] = 0;
        // std::cout << nameMsg << "namemsg" << std::endl;
        std::string(nameMsg);
        std::istringstream split(nameMsg);
        char devide = '_';
        std::string test;
        std::list<std::string> recvmsg;
        while(getline(split, test, devide))
        {
            std::cout << test << std::endl;
        }

        if(strLen == -1)
            return -1;
        
        if(std::string(nameMsg) == "1_����")
        {
            //�� �� ���� ������
        }
        else if(std::string(nameMsg) == "2_����")
        {
            //�ʴ��϶�� ���ϰ���
        }
        else if(std::string(nameMsg) == "3")
        {
            //������Ȳ
            std::cout << nameMsg << std::endl;
        }
        fputs(nameMsg.c_str(), stdout);
    }
    return 0;
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}