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

std::vector<std::string> userid;//닉네임 저장용
std::map<SOCKET, char*> devide;//소켓저장용

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

    while(1)
    {
        clntAdrSz = sizeof(clntAdr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

        WaitForSingleObject(hMutex, INFINITE);
        clntSocks[clntCnt++] = hClntSock;
        ReleaseMutex(hMutex);
        int msglen;
        char nickname[NAME_SIZE];
        msglen = recv(hClntSock, nickname, sizeof(nickname)-1,0);//닉네임 받고
        nickname[msglen] = 0;//마지막에널문자용

        std::string user_name(nickname);
        std::cout << userid.size();
        bool duplication;
        if(std::find(userid.begin(), userid.end(), user_name) != userid.end()) //vector 탐색 중복용        
        {
            duplication = false;
            std::string msg = "re";
            // std::cout << "test";
            send(hClntSock, msg.c_str(), strlen(msg.c_str()), 0);
        }
        else
        {
            duplication = true;
            std::string msg = "ok";
            // std::cout << "ㅌㄱ";
            send(hClntSock, msg.c_str(), strlen(msg.c_str()), 0);
            userid.push_back(user_name);//닉네임 벡터 저장
        }
        // for (int i = 0; i < userid.size(); i++) //확인용
        // {
        //     std::cout << userid[i];
        // }

        //map 소켓 닉네임 저장
        devide [clntSocks[clntCnt-1]] = nickname;

        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);
        std::cout << "Connected client IP:" << inet_ntoa(clntAdr.sin_addr) << ", TIME:" << (now->tm_hour) << ":" << now->tm_min << std::endl;
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI HandleClnt(void *arg)
{
    //들어와서 바로 소켓 구분해서 보내주기
    //구분자 붙여서 보내주기
    SOCKET hClntSock=*((SOCKET*)arg);
    int strLen = 0, i;
    char msg[BUF_SIZE];
    while((strLen = recv(hClntSock, msg, sizeof(msg), 0))!=0)
    {
        if(std::string(msg) == "1")//1:1채팅
        {
            //친구닉네임 입력하게 하기
            //친구닉네임 수신후 맞는 소켓 찾아서 둘이 채팅하게 하기
        }
        else if(std::string(msg) == "2")//1:다 채팅
        {
            SendMsg(msg, strLen);
        }
        else //친구찾기
        {
            std::cout << "친구찾기";
            std::string user_list;
            //접속현황 띄워주고 친추할 인원 수신받기
            for(int i =0; i<userid.size() ; i++)
            {                
                user_list = "접속인원: " + userid[i] + ", ";
                std::cout << user_list;
            }
        }
        // SendMsg(msg, strLen);
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