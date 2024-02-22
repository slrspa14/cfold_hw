#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI HandleClnt(void *arg);
void SendMsg(char *msg, int len);
void ErrorHandling(char *msg);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSz;
    HANDLE hThread;
    if (argc !=2) //디스크립터 값 2아니라면
    {
        printf("Usage:%s <port>\n", argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)//윈속 소켓 명시 및 라이브러리 초기화
        ErrorHandling("WSAStartup() error");
    hMutex = CreateMutex(NULL, FALSE, NULL);//쓰레드 임계영역 관리용 Mutex
    hServSock = socket(PF_INET, SOCK_STREAM, 0);//서버소켓생성
    //초기화
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    //할당
    if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    //연결요청
    if(listen(hServSock, 5)==SOCKET_ERROR)//5개까지 대기
        ErrorHandling("listen() error");
    while(1)
    {
        clntAdrSz = sizeof(clntAdr);//클라사이즈구하고
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);//연결수락(소켓,클라주소,클라사이즈)
        
        WaitForSingleObject(hMutex, INFINITE); //임계영역 시작?
        clntSocks[clntCnt++] = hClntSock;//클라 입장마다 늘어나는 cnt로 배열에 클라소켓 집어넣고
        ReleaseMutex(hMutex); //끝?
        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);//쓰레드 생성
        printf("Connected client IP:%s \n", inet_ntoa(clntAdr.sin_addr));
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI HandleClnt(void *arg)
{
    SOCKET hClntSock=*((SOCKET*)arg); //형변환
    int strLen = 0, i;
    char msg[BUF_SIZE];
    while((strLen = recv(hClntSock, msg, sizeof(msg), 0))!=0) //수신:소켓,메시지,메시지크기만큼,플래그
        SendMsg(msg, strLen);

    WaitForSingleObject(hMutex, INFINITE);//단일개체대기시키는 함수,임계영역시작
    for (i = 0; i < clntCnt; i++) //연결종료용
    {
        if(hClntSock == clntSocks[i])
        {
            while(i++<clntCnt-1)
                clntSocks[i] = clntSocks[i+1];
            break;
        }
    }
    clntCnt--;
    ReleaseMutex(hMutex);//임계영역 끝내고
    closesocket(hClntSock);
    return 0;
}
void SendMsg(char *msg, int len) //전체발송
{
    int i;
    WaitForSingleObject(hMutex, INFINITE);
    for(i = 0; i<clntCnt; i++)
        send(clntSocks[i], msg, len, 0); //소켓 핸들값, 보낼 데이터 버퍼의 주소값, 보낼 바이트 수, 전달옵션 거의0
    ReleaseMutex(hMutex);
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}