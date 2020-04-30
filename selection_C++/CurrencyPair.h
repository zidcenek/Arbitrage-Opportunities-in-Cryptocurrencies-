//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_CURRENCYPAIR_H
#define UNTITLED_CURRENCYPAIR_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

class CurrencyPair {
public:
    CurrencyPair();
    explicit CurrencyPair(const string & input, const string & currency_name);
    const vector<double> &getSupply() const;
    const vector<double> &getDemand() const;
    long double getTimestamp() const;
    string to_JSON() const;
    string array_to_string(const vector<double> & arr) const;
protected:
    static vector<double> parse_demand(stringstream my_stream);
private:
    string trade_id;
    vector<double> supply;
    vector<double> demand;
    long double timestamp;
    string currency;
};


#endif //UNTITLED_CURRENCYPAIR_H
