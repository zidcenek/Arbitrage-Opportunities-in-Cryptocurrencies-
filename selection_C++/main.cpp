//
// Created by zidce on 13.03.2020.
//
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <experimental/filesystem>
#include <thread>
#include "Arbitrage.h"

using namespace std;
namespace fs = std::experimental::filesystem;


void split_work(const vector<string> & paths){
    printf("Starting thread\n");
//    int i = 0;
    for(const auto & path: paths){
        printf("%s\n", path.c_str());
        FilesManager fm = FilesManager(path);
        vector<vector<Triplet> > triplets = fm.select_files();
        for(const auto & vec: triplets){
//            if(i++ < 1)
//                continue;
            for(const Triplet & triplet: vec){
//                if(i++ < 2)
//                    continue;
                string ending = "2020-03-02";
                const string & tmp = triplet.getOutput_filename();
                if(! (tmp.substr(tmp.size() - ending.size(), ending.size()) == ending))
                    continue;
                cout << triplet.getOutput_filename() << endl;
                cout << triplet.getFile1() << " " << triplet.getFile2() << " " << triplet.getFile3() << " " << endl;
                Arbitrage arbitrage = Arbitrage();
                if(arbitrage.initialize(triplet))
                    arbitrage.run();
            }
        }
    }
    printf("Ending thread\n");
}

int main() {
    int number_of_chunks = 1; // min number of threads
    vector<string> paths;
//    paths.emplace_back("../../../../data/data_02-04=10/");
//    paths.emplace_back("../../../../data/data_02-18=21/");
//    paths.emplace_back("../../../../data/data_02-20=23/");
//    paths.emplace_back("../../../../data/data_02-23=26/");
//    paths.emplace_back("../../../../data/data_02-26=01/");
//    paths.emplace_back("../../../../data/data_03-01=06/");

//    paths.emplace_back("../../../../data/corrupted/out/data_02-18=21/");
//    paths.emplace_back("../../../../data/corrupted/out/data_02-20=23/");
//    paths.emplace_back("../../../../data/corrupted/out/data_02-23=26/");
//    paths.emplace_back("../../../../data/corrupted/out/data_02-26=01/");
    paths.emplace_back("../../../../data/corrupted/out/data_03-01=06/");
//    paths.emplace_back("../../../../data/testing/");

//    paths.emplace_back("../../../../data/data_03-06=10/");
//    paths.emplace_back("../../../../data/data_03-11=15/");
//    paths.emplace_back("../../../../data/data_03-16=21/");
//    paths.emplace_back("../../../../data/data_03-22=30/");
//    paths.emplace_back("../../../../data/data_03-31=09/");
//    paths.emplace_back("../../../../data/data_04-10=12/");
//    paths.emplace_back("../../../../data/data_04-13=16/");
//    paths.emplace_back("../../../../data/data_04-17=20/");
    vector<vector<string>> subVecs{};
    auto itr = paths.cbegin();
    int jump = paths.size() / number_of_chunks;
    if(jump == 0)
        jump = 1;
    while (itr < paths.cend()){
        subVecs.emplace_back(std::vector<string>{itr, itr+jump});
        itr += jump;
    }
    vector<thread> workers;
    for(const auto & item: subVecs){
        workers.emplace_back(thread(split_work, item));
    }
    for(auto & t: workers)
        t.join();
    return 0;
}