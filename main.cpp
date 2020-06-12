#include <iostream>
#include <vector>
#include <thread>
#include "Jobs.h"
#include "Row.h"
#include <regex>
#include <filesystem>
#include <fstream>
#include <random>

Jobs<Row> lineJobs;
Jobs<std::string> fileJobs;
std::atomic<int> c1 (0);
std::atomic<int> c2 (0);
std::mutex write_mutex;


void random_sleep(){
    std::mt19937_64 eng{std::random_device{}()};  // or seed however you want
    std::uniform_int_distribution<> dist{0, 30};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
}


void producer(const std::string& directory){
    std::string line;
    for(auto& f: std::filesystem::directory_iterator(directory)){
        std::unique_lock ul(write_mutex);
        std::cout << f.path() << " " << f.is_regular_file() << std::endl;
        ul.unlock();
        if(f.is_regular_file()){
            fileJobs.put(f.path());
            random_sleep();
        }
    }
    std::unique_lock ul(write_mutex);
    std::cout << "Exit thread p" << std::endl;
    ul.unlock();
}

void consumer1(){
    std::string line;
    std::optional<std::string> file_path;
    while (true) {
        file_path = fileJobs.get();
        random_sleep();
        if(file_path.has_value()){
            std::ifstream file(file_path.value());
            std::unique_lock ul(write_mutex);
            std::cout << "Opening file: " << file_path.value() << std::endl;
            ul.unlock();
            if (file.is_open()) {
                int line_count = 0;
                while (std::getline(file, line)) {
                    lineJobs.put(Row(file_path.value(), line_count++, line));
                    c1++;
                    random_sleep();
                }
                file.close();
            }
        } else {
            random_sleep();
            if(fileJobs.producer_is_ended()) {
                std::unique_lock ul(write_mutex);
                std::cout << "Exit thread c1" << std::endl;
                ul.unlock();
                return;
            }
        }

    }
}

void consumer2(const std::string& re){
    std::regex regex(re);
    random_sleep();
    std::optional<Row> row;
    while (true){
        row = lineJobs.get();
        if(row.has_value()) {
            c2++;
            random_sleep();
//        if(std::regex_match(row.value().getLineContent(), regex)){
//            std::unique_lock ul(write_mutex);
//            std::cout << "File: " << row.value().getFileName() << " Line: " << row.value().getLineNumber() << " Match: " << row.value().getLineContent() << std::endl;
//            ul.unlock();
//        }
        } else{
            random_sleep();
            if(lineJobs.producer_is_ended()){
                std::unique_lock ul(write_mutex);
                std::cout << "Exit thread c2" << std::endl;
                ul.unlock();
                return;
            }
        }
    }
}



int main() {
    const std::string directory = "/home/stefano/CLionProjects/pds_lab4/ex_files/";
    const std::string re = "(.*)(work)(.*)";

    const int threadNumber = 3;
    std::vector<std::thread> threads_c1;
    std::vector<std::thread> threads_c2;

    // lancio i thread dei consumers1
    threads_c1.reserve(threadNumber);
    for(int i=0; i<threadNumber; i++)
        threads_c1.emplace_back(std::thread(consumer1));

    // lancio i thread dei consumers2
    threads_c2.reserve(threadNumber);
    for(int i=0; i<threadNumber; i++)
        threads_c2.emplace_back(std::thread(consumer2, std::ref(re)));

    producer(directory);
    fileJobs.producer_end();

    for(auto& t: threads_c1)
        t.join();
    lineJobs.producer_end();

    for(auto& t: threads_c2)
        t.join();

    std::cout << "C1 value: " << c1 << std::endl;
    std::cout << "C2 value: " << c2 << std::endl;
    return 0;
}

