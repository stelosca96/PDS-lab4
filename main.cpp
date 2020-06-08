#include <iostream>
#include <vector>
#include <thread>
#include "Jobs.h"
#include "Row.h"
#include <regex>
#include <filesystem>
#include <fstream>

Jobs<Row> jobs;

void producer(std::string directory){
    std::string line;
    for(auto& f: std::filesystem::directory_iterator(directory)){
        std::cout << f.path() << " " << f.is_regular_file() << std::endl;
        if(f.is_regular_file()){
            std::ifstream file(f.path());
            if(file.is_open()){
                int line_count = 0;
                while (std::getline(file, line))
                    jobs.put(Row(f.path().filename(), line_count++, line));
                file.close();
            }
        }
    }
}

[[noreturn]] void consumer(std::string re){
    std::regex regex(re);
    Row row("", 0, "");
    while (true){
        row = jobs.get();
//        std::cout << row.getFileName() << " " << row.getLineNumber() << std::endl;
        if(std::regex_match(row.getLineContent(), regex))
            std::cout << "File: " << row.getFileName() << " Line: " << row.getLineNumber() << " Match: " << row.getLineContent() << std::endl;
    }
   }



int main() {
    const std::string directory = "/home/stefano/CLionProjects/pds_lab4/ex_files/";
    const std::string re = "(.*)(work)(.*)";

    const int threadNumber = 3;
    std::vector<std::thread> threads;

    // lancio i thread dei consumers
    for(int i=0; i<threadNumber; i++)
        threads.emplace_back(std::thread(consumer, std::ref(re)));

    producer(directory);

//    std::regex b(re); // Geeks followed by any character
//    std::string text = "I love my work.";
//    Row row("pippo", 0, "I love my work .");
//    std::cout << row.getLineContent() << std::endl;
//    if(std::regex_match(text, b)){
//        std::cout << "regex match" << std::endl;
//        std::cout << "File name: " << row.getFileName() << " Line: " << row.getLineNumber() << " Match " << row.getLineContent();
//    }


    for(auto& t: threads)
        t.join();

    return 0;
}

