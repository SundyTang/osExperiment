//进程间通信(signal,kill)
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
pid_t pid1,pid2;
int judge=0;
//父进程信号处理程序
void handler1(int ignore)
{
    //向子进程发送信号
    kill(pid1,16);
    kill(pid2,17);
}
//子进程1信号处理程序
void handler2(int ignore)
{
    printf("\nChild process 1 is killed by parent process!!\n");
    exit(0);
}
//子进程2信号处理程序
void handler3(int ignore)
{
    printf("\nChild process 2 is killed by parent process!!\n");
    exit(0);
}
int main()
{
    //创建子进程1
    while((pid1=fork())==-1);
    if(pid1>0)  //判断是否在父进程中
    {
        signal(2,handler1);
        //创建子进程2
        while((pid2=fork())==-1);
        if(pid2>0)
        {
            printf("Parent process(pid is %d) is waiting for the signal!!\n",getpid());
            pid_t end1=wait(0);
            pid_t end2=wait(0);
            printf("Parent process is killed !!\n");
            exit(0);
        }
        /*不能将子进程2的执行语句放在 括号外，否则的话子进程2将不能正常执行*/
        else if(pid2==0)  //子进程2
        {
            //等待父进程发来信号
            signal(17,handler3);
            for(;;);
        }   
    }
    else if(pid1==0)  //子进程1
    {
        //等待父进程发来信号
        signal(16,handler2);
        for(;;);
    }
}
