// #include <iostream>
// #include <windows.h>
// #include <process.h>
// using namespace std;
// unsigned WINAPI ThreadFunc(void *arg);

// int main(int argc, char * argv[])
// {
//     HANDLE hThread;
//     unsigned threadID;
//     int param = 5;

//     hThread = (HANDLE)_beginthreadex(NULL,0,ThreadFunc,(void*)&param,0,&threadID);
//     if(hThread == 0)
//     {
//         cout << "_begintthreadex() error " << std::endl;
//         return -1;
//     }
//     Sleep(10000);
//     puts("end of main");
//     return 0;
// }
// unsigned WINAPI ThreadFunc(void *arg)
// {
//     int i;
//     int cnt = *((int *)arg);
//     for (i = 0; i < cnt; i++)
//     {
//         Sleep(1000); cout << "running thread" << endl;
//     }
//     return 0;
// }
// #include <iostream>

// int main()
// {
//     int num1 = 0;
//     int num2 = 0;
//     std::cout << "숫자입력:";
//     std::cin >> num1;
//     std::cout << "숫자입력:";
//     std::cin >> num2;
//     int result = num1 + num2;
//     std::cout << result;
//     int sd;
//     return 0;
// }
// #include <iostream>

// int main()
// {
//     int num1 = 0;
//     int num2 = 0;
//     int num3 = 0;
//     std::cout << "숫자입력:";
//     std::cin >> num1;
//     std::cout << "숫자입력:";
//     std::cin >> num2;
//     std::cout << "숫자입력:";
//     std::cin >> num3;
//     int result = num1 + num2;
//     std::cout << result;
//     return 0;
// }

// #define _WINSOCK_DEPRECATED_NO_WARNINGS
// #define _CRT_SECURE_NO_WARNINGS

// #include <stdio.h>
// #include <winsock2.h>
// #include <process.h>
// #include <string.h>

// int server_init();
// int server_close();
// unsigned int WINAPI do_chat_service(void* param);
// unsigned int WINAPI recv_and_forward(void* param);
// int add_client(int index);
// int read_client(int index);
// void remove_client(int index);
// int notify_client(char *message);
// char* get_client_ip(int index);

// typedef struct sock_info
// {
//     SOCKET s;
//     HANDLE ev;
//     char nick[50];
//     char ipaddr[50];
// }SOCK_INFO;

// int port_number = 9195;
// const int client_count = 10;
// SOCK_INFO sock_array[client_count +1];
// int total_socket_count = 0;

// int main(int argc, char* argv[])
// {
//     unsigned int tid;
//     char message[MAXBYTE] = "";
//     HANDLE mainthread;

//     printf("\n사용법: mcodes_server [포트번호]\n");
//     printf("\t\tex) mcoeds_server.exe 9195\n");
//     printf("\t\tex)mcodes_server.exe\n\n");

//     if(argv[1] != NULL)
//         port_number = atoi(argv[1]);
    
//     mainthread = (HANDLE)_beginthreadex(NULL, 0, do_chat_service, (void*)0, 0, &tid);
//     if(mainthread)
//     {
//         while(1)
//         {
//             gets_s(message, MAXBYTE);
//             if(strcmp(message, "/x") == 0) //사이즈 0이면 멈추기
//                 break;
//                 notify_client(message);
//         }
//         server_close();
//         WSACleanup();
//         CloseHandle(mainthread);
//     }
//     return 0;
// }

// int server_init()
// {
//     WSADATA wsadata;
//     SOCKET s;
//     SOCKADDR_IN server_address;

//     memset(&sock_array, 0, sizeof(sock_array));
//     total_socket_count = 0;
//     if(WSAStartup(MAKEWORD(2, 2), &wsadata) !=0)
//     {
//         puts("WSAStartup 에러");
//         return -1;
//     }
//     if((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
//     {
//         puts("socket 에러");
//         return -1;
//     }
//     memset(&server_address, 0, sizeof(server_address));
//     server_address.sin_family = AF_INET;
//     server_address.sin_addr.s_addr = htonl(INADDR_ANY);
//     server_address.sin_port = htons(port_number);

//     if(bind(s, (struct sockaddr *)&server_address, sizeof(server_address)))
// }