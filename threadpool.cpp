/* 
 * File:   ThreadPool.cpp
 * Author: marcus
 * 
 * Created on July 30, 2014, 11:22 AM
 */

#include "threadpool.h"

#if (__cplusplus >= 201103L)
threadpool::threadpool(int size) : size(size) {
  this->free = size;
}

threadpool::~threadpool() {
}

void threadpool::addThread(func_t func) {
  std::unique_lock<mu_t> l(lock);
  free_thread.wait(l, [this](){ return free != 0; });
  free--;
  std::thread tmp ([this](func_t func){
    func();
    free++;
    free_thread.notify_one();
  }, func);
  tmp.detach();
}

void threadpool::wait() {
  std::unique_lock<std::mutex> l(lock);
  free_thread.wait(l, [this](){ return free == size; });
}

#else

threadpool::threadpool(int size) : size(size) {
  this->free = size;
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&free_thread, NULL);
}

threadpool::~threadpool() {
}

void *add_thread_func(void *data_ptr) {
  pthread_struct *data = (pthread_struct *) data_ptr;
  data->func(data->thread_struct);
  pthread_mutex_lock(data->lock);
  (*data->free)++;
  pthread_cond_signal(data->free_thread);
  pthread_mutex_unlock(data->lock);
  pthread_exit(NULL);
}

void threadpool::addThread(func_t func, void *struc) {
  pthread_t thr_id;
  pthread_mutex_lock(&lock);
  while (free == 0)
    pthread_cond_wait(&free_thread, &lock); 
  free--;
  pthread_mutex_unlock(&lock);
  pthread_create(&thr_id, NULL, add_thread_func, 
      (void*) new pthread_struct(func, struc, &free, &lock, &free_thread));
  pthread_detach(thr_id);
}

void threadpool::wait() {
  pthread_mutex_lock(&lock);
  while(free != size)
    pthread_cond_wait(&free_thread, &lock);
  pthread_mutex_unlock(&lock);
}
#endif
