//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_CURRENCYPAIR_H
#define UNTITLED_CURRENCYPAIR_H

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <map>

using namespace std;

class CurrencyPair {
public:
    CurrencyPair();
    explicit CurrencyPair(const string & input, const string & currency_name);
    const vector<double> &getSupply() const;
    const vector<double> &getDemand() const;
    long double getTimestamp() const;
    string toJSON() const;
    int getTradeId() const;
    string arrayToString(const vector<double> &arr) const;
    double getTolerance() const;

protected:
    static vector<double> parseDemand(stringstream my_stream);
private:
    long long trade_id;
    vector<double> supply;
    vector<double> demand;
    long double timestamp;
    string currency;
    static const inline std::map<string, double> tolerance = {
            {"BCHBNB", 15.382543325424194},
            {"BCHBTC", 1.7832690477371216},
            {"BCHUSDT", 1.0603818893432617},
            {"BNBBTC", 2.0738070011138916},
            {"BNBETH", 4.916260719299316},
            {"BNBUSDT", 1.5324095487594604},
            {"BTCUSDT", 1.0402958393096924},
            {"EOSBNB", 11.83772885799408},
            {"EOSBTC", 3.1765609979629517},
            {"EOSETH", 5.863194942474365},
            {"EOSUSDT", 1.0864425897598267},
            {"ETHBTC", 1.0877046585083008},
            {"ETHUSDT", 1.0424208641052246},
            {"LTCBNB", 19.0436270236969},
            {"LTCBTC", 2.5378408432006836},
            {"LTCETH", 6.289291501045227},
            {"LTCUSDT", 1.3222262859344482},
            {"TRXBNB", 14.252377033233643},
            {"TRXBTC", 3.204934239387512},
            {"TRXETH", 5.471882224082947},
            {"TRXUSDT", 2.351563334465027},
            {"TRXXRP", 20.31471037864685},
            {"TUSDUSDT", 19.26926052570343},
            {"XMRBNB", 29.276237845420837},
            {"XMRBTC", 5.128070116043091},
            {"XMRETH", 12.862172961235046},
            {"XMRUSDT", 2.9835082292556763},
            {"XRPBNB", 14.457493901252747},
            {"XRPBTC", 2.0687578916549683},
            {"XRPETH", 4.037391304969788},
            {"XRPUSDT", 1.128029465675354}
    };
};


#endif //UNTITLED_CURRENCYPAIR_H
