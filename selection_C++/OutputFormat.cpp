//
// Created by zidce on 27.04.2020.
//

#include "OutputFormat.h"

OutputFormat::OutputFormat() {}

OutputFormat::OutputFormat(const OutputFormat & other)
    :   supply_gain_indexes(other.supply_gain_indexes),
        demand_gain_indexes(other.demand_gain_indexes),
        score(other.score),
        supply_gain(other.supply_gain),
        demand_gain(other.demand_gain),
        calculation_type_linear(other.calculation_type_linear),
        name(other.name),
        current(other.current)

{}

OutputFormat::OutputFormat(long double score1, const vector<int> & supply_gain_indexes1, const vector<int> & demand_gain_indexes1,
                           long double demand_gain1, long double supply_gain1, bool calculation_type_linear1,
                           const vector<CurrencyPair> & current1, int name1)
                           :supply_gain_indexes(supply_gain_indexes1),
                           demand_gain_indexes(demand_gain_indexes1),
                           current(current1){
    score = score1;
    supply_gain = supply_gain1;
    demand_gain = demand_gain1;
    calculation_type_linear = calculation_type_linear1;
    name = name1;
}

stringstream OutputFormat::to_JSON(const string & coma, long double timestamp, int origin) {
    if(timestamp - getLatestTimestamp() == 0) {
        cout << timestamp - getLatestTimestamp() << " from " << origin << endl;
    }
//    cout << timestamp - getLatestTimestamp() << endl;
    stringstream ofs;
    ofs << coma << "{\"score\": " << score << ",";
    ofs << "\"supply_gain_index\": [" << demand_gain_indexes[0] << ", " << demand_gain_indexes[1]
        << ", " << demand_gain_indexes[2] << "],";
    ofs << "\"demand_gain_index\": [" << supply_gain_indexes[0] << ", " << supply_gain_indexes[1]
        << ", " << supply_gain_indexes[2] << "],";
    ofs << "\"supply_gain\": " << supply_gain << ",";
    ofs << "\"demand_gain\": " << demand_gain << ",";
    ofs << "\"calculation_type_linear\": " << calculation_type_linear << ",";
    ofs << "\"time_delta\": " << timestamp - getLatestTimestamp() << ",";
    ofs << "\"pairs\": [";
    bool first_item = true;
    for (const auto &item: current) {
        if (first_item)
            first_item = false;
        else
            ofs << ",";
        ofs << item.to_JSON();
    }
    ofs << "]}";
    return ofs;
}

bool OutputFormat::eq (const OutputFormat & other){
    vector<long double> first, second;
    int match_counter = 0;
    if(score != other.score)
        return false;
    for(const auto & curr: current)
        first.push_back(curr.getTimestamp());
    for(const auto & curr: other.current)
        second.push_back(curr.getTimestamp());
    if(first.size() != second.size())
        return false;
    for(int i = 0; i < first.size(); i++){
        if(first[i] == second[i])
            match_counter++;
    }
    return match_counter >= 2;
}


long double OutputFormat::getLatestTimestamp() {
    long double max = -1;
    for(const auto & curr: current){
//        printf("%9.5f ", curr.getTimestamp());
        if(curr.getTimestamp() > max)
            max = curr.getTimestamp();
    }
//    cout << endl;
    return max;
}