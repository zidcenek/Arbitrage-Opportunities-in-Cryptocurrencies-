//
// Created by zidce on 13.03.2020.
//

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Triplet.cpp"
#include "CurrencyPair.cpp"

using namespace std;

const string OUTPUT_DIRECTORY = "../output/";  // defines output directory
int counter = 0;

class Arbitrage{
public:
    Arbitrage(){
        stop = false;
    }

    /**
     * Initializes the whole class with given parameters
     * @param triplet - Triplet class containing the files
     * @return - true or false based on the initialization status
     */
    bool initialize(const Triplet & triplet){
        openFile(triplet.getFile1());
        openFile(triplet.getFile2());
        openFile(triplet.getFile3());
        output_name = triplet.getOutput_filename();
        for(auto &df: dataframes){
            string tmp;
            getline(*df, tmp);
            current.emplace_back(CurrencyPair(tmp));
        }
        return true;
    }

    /**
     * the main cycle that goes through the files and tries to find arbitrages
     */
    void run(){
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
                ofs << coma << "{score: " << score << ",";
                ofs << "pairs: [";
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
    long double detection(){
        long double percentage = current[1].getSupply()[0] / current[0].getSupply()[0] / current[2].getSupply()[0];
        return percentage;
    }


protected:
    bool openFile(string const& filename){
        ifstream *fin = new ifstream("../data/" + filename);
        if (! fin->is_open() ){
            cout << "File could not be opened" << endl;
            return false;
        }
        dataframes.push_back(fin);
        return true;
    }

    void getNext(int index){
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
            current[index] = CurrencyPair(tmp);
        } catch(const exception& e) {
            cout << "wrong line no." << counter++ << endl;
        }

    }

    int getOldest(){
        vector<double> tmp;
        for(auto const& item: current){
            tmp.push_back(item.getTimestamp());
        }
        return std::min_element(tmp.begin(), tmp.end()) - tmp.begin();
    }


private:
    vector<ifstream*> dataframes;
    vector<CurrencyPair> current;
    bool stop;
    string output_name;
};