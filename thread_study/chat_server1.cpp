// #include <iostream>
// #include <thread>
// #include <vector>
// #include <stdlib.h>
// #include <process.h>
// #include <winsock2.h>
// #include <sys/types.h>


// class ChatServer
// { 
//     private:
//         int serverSock;
//         std::vector<int> clientSocks;
//     public:
//         ChatServer(int port)
//         {
//             serverSock = socket(AF_INET, SOCK_STREAM, 0);
//             sockaddr_in serverAddr;
//             serverAddr.sin_family = AF_INET;
//             serverAddr.sin_port = htons(port);
//             serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//             bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
//             listen(serverSock, 5);
//         }
//         void start()
//         {
//             while(1)
//             {
//                 int clientSock = accept(serverSock, NULL, NULL);
//                 clientSocks.push_back(clientSock);
//                 std::thread([this, clientSock]()
//                 {
//                     char buffer[1024];
//                     while(1)
//                     {
//                         ssize_t received = recv(clientSock, buffer, sizeof(buffer) -1, 0);
//                         if(received <= 0) return;
//                         buffer[received] = '\0';
//                         for(int otherSock : clientSocks)
//                         {
//                             if(otherSock != clientSock)
//                             send(otherSock, buffer, received, 0);
//                         }
//                     }

//                 }).detach();
                
//             }
//         }
// };

// int main()
// {
//     ChatServer server(12345);
//     server.start();
//     return 0;
// }

// #include <iostream>
// #include <thread>
// #include <vector>
// #include <unistd.h>
// #include <sys/types.h>
// #include <winsock2.h>

// class ChatServer {
// private:
//     int serverSock;
//     std::vector<int> clientSocks;
// public:
//     ChatServer(int port) {
//         serverSock = socket(AF_INET, SOCK_STREAM, 0);
//         sockaddr_in serverAddr;
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(port);
//         serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//         bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
//         listen(serverSock, 5);
//     }
//     void start() {
//         while (true) {
//             int clientSock = accept(serverSock, NULL, NULL);
//             clientSocks.push_back(clientSock);
//             std::thread([this, clientSock]() {
//                 char buffer[1024];
//                 while (true) {
//                     ssize_t received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
//                     if (received <= 0) return;
//                     buffer[received] = '\0';
//                     for (int otherSock : clientSocks) {
//                         if (otherSock != clientSock) {
//                             send(otherSock, buffer, received, 0);
//                         }
//                     }
//                 }
//             }).detach();
//         }
//     }
// };  

// int main() {
//     ChatServer server(12345);
//     server.start();
//     return 0;
// }
