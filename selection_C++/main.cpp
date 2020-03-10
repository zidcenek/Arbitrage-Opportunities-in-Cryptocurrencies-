#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

class FilesReader{

};


class CurrencyPair{
public:
    explicit CurrencyPair(const string & input){
        string tmp;
        row = input;
        stringstream s(input);
        std::getline(s, tmp, ';');
        trade_id = stoi(tmp);
        std::getline(s, tmp, ';');
        demand = parse_demand(stringstream(tmp));
        std::getline(s, tmp, ';');
        supply = parse_demand(stringstream(tmp));
        std::getline(s, tmp, ';');
//        cout << "------" << tmp << endl;
        timestamp = stold(tmp);
        score = 1.1;
        currency = "BNB";
    }

    const vector<double> &getSupply() const {
        return supply;
    }

    const vector<double> &getDemand() const {
        return demand;
    }

    double getTimestamp() const {
        return timestamp;
    }

    string to_JSON() const {
        string json = "{";
        json.append("id:" + to_string(trade_id) + ",");
        json.append("demand: " + array_to_string(demand) + ",");
        json.append("supply: " + array_to_string(supply) + ",");
        json.append("timestamp:" + to_string(timestamp) + ",");
        json.append("currency:'" + currency + "'");
        json.append("}");
        return json;
    }
    string array_to_string(const vector<double> & arr) const {
        string output = "[";
        for(int i = 0; i < arr.size() - 1; i += 2){
            if(i)
                output.append(", ");
            output.append("[" + to_string(arr[i]) + "," + to_string(arr[i+1]) + "]");
        }
        output.append("]");
        return output;
    }

protected:
    static vector<double> parse_demand(stringstream my_steam){
        vector<double> output;
        string word;
        vector<string> row;
        while (std::getline(my_steam, word, '\'')) {
            row.push_back(word);
        }
        for(int i = 0; i < row.size() ; i++ ){
            if(i % 2 == 0)
                continue;
            output.push_back(stod(row[i]));
        }
        return output;
    }
private:
    string row;
    int trade_id;
    vector<double> supply;
    vector<double> demand;
    long double timestamp;
    string currency;
    long double score;
};

class Arbitrage{
public:
    Arbitrage(){
        stop = false;
    }
    bool initialize(vector<string> const& filenames){
        if(filenames.size() != NUMBER_OF_CURRENCIES)
            return false;
        for(auto const&fname : filenames){
            openFile(fname);
        }
        for(auto &df: dataframes){
            string tmp;
            getline(*df, tmp);
            current.emplace_back(CurrencyPair(tmp));
        }
        return true;
    }

    void run(){
        string coma;
        ofstream ofs("../output/out.json");
        if(!ofs.good()){
            cout << "Could not open output file" << endl;
            return;
        }
        ofs << "[";
        while(! stop){
            int index = getOldest();
            getNext(index);
            if(detection()){
                long double score = 1.1;
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
    }

    bool detection(){
        long double percentage = current[1].getSupply()[0] / current[0].getSupply()[0] / current[2].getSupply()[0];
        if(percentage > 1){
            cout << percentage << endl;
            return true;
        }
        return false;
    }

protected:
    bool openFile(string const& filename){
        ifstream *fin = new ifstream("../data/" + filename);
        if (! fin->is_open() ){
            cout << "file could not be opened" << endl;
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
        current[index] = CurrencyPair(tmp);
    }

    int getOldest(){
        vector<double> tmp;
        for(auto const& item: current){
//            printf("%f\n", item.getTimestamp());
            tmp.push_back(item.getTimestamp());
        }
        return std::min_element(tmp.begin(), tmp.end()) - tmp.begin();
    }


private:
    FilesReader files_reader;
    vector<string> dataframes2;
    vector<ifstream*> dataframes;
    vector<CurrencyPair> current;
    bool stop;
    vector<string> output;
    string output_name;
    static const int NUMBER_OF_CURRENCIES = 3;
};

vector<double> parse_demand(stringstream demand){
    vector<double> output;
    string word;
    vector<string> row;
    while (std::getline(demand, word, '\'')) {
        row.push_back(word);
    }
    for(int i = 0; i < row.size() ; i++ ){
        if(i % 2 == 0)
            continue;
        output.push_back(stoi(row[i]));
    }
    return output;
}


int main() {
    ifstream fin("../pokus");
    vector<string> filenames;
    filenames.emplace_back("../data/BCHBNB-2020-02-26");
    filenames.emplace_back("../data/BCHBTC-2020-02-26");
    filenames.emplace_back("../data/BNBBTC-2020-02-26");
    Arbitrage arbitrage = Arbitrage();
    arbitrage.initialize(filenames);
    arbitrage.run();

    return 0;
}