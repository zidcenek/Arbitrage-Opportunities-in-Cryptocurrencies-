//
// Created by zidce on 13.03.2020.
//
#include <string>

using namespace std;

const string & DATA_PATH = "../data/input/data1/";  // defines directory with data

/**
 * Stores the 3 related files and the output filename
 */
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