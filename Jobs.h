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

public:

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
    while (queue.size() < 1)
        cv.wait(lock);
    std::cout << "get: " << queue.size() << std::endl;
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
        std::cout << "Locked put: " << queue.size() << std::endl;
        cv_queue.wait(lock);
    }
    std::cout << "put: " << queue.size() << std::endl;
    queue.push(job);
    cv.notify_one();
}

#endif //PDS_LAB4_JOBS_H
