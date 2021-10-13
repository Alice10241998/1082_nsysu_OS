#include <iostream>
#include <time.h>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
pthread_t thr[N];
class my_monitor{
    private:
        int state[N];
        //sem_t mutex;
        //sem_t s[N];
        pthread_cond_t sta[N];
	pthread_mutex_t mutex;
	pthread_attr_t attr;
    public:
        //my_monitor();
        void show_all();
        void test(int i);
        void init();
        void get_forks(int i);
        void put_forks(int i);
};
my_monitor mon;
void my_monitor::show_all(){
    int t;
    printf("\t\t");
    for(t = 0;t < N;t++){
        if(state[t] == 0)
            printf("   T");
        else if(state[t] == 1)
            printf("   H");
        else if(state[t] == 2)
            printf("   E");
    }
    printf("\n");
}
void my_monitor::init()
{
        pthread_mutex_init(&mutex,NULL);	//mutex初始化
        for(int i=0 ; i< 5 ;i++)
        	pthread_cond_init(&sta[i],NULL);//variable condition 初始化	

}
void my_monitor::test(int i){                 //等於兩個迴圈,當條件不符時,就會進入pthread_cond_wait繼續等待
    while(!(state[i] == 1 && state[LEFT] != 2 && state[RIGHT] != 2)){      
       pthread_cond_wait(&sta[i],&mutex);     //&mutex當左邊及右邊同時叫醒時,就依序左再右,可防止同時運時造成delock      
       //pthread_cond_signal(&sta[i]);
       //sem_post(&s[i]);
    }
     state[i] = 2;
     printf("philosopher %d is eating now        ",i+1);
        show_all();

}
void my_monitor::get_forks(int i){
        pthread_mutex_lock(&mutex);
        //sem_wait(&mutex);
        state[i] = 1;
        printf("philosopher %d is tring to get forks",i+1);
        show_all();
        test(i);
        pthread_mutex_unlock(&mutex);
        //rand();
        //sem_post(&mutex);
        //sem_wait(&s[i]);
        sleep(1);
}
void my_monitor::put_forks(int i){
        pthread_mutex_lock(&mutex);
        //sem_wait(&mutex);
        //if(state[i]==2)
        //{
        	state[i] = 0;
        //}
        printf("philosopher %d is putting forks     ",i+1);
        show_all();
       // test(LEFT);
       // test(RIGHT);
        pthread_cond_signal(&sta[LEFT]);//叫左邊叫醒等待吃的人
        pthread_cond_signal(&sta[RIGHT]);//叫右邊叫醒等待吃的人
        pthread_mutex_unlock(&mutex);
        //rand();
        //sem_post(&mutex);
        sleep(1);
}
/*my_monitor::my_monitor(){
    sem_init(&mutex, 0, 1);
}*/
void* philosopher(void* num){
    int i = *(int*)num;
    while(1){
        //sleep(1);
        rand();
        mon.get_forks(i);
        //sleep(3);
        rand();
        mon.put_forks(i);
    }
    pthread_exit(0);
    //return num;
}
int main(){
    int t,p[N] = {0,1,2,3,4};
    printf("--- 下列狀態表示：T = thinking , H = hungry , E = eating ---\n");
    mon.init();
    for(t = 0;t < N;t++){

        pthread_create(&thr[t], NULL, philosopher, (void*)&p[t]);
    }
    for(t = 0;t < N;t++){
        pthread_join(thr[t],NULL);
    }
    return 0;
}
