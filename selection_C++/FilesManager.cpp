//
// Created by zidce on 18.03.2020.
//

#include "FilesManager.h"

/**
 * Constructs itself and initializes its parameters
 * @param path
 */
FilesManager::FilesManager(const string & path){
    files = get_all_files_in_directory(path);
    currencies_combinations = makeCombi(currencies, 3);
}
/**
 * Agregates all of the found files into appropriate triplets
 * @return - 2D array of Triplets (filenames)
 */
vector<vector<Triplet> > FilesManager::select_files() {
    vector<vector<Triplet> > result = vector<vector<Triplet> >();
    vector <string> selected;
    while( !(selected = select_files_by_date()).empty()){
        result.emplace_back(select_triples(selected));
    }
    return result;
}

/**
 * Goes through all the filenames and matches together the appropriate ones into triplet
 * @param filenames - filenames are expected to be of the same date
 * @return - vector of triplets
 */
vector<Triplet> FilesManager::select_triples(const vector<string> & filenames) const {
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

/**
 * Checks wheter the pair (set1) is contained in the vector (set2)
 * @param set1
 * @param set2
 * @return - true if pair is subset or vector
 */
bool FilesManager::is_subset(const pair<string, string> & set1, const vector<string> & set2) const {
    int counter = 0;
    for(const auto & s2: set2){
        if(s2 == set1.first || s2 == set1.second)
            counter ++;
    }
    return counter == 2;
}

/**
 * Goes through all the files and returns only those with the same date at the end (eg. 2020-02-28)
 * @return - vector of filenames with the same date
 */
vector<string> FilesManager::select_files_by_date(){
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
/**
 * Selects all the files in the given directory
 * @param path
 * @return
 */
vector<string> FilesManager::get_all_files_in_directory(const string & path){
    vector<string> filenames;
    glob_t glob_result;
    glob((path + "*").c_str(),GLOB_TILDE, nullptr,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        filenames.emplace_back(glob_result.gl_pathv[i]);
    }
    return filenames;
}

/**
 * Recursive function helping the makeCombiUtil function iterate over all the possibilities
 * @param ans
 * @param tmp
 * @param n
 * @param left
 * @param k
 * @param arr
 */
void FilesManager::makeCombiUtil(vector<vector<string> >& ans,
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
/**
 * Creates all possible combinations (length k) (without repetition) of the given array
 * @param arr
 * @param k
 * @return
 */
vector<vector<string> > FilesManager::makeCombi(const vector<string> & arr, int k)
{
    vector<vector<string> > ans;
    vector<string> tmp;
    makeCombiUtil(ans, tmp, arr.size(), 1, k, arr);
    return ans;
}