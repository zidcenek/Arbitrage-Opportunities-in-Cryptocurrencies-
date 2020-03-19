//
// Created by zidce on 18.03.2020.
//

#include "Triplet.h"

Triplet::Triplet(const string & f1, const string & f2, const string & f3, const string & date, const string & output_name){
    file1 = f1;
    file2 = f2;
    file3 = f3;
    linear = false;
    shuffle();
    output_filename = output_name + date;
//    cout << endl;
//    cout << output_filename << endl;
//    cout << file1 << " " << file2 << " " << file3 << endl;
//    cout << endl;
    file1 = DATA_PATH + file1 + date;
    file2 = DATA_PATH + file2 + date;
    file3 = DATA_PATH + file3 + date;
}
/**
 * shuffles file1, file2, file3 to a desired output
 * => desired output AB, AC, CB (meaning AAABBB, AAACCC, CCCBBB)
 */
void Triplet::shuffle() {
    pair<string, string> pair1, pair2, pair3;
    string tmp;
    pair1 = split_filename(file1);
    pair2 = split_filename(file2);
    pair3 = split_filename(file3);

    if(pair1.first == pair2.first){
        if(pair1.second == pair3.second) {
            cout << "case 1" << endl;
            return;  // AB, AC, CB
        }
        tmp = file1;
        file1 = file2;
        file2 = tmp;
        cout << "case 2" << endl;
        return;  // AC, AB, CB
    }
    if(pair1.first == pair3.first){
        if(pair1.second == pair2.second){
            tmp = file3;
            file3 = file2;
            file2 = file3;
            cout << "case 3" << endl;
            return;  // AB, CB, AC
        }
        tmp = file3;
        file3 = file2;
        file2 = file1;
        file1 = tmp;
        cout << "case 4" << endl;
        return;  // AC, CB, AB
    }
    if(pair2.first == pair3.first){
        if(pair2.second == pair1.second){
            tmp = file1;
            file1 = file2;
            file2 = file3;
            file3 = tmp;
            cout << "case 5" << endl;
            return;  // CB, AB, AC
        }
        tmp = file1;
        file1 = file3;
        file3 = tmp;
        cout << "case 6" << endl;
        return;  // CB, AC, AB
    }
    cout << "case 7 + 8" << endl;
    linear = true;
}

pair<string, string> Triplet::split_filename(const string &filename) {
    pair<string, string> tmp;
    tmp.first = filename.substr(0, 3);
    tmp.second = filename.substr(3, filename.size() - 3);
    return tmp;
}

const string & Triplet::getFile1() const {
    return file1;
}

const string & Triplet::getFile2() const {
    return file2;
}

const string & Triplet::getFile3() const {
    return file3;
}

const string & Triplet::getOutput_filename() const {
    return output_filename;
}

const bool & Triplet::getLinear() const{
    return linear;
}