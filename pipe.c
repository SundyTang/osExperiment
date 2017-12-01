//进程间管道通信
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<stdio.h>
#include<sys/wait.h>
//fd[0]为读端，fd[1]为写端
int pid1,pid2;
int main()
{
    int fd[2];
    char OutPipe[100],InPipe[100];
    pipe(fd);  //创建管道
    while((pid1=fork())==-1);   //创建进程1
    if(pid1==0)
    {
        printf("fd[1]: %d\n",fd[1]);
        lockf(fd[1],1,0);   //锁定管道，禁止其他进程操作，写时不能读
        sprintf(OutPipe,"\nChild process 1 is sending message!\n");
        write(fd[1],OutPipe,50);    //向管道内写数据
        lockf(fd[1],0,0);   //解除管道的锁定
        exit(0);
    }
    else
    {
        while((pid2=fork())==-1);   //创建进程2
        if(pid2==0)
        {
            lockf(fd[1],1,0);   //锁定管道，禁止其他进程操作写时不能读
            sprintf(OutPipe,"\nChild process 2 is sending message!\n");
            write(fd[1],OutPipe,50);    //向管道内写数据
            lockf(fd[1],0,0);   //解除管道的锁定
            exit(0);
        }
        else{
            wait(0);
            read(fd[0],InPipe,50);  //从管道内读取进程1发送的数据
            printf("%s\n",InPipe);
            wait(0);
            read(fd[0],InPipe,50);  //从管道内读取进程2发送的数据
            printf("%s\n",InPipe);
            exit(0);
        }
    }
}
