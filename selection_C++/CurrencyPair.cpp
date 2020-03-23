//
// Created by zidce on 18.03.2020.
//

#include "CurrencyPair.h"

/**
 * constructor
 * @param input - one line from the file, which si parsed and stored in the class properties
 */
CurrencyPair::CurrencyPair(const string & input, const string & currency_name){
    string tmp;
    stringstream s(input);
    std::getline(s, tmp, ';');
    trade_id = tmp;
    std::getline(s, tmp, ';');
    demand = parse_demand(stringstream(tmp));
    std::getline(s, tmp, ';');
    supply = parse_demand(stringstream(tmp));
    std::getline(s, tmp, ';');
    timestamp = stold(tmp);
    currency = currency_name;
}

const vector<double> & CurrencyPair::getSupply() const {
    return supply;
}

const vector<double> & CurrencyPair::getDemand() const {
    return demand;
}

double CurrencyPair::getTimestamp() const {
    return timestamp;
}

/**
 * Transforms the class parameters and returns them in a JSON format
 * @return string in a JSON format
 */
string CurrencyPair::to_JSON() const {
    string json = "{";
    json.append("\"id\":" + trade_id + ",");
    json.append("\"demand\": " + array_to_string(demand) + ",");
    json.append("\"supply\": " + array_to_string(supply) + ",");
    json.append("\"timestamp\":" + to_string(timestamp) + ",");
    json.append("\"currency\":\"" + currency + "\"");
    json.append("}");
    return json;
}
/**
 * Transorms an array into JSON string of array of pairs
 * @param arr
 * @return - string in a JSON format
 */
string CurrencyPair::array_to_string(const vector<double> & arr) const {
    string output = "[";
    for(int i = 0; i < arr.size() - 1; i += 2){
        if(i)
            output.append(", ");
        output.append("[" + to_string(arr[i]) + "," + to_string(arr[i+1]) + "]");
    }
    output.append("]");
    return output;
}

/**
 * Parses a string into array of doubles
 * @param my_stream
 * @return - vecotr of doubles
 */
vector<double> CurrencyPair::parse_demand(stringstream my_stream) {
    vector<double> output;
    string word;
    vector<string> row;
    while (std::getline(my_stream, word, '\'')) {
        row.push_back(word);
    }
    for(int i = 0; i < row.size() ; i++ ){
        if(i % 2 == 0)
            continue;
        output.push_back(stod(row[i]));
    }
    return output;
}
;