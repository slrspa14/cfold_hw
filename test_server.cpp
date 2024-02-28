#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <thread>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define NAME_SIZE 20

unsigned WINAPI HandleClnt(void *arg);
unsigned WINAPI single_chat(void *arg);
void SendMsg(char *msg, int len);
void ErrorHandling(char *msg);
void nickname_check(void *arg);//중복검사용
void multi_chat(char *msg, int len);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

std::vector<std::string> userid;//닉네임 저장용
std::map<SOCKET, std::string> devide;//소켓저장용

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    HANDLE hThread, chat_thread;
    int clntAdrSz;
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

        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);

        std::cout << "Connected client IP:" << inet_ntoa(clntAdr.sin_addr) << ", TIME:" << (now->tm_hour) << ":" << now->tm_min << std::endl;
        
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void nickname_check(void *arg)
{
    SOCKET hClntSock=*((SOCKET*)arg);
    int msglen;
    char nickname[NAME_SIZE];
    while(1)
    {
        msglen = recv(hClntSock, nickname, sizeof(nickname)-1,0);//닉네임 받고
        nickname[msglen] = 0;//마지막에널문자용

        std::string user_name(nickname);
        // std::cout << userid.size();
        bool duplication;//중복용
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
            // devide.insert(clntSocks[clntCnt-1], user_name);
            devide [clntSocks[clntCnt-1]] = user_name;//map 소켓 닉네임 저장
            break;
        }
    }
}

void multi_chat(char *msg, int len)
{
    std::cout << "test" << std::endl;
    std::cout << "안되면 접음 ㅅㄱ" << std::endl;
    //단톡입장 물어보기
    if(msg == "y" || msg == "Y")
    {
        int i;
        WaitForSingleObject(hMutex, INFINITE);
        for(i = 0; i<clntCnt; i++)
            send(clntSocks[i], msg, len, 0);
        ReleaseMutex(hMutex);
    }
    else
        std::cout << "다시" << std::endl;
}

// unsigned WINAPI single_chat(void *arg)
// {
//     SOCKET hClntSock = *((SOCKET*)arg);
//     int strLen = 0;
//     char msg[BUF_SIZE];

//     // 데이터 수신 및 처리
//     while((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != -1) // 클라이언트로부터 데이터 읽어들임(데이터 recv)
//     {
//         SendMsg(hClntSock, msg, strLen);                                   // 읽은 데이터 처리 함수
//     }
//     // 클라이언트 연결이 종료되었을 때, 클라이언트 소켓을 관리하는 배열에서 해당 클라이언트 소켓 제거
//     WaitForSingleObject(hMutex, INFINITE);

//     // 채팅방에서 보내는 소켓 삭제
//     int count = 0;
//     for(auto i:singleChat)
//     {
//         if(i != hClntSock)
//             count++;
//         else
//             break;
//     }
//     if(count%2 == 0)
//     {
//         singleChat.erase(singleChat.begin() + (count+1));
//         singleChat.erase(singleChat.begin() + count);
//     }
//     else
//     {
//         singleChat.erase(singleChat.begin() + count);
//         singleChat.erase(singleChat.begin() + (count-1));
//     }
// }

unsigned WINAPI HandleClnt(void *arg)
{
    SOCKET hClntSock=*((SOCKET*)arg);
    nickname_check((void*)&hClntSock);//중복검사 호출하고
    //스위치로 돌리기
    int strLen = 0, i;
    char msg[BUF_SIZE];
    while((strLen = recv(hClntSock, msg, sizeof(msg), 0))!=0)
    {
        int select = atoi(msg);
        if(!strncmp(msg, "1", 1))
        {
            //친구입력하라하기
            send(hClntSock, "1친구선택", strlen("1친구선택"), 0);
        }
        else if(!strncmp(msg ,"2", 1))
        {
            // multi_chat(msg, strLen);
            // std::thread multi(multi_chat);
        }
        else if(!strncmp(msg ,"3", 1))
        {        
            std::string user_list, test;
            //접속현황 띄워주고 친추할 인원 수신받기
            for(int i =0; i<userid.size() ; i++)
            {
                user_list += userid[i];
                if(i != userid.size() -1)
                    user_list += ", ";
                test = "3" + user_list;
            }
            // std::cout << test << std::endl;
            // std::cout << strlen(test.c_str()) << std::endl;
            send(hClntSock, test.c_str(), strlen(test.c_str()), 0);//접속 리스트 보내주고
        }
        else if(!strncmp(msg ,"4", 1))
        {
            std::cout << "4번진입" << std::endl;
            std::string(msg);
            send(hClntSock, "4", strlen("4"), 0);
            Sleep(3);
            send(hClntSock, "menu", strlen("menu"), 0);
            std::cout << "보냄" << std::endl;
        }
        else if(!strncmp(msg, "5", 1))
        {
            std::cout << "1:1 채팅방" << std::endl;
            std::thread(single_chat, (void*)&hClntSock);
        }
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