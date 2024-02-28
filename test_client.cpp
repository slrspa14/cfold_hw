#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <string>
#include <list>
#include <vector>

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void *arg);
unsigned WINAPI RecvMsg(void *arg);
void ErrorHandling(char * msg);

char nickname[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

std::vector<std::string> domodachi;
std::string add_friend;
// HANDLE hMutex;

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

    // hMutex = CreateMutex(NULL, FALSE, NULL);
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if(connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling((char*)"connect() error!");
    // std::cin.get();
    // std::cin.get();
    std::cout << "닉네임을 입력해주세요." << std::endl;
    std::cin >> nickname;
    send(hSock, nickname, strlen(nickname), 0);//닉네임 전송
    // std::cin.clear();/
    //중복결과 받기
    while(1)
    {
        recv(hSock, msg, BUF_SIZE, 0);
        if(std::string (msg) == "re")
        {
            std::cout << "중복된 닉네임이 접속중입니다. 다시 입력해주세요." << std::endl;
            std::cout << "닉네임: ";
            std::cin >> nickname;
            send(hSock, nickname, strlen(nickname), 0);
            // std::cin.clear();
        }
        else
            break;
    }
    int select;
    std::cout << "====================" << std::endl;
    std::cout << "1. 1:1채팅" << std::endl;
    std::cout << "2. 1:다수 채팅" << std::endl;
    std::cout << "3. 친구추가" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "메뉴선택:";
    std::cin >> select;
    // std::cin.clear();

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
        // std::cin.clear();
        // std::cin.get();
    }
    std::string choice = std::to_string(select);//형변환
    send(hSock, choice.c_str(), strlen(choice.c_str()), 0);//전송
    
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
    int hSock = *((SOCKET*)arg);//소켓도 잘 나옴
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen, i = 0;
    std::string  recv_msg, sel_friend;
    while(strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE-1, 0))
    {
        nameMsg[strLen] = 0;
        if(strLen == -1)
            return -1;
        if(std::string(nameMsg) == "menu")
        {
            int select;
            std::cout << "====================" << std::endl;
            std::cout << "1. 1:1채팅" << std::endl;
            std::cout << "2. 1:다수 채팅" << std::endl;
            std::cout << "====================" << std::endl;
            std::cout << "메뉴선택:";
            std::cin >> select;
            std::string test = std::to_string(select);
            send(hSock, test.c_str(), strlen(test.c_str()), 0);
        }
        else if(!strncmp(nameMsg, "1", 1))
        {
            //친구목록
            for(int i = 0 ; i < domodachi.size() ; i++)
                std::cout << domodachi[i] << std::endl;
            std::cout << "친구고르기:";
            std::cin >> sel_friend;
            std::string com_friend = "5" + sel_friend;
            send(hSock, com_friend.c_str(), strlen(com_friend.c_str()), 0);
        }
        else if(!strncmp(nameMsg, "2", 1))
        {
            //친구목록 받아오고
            // std::cout << "2번진입";
            send(hSock, "2", strlen("2"),0);
        }
        // else if(std::string(nameMsg) == "3")
        else if(!strncmp(nameMsg, "3", 1))
        {
            //접속현황
            // WaitForSingleObject(hMutex, INFINITE);//아니네
            for(i = 1 ; i < strLen ; i++)
            {
                recv_msg += nameMsg[i];
            }
            std::cout << "추가가능한 접속인원: " << recv_msg << std::endl;
            std::cout << "친구추가를 원하시면 닉네임을 입력해주세요." << std::endl;
            std::cout << "입력: ";
            std::cin >> msg;
            if(std::string(msg) == nickname)
            {
                while(1)
                {
                    std::cout << "다른 유저의 이름을 입력해주세요." << std::endl;
                    std::cin >> msg;
                    if(std::string(msg) != nickname)
                        break;
                }
            }
            std::string addition = "4";
            addition.append(msg);
            add_friend = std::string(msg);
            // std::cout << addition << "addition확인" << std::endl;
            send(hSock, addition.c_str(), strlen(addition.c_str()), 0);
            // std::cin.clear();
            // ReleaseMutex(hMutex);
        }
        else if(!strncmp(nameMsg, "4", 1))
        {
            domodachi.push_back(add_friend); //친추하고 메뉴로
            std::cout << "친구추가 되었습니다." << std::endl;
        }
        else if(!strncmp(nameMsg, "99", 1))//오류같은거 나오면 받아서 올라가게 근데 그러면 recv에 계속 대기상태인데
        {
            continue;
        }
        // fputs(nameMsg.c_str(), stdout);
    }
    return 0;
}
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}