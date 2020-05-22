//
// Created by zidce on 13.03.2020.
//
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <string.h>
#include "Arbitrage.h"
#include <sys/stat.h>
#include <cmath>

using namespace std;

void thread_start(const vector<string> &paths, const string & output_path){
    printf("Starting thread\n");
    for(const auto & path: paths){
        printf("%s\n", path.c_str());
        FilesManager fm = FilesManager(path);
        vector<vector<Triplet> > triplets = fm.selectFiles();
        for(const auto & vec: triplets){
            for(const Triplet & triplet: vec){
                const string & tmp = triplet.getOutputFilename();
                cout << triplet.getOutputFilename() << endl;
                cout << triplet.getFile1() << " " << triplet.getFile2() << " " << triplet.getFile3() << " " << endl;
                Arbitrage arbitrage = Arbitrage();
                if(arbitrage.initialize(triplet, output_path))
                    arbitrage.run();
            }
        }
    }
    printf("Ending thread\n");
}

vector<string> get_subdirectories(const string &path){
    vector<string> subdirectories;
    glob_t glob_result;
    string new_path = path;
    if(path.substr(path.size()-1, 1) != "/"){
        new_path += "/";
    }
    glob((new_path + "*").c_str(),GLOB_TILDE, nullptr, &glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        struct stat s;
        if(stat(glob_result.gl_pathv[i], &s) == 0){
            if(s.st_mode & S_IFDIR){
                string tmp = glob_result.gl_pathv[i];
                tmp += '/';
                subdirectories.emplace_back(tmp);
            }
        }
    }
    return subdirectories;
}

int main(int argc, char *argv[]) {
    int number_of_threads = 1; // min number of threads
    bool recursive_flag = false;
    vector<string> paths;
    string path, output_path;
    // checking passed arguments
    if(argc > 1){
        path = argv[1];
        printf("Path: %s\n", argv[1]);
    } else {
        printf("Please specify the input directory path\n");
        return 0;
    }
    if(argc > 2){
        output_path = argv[2];
        printf("Output path: %s\n", argv[2]);
        if(output_path.substr(output_path.size()-1, 1) != "/"){
            output_path += "/";
        }
    } else {
        printf("Please specify the output directory path\n");
        return 0;
    }
    if(argc > 3 && strcmp(argv[3], "-r") == 0){
        recursive_flag = true;
        printf("Recursive directories (depth 1): %s\n", argv[1]);
        if(argc > 5 && strcmp(argv[4], "-t") == 0){
            try{
                number_of_threads = stoi(argv[5]);
            }catch(exception & e) {
                printf("Please write an integer representing number of threads.\n");
            }
        }
    }else {
        if(path.substr(path.size()-1, 1) != "/"){
            path += "/";
        }
        printf("No recusive depth (no threads), for more threads use (-r and -t).\n");
        number_of_threads = 1;
    }
    printf("Starting on %d thread(s).", number_of_threads);

//     getting all subdirctories
    if(recursive_flag){
        for(auto & item: get_subdirectories(path))
            paths.emplace_back(item);
    } else{
        paths.emplace_back(path);
    }
    cout << "size" << paths.size() << endl;


    // starting threads
    vector<vector<string>> subVecs{};
    auto itr = paths.cbegin();
    int jump = floor(paths.size() / number_of_threads) + 1;
    if(jump == 0)
        jump = 1;
    while (itr < paths.cend()){
        subVecs.emplace_back(vector<string>{itr, itr+jump});
        itr += jump;
    }
    vector<thread> workers;
    for(const auto & item: subVecs){
        workers.emplace_back(thread(thread_start, item, output_path));
    }
    for(auto & t: workers)
        t.join();
    return 0;
}