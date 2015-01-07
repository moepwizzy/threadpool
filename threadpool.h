/* 
 * File:   ThreadPool.h
 * Author: marcus
 *
 * Created on July 30, 2014, 11:22 AM
 */

#ifndef THREADPOOL_H
#define	THREADPOOL_H

#include <thread>
#include <mutex>
#include <condition_variable>

class threadpool {
public:
    threadpool(int);
    virtual ~threadpool();
    void addThread(std::function<void()>);
    void wait();
private:
    int size;
    int free;
    std::mutex lock;
    std::condition_variable free_thread;
};
#endif	/* THREADPOOL_H */

