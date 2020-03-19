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
#include "Arbitrage.h"

using namespace std;
namespace fs = std::experimental::filesystem;

int main() {
    FilesManager fm = FilesManager(DATA_PATH);
    vector<vector<Triplet> > triplets = fm.select_files();
    for(const auto & vec: triplets){
        for(const Triplet & triplet: vec){
            cout << triplet.getOutput_filename() << endl;
            cout << triplet.getFile1() << " " << triplet.getFile2() << " " << triplet.getFile3() << " " << endl;
            Arbitrage arbitrage = Arbitrage();
            arbitrage.initialize(triplet);
            arbitrage.run();
        }
    }
    return 0;
}