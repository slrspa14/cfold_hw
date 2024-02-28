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
void nickname_check(void *arg);//�ߺ��˻��
void multi_chat(char *msg, int len);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

std::vector<std::string> userid;//�г��� �����
std::map<SOCKET, std::string> devide;//���������

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
    //���ӽð���
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
        msglen = recv(hClntSock, nickname, sizeof(nickname)-1,0);//�г��� �ް�
        nickname[msglen] = 0;//���������ι��ڿ�

        std::string user_name(nickname);
        // std::cout << userid.size();
        bool duplication;//�ߺ���
        if(std::find(userid.begin(), userid.end(), user_name) != userid.end()) //vector Ž�� �ߺ���        
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
            // std::cout << "����";
            send(hClntSock, msg.c_str(), strlen(msg.c_str()), 0);
            userid.push_back(user_name);//�г��� ���� ����
            // devide.insert(clntSocks[clntCnt-1], user_name);
            devide [clntSocks[clntCnt-1]] = user_name;//map ���� �г��� ����
            break;
        }
    }
}

void multi_chat(char *msg, int len)
{
    std::cout << "test" << std::endl;
    std::cout << "�ȵǸ� ���� ����" << std::endl;
    //�������� �����
    if(msg == "y" || msg == "Y")
    {
        int i;
        WaitForSingleObject(hMutex, INFINITE);
        for(i = 0; i<clntCnt; i++)
            send(clntSocks[i], msg, len, 0);
        ReleaseMutex(hMutex);
    }
    else
        std::cout << "�ٽ�" << std::endl;
}

// unsigned WINAPI single_chat(void *arg)
// {
//     SOCKET hClntSock = *((SOCKET*)arg);
//     int strLen = 0;
//     char msg[BUF_SIZE];

//     // ������ ���� �� ó��
//     while((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != -1) // Ŭ���̾�Ʈ�κ��� ������ �о����(������ recv)
//     {
//         SendMsg(hClntSock, msg, strLen);                                   // ���� ������ ó�� �Լ�
//     }
//     // Ŭ���̾�Ʈ ������ ����Ǿ��� ��, Ŭ���̾�Ʈ ������ �����ϴ� �迭���� �ش� Ŭ���̾�Ʈ ���� ����
//     WaitForSingleObject(hMutex, INFINITE);

//     // ä�ù濡�� ������ ���� ����
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
    nickname_check((void*)&hClntSock);//�ߺ��˻� ȣ���ϰ�
    //����ġ�� ������
    int strLen = 0, i;
    char msg[BUF_SIZE];
    while((strLen = recv(hClntSock, msg, sizeof(msg), 0))!=0)
    {
        int select = atoi(msg);
        if(!strncmp(msg, "1", 1))
        {
            //ģ���Է��϶��ϱ�
            send(hClntSock, "1ģ������", strlen("1ģ������"), 0);
        }
        else if(!strncmp(msg ,"2", 1))
        {
            // multi_chat(msg, strLen);
            // std::thread multi(multi_chat);
        }
        else if(!strncmp(msg ,"3", 1))
        {        
            std::string user_list, test;
            //������Ȳ ����ְ� ģ���� �ο� ���Źޱ�
            for(int i =0; i<userid.size() ; i++)
            {
                user_list += userid[i];
                if(i != userid.size() -1)
                    user_list += ", ";
                test = "3" + user_list;
            }
            // std::cout << test << std::endl;
            // std::cout << strlen(test.c_str()) << std::endl;
            send(hClntSock, test.c_str(), strlen(test.c_str()), 0);//���� ����Ʈ �����ְ�
        }
        else if(!strncmp(msg ,"4", 1))
        {
            std::cout << "4������" << std::endl;
            std::string(msg);
            send(hClntSock, "4", strlen("4"), 0);
            Sleep(3);
            send(hClntSock, "menu", strlen("menu"), 0);
            std::cout << "����" << std::endl;
        }
        else if(!strncmp(msg, "5", 1))
        {
            std::cout << "1:1 ä�ù�" << std::endl;
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