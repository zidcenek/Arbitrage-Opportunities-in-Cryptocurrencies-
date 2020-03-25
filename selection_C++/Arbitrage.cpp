//
// Created by zidce on 18.03.2020.
//

#include "Arbitrage.h"

Arbitrage::Arbitrage(){
    stop = false;
}

/**
 * Initializes the whole class with given parameters
 * @param triplet - Triplet class containing the files
 * @return - true or false based on the initialization status
 */
bool Arbitrage::initialize(const Triplet & triplet){
    counter = 0;
    openFile(triplet.getFile1());
    openFile(triplet.getFile2());
    openFile(triplet.getFile3());
    currency_pair_names.push_back(triplet.getCurrency1());
    currency_pair_names.push_back(triplet.getCurrency2());
    currency_pair_names.push_back(triplet.getCurrency3());
    calculation_type_linear = triplet.getLinear();
    output_name = triplet.getOutput_filename();
    output_directory_name = triplet.getOutputDirectoryName();
    for(int i = 0 ; i < dataframes.size(); i++){
        string tmp;
        getline(*dataframes[i], tmp);
        current.emplace_back(CurrencyPair(tmp, currency_pair_names[i]));
    }
    return true;
}

/**
 * calculates a different type of score based on the linear_calculation_type
 * @param a
 * @param b
 * @param c
 * @param demand_flag -> determines the calculation type
 * @return -> calculated score
 */
long double Arbitrage::calculateScore(double a, double b, double c, bool demand_flag) const {
    if(calculation_type_linear){
        if (demand_flag)
            return a * b * c;
        return 1 / a / b / c;
    }
    if (demand_flag)
        return a / b / c;
    return b * c / a;
}

/**
 * calculates at which position is the max_gain
 * @param pairs1
 * @param pairs2
 * @param pairs3
 * @param demand_flag -> determines the calculation type
 * @param best_gain -> out parameter for the max_gain
 * @return -> position of the max_gain
 */
vector<int> Arbitrage::calculateMaxGainPosition(
        vector<double> pairs1, vector<double> pairs2, vector<double> pairs3, bool demand_flag, long double & best_gain
) const {
    long double max_gain = 0;
    vector<int> best_indexes(3, 0);
    // goes through all the combinations of the best demand/supply
    for(int i = 0; i < pairs1.size() - 1; i += 2){
        for(int j = 0; j < pairs2.size() - 1; j += 2){
            for(int k = 0; k < pairs2.size() - 1; k += 2){
                long double score = calculateScore(pairs1.at(i), pairs2.at(j), pairs3.at(k), demand_flag);
                if(score > 1) {
                    long double gain = calculate_narrowest(
                            pair<double, double> (pairs1.at(i), pairs1.at(i+1)),
                            pair<double, double> (pairs2.at(j), pairs2.at(j+1)),
                            pair<double, double> (pairs3.at(k), pairs3.at(k+1))
                            ) * (score - 1);
                    if(gain > max_gain) {
                        max_gain = gain;
                        best_indexes[0] = i / 2;  // "/ 2" because of i += 2 etc.
                        best_indexes[1] = j / 2;  // etc.
                        best_indexes[2] = k / 2;  // etc.
                    }
                }
            }
        }
    }
    max_gain = max_gain * 0.999 * 0.999 * 0.999;  // due to binance fee for every trade
    best_gain = max_gain;
//    cout << best_indexes[0] << best_indexes[1] << best_indexes[2] << endl;
//    cout << max_gain << endl;
    return best_indexes;
}

/**
 * calculates the narrowest part of the triangle (based on the amounts)
 * @param pair1
 * @param pair2
 * @param pair3
 * @return -> smallest thickness
 */
long double Arbitrage::calculate_narrowest(
        pair<double, double> pair1, pair<double, double> pair2, pair<double, double> pair3
        ) const{
    vector<long double> thickness;
    thickness.push_back(min(pair2.first * pair2.second, pair1.second));
    thickness.push_back(min(pair3.first * pair3.second, pair2.second));
    thickness.push_back(min(pair1.first * pair1.second, pair3.second));
    if(calculation_type_linear){
        thickness[1] = thickness[1] / pair1.first;
        thickness[2] = thickness[2] / pair1.first / pair2.first;
    }else {
        thickness[1] = thickness[1] * pair1.first;
        thickness[2] = thickness[2] * pair1.first * pair2.first;
    }
    return *min_element(thickness.begin(), thickness.end());
}

/**
 * the main cycle that goes through the files and tries to find arbitrages
 */
void Arbitrage::run(){
    string coma;
    const int dir = system(("mkdir -p " + OUTPUT_DIRECTORY + output_directory_name).c_str());
    if(dir){
        cout << "Output directory either does not exist or could not be created" << endl;
        return;
    }

    ofstream ofs(OUTPUT_DIRECTORY + output_directory_name + output_name + ".json");
    if(!ofs.good()){
        cout << "Could not open output file" << endl;
        return;
    }
    ofs << "[";
    while(! stop){
        int index = getOldest();
        long double score, supply_gain, demand_gain;
        vector<int> supply_gain_indexes, demand_gain_indexes;
        getNext(index);
        if((score = detection()) > 1){
            if( calculation_type_linear ){
                supply_gain_indexes = calculateMaxGainPosition(current[0].getSupply(), current[1].getSupply(),
                                                       current[2].getSupply(), false, supply_gain);
                demand_gain_indexes = calculateMaxGainPosition(current[0].getDemand(), current[1].getDemand(),
                                                       current[2].getDemand(), true, demand_gain);
            } else {
                supply_gain_indexes = calculateMaxGainPosition(current[0].getSupply(), current[1].getDemand(),
                                                       current[2].getDemand(), false, supply_gain);
                demand_gain_indexes = calculateMaxGainPosition(current[0].getDemand(), current[1].getSupply(),
                                                       current[2].getSupply(), true, demand_gain);
            }
            bool first_item = true;
            if(supply_gain_indexes.size() != 3 || demand_gain_indexes.size() != 3)
                continue;
            ofs << coma << "{\"score\": " << score << ",";
            ofs << "\"supply_gain_index\": [" << demand_gain_indexes[0] << ", " << demand_gain_indexes[1]
                << ", " << demand_gain_indexes[2] << "],";
            ofs << "\"demand_gain_index\": [" << supply_gain_indexes[0] << ", " << supply_gain_indexes[1]
                << ", " << supply_gain_indexes[2] << "],";
            ofs << "\"supply_gain\": " << supply_gain << ",";
            ofs << "\"demand_gain\": " << demand_gain << ",";
            ofs << "\"calculation_type_linear\": " << calculation_type_linear << ",";
            ofs << "\"pairs\": [";
            for(const auto& item: current){
                if(first_item)
                    first_item = false;
                else
                    ofs << ",";
                ofs << item.to_JSON();
            }
            ofs << "]}";
            coma = ",";
        }
    }
    ofs << "]";
    ofs.close();
    for(auto & df: dataframes){
        df->close();
    }
}

/**
 * calculates if an arbitration occurred
 * @return long double representing the gain or the lost > 1 symbols gain
 */
long double Arbitrage::detection(){
    long double case1, case2;
    if(calculation_type_linear) {
        case1 = current[0].getDemand()[0] * current[1].getDemand()[0] * current[2].getDemand()[0];
        case2 = 1 / current[0].getSupply()[0] / current[1].getSupply()[0] / current[2].getSupply()[0];
    } else {
        case1 = current[1].getDemand()[0] * current[2].getDemand()[0] / current[0].getSupply()[0];
        case2 = current[0].getDemand()[0] / current[1].getSupply()[0] / current[2].getSupply()[0];
    }
    if(case1 < case2)
        return case2;
    else
        return case1;
}

bool Arbitrage::openFile(string const& filename){
    ifstream *fin = new ifstream("../data/" + filename);
    if (! fin->is_open() ){
        cout << "File could not be opened" << endl;
        return false;
    }
    dataframes.push_back(fin);
    return true;
}

void Arbitrage::getNext(int index){
    if(dataframes[index]->eof()) {
        stop = true;
        return;
    }
    string tmp;
    getline(*dataframes[index], tmp);
    if(tmp.empty()) {
        stop = true;
        return;
    }
    try {
        current[index] = CurrencyPair(tmp, currency_pair_names[index]);
    } catch(const exception& e) {
        if(++counter % 1000 == 0)
            cout << "wrong line no." << counter << endl;
    }

}

int Arbitrage::getOldest(){
    vector<double> tmp;
    for(auto const& item: current){
        tmp.push_back(item.getTimestamp());
    }
    return std::min_element(tmp.begin(), tmp.end()) - tmp.begin();
}


