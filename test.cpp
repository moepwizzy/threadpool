/* 
 * File:   main.cpp
 * Author: marcus
 *
 * Created on July 30, 2014, 11:22 AM
 */

#include <iostream>
#include <time.h>
#if (__cplusplus >= 201103L)
#include <mutex>
#else
#include <pthread.h>
#endif
#include <unistd.h>
#include "threadpool.h"

void waste_time() {
  for(int i = 0; i < 1000; i++) {
    ++i;
    --i;
  }
}
#if (__cplusplus >= 201103L)
int main(int argc, char** argv) {
  clock_t c;
  time_t t1,t2;
  int number_of_available_treads = 2;
  std::mutex locked;
  threadpool pool (number_of_available_treads);
  for (int number_of_created_treads = 1; number_of_created_treads <= 10;
      number_of_created_treads++) {
    int sum = 0;
    time(&t1);
    c = clock();
    int loop_end = 1000000/number_of_created_treads;
    for (int i = 0; i < number_of_created_treads; i++)
      pool.addThread([&sum,loop_end,&locked](){ 
        for (int j = 0; j < loop_end ; j++) {
          waste_time();
          std::unique_lock<std::mutex> l(locked);
          sum += 1;
        }
      });
    pool.wait();
    time(&t2);
    std::cout<<"threads: "<<number_of_created_treads
        <<"\tclock: "<<clock()-c
        <<"\ttime: "<<t2-t1
        <<"\tdiff: "<<sum-loop_end*number_of_created_treads<<std::endl;
  }
  return 0;
}
#else
struct thread_struct {
  int loop_end;
  int *sum;
  pthread_mutex_t *m;
  thread_struct(int loop_end, int *sum, pthread_mutex_t *m)
    : loop_end(loop_end), sum(sum), m(m) {};
};

void thread_function(void* thread_struct_ptr) {
  thread_struct *data = (thread_struct *) thread_struct_ptr;
  for (int i = 0; i < data->loop_end; ++i) {
//    std::cout<<"moep"<<std::endl;
    waste_time();
    pthread_mutex_lock(data->m);
    (*data->sum)++;
    pthread_mutex_unlock(data->m);
  }
}

int main(int argc, char** argv) {
  clock_t c;
  time_t t1,t2;
  int number_of_available_treads = 2;
  pthread_mutex_t locked;
  pthread_mutex_init(&locked, NULL);
  threadpool pool (number_of_available_treads);
  for (int number_of_created_treads = 1; number_of_created_treads <= 10;
      number_of_created_treads++) {
    int sum = 0;
    time(&t1);
    c = clock();
    int loop_end = 1000000/number_of_created_treads;
    for (int i = 0; i < number_of_created_treads; i++)
      pool.addThread(thread_function, 
          (void *) new thread_struct(loop_end, &sum, &locked));
    pool.wait();
    time(&t2);
    std::cout<<"threads: "<<number_of_created_treads
        <<"\tclock: "<<clock()-c
        <<"\ttime: "<<t2-t1
        <<"\tdiff: "<<sum-loop_end*number_of_created_treads<<std::endl;
  }
  pthread_mutex_destroy(&locked);
  return 0;
}
#endif
