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
