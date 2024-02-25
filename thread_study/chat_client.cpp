#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void *arg);
unsigned WINAPI RecvMsg(void *arg);
void ErrorHandling(char * msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{    
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN servAdr;
    HANDLE hSndThread, hRcvThread;
    if(argc !=4)
    {
        printf("Usage : %s <IP> <port> <name> \n", argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2,2), &wsaData) !=0) //���� ���ϸ�� �� ���̺귯�� �ʱ�ȭ
        ErrorHandling("WSAStartup() error!");
    sprintf(name, "[%s]", argv[3]);
    hSock = socket(PF_INET, SOCK_STREAM, 0); //���ϻ���
    //�ʱ�ȭ
    //�����ּ� ����ü ����
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if(connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) //����
        ErrorHandling("connect() error!");
    hSndThread = 
        (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);//send�� thread
    hRcvThread =
        (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);//recv�� thread
    WaitForSingleObject(hSndThread, INFINITE);//���ϰ�ü ��� hSndThread�� ���������� �����
    WaitForSingleObject(hRcvThread, INFINITE);//���ϰ�ü ��� hRcvThread�� ���������� �����
    //������ü ��� ��Ű���� WaitForMultipleObjects
    closesocket(hSock);
    WSACleanup();//���� ����
    return 0;
}
unsigned WINAPI SendMsg(void * arg) //send thread main
{
    SOCKET hSock = *((SOCKET*)arg); //���� ����ȯ
    char nameMsg[NAME_SIZE + BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n") || !strcmp(msg, "��2\n") || !strcmp(msg, "����\n"))//q, Q ������ ������
        {
            closesocket(hSock);//���Ͽ�������
            exit(0);
        }
        sprintf(nameMsg, "%s %s", name, msg); //�迭�� �ۼ��� ����Ʈ �� ���� �� �ι��� ������
        send(hSock, nameMsg, strlen(nameMsg), 0);//���� �ڵ鰪, ���� ������ ������ �ּҰ�, ���� ����Ʈ ��, ���޿ɼ� ����0
    }
    return 0;
}
unsigned WINAPI RecvMsg(void * arg) //read thread main
{
    int hSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen;
    while(1)
    {
        strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE-1, 0);//�����ڵ鰪, �޴� ������ ������ �ּҰ�, ���� ����Ʈ ��, ������ �ɼ�, �λ��� �������� ������ -1
        //���ŵ� �������� ũ�� ��ȯ
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
