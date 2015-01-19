/* 
 * File:   ThreadPool.h
 * Author: marcus
 *
 * Created on July 30, 2014, 11:22 AM
 */

#ifndef THREADPOOL_H
#define	THREADPOOL_H

#include <iostream>
#if (__cplusplus >= 201103L)
#include <thread>
#include <mutex>
#include <condition_variable>
#else
#include <pthread.h>
#endif

#if (__cplusplus >= 201103L)
typedef std::function<void()> func_t;
typedef std::mutex mu_t;
typedef std::condition_variable cond_t;
#else
typedef void (*func_t)(void *);
typedef pthread_mutex_t mu_t;
typedef pthread_cond_t cond_t;
struct pthread_struct {
  void (*func)(void *);
  void *thread_struct;
  int free;
  mu_t lock;
  cond_t free_thread;
  pthread_struct(void func(void *), void* thr_str, int& f, mu_t& m, cond_t& c) 
    : func(func), thread_struct(thr_str), free(f), lock(m), free_thread(c) { };
};
#endif

class threadpool {
 public:
  threadpool(int);
  virtual ~threadpool();
#if (__cplusplus >= 201103L)
  void addThread(func_t);
#else 
  void addThread(func_t, void*);
#endif
  void wait();
 private:
  const int size;
  int free;
  mu_t lock;
  cond_t free_thread;
};
#endif	/* THREADPOOL_H */

