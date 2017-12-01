//模拟内存分配和撤销
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#define PROCESS_NAME_LEN 32 //进程名长度
#define MIN_SLICE 10    //最小碎片的大小
#define DEFAULT_MEM_SIZE 1024   //默认内存大小 
#define DEFAULT_MEM_START 0 //起始位置
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3
int mem_size=DEFAULT_MEM_SIZE;//内存大小
int ma_algorithm=MA_FF; //当前内存分配算法
static int pid=0;   //初始pid
int flag=0;     //设置内存大小 标志
/*描述每一个空闲块的数据结构*/
typedef struct freeBlock{
    int size;   //空闲块大小 
    int start_addr;     //空闲块起始地址
    struct freeBlock *next;
}free_block_type;

/*指向内存中空闲块链表的首指针*/
free_block_type *free_block_head;

/*每个进程分配到的内存块的描述*/
typedef struct allocatedBlock{
    int pid;
    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocatedBlock *next;
}allocated_block;

/*进程分配内存块链表的首指针*/
allocated_block *allocated_block_head=NULL;

/*初始化空闲块，默认为一块，可以指定大小及起始地址*/
free_block_type* init_free_block(int mem_size)
{
    free_block_type *fb;
    fb=(free_block_type*)malloc(sizeof(free_block_type));
    if(fb==NULL) 
    {
        printf("No men\n");
        return NULL;
    }
    fb->size=mem_size;
    fb->start_addr=0;
    fb->next=NULL;
    return fb;
} 

/*显示菜单*/
void display_menu()
{
    printf("\nPlease choose the option:\n");
    printf("1. Set memory size(default=%d)\n",DEFAULT_MEM_SIZE);
    printf("2. Select memoey allocation algorithm\n");
    printf("3. New process\n");
    printf("4. Terminate a process\n");
    printf("5. Display memory usage\n");
    printf("0.  Exit\n");
}

/*设置内存大大小*/
int set_mem_size()
{
    int size;
    if(flag!=0)
    {
        printf("Cannot set memory size again\n");
        return 0;
    }
    printf("New total memory size = ");
    scanf("%d",&size);
    if(size>0)
    {
        mem_size=size;
        free_block_head->size=mem_size;
    }
    flag=1;
    return 1;
}

/*按FF算法重新整理内存空闲块链表*/
/*整理时按照空闲块的起始地址排列*/
void rearrange_FF()
{
    int addr,count=0,judge=0;
    free_block_type *fb=free_block_head,*fb_pre;
    free_block_type *temp,*temp_pre;
    while(fb!=NULL)//检测此时空闲内存块的数量
    {
        count++;
        fb=fb->next;
    }
    fb=free_block_head;
    fb_pre=NULL;
    /*将内存块起始地址按从小到大进行排序*/
    for(int i=1;i<count;i++)
    {
        judge=0;
        free_block_type *cur=fb->next,*cur_pre=fb;
        temp=fb;temp_pre=fb_pre;
        addr=fb->start_addr;
        while(cur!=NULL)
        {
            if((cur->start_addr)<addr)
            {
                judge=1;
                addr=cur->start_addr;
                temp_pre=cur_pre;temp=cur;
                cur_pre=cur;cur=cur->next;
            }
            else
            {
                cur_pre=cur;cur=cur->next;
            }
        }
        if(judge==1)
        {
            if(fb==free_block_head)
            {
                free_block_head=temp;
                temp_pre->next=temp->next;
                temp->next=fb;
            }
            else
            {
                temp_pre->next=temp->next;
                temp->next=fb;
                fb_pre->next=temp;
            }
            fb_pre=temp;
            fb=temp->next;
        }
        else
        {
            fb_pre=fb;
            fb=fb->next;
        }
    }
}

/*按照三种算法进行整理时，代码不同之处只是排序对象不同，因此将排序对象修改即可*/
/*按BF算法重新整理内存空闲块链表*/
/*整理时按照空闲块的大小从小到大排列*/
void rearrange_BF()
{
    int temp_size,count=0,judge=0;
    free_block_type *fb=free_block_head,*fb_pre;
    free_block_type *temp,*temp_pre;
    while(fb!=NULL)//检测此时空闲内存块的数量
    {
        count++;
        fb=fb->next;
    }
    fb=free_block_head;
    fb_pre=NULL;
    /*将内存块容量按从小到大进行排序,类似于选择排序*/
    for(int i=1;i<count;i++)
    {
        judge=0;
        free_block_type *cur=fb->next,*cur_pre=fb;
        temp=fb;temp_pre=fb_pre;
        temp_size=fb->size;
        //找出当前的起始地址最小的空闲块，让其提到fb前面
        while(cur!=NULL)
        {
            if((cur->size)<temp_size)
            {
                judge=1;
                temp_size=cur->size;
                temp_pre=cur_pre;temp=cur;
                cur_pre=cur;cur=cur->next;
            }
            else
            {
                cur_pre=cur;cur=cur->next;
            }
        }
        //如果judge=1,则链表后面有起始地址更小的空闲块
        if(judge==1)
        {
            if(fb==free_block_head)//如果是在链表头指针，则另外处理
            {
                free_block_head=temp;
                temp_pre->next=temp->next;
                temp->next=fb;
            }
            else
            {
                temp_pre->next=temp->next;
                temp->next=fb;
                fb_pre->next=temp;
            }
            fb_pre=temp;
            fb=temp->next;
        }
        else
        {
            fb_pre=fb;
            fb=fb->next;
        }
    }
}

/*按WF算法重新整理内存空闲块链表*/
/*整理时按照空闲块的大小从大到小排列*/
void rearrange_WF()
{
    int temp_size,count=0,judge=0;
    free_block_type *fb=free_block_head,*fb_pre;
    free_block_type *temp,*temp_pre;
    while(fb!=NULL)//检测此时空闲内存块的数量
    {
        count++;
        fb=fb->next;
    }
    fb=free_block_head;
    fb_pre=NULL;
    /*将内存块容量按从大到小进行排序*/
    for(int i=1;i<count;i++)
    {
        judge=0;
        free_block_type *cur=fb->next,*cur_pre=fb;
        temp=fb;temp_pre=fb_pre;
        temp_size=fb->size;
        while(cur!=NULL)
        {
            if((cur->size)>temp_size)
            {
                judge=1;
                temp_size=cur->size;
                temp_pre=cur_pre;temp=cur;
                cur_pre=cur;cur=cur->next;
            }
            else
            {
                cur_pre=cur;cur=cur->next;
            }
        }
        if(judge==1)
        {
            if(fb==free_block_head)
            {
                free_block_head=temp;
                temp_pre->next=temp->next;
                temp->next=fb;
            }
            else
            {
                temp_pre->next=temp->next;
                temp->next=fb;
                fb_pre->next=temp;
            }
            fb_pre=temp;
            fb=temp->next;
        }
        else
        {
            fb_pre=fb;
            fb=fb->next;
        }
    }
}

/*按指定算法整理内存空闲块链表*/
void rearrange(int algorithm)
{
    switch(algorithm)
    {
        case MA_FF:
            rearrange_FF();
            break;
        case MA_BF:
            rearrange_BF();
            break;
        case MA_WF:
            rearrange_WF();
            break;
        default:
            break;
    }
}

/*设置当前的分配算法*/
void set_algorithm()
{
    int algorithm;
    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n\n");
    scanf("%d",&algorithm);
    if(algorithm>=1&&algorithm<=3)
    ma_algorithm=algorithm;
    /*按指定算法重新排列空闲区链表*/
    rearrange(ma_algorithm);
}

/*分配内存模块*/
int allocate_mem(allocated_block* ab)
{
    free_block_type *fbt,*pre;
    int request_size=ab->size;
    fbt=pre=free_block_head;
    while(fbt!=NULL)
    {
        /*判断当前空闲块大小是否够分配*/
        if(request_size<=(fbt->size))
        {
            if(((fbt->size)-request_size)<=5) //判断分配后剩余的内存块大小是否小于10
            {
                //如果小于10，则删去此空空闲内存块
                if(fbt==free_block_head)
                {
                    ab->start_addr=fbt->start_addr;
                    free_block_head=fbt->next;
                    free(fbt);
                }
                else
                {
                    ab->start_addr=fbt->start_addr;
                    pre->next=fbt->next;
                    free(fbt);
                }
            }
            else
            {
                //如果大于10，则修改被分配的内存块的大小和起始地址即可
                ab->start_addr=fbt->start_addr;
                fbt->size=fbt->size-request_size;
                fbt->start_addr=fbt->start_addr+request_size;
            }
            return 1;
        }
        else
        {
            pre=fbt;
            fbt=fbt->next;
        }
    }
    return -1;
}

/*创建新的进程，主要是获取内存的申请数量*/
int new_process()
{
    allocated_block *ab;
    int size;
    int ret;
    ab=(allocated_block*)malloc(sizeof(allocated_block));
    if(!ab) exit(-5);
    ab->next=NULL;
    pid++;
    sprintf(ab->process_name,"PROCESS-%02d",pid);
    ab->pid=pid;
    printf("Memory for %s: ",ab->process_name);
    scanf("%d",&size);
    if(size>0)  ab->size=size;
    ret=allocate_mem(ab);//从空闲区分配内存，ret=1表示分配成功
    /*如果此时allocated_block_head尚未赋值，则赋值*/
    if((ret==1)&&(allocated_block_head==NULL))
    {
        allocated_block_head=ab;
        return 1;
    }
    /*分配成功，将已分配块的描述插入已分配链表*/
    else if(ret==1)
    {
        ab->next=allocated_block_head;
        allocated_block_head=ab;
        return 2;
    }
    /*分配不成功*/
    else if(ret==-1)
    {
        printf("Allocation fail\n");
        free(ab);
        return -1;
    }
    return 3;
}

/*将ab所表示的已分配区归还，并进行可能的合并*/
int free_mem(allocated_block *ab)
{
    int algorithm=ma_algorithm;
    free_block_type *fbt,*pre,*work;
    fbt=(free_block_type*)malloc(sizeof(free_block_type));
    if(!fbt)    return -1;
    else{
        fbt->size=ab->size;
        fbt->start_addr=ab->start_addr;
        fbt->next=NULL;
        pre=free_block_head;
        work=free_block_head->next;
        while(work!=NULL)
        {
            pre=work;
            work=work->next;
        }
        pre->next=fbt;
        rearrange_FF();//按照起始地址大小进行排序，即按FF算法将空闲块进行调整
        pre=free_block_head;
        work=free_block_head->next;
        while(work!=NULL)
        {
            //判断前后两个空闲块是否相邻，若相邻，则合并
            if((pre->size)+(pre->start_addr)==(work->start_addr))
            {
                pre->size=pre->size+work->size;
                pre->next=work->next;
                free(work);//释放work指针指向的内容
                work=pre->next;
            }
            else
            {
                pre=work;
                work=work->next;
            }
        }
        rearrange(algorithm);//按当前算法进行空闲块的整理
    }
}

/*释放ab数据结构节点*/
int dispose(allocated_block *free_ab)
{
    allocated_block *pre,*ab;
    if(free_ab==allocated_block_head)
    {
        allocated_block_head=allocated_block_head->next;
        free(free_ab);
        return 1;
    }
    pre=allocated_block_head;
    ab=allocated_block_head->next;
    while(ab!=free_ab)
    {
        pre=ab;
        ab=ab->next;
    }
    pre->next=ab->next;
    free(ab);
    return 2;
}

/*寻找相应进程内存分配的节点*/
allocated_block *find_process(int p)
{
    allocated_block *temp=allocated_block_head;
    while(temp!=NULL)
    {
        if((temp->pid)==p)
        return temp;
        else
        temp=temp->next;
    }
}

/*删除进程，归还分配的存储空间，并删除描述该进程内存分配的节点*/
void kill_process()
{
    allocated_block *ab;
    int p;
    printf("Kill Process,pid=");
    scanf("%d",&p);
    ab=find_process(p);
    if(ab!=NULL)
    {
        free_mem(ab);//释放ab所表示的分配区
        dispose(ab);//释放ab数据结构节点
    }
}

/*显示当前内存的使用情况，包括空闲区的使用情况和已经分配的情况*/
void display_mem_usage()
{
    free_block_type *fbt=free_block_head;
    allocated_block *ab=allocated_block_head;
    if(fbt==NULL)   return;
    printf("\n-----------------------------------------------------------------\n");
    printf("Memory information:\n");
    /*显示空闲区*/
    printf("Free Memory:\n");
    printf("%20s %20s\n","start_addr","size");
    while(fbt!=NULL)
    {
        printf("%20d %20d\n",fbt->start_addr,fbt->size);
        fbt=fbt->next;
    }
    /*显示已分配区*/
    printf("\nUsed Memory:\n");
    printf("%10s %20s %20s %10s\n","pid","ProcessName","start_addr","size");
    while(ab!=NULL)
    {
        printf("%10d %20s %20d %10d\n",ab->pid,ab->process_name,ab->start_addr,ab->size);
        ab=ab->next;
    }
    printf("-----------------------------------------------------------------\n");
}
/*释放链表*/
void do_exit()
{
    free_block_type *fb=free_block_head->next,*pre=free_block_head;
    allocated_block *ab=allocated_block_head->next,*pre1=allocated_block_head;
    while(fb!=NULL)
    {
        pre->next=fb->next;
        free(fb);
        fb=pre->next;
    }
    free(pre);
    while(ab!=NULL)
    {
        pre1->next=ab->next;
        free(ab);
        ab=pre1->next;
    }
    free(pre1);
}
int main()
{
    char choice;
    pid=0;
    int judge=1;
    free_block_head=init_free_block(mem_size);
    //初始化空闲区
    display_menu();//显示菜单
    while(judge==1)
    {
        choice=getchar();//获取用户输入
        switch(choice)
        {
            case '1':
                set_mem_size();//设置内存大小
                display_menu();//显示菜单
                break;
            case '2':
                set_algorithm();//设置分配内存算法
                display_menu();//显示菜单
                flag=1;
                break;
            case '3':
                new_process();//创建新进程
                display_menu();//显示菜单
                flag=1;
                break;
            case '4':
                kill_process();//删除进程
                display_menu();//显示菜单
                flag=1;
                break;
            case '5':
                display_mem_usage();//显示内存使用情况
                display_menu();//显示菜单
                flag=1;
                break;
            case '0':
                do_exit();//释放链表
                exit(0);//退出
            default:
                break;
        }
    }
}
