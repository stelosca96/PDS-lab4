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
//    std::queue<T> queue;
    static const int max_queue_size = 10;
    std::vector<T> array;
    std::condition_variable cv;
    std::mutex mutex;
    int index_r=0;
    int index_w = 0;
    bool full = false;
    std::condition_variable cv_queue;
    std::mutex mutex_queue;
    T terminator;
    bool producer_stopped = false;

public:
    explicit Jobs(T terminator);

    void producer_end();

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
//    if(producer_stopped)
//        return terminator;
    while (!full && (index_r == index_w)){
        cv.wait(lock);
    }
//    std::cout << "get: " << queue.size() << std::endl;
    T row = array[index_r];
    full = false;
    index_r = ++index_r % max_queue_size;
    cv_queue.notify_one();
    return row;
}

template<class T>
void Jobs<T>::put(T job){
//    std::lock_guard<std::mutex> l(mutex);
    std::unique_lock<std::mutex> lock(mutex);
    if(full){
//        std::cout << "Locked put: " << queue.size() << std::endl;
        cv_queue.wait(lock);
    }
//    std::cout << "put: " << queue.size() << std::endl;
    array[index_w] = job;
    index_w = ++index_w % max_queue_size;
    full = index_r == index_w;
    cv.notify_one();
}

template<class T>
void Jobs<T>::producer_end() {
    std::unique_lock<std::mutex> lock(mutex);
    producer_stopped = true;
}

template<class T>
Jobs<T>::Jobs(T terminator):terminator(terminator) {
}


#endif //PDS_LAB4_JOBS_H
