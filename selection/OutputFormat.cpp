//
// Created by zidce on 27.04.2020.
//

#include <iomanip>
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

stringstream OutputFormat::to_JSON(const string & coma, long double timestamp) {
    long double latest = getLatestTimestamp();
    stringstream ofs;
    ofs << coma << "{\"score\": " << setprecision(10) << score << ",";
    ofs << "\"supply_gain_index\": [" << supply_gain_indexes[0] << ", " << supply_gain_indexes[1]
        << ", " << supply_gain_indexes[2] << "],";
    ofs << "\"demand_gain_index\": [" << demand_gain_indexes[0] << ", " << demand_gain_indexes[1]
        << ", " << demand_gain_indexes[2] << "],";
    ofs << "\"supply_gain\": " << supply_gain << ",";
    ofs << "\"demand_gain\": " << demand_gain << ",";
    ofs << "\"time_delta\": " << timestamp - latest << ",";
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
    if(score != other.score) {
//        printf("%.10Lf %.10Lf\n", score, other.score);
//        for(const auto & curr: current) {
//            printf("%6.10Lf ", curr.getTimestamp());
//        }
//        cout << endl;
//        for(const auto & curr: other.current)
//            printf("%6.10Lf ", curr.getTimestamp());
//        cout << endl;
        return false;
    }
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
//    if(match_counter < 2){
//        if(score != other.score) {
//            printf("%.10Lf %.10Lf\n", score, other.score);
//            for(const auto & curr: current)
//                printf("%6.10Lf ", curr.getTimestamp());
//            cout << endl;
//            for(const auto & curr: other.current)
//                printf("%6.10Lf ", curr.getTimestamp());
//            cout << endl;
//            return false;
//        }
//    }
    return match_counter >= 2;
}


long double OutputFormat::getLatestTimestamp() {
    long double max = -1;
    for(const auto & curr: current){
//        printf("%9.5Lf ", curr.getTimestamp());
        if(curr.getTimestamp() > max)
            max = curr.getTimestamp();
    }
//    cout << endl;
//    printf("%9.5Lf\n", max);
    return max;
}

OutputFormat& OutputFormat::operator=(const OutputFormat &rhs) {
    return *this;
}