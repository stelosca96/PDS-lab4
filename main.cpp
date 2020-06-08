#include <iostream>
#include <vector>
#include <thread>
#include "Jobs.h"
#include "Row.h"
#include <regex>
#include <filesystem>
#include <fstream>

Jobs<Row> linejobs;
Jobs<std::string> fileJobs;

void producer(const std::string& directory){
    std::string line;
    for(auto& f: std::filesystem::directory_iterator(directory)){
        std::cout << f.path() << " " << f.is_regular_file() << std::endl;
        if(f.is_regular_file()){
            fileJobs.put(f.path());
        }
    }
    std::cout << "Exit thread p" << std::endl;

    fileJobs.producer_end();
}

void consumer1(){
    std::string line;
    std::string file_path;
    while (fileJobs.continue_check()) {
        file_path = fileJobs.get();
        std::ifstream file(file_path);
        std::cout << "Opening file: " << file_path << std::endl;
        if (file.is_open()) {
            int line_count = 0;
            while (std::getline(file, line))
                linejobs.put(Row(file_path, line_count++, line));
            file.close();
        }
    }
    std::cout << "Exit thread c1" << std::endl;
    linejobs.producer_end();
}

void consumer2(const std::string& re){
    std::regex regex(re);
    Row row("", 0, "");
    while (linejobs.continue_check()){
        row = linejobs.get();
//        std::cout << row.getFileName() << " " << row.getLineNumber() << std::endl;
        if(std::regex_match(row.getLineContent(), regex))
            std::cout << "File: " << row.getFileName() << " Line: " << row.getLineNumber() << " Match: " << row.getLineContent() << std::endl;
    }

    // devo aggiungere altri elementi sentinella alla lista perchè un solo valore chiuderebbe un solo thread
    linejobs.put(Row("", -1, ""));
    std::cout << "Exit thread c2" << std::endl;
   }



int main() {
    const std::string directory = "/home/stefano/CLionProjects/pds_lab4/ex_files/";
    const std::string re = "(.*)(work)(.*)";

    const int threadNumber = 3;
    std::vector<std::thread> threads;

    // lancio i thread dei consumers1
    for(int i=0; i<threadNumber; i++)
        threads.emplace_back(std::thread(consumer1));

    // lancio i thread dei consumers2
    for(int i=0; i<threadNumber; i++)
        threads.emplace_back(std::thread(consumer2, std::ref(re)));

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

