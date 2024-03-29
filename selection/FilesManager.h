//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_FILESMANAGER_H
#define UNTITLED_FILESMANAGER_H

#include "Triplet.h"
#include <string>
#include <vector>
#include <glob.h>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

class FilesManager{
public:

    FilesManager(const string & path);

    vector<vector<Triplet> > selectFiles();
private:
    vector<Triplet> selectTriples(const vector<string> &filenames) const;
    bool isSubset(const pair<string, string> &set1, const vector<string> &set2) const;
    vector<string> selectFilesByDate();
    static vector<string> getAllFilesInDirectory(const string &path);
    void makeCombiUtil(vector<vector<string> >& ans,
                       vector<string>& tmp, int n, int left, int k, vector<string> arr);
    vector<vector<string> > makeCombi(const vector<string> & arr, int k);
protected:
    static const int date_size = 11;
    const vector<string> currencies = {"USDT", "BTC", "LTC", "ETH", "XRP", "BCH", "EOS", "BNB", "TRX", "XMR"};
    vector<vector<string> > currencies_combinations;
    vector<string> files;
    string current_date;
    string data_path;
};

#endif //UNTITLED_FILESMANAGER_H
