#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
class Stack
{
  int index;
  int size_;
  int*buffer;
public:
  Stack(int size):
     size_(size),
     buffer(new int [size_]),
     index(-1){};

  int set(int elem)
  {
    if(index!=size_)
      {
            buffer[++index]=elem;



      }
      else
      {
        cout<<"Full"<<endl;
      }
      return 0;
  }
  int get()
  {
    if(index!=-1)
    {
      --index;
      return buffer[index+1];
    }
    else
    cout<<"epthy"<<endl;
    return 0;
  }
  int get_index()const
  {
    return index;
  }
  int get_size()const
  {
    return size_;
  }
  ~Stack()
  {
    delete[] buffer;
  }
};

Stack S(10);//global stack
//srand(time(NULL));
pthread_mutex_t mutex;//global mutex for critic zone
pthread_cond_t cond_p;//condition for Producer
pthread_cond_t cond_c;//condition for Consumer

void*Producer(void*)
{
  int P_element;//elem which must set in the stack
  while(true)
  {
    P_element=rand()%4;//make digit
    pthread_mutex_lock(&mutex);//lock critic zone

    if(S.get_index()==S.get_size())//if stack full must wait
        {
          cout<<"Producer wait"<<endl;
          pthread_cond_wait(&cond_p,&mutex);
        }
      cout<<"Producer set -"<<P_element<<endl;
      cin.get();
    S.set(P_element);//set element
    pthread_cond_signal(&cond_c);//send signal
    pthread_mutex_unlock(&mutex);//unlock


  }
  pthread_exit(0);
}


void*Consumer(void*)
{

  while(true)
  {
    pthread_mutex_lock(&mutex);//lock critic zone

    if(S.get_index()==-1)//if stack empthy Consumer must wait
      {
        cout<<"Consumer wait"<<endl;
      pthread_cond_wait(&cond_c,&mutex);
      }
    cout<<"Consumer get-"<<S.get()<<endl;
    cin.get();
      pthread_cond_signal(&cond_p);//send signal
      pthread_mutex_unlock(&mutex);//deactive mutex
  }
  pthread_exit(0);
}


int main()
{
  pthread_mutex_init(&mutex,NULL);//init mutex
  pthread_cond_init(&cond_p,NULL);//init condition for Producer
  pthread_cond_init(&cond_c,NULL);//init condition for Consumer

  pthread_t producer;// descriptor for thread Producer
  pthread_t consumer;//descriptor for thread Consumer
  pthread_create(&producer,NULL,&Producer,NULL);//creat thread
  pthread_create(&consumer,NULL,&Consumer,NULL);//creat thread
  pthread_join(producer,NULL);//main thread must wait to the other thread
  pthread_join(consumer,NULL);//as the top
  pthread_mutex_destroy(&mutex);//destroy mutex
  pthread_cond_destroy(&cond_p);//destroy condition Producer
  pthread_cond_destroy(&cond_c);//destroy condition for Consumer
  return 0;
}
