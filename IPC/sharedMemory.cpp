#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;


class Buffer
{
 int buffer_;

public:
  Buffer():flag(false){};
  bool flag;
  int get_value()const
  {
    return buffer_;
  }
  int set_value(int element)
  {
    buffer_=element;
  }
};


int main()
{
  pthread_cond_t* parrent_cond;
  pthread_cond_t* child_cond;
  pthread_mutex_t* mutex;
  int shm_id_cond_p;
  int shm_id_cond_c;
  int shm_id_mutex_;

pthread_cond_init(parrent_cond,0);
pthread_cond_init(child_cond,0);
pthread_mutex_init(mutex,0);



  //Condition for Parrent
  if((shm_id_cond_p=shmget(IPC_PRIVATE,sizeof(pthread_cond_t),IPC_CREAT))==-1)
  {
    cerr<<"Error shared cond_p";
    return -1;
  }
  parrent_cond=(pthread_cond_t*)shmat(shm_id_cond_p,0,0);
  //Condition for Child
  if((shm_id_cond_c=shmget(IPC_PRIVATE,sizeof(pthread_cond_t),IPC_CREAT))==-1)
  {
    cerr<<"Error shared cond_c";
    return -2;
  }
  if((shm_id_mutex_=shmget(IPC_PRIVATE,sizeof(pthread_mutex_t),IPC_CREAT))==-1)
  {
    cerr<<"Error shared mutex";
    return -3;
  }

  int shm_id;
  key_t key_=401;
  pid_t pid;
  if((shm_id=shmget(key_,sizeof(Buffer),IPC_CREAT|S_IRUSR))==-1)
  {
    cerr<<"Error in processs creating shared memory"<<endl;
    return -5;
  }

  if((pid=fork())==-1)
  {
    cerr<<"Error in process creating child process";
    return -6;
  }

if(pid==0)
{
  Buffer*Shared_Buffer=(Buffer*)shmat(shm_id,0,SHM_RDONLY);
  while(true)
  {
    pthread_mutex_lock(mutex);
  if(Shared_Buffer->flag==false)
   {
    cout<<"child wait"<<endl;
    pthread_cond_wait(child_cond,mutex);
   }

  cout<<"child get-"<<Shared_Buffer->get_value();
  Shared_Buffer->flag=false;
  pthread_cond_signal(parrent_cond);
  pthread_mutex_unlock(mutex);
  cin.get();
  }
}

if(pid>0)
{
  int element;
  Buffer*Shared_Buffer=(Buffer*)shmat(shm_id,0,0);
  while(true)
  {
    pthread_mutex_lock(mutex);
    if(Shared_Buffer->flag!=true)
    {
      cout<<"Parrent wait"<<endl;
      pthread_cond_wait(parrent_cond,mutex);
    }

    element=rand()%5;
    cout<<"parrent sent"<<element<<endl;
    Shared_Buffer->set_value(element);
    Shared_Buffer->flag=true;
    pthread_cond_signal(child_cond);
    pthread_mutex_unlock(mutex);
    cin.get();
  }
}

pthread_cond_destroy(child_cond);
pthread_cond_destroy(parrent_cond);
pthread_mutex_destroy(mutex);

  return 0;
}
