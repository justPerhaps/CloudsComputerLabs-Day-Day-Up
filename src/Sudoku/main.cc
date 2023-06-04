#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include "sudoku.h"

#define TNUM 10//线程数

char Answer[Task_num][128];//答案数组
char puzzle[Task_num][128];//数独题目数组

int total=0;//数独总数
int S_count=0;//当前的数独序号
bool finish=false;//

int Tdata[Task_num];//线程运行函数的参数
pthread_t th[Task_num];//线程号
pthread_mutex_t mutex;//定义一个锁

int JobID()
{
    int now_JobID = 0;
    pthread_mutex_lock(&mutex);//加锁
    if (S_count>=total)
    {
        pthread_mutex_unlock(&mutex);
        return -1;//所有任务已解决
    }
    now_JobID=S_count++;//当前jobid为数独序号加1
    if (now_JobID==total-1)
    {
        finish=true;
    }
    pthread_mutex_unlock(&mutex);//解锁
    return now_JobID;
}


void *Sudoku_solve(void *args)
{
    bool (*solve)(int,int[],int,int[])=solve_sudoku_basic;//使用basic算法
    int board[N];
    int spaces[N];
    while (!finish)
    {
        int id=JobID();
        if (id==-1)//id为-1表示所有问题已解决，退出解题函数
            break;
        int t=input(puzzle[id],board,spaces);
        if (solve(0,board,t,spaces))//调用solve函数
        {
            if (!solved(board))
                assert(0);
            for (int i=0;i<N;i++)//保存结果
                Answer[id][i] = char('0' + board[i]);
        }
        else
        {
            printf("No: %d,无解\n", id);
        }
    }
}


void Create_Pthread()//创建线程
{
    for (int i = 0; i < TNUM; i++)
    {
        if (pthread_create(&th[i], NULL, Sudoku_solve,(void*)&Tdata[i])!= 0)
        {
            perror("pthread_create failed");
            exit(1);
        }
    }
}

void End_Pthread()//结束线程
{
    for (int i = 0; i < TNUM; i++)
        pthread_join(th[i], NULL);
}

void Print()
{
    for(int i=0;i<total; i++)
    {
        printf("%s\n",Answer[i]);
    }
}

int64_t now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL); // 获取当前精确时间
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char *argv[])
{
    init_neighbors();
    int64_t start = now();//开始计时

    char file_path[128];
    char *_= fgets(file_path, sizeof file_path, stdin);

    if (file_path[strlen(file_path) - 1] == '\n')
        file_path[strlen(file_path) - 1] = '\0';//将文件中的换行符转换

    FILE *fp = fopen(file_path, "r");// 打开文件

    printf("成功打开文件！");
    Create_Pthread();//创建线程（可以自行设置线程数量）
    while (fgets(puzzle[total], sizeof puzzle, fp) != NULL)
    {
        if (strlen(puzzle[total]) >= N)
        {
            pthread_mutex_lock(&mutex);
            total++;
            pthread_mutex_unlock(&mutex);
        }
    }
    End_Pthread();//判断是否结束线程
    printf("开始输出答案。。。");
    Print();//输出答案
    int64_t end = now();
    double sec = (end - start) / 1000000.0;
    //printf("%f sec %f ms each %d\n", sec, 1000 * sec / total, total); // 输出运行时间
    return 0;
}
