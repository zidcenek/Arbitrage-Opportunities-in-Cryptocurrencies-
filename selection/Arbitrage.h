//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_ARBITRAGE_H
#define UNTITLED_ARBITRAGE_H

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <numeric>
#include "CurrencyPair.h"
#include "FilesManager.h"
#include "OutputFormat.h"

using namespace std;

//const string OUTPUT_DIRECTORY = "../../statistics/output_data/";  // defines output directory
//const string OUTPUT_DIRECTORY = "../../../../data/testing-out/";  // defines output directory
const string OUTPUT_DIRECTORY = "../../statistics/test/";  // defines output directory

class Arbitrage {
public:
    Arbitrage();
    bool initialize(const Triplet & triplet, const string & output_path);
    void run();
    long double detection();
protected:
    bool openFile(string const& filename);
    bool getNext(int index);
    vector<int> calculateMaxGainPosition(vector<double> pairs1, vector<double> pairs2, vector<double> pairs3,
            bool demand_flag, long double & best_gain, long double & new_score) const;
    long double calculateScore(double a, double b, double c, bool demand_flag) const;
    long double calculateNarrowest(pair<double, double> pair1, pair<double, double> pair2, pair<double, double> pair3,
                                   bool demand_flag) const;
    int getOldest();
    bool closeTimestamps(double t1, double t2, double t3, double tolerance);
    bool reinitialize();
private:
    vector<ifstream*> dataframes;
    vector<CurrencyPair> current;
    vector<CurrencyPair> buffer;
    vector<string> currency_pair_names;
    bool stop;
    string output_name;
    string output_directory_name;
    int counter;
    bool calculation_type_linear;
    int arbitrages;
    int without_fees;
    int all;
    long double fees;
    int stuck_counter;
    int reinitialization_counter;
    vector<int> looked_into;
    string OUTPUT_DIRECTORY;
};


#endif //UNTITLED_ARBITRAGE_H
