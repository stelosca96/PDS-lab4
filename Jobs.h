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
#include <optional>

template <class T>
class Jobs {
private:
    std::queue<T> queue;
    std::condition_variable cv;
    std::mutex mutex;

    std::condition_variable cv_queue;
    static const int max_queue_size = 10;
    bool producer_stopped = false;

public:

    void producer_end();

    // inserisce un job in coda in attesa di essere processato, può
    // essere bloccante se la coda dei job è piena
    void put(T job);

    // legge un job dalla coda e lo rimuove
    // si blocca se non ci sono valori in coda
    std::optional<T> get();

    bool producer_is_ended();
};

template<class T>
std::optional<T> Jobs<T>::get() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait_for(lock, std::chrono::milliseconds(100),
            [this](){ return !queue.empty() || producer_stopped;});
    if(queue.empty())
        return std::nullopt;
//    std::cout << "queue size: " <<  queue.size() << " " << (!queue.empty() || producer_stopped) << "Type: " << typeid(T).name()  << std::endl;
    std::optional<T> row = queue.front();
    queue.pop();
    cv_queue.notify_one();
    return row;
//    return std::nullopt;
}

template<class T>
void Jobs<T>::put(T job){
    std::unique_lock<std::mutex> lock(mutex);
    cv_queue.wait(lock, [this](){return queue.size() < max_queue_size;});
    queue.push(job);
    cv.notify_one();
}

template<class T>
void Jobs<T>::producer_end() {
    std::lock_guard<std::mutex> lock(mutex);
    producer_stopped = true;
}

template<class T>
bool Jobs<T>::producer_is_ended(){
    std::lock_guard<std::mutex> lock(mutex);
    return producer_stopped;
}

#endif //PDS_LAB4_JOBS_H
