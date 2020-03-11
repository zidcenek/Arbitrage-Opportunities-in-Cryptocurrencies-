#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <experimental/filesystem>
#include <glob.h>

using namespace std;
namespace fs = std::experimental::filesystem;

const string DATA_PATH = "../data/input/data1/";
//const string DATA_PATH = "../data/";
const string OUTPUT_DIRECTORY = "../output/";
int counter = 0;
struct Triplet{
public:
    Triplet(const string & f1, const string & f2, const string & f3, const string & date, const string & output_name){
        file1 = DATA_PATH + f1 + date;
        file2 = DATA_PATH + f2 + date;
        file3 = DATA_PATH + f3 + date;
        output_filename = output_name + date;
    }

    const string &getFile1() const {
        return file1;
    }

    const string &getFile2() const {
        return file2;
    }

    const string &getFile3() const {
        return file3;
    }

    const string &getOutput_filename() const {
        return output_filename;
    }

private:
    string file1;
    string file2;
    string file3;
    string output_filename;
};


class FilesManager{
public:
    FilesManager(){
        files = get_all_files_in_directory(DATA_PATH);
        currencies_combinations = makeCombi(currencies, 3);
    }
    vector<vector<Triplet> > select_files() {
        vector<vector<Triplet> > result = vector<vector<Triplet> >();
        vector <string> selected;
        while( !(selected = select_files_by_date()).empty()){
            result.emplace_back(select_triples(selected));
        }
        return result;
    }
private:
    vector<Triplet> select_triples(const vector<string> & filenames) const {
        vector<Triplet> triplets = vector<Triplet>();
        vector<pair<string, string> > decomposed = vector<pair<string, string> >();
        for(auto & file: filenames){
            // decomposes filename to the 2 currencies
            pair<string, string> currency_pair;
            if(file.size() - DATA_PATH.size() == 17){
                currency_pair.first = file.substr(DATA_PATH.size(), 3);
                currency_pair.second = file.substr(DATA_PATH.size() + 3, 3);
            } else if (file.size() - DATA_PATH.size() == 18){
                currency_pair.first = file.substr(DATA_PATH.size(), 3);
                currency_pair.second = file.substr(DATA_PATH.size() + 3, 4); // for USDT
            }
            decomposed.push_back(currency_pair);
        }
        // is subset
        for(const auto & comb: currencies_combinations){
            vector <pair<string, string> > res;
            for(const auto & dec: decomposed){
                if(is_subset(dec, comb))
                    res.emplace_back(dec);
            }
            if(res.size() == 3){
                triplets.emplace_back(Triplet(res[0].first + res[0].second, res[1].first + res[1].second
                        , res[2].first + res[2].second, current_date, comb[0] + comb[1] + comb[2]));
            }
        }
        return triplets;

    }

    bool is_subset(const pair<string, string> & set1, const vector<string> & set2) const {
        int counter = 0;
        for(const auto & s2: set2){
            if(s2 == set1.first || s2 == set1.second)
                counter ++;
        }
        return counter == 2;
    }

    vector<string> select_files_by_date(){
        vector<string> filtered_files;
        vector<string> selected_files;
        vector<string> unselected_files;
        copy_if (files.begin(), files.end(), std::back_inserter(filtered_files),
                 [](string s){return s.size() - DATA_PATH.size() == 17 || s.size() - DATA_PATH.size() == 18 ;} );

        if(filtered_files.empty())
            return vector<string>();
        const string date = filtered_files[0].substr(filtered_files[0].size() - date_size, date_size);
        current_date = date;
        for(auto const & file: filtered_files){
            string substr = file.substr(file.size() - date_size, date_size);
            if(date == substr){
                selected_files.emplace_back(file);
            } else {
                unselected_files.emplace_back(file);
            }
        }
        files = unselected_files;
        return selected_files;
    }

    static vector<string> get_all_files_in_directory(const string & path){
        vector<string> filenames;
        glob_t glob_result;
        glob((path + "*").c_str(),GLOB_TILDE,NULL,&glob_result);
        for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
            filenames.emplace_back(glob_result.gl_pathv[i]);
        }
        return filenames;
    }


    void makeCombiUtil(vector<vector<string> >& ans,
                       vector<string>& tmp, int n, int left, int k, vector<string> arr)
    {
        if (k == 0) {
            ans.push_back(tmp);
            return;
        }
        for (int i = left; i <= n; ++i)
        {
            tmp.emplace_back(arr[i-1].c_str());
            makeCombiUtil(ans, tmp, n, i + 1, k - 1, arr);
            tmp.pop_back();
        }
    }
    vector<vector<string> > makeCombi(vector<string> arr, int k)
    {
        vector<vector<string> > ans;
        vector<string> tmp;
        makeCombiUtil(ans, tmp, arr.size(), 1, k, arr);
        return ans;
    }

protected:
    static const int date_size = 11;
    const vector<string> currencies = {"USDT", "BTC", "LTC", "ETH", "XRP", "BCH", "EOS", "BNB", "TRX", "XMR"};
    vector<vector<string> > currencies_combinations;
    vector<string> files;
    string current_date;
};


class CurrencyPair{
public:
    explicit CurrencyPair(const string & input){
        string tmp;
        row = input;
        stringstream s(input);
        std::getline(s, tmp, ';');
        trade_id = tmp;
        std::getline(s, tmp, ';');
        demand = parse_demand(stringstream(tmp));
        std::getline(s, tmp, ';');
        supply = parse_demand(stringstream(tmp));
        std::getline(s, tmp, ';');
        timestamp = stold(tmp);
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
        json.append("id:" + trade_id + ",");
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
    string trade_id;
    vector<double> supply;
    vector<double> demand;
    long double timestamp;
    string currency;
};

class Arbitrage{
public:
    Arbitrage(){
        stop = false;
    }

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
    static const int NUMBER_OF_CURRENCIES = 3;
};


int main() {
    FilesManager fm = FilesManager();
    vector<vector<Triplet> > triplets = fm.select_files();

    int a = 10;
    for(const auto & vec: triplets){
        for(const Triplet & triplet: vec){
            if(a){
                a--;
                continue;
            }
            cout << triplet.getOutput_filename() << endl;
            cout << triplet.getFile1() << " " << triplet.getFile2() << " " << triplet.getFile3() << " " << endl;
            Arbitrage arbitrage = Arbitrage();
            arbitrage.initialize(triplet);
            arbitrage.run();
        }
    }
    return 0;
}