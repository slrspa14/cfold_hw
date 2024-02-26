#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define NAME_SIZE 20

unsigned WINAPI HandleClnt(void *arg);
void SendMsg(char *msg, int len);
void ErrorHandling(char *msg);
// unsigned WINAPI random_chat(void *arg);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
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
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
        ErrorHandling((char*)"WSAStartup() error");
    hMutex = CreateMutex(NULL, FALSE, NULL);
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));
    
    if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling((char*)"bind() error");
    
    if(listen(hServSock, 5)==SOCKET_ERROR)
        ErrorHandling((char*)"listen() error");
    //접속시간용
    std::time_t t=std::time(0);
    std::tm* now = std::localtime(&t);
    // std::vector<std::string> userid;

    while(1)
    {
        clntAdrSz = sizeof(clntAdr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

        WaitForSingleObject(hMutex, INFINITE);
        clntSocks[clntCnt++] = hClntSock;
        ReleaseMutex(hMutex);

        char nickname[NAME_SIZE];
        recv(hClntSock, nickname, sizeof(nickname),0);//닉네임 받고 바로 저장하기 구별용으로
        std::string user_name(nickname);
        std::vector<std::string> userid;
        
        for(int i = 0; i < clntCnt; i++)
        {
            while(userid[i] == user_name)
            {
                std::cout << "중복임" << std::endl;
                sort(userid.begin(), userid.end()); //정렬 시키고
                userid.erase(unique(userid.begin(), userid.end(), userid.end()));//중복값 지우고
                std::string msg = "re";
                send(hClntSock, msg.c_str(), strlen(msg.c_str()), 0);//다시하라고 문자전송
            }
        }
        userid.push_back(user_name);//닉네임 벡터 저장

        //map 소켓 닉네임 저장
        std::map<SOCKET, char*> devide;
        devide [clntSocks[clntCnt-1]] = nickname;

        std::string test = "좆같다";
        send(hClntSock, test.c_str(), strlen(test.c_str()), 0);
        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);
        std::cout << "Connected client IP:" << inet_ntoa(clntAdr.sin_addr) << ", TIME:" << (now->tm_hour) << ":" << now->tm_min << std::endl;
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI HandleClnt(void *arg)
{
    SOCKET hClntSock=*((SOCKET*)arg);
    int strLen = 0, i;
    char msg[BUF_SIZE];    
    while((strLen = recv(hClntSock, msg, sizeof(msg), 0))!=0)
    {   
        //받은 내용 전부 보내주려고
        //단톡용
        SendMsg(msg, strLen);
    }

    WaitForSingleObject(hMutex, INFINITE);
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
    ReleaseMutex(hMutex);
    closesocket(hClntSock);
    return 0;
}
void SendMsg(char *msg, int len)
{
    int i;
    WaitForSingleObject(hMutex, INFINITE);
    for(i = 0; i<clntCnt; i++)
        send(clntSocks[i], msg, len, 0);
    ReleaseMutex(hMutex);
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}