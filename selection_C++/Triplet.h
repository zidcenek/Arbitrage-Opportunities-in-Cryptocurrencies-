//
// Created by zidce on 18.03.2020.
//

#ifndef UNTITLED_TRIPLET_H
#define UNTITLED_TRIPLET_H

#include <iostream>
#include <string>

using namespace std;

inline const string & DATA_PATH = "../data/input/test/";  // defines directory with data

/**
 * Stores the 3 related files and the output filename
 */
class Triplet {
public:
    Triplet(const string & f1, const string & f2, const string & f3, const string & date, const string & output_name);
    void shuffle();
    pair<string, string> split_filename(const string &filename);
    const string & getFile1() const;
    const string & getFile2() const;
    const string & getFile3() const;
    const string & getOutput_filename() const;
    const bool & getLinear() const;
private:
    string file1;
    string file2;
    string file3;
    string output_filename;
    bool linear;
};


#endif //UNTITLED_TRIPLET_H
