//
// Created by zidce on 18.03.2020.
//

#include "Arbitrage.h"

Arbitrage::Arbitrage(){
    stop = false;
    stuck_counter = 0;
    arbitrages = 0;
    without_fees = 0;
    all = 0;
    fees = 0.999 * 0.999 * 0.999;
    looked_into = vector<int>(3, 0);
    stucked = vector<int>(3, 0);
    counter = 0;
    calculation_type_linear = false;

}

/**
 * Initializes the whole class with given parameters
 * @param triplet - Triplet class containing the files
 * @return - true or false based on the initialization status
 */
bool Arbitrage::initialize(const Triplet & triplet, const string & output_path){
    openFile(triplet.getFile1());
    openFile(triplet.getFile2());
    openFile(triplet.getFile3());
    currency_pair_names.push_back(triplet.getCurrency1());
    currency_pair_names.push_back(triplet.getCurrency2());
    currency_pair_names.push_back(triplet.getCurrency3());
    calculation_type_linear = triplet.getLinear();
    output_name = triplet.getOutput_filename();
    output_directory_name = triplet.getOutputDirectoryName();
    OUTPUT_DIRECTORY = output_path;
    for(int i = 0 ; i < dataframes.size(); i++){
        while(true){
            if(dataframes[i]->eof()) {
                for(auto & df: dataframes){
                    df->close();
                }
                cout << "eof return" << endl;
                return false;
            }
            string tmp;
            try {
                getline(*dataframes[i], tmp);
                current.emplace_back(CurrencyPair(tmp, currency_pair_names[i]));
                buffer.emplace_back(CurrencyPair(tmp, currency_pair_names[i]));
                break;
            } catch(const exception& e) {
                cout << "wrong line no." << triplet.getOutput_filename() <<  counter << endl; // todo - delete (err message)
                if(++counter % 1000 == 0)
                    cout << "wrong line no." << triplet.getOutput_filename() <<  counter << endl;
            }
        }
//        current.emplace_back(CurrencyPair());
        while(! getNext(i)){
            if (stop)
                return false;
        }
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
        vector<double> pairs1, vector<double> pairs2, vector<double> pairs3, bool demand_flag, long double & best_gain,
        long double & new_score
) const {
    if(calculation_type_linear)
        cout << "linear" << endl;
    long double max_gain = 0;
    vector<int> best_indexes(3, 0);
    new_score = 0;
    // goes through all the combinations of the best demand/supply
    for(int i = 0; i < pairs1.size() - 1; i += 2){
        for(int j = 0; j < pairs2.size() - 1; j += 2){
            for(int k = 0; k < pairs2.size() - 1; k += 2){
                long double score = calculateScore(pairs1.at(i), pairs2.at(j), pairs3.at(k), demand_flag);
                score = score * fees;  // due to binance fee for every trade
                if(score > 1) {
                    long double gain = calculate_narrowest(
                            pair<double, double> (pairs1.at(i), pairs1.at(i+1)),
                            pair<double, double> (pairs2.at(j), pairs2.at(j+1)),
                            pair<double, double> (pairs3.at(k), pairs3.at(k+1)),
                            demand_flag) * (score - 1);
                    if(gain > max_gain) {
                        max_gain = gain;
                        best_indexes[0] = i / 2;  // "/ 2" because of i += 2 etc.
                        best_indexes[1] = j / 2;  // etc.
                        best_indexes[2] = k / 2;  // etc.
                        new_score = score;
                    }
                }
            }
        }
    }
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
        pair<double, double> pair1, pair<double, double> pair2, pair<double, double> pair3, bool demand_flag
        ) const{
    vector<long double> thickness;
    if(calculation_type_linear){
        if(demand_flag){
            thickness.push_back(pair1.second);
            thickness.push_back(pair2.second * pair1.first);
            thickness.push_back(pair3.second * pair2.first * pair1.first);
        } else {
            thickness.push_back(pair1.second);
            thickness.push_back(pair2.second / pair2.first / pair3.first);
            thickness.push_back(pair3.second / pair3.first);
        }
    } else{
        if(demand_flag){
            thickness.push_back(pair1.second);
            thickness.push_back(pair2.second);
            thickness.push_back(pair3.second / pair2.first);
        } else {
            thickness.push_back(pair1.second);
            thickness.push_back(pair2.second);
            thickness.push_back(pair3.second * pair3.first / pair1.first);
        }
    }
    return *min_element(thickness.begin(), thickness.end());
}

/**
 * the main cycle that goes through the files and tries to find arbitrages
 */
void Arbitrage::run(){
    int a = 0;
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
    ofs << "{";
    ofs << "\"arbitrage_stats\":[";
    bool start_of_sequence = true;
    // represents the first and current in sequence of arbitrage arbitrage, if many follow, it will be saved as only 1
    OutputFormat * first_in_sequence, * current_in_sequence;
    while(! stop){
        int index = getOldest();
        if(! getNext(index))
            continue;
        long double score, supply_gain, demand_gain, supply_score, demand_score;
        vector<int> supply_gain_indexes, demand_gain_indexes;
        if(!looked_into.empty()) {
//            cout << "skiping" << endl;
            continue;
        }
        all++;
        if((score = detection()) > 1) {
            // checks if timestamps are maximally 1 minute apart from each other
            if ( ! closeTimestamps(current[0].getTimestamp(), current[1].getTimestamp(), current[2].getTimestamp()))
                continue;
            without_fees++;
            // writes stats only for those with score higher even after fees
            if (score * fees > 1) {
                arbitrages++;
                    if (calculation_type_linear) {
                        supply_gain_indexes = calculateMaxGainPosition(current[0].getSupply(), current[1].getSupply(),
                                                                       current[2].getSupply(), false, supply_gain, supply_score);
                        demand_gain_indexes = calculateMaxGainPosition(current[0].getDemand(), current[1].getDemand(),
                                                                       current[2].getDemand(), true, demand_gain, demand_score);
                    } else {
                        supply_gain_indexes = calculateMaxGainPosition(current[0].getSupply(), current[1].getDemand(),
                                                                       current[2].getDemand(), false, supply_gain, supply_score);
                        demand_gain_indexes = calculateMaxGainPosition(current[0].getDemand(), current[1].getSupply(),
                                                                       current[2].getSupply(), true, demand_gain, demand_score);
                    }
                if (supply_gain_indexes.size() != 3 || demand_gain_indexes.size() != 3)
                    continue;

                score = max(supply_score, demand_score);
                if(start_of_sequence){
                    // start of sequence -> first and current are the same
                    start_of_sequence = false;
                    first_in_sequence = new OutputFormat(score, supply_gain_indexes, demand_gain_indexes, demand_gain,
                                                   supply_gain, calculation_type_linear, current, a++);
                    current_in_sequence = first_in_sequence;
                } else {
                    // if more follows after first in sequence
                    auto * tmp = new OutputFormat(score, supply_gain_indexes, demand_gain_indexes, demand_gain,
                                      supply_gain, calculation_type_linear, current, a++);
                    // if the new and current equal each other, continue
                    if(current_in_sequence->eq(*tmp)){
                        delete current_in_sequence;
                        current_in_sequence = new OutputFormat(*tmp);
                        delete tmp;
                        continue;
                    } else{
                        // else save the first sequence with deltatime
                        vector<long double> temp;
                        for(const auto & curr: current) {
                            temp.push_back(curr.getTimestamp());
                        }
                        ofs << first_in_sequence->to_JSON(coma, tmp->getLatestTimestamp()).rdbuf();
//                        cout << "/------------------" << endl;
                        coma = ",";
                        delete first_in_sequence;
                        first_in_sequence = new OutputFormat(*tmp);
                        delete tmp;
                    }
                }

            } else {
                if( ! start_of_sequence ){
                    // else save the first sequence with deltatime of current
                    vector<long double> tmp;
                    for(const auto & curr: current)
                        tmp.push_back(curr.getTimestamp());
                    ofs << first_in_sequence->to_JSON(coma, *max_element(tmp.begin(), tmp.end())).rdbuf();
                    coma = ",";
                    delete first_in_sequence;
                    start_of_sequence = true;
                }
            }
        } else {
            if( ! start_of_sequence ){
                // else save the first sequence with deltatime of current
                vector<long double> tmp;
                for(const auto & curr: current)
                    tmp.push_back(curr.getTimestamp());
                ofs << first_in_sequence->to_JSON(coma, *max_element(tmp.begin(), tmp.end())).rdbuf();
                coma = ",";
                delete first_in_sequence;
                start_of_sequence = true;
            }
        }
    }
    ofs << "],";
    ofs << "\"arbitrages_count\": " << arbitrages << ",";
    ofs << "\"without_fees_count\": " << without_fees << ",";
    ofs << "\"all_count\": " << all;
    ofs << "}";
    ofs.close();
    cout << arbitrages << ":" << without_fees << ":" << all << endl;
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
    if(case1 < case2) {
        return case2;
    }
    else {
        return case1;
    }
}

/**
 * This method tries to open the given file
 * @param
 * @return - bool representing opening status
 */
bool Arbitrage::openFile(string const& filename){
    ifstream *fin = new ifstream("../data/" + filename);
    if (! fin->is_open() ){
        cout << "File could not be opened" << endl;
        return false;
    }
    dataframes.push_back(fin);
    return true;
}

/**
 * Overwites the value in current[index] to a new CurrencyPair value
 * @param index -> represents which value to overwrite
 * @return -> true if the overwrite went well
 */
bool Arbitrage::getNext(int index){
    if(dataframes[index]->eof()) {
        stop = true;
        return false;
    }

    string tmp;
    long double old_timestamp = buffer[index].getTimestamp();
    getline(*dataframes[index], tmp);
    if(tmp.empty()) {
        stop = true;
        return false;
    }
    CurrencyPair tempCP;
    try {
        tempCP = CurrencyPair(tmp, currency_pair_names[index]);
    } catch(const exception& e) {
        if(++counter % 1000 == 0)
            cout << "wrong line no." << counter << endl;
        return false;
    }
    int hours = 1;
    if(tempCP.getTimestamp() < (old_timestamp + 3600*hours) && tempCP.getTimestamp() > (old_timestamp - 3600*hours)) {
        current[index] = buffer[index];
        buffer[index] = tempCP;
        if(! looked_into.empty()) {
            // looked_into makes sure all currency pairs have been properly initialized
            if (looked_into[index] == 1)
                looked_into[index] = 2;
            else
                looked_into[index] = 1;
            int sum = std::accumulate(std::begin(looked_into), std::end(looked_into), 0);
            if(sum == 6) {
                looked_into.clear();
                cout << "Initialized successfully" << endl;
            }
        }
        stuck_counter = 0;
        return true;
    }else {
        if(stuck_counter == 40){
            cout << "Big jump in timestamps trying to reinitialize" << endl;
            for(int i = 0; i < buffer.size(); i++){
                if(dataframes[i]->eof()) {
                    stop = true;
                    return false;
                }
                try{
                    getline(*dataframes[index], tmp);
                    if(tmp.empty()) {
                        stop = true;
                        return false;
                    }
                    tempCP = CurrencyPair(tmp, currency_pair_names[index]);
                    buffer[i] = tempCP;
                } catch (const exception &e){
                    stop = true;
                    return false;
                }
            }
            looked_into = vector<int>(3, 0);
            stuck_counter = 0;
        }
        if(++stuck_counter % 1000 == 0)
            printf("%9.5Lf %9.5Lf\n", old_timestamp, tempCP.getTimestamp());
    }
    return false;

}

/**
 * Checks if the given timestamps are within a minute from each other
 * @param t1
 * @param t2
 * @param t3
 * @return
 */
bool Arbitrage::closeTimestamps(double t1, double t2, double t3) {
    int seconds_difference = 60;
    if(t1 - seconds_difference < t2 && t2 < t1 + seconds_difference){
        if(t1 - seconds_difference < t3 && t3 < t1 + seconds_difference){
            if(t3 - seconds_difference < t2 && t2 < t3 + seconds_difference){
                return true;
            }
        }
    }
    return false;
}


/**
 * returns an index of an element with the oldest timestamp (min out of timestamps in the buffer)
 * @return
 */
int Arbitrage::getOldest(){
    vector<double> tmp;
    for(auto const& item: buffer){
        tmp.push_back(item.getTimestamp());
//        printf("%9.5Lf ", item.getTimestamp());
    }
//    cout << endl << std::min_element(tmp.begin(), tmp.end()) - tmp.begin() << endl;
    return std::min_element(tmp.begin(), tmp.end()) - tmp.begin();
}


