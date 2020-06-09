//
// Created by stefano on 08/06/20.
//

#ifndef PDS_LAB4_JOBS_H
#define PDS_LAB4_JOBS_H

#include <iostream>
#include <queue>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <atomic>

template <class T>
class Jobs {
private:
    std::queue<T> queue;
    std::condition_variable cv;
    std::mutex mutex;

    std::condition_variable cv_queue;
    std::mutex mutex_queue;
    const int max_queue_size = 10;
    T terminator;
    std::atomic<bool> producer_stopped = false;

public:
    Jobs(T terminator);

    void producer_end();
    bool continue_check();

    // inserisce un job in coda in attesa di essere processato, può
    // essere bloccante se la coda dei job è piena
    void put(T job);

    // legge un job dalla coda e lo rimuove
    // si blocca se non ci sono valori in coda
    T get();
};

template<class T>
T Jobs<T>::get() {
    std::unique_lock<std::mutex> lock(mutex);
    if(producer_stopped)
        return terminator;
    while (queue.size() < 1){
        cv.wait(lock);
    }
//    std::cout << "get: " << queue.size() << std::endl;
    T row = queue.front();
    queue.pop();
    cv_queue.notify_one();
    return row;
}

template<class T>
void Jobs<T>::put(T job){
//    std::lock_guard<std::mutex> l(mutex);
    std::unique_lock<std::mutex> lock(mutex);
    if(queue.size() >= max_queue_size){
//        std::cout << "Locked put: " << queue.size() << std::endl;
        cv_queue.wait(lock);
    }
//    std::cout << "put: " << queue.size() << std::endl;
    queue.push(job);
    cv.notify_one();
}

template<class T>
void Jobs<T>::producer_end() {
    std::unique_lock<std::mutex> lock(mutex);
    producer_stopped = true;
}

template<class T>
bool Jobs<T>::continue_check() {
    std::unique_lock<std::mutex> lock(mutex);
    return !(producer_stopped && queue.size() == 0);
}

template<class T>
Jobs<T>::Jobs(T terminator):terminator(terminator) {}


#endif //PDS_LAB4_JOBS_H
