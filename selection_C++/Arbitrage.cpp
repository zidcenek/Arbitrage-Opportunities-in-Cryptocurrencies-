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
    for(int i = 0 ; i < dataframes.size(); i++){
        string tmp;
        getline(*dataframes[i], tmp);
        current.emplace_back(CurrencyPair(tmp, currency_pair_names[i]));
    }
    return true;
}

/**
 * the main cycle that goes through the files and tries to find arbitrages
 */
void Arbitrage::run(){
    string coma;
    ofstream ofs(OUTPUT_DIRECTORY + output_name + ".json");
    if(!ofs.good()){
        cout << "Could not open output file" << endl;
        return;
    }
    ofs << "[";
    while(! stop){
        int index = getOldest();
        getNext(index);
        long double score;
        if((score = detection()) > 1){
            bool first_item = true;
            ofs << coma << "{\"score\": " << score << ",";
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
        case2 = current[0].getSupply()[0] * current[1].getSupply()[0] * current[2].getSupply()[0];
    } else {
        case1 = current[0].getSupply()[0] / current[1].getDemand()[0] / current[2].getDemand()[0];
        case2 = current[0].getDemand()[0] / current[1].getSupply()[0] / current[2].getSupply()[0];
    }
//    if(case1 < 1.0 && case2 < 1.0)
//        cout << case1 << " - " << case1 << " ---- " << calculation_type_linear << output_name << endl;
    if(case1 < case2)
        return case1;
    else
        return case2;
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
        cout << "wrong line no." << counter++ << endl;
    }

}

int Arbitrage::getOldest(){
    vector<double> tmp;
    for(auto const& item: current){
        tmp.push_back(item.getTimestamp());
    }
    return std::min_element(tmp.begin(), tmp.end()) - tmp.begin();
}


