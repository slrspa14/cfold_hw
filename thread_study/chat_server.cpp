#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <ctime>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define NAME_SIZE 20

unsigned WINAPI HandleClnt(void *arg);
void SendMsg(char *msg, int len);
void ErrorHandling(char *msg);
unsigned WINAPI random_chat(void *arg);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
char name[NAME_SIZE] = "[DEFAULT]";
HANDLE hMutex;

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSz;
    HANDLE hThread, chat_thread;
    if (argc !=2)
    {
        printf("Usage:%s <port>\n", argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)//?��?�� ?���? 명시 �? ?��?��브러�? 초기?��
        ErrorHandling("WSAStartup() error");
    hMutex = CreateMutex(NULL, FALSE, NULL);//?��?��?�� ?��계영?�� �?리용 Mutex
    hServSock = socket(PF_INET, SOCK_STREAM, 0);//?��버소켓생?��
    //초기?��
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    //?��?��
    if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    //?��결요�?
    if(listen(hServSock, 5)==SOCKET_ERROR)//5개까�? ???�?
        ErrorHandling("listen() error");
    std::time_t t=std::time(0); //?��?��?��간뽑�?(64비트?��?��?��)
    std::tm* now = std::localtime(&t);//?��?��?�� ?��맷팅
    while(1)
    {
        clntAdrSz = sizeof(clntAdr);//?��?��?��?��즈구?���?
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);//?��결수?��(?���?,?��?��주소,?��?��?��?���?)
        
        WaitForSingleObject(hMutex, INFINITE); //?��계영?�� ?��?��
        clntSocks[clntCnt++] = hClntSock;
        ReleaseMutex(hMutex); //?��
        // chat_thread = (HANDLE)_beginthreadex(NULL, 0, random_chat, (void*)&hClntSock, 0, NULL);//asd
        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);//?��?��?�� ?��?��
        
        printf("Connected client IP:%s, time:%d:%d\n", inet_ntoa(clntAdr.sin_addr), (now->tm_hour), (now->tm_min));
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

// unsigned WINAPI random_chat(void *arg) //메뉴?�� 보여주기 ?���? 받으�? ?��?��?��?�� ????��?���? ?��켓이?��
// {
//     SOCKET hClntSock=*((SOCKET*)arg); //?���??��
//     char menu[BUF_SIZE] = "1. 단톡\n2. 갠톡";
//     send(clntSocks[clntCnt], menu, sizeof(menu), 0);
//     return 0;
// }

unsigned WINAPI HandleClnt(void *arg)
{
    SOCKET hClntSock=*((SOCKET*)arg); //?���??��
    int strLen = 0, i;
    char msg[BUF_SIZE];
    while((strLen = recv(hClntSock, msg, sizeof(msg), 0))!=0) //?��?��:?���?,메시�?,메시�??��기만?��,?��?���?
        SendMsg(msg, strLen);

    WaitForSingleObject(hMutex, INFINITE);//?��?��개체???기시?��?�� ?��?��,?��계영?��?��?��
    for (i = 0; i < clntCnt; i++)
    {
        if(hClntSock == clntSocks[i])
        {
            while(i++<clntCnt-1)
                clntSocks[i] = clntSocks[i+1];
            break;
        }
    }
    clntCnt--;
    ReleaseMutex(hMutex);//?��계영?�� ?��?���?
    closesocket(hClntSock);
    return 0;
}
void SendMsg(char *msg, int len) //?��체발?��
{
    int i;
    WaitForSingleObject(hMutex, INFINITE);
    for(i = 0; i<clntCnt; i++)
        send(clntSocks[i], msg, len, 0); //?���? ?��?���?, 보낼 ?��?��?�� 버퍼?�� 주소�?, 보낼 바이?�� ?��, ?��?��?��?�� 거의0
    ReleaseMutex(hMutex);
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);

}