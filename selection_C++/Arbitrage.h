//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_ARBITRAGE_H
#define UNTITLED_ARBITRAGE_H

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "CurrencyPair.h"
#include "FilesManager.h"

using namespace std;

const string OUTPUT_DIRECTORY = "../../statistics/test/";  // defines output directory

class Arbitrage {
public:
    Arbitrage();
    bool initialize(const Triplet & triplet);
    void run();
    long double detection();
protected:
    bool openFile(string const& filename);
    void getNext(int index);
    int getOldest();
private:
    vector<ifstream*> dataframes;
    vector<CurrencyPair> current;
    vector<string> currency_pair_names;
    bool stop;
    string output_name;
    int counter;
    bool calculation_type_linear;
};


#endif //UNTITLED_ARBITRAGE_H
