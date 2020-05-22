//
// Created by zidce on 27.04.2020.
//

#ifndef UNTITLED_OUTPUTFORMAT_H
#define UNTITLED_OUTPUTFORMAT_H

#include <vector>
#include "CurrencyPair.h"
#include <iostream>
using namespace std;

class OutputFormat {
public:
    OutputFormat();
    OutputFormat(const OutputFormat & other);
    OutputFormat(long double score1, const vector<int> & supply_gain_indexes1, const vector<int> & demand_gain_indexes1,
                 long double demand_gain1, long double supply_gain1, bool calculation_type_linear1,
                 const vector<CurrencyPair> & current1, int name1);
    OutputFormat& operator=(const OutputFormat& rhs);
    stringstream toJSON(const string &coma, long double timestamp);
    long double getLatestTimestamp();
    bool eq ( const OutputFormat & other);
private:
    long double score;
    vector<int> supply_gain_indexes;
    vector<int> demand_gain_indexes;
    long double demand_gain;
    long double supply_gain;
    bool calculation_type_linear;
    vector<CurrencyPair> current;
    int name;
};


#endif //UNTITLED_OUTPUTFORMAT_H
