// #include <iostream>
// #include <windows.h>
// #include <process.h> /*_beginthreadex, _endthredex*/
// using namespace std;

// unsigned WINAPI ThreadFunc(void *arg);

// int main(int argc, char * argv[])
// {
//     HANDLE hThread;
//     unsigned threadID;
//     int param = 5;

//     hThread = (HANDLE)_beginthreadex(NULL,0,ThreadFunc,(void*)&param,0,&threadID);
//     //thread 메인함수 threadfunc에 변수 param의 주소값 전달, 쓰레드 생성요구
//     if(hThread == 0)
//     {
//         cout << "_begintthreadex() error " << std::endl;
//         return -1;
//     }
//     Sleep(10000);
//     puts("end of main");
//     return 0;
// }
// //매개변수의 전달방향, 할당된 스택의 반환법 등 포함한 함수의 호출규약
// //beginthreadex 함수가 요구하는 호출규약을 지키기 위해 삽입되어있음
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

// #include <stdio.h>
// #include <windows.h>
// #include <process.h>
// unsigned WINAPI ThreadFunc(void *arg);

// int main(int argc, char *argv[])
// {
//     HANDLE hThread;
//     DWORD wr;
//     unsigned threadID;
//     int param = 5;

//     hThread = (HANDLE)_beginthreadex(NULL,0,ThreadFunc,(void*)&param, 0, &threadID);
//     if (hThread == 0)
//     {
//         puts("_beginthreadex() error");
//         return -1;
//     }
//     if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED)
//     {
//         puts("thread wait error");
//         return -1;
//     }

//     printf("wait result: %s\n", (wr==WAIT_OBJECT_0) ? "signaled":"time-out");
//     puts("end of main");
//     return 0;    
    
// }

// unsigned WINAPI ThreadFunc(void *arg)
// {
//     int i;
//     int cnt = *((int*)arg);
//     for(i = 0; i <cnt ; i++)
//     {
//         Sleep(1000); puts("running thread");
//     }
//     return 0;
// }

// #include <stdio.h>
// #include <windows.h>
// #include <process.h>

// #define NUM_THREAD 50
// unsigned WINAPI threadInc(void *arg);
// unsigned WINAPI threadDes(void *arg);
// long long num = 0;
//동기화 전 값이 계속 바뀐다
// int main(int argc, char *argv[])
// {
//     HANDLE tHandles[NUM_THREAD];
//     int i;

//     printf("sizeof long long: %d \n", sizeof(long long));
//     for(i = 0; i <NUM_THREAD ; i++)
//     {
//         if(i%2)
//             tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
//         else
//             tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
//     }
//     WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
//     printf("result: %lld \n", num);
//     return 0;
// }

// unsigned WINAPI threadInc(void *arg)
// {
//     int i;
//     for(i = 0; i<5000000; i++)
//         num +=1;
//     return 0;
// }

// unsigned WINAPI threadDes(void *arg)
// {
//     int i;
//     for(i=0; i<5000000 ; i++)
//         num -=1;
//     return 0;
// }

//동기화
// #include <stdio.h>
// #include <windows.h>
// #include <process.h>
// #define NUM_THREAD 50
// unsigned WINAPI threadInc(void *arg);
// unsigned WINAPI threadDes(void *arg);
// long long num = 0;
// CRITICAL_SECTION cs;

// int main(int argc, char *argv[])
// {
//     HANDLE tHandles[NUM_THREAD];
//     int i;
//     InitializeCriticalSection(&cs);

//     for(i = 0; i <NUM_THREAD ; i++)
//     {
//         if(i%2)
//             tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
//         else
//             tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
//     }
//     WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
//     printf("result: %lld \n", num);
//     return 0;
// }

// unsigned WINAPI threadInc(void *arg)
// {
//     int i;
//     EnterCriticalSection(&cs);
//     for(i = 0; i<50000000; i++)
//         num +=1;
//     LeaveCriticalSection(&cs);
//     return 0;
// }

// unsigned WINAPI threadDes(void *arg)
// {
//     int i;
//     EnterCriticalSection(&cs);
//     for(i=0; i<50000000; i++)
//         num -=1;
//     LeaveCriticalSection(&cs);
//     return 0;
// }
