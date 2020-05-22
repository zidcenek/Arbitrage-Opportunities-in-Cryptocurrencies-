//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_TRIPLET_H
#define UNTITLED_TRIPLET_H

#include <iostream>
#include <string>

using namespace std;

//inline const string & data_path = "../data/input/data/";  // defines directory with data
//inline const string & data_path = "../../../../data/data_02-26=01/";  // defines directory with data



/**
 * Stores the 3 related files and the output filename
 */
class Triplet {
public:
    Triplet(const string & f1, const string & f2, const string & f3, const string & date, const string & output_name,
            const string & data_path);
    void shuffle();
    pair<string, string> splitCurrency(const string &filename);
    const string & getFile1() const;
    const string & getFile2() const;
    const string & getFile3() const;
    const string & getCurrency1() const;
    const string & getCurrency2() const;
    const string & getCurrency3() const;
    const string & getOutputFilename() const;
    const bool & getLinear() const;
private:
    string file1;
    string file2;
    string file3;
    string currency1;
    string currency2;
public:
    const string &getOutputDirectoryName() const;

private:
    string currency3;
    string output_filename;
    string output_directory_name;
    bool linear;
};


#endif //UNTITLED_TRIPLET_H
