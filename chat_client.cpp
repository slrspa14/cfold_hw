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
    if(WSAStartup(MAKEWORD(2,2), &wsaData) !=0) //윈속 소켓명시 및 라이브러리 초기화
        ErrorHandling("WSAStartup() error!");
    sprintf(name, "[%s]", argv[3]);
    hSock = socket(PF_INET, SOCK_STREAM, 0); //소켓생성
    //초기화
    //서버주소 구조체 설정
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if(connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) //연결
        ErrorHandling("connect() error!");
    hSndThread = 
        (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);//send용 thread
    hRcvThread =
        (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);//recv용 thread
    WaitForSingleObject(hSndThread, INFINITE);//단일개체 대기 hSndThread가 끝날때까지 대기중
    WaitForSingleObject(hRcvThread, INFINITE);//단일개체 대기 hRcvThread가 끝날때까지 대기중
    //여러개체 대기 시키려면 WaitForMultipleObjects
    closesocket(hSock);
    WSACleanup();//윈속 해제
    return 0;
}
unsigned WINAPI SendMsg(void * arg) //send thread main
{
    SOCKET hSock = *((SOCKET*)arg); //소켓 형변환
    char nameMsg[NAME_SIZE + BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))//q, Q 누르면 나가게
        {
            closesocket(hSock);//소켓연결종료
            exit(0);
        }
        sprintf(nameMsg, "%s %s", name, msg); //배열에 작성된 바이트 수 리턴 끝 널문자 계산안함
        send(hSock, nameMsg, strlen(nameMsg), 0);//소켓 핸들값, 보낼 데이터 버퍼의 주소값, 보낼 바이트 수, 전달옵션 거의0
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
        strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE-1, 0);//소켓핸들값, 받는 데이터 버퍼의 주소값, 받을 바이트 수, 받을때 옵션, 널빼고 받으려고 사이즈 -1
        //수신된 데이터의 크기 반환
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
