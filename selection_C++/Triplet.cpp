//
// Created by zidce on 18.03.2020.
//

#include "Triplet.h"

Triplet::Triplet(const string & f1, const string & f2, const string & f3, const string & date,
        const string & output_name, const string & data_path){
    currency1 = f1;
    currency2 = f2;
    currency3 = f3;
    output_directory_name = output_name + "/";
    linear = false;
    shuffle();
    output_filename = output_name + date;
    file1 = data_path + currency1 + date;
    file2 = data_path + currency2 + date;
    file3 = data_path + currency3 + date;
}
/**
 * shuffles currency1, currency2, currency3 to a desired output
 * => desired output AB, AC, CB (meaning AAABBB, AAACCC, CCCBBB)
 */
void Triplet::shuffle() {
    pair<string, string> pair1, pair2, pair3;
    string tmp;
    pair1 = split_currency(currency1);
    pair2 = split_currency(currency2);
    pair3 = split_currency(currency3);

    if(pair1.first == pair2.first){
        if(pair1.second == pair3.second) {
            return;  // AB, AC, CB
        }
        tmp = currency1;
        currency1 = currency2;
        currency2 = tmp;
        return;  // AC, AB, CB
    }
    if(pair1.first == pair3.first){
        if(pair1.second == pair2.second){
            tmp = currency3;
            currency3 = currency2;
            currency2 = currency3;
            return;  // AB, CB, AC
        }
        tmp = currency3;
        currency3 = currency2;
        currency2 = currency1;
        currency1 = tmp;
        return;  // AC, CB, AB
    }
    if(pair2.first == pair3.first){
        if(pair2.second == pair1.second){
            tmp = currency1;
            currency1 = currency2;
            currency2 = currency3;
            currency3 = tmp;
            return;  // CB, AB, AC
        }
        tmp = currency1;
        currency1 = currency3;
        currency3 = tmp;
        return;  // CB, AC, AB
    }
    linear = true;
}

pair<string, string> Triplet::split_currency(const string &filename) {
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

const string & Triplet::getCurrency1() const {
    return currency1;
}
const string & Triplet::getCurrency2() const {
    return currency2;
}
const string & Triplet::getCurrency3() const {
    return currency3;
}

const string & Triplet::getOutput_filename() const {
    return output_filename;
}

const bool & Triplet::getLinear() const{
    return linear;
}

const string &Triplet::getOutputDirectoryName() const {
    return output_directory_name;
}
