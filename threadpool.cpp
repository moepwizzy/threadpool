/* 
 * File:   ThreadPool.cpp
 * Author: marcus
 * 
 * Created on July 30, 2014, 11:22 AM
 */

#include "threadpool.h"

threadpool::threadpool(int size) {
    this->size = size;
    this->free = size;
}

threadpool::~threadpool() {
}

void threadpool::addThread(std::function<void()> func) {
    std::unique_lock<std::mutex> l(lock);
    free_thread.wait(l, [this](){ return free != 0; });
    free--;
    std::thread tmp ([this](std::function<void()> func){
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
