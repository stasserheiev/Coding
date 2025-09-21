#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

std::vector<unsigned char> createVectorOfBytesFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Error: Failed to read file " << filename << std::endl;
        return {};
    }

    return buffer;
}

bool allSame(vector<unsigned char> vec) {
    bool allSame = true;

    if (!vec.empty()) {
        unsigned char first = vec[0];
        for (unsigned char b : vec) {
            if (b != first) {
                allSame = false;
                break;
            }
        }
    }
    else {
        allSame = true;
    }
    return allSame;
}

void RLE_coder(const std::string& infile, std::string outfile = "") {
    std::vector<unsigned char> bytes = createVectorOfBytesFromFile(infile);
    vector<unsigned char> temp_bytes = bytes;

    /*If result's filename is not specified then change the extension to ".rle"*/
    if (outfile.empty()) {
        fs::path inPath(infile);
        inPath.replace_extension(".rle");
        outfile = inPath.string();
    }


    /*RLE coding: find L for the first unique symbol*/
    vector<unsigned char> output;
    vector<unsigned char> output_unique;

    int n_unique = 0;
    int k = 0;



    while (!temp_bytes.empty()) {
        size_t len = temp_bytes.size();
        unsigned char i = temp_bytes[0];


        unsigned char l = 0;
        
        
        for (int j = 0; j <= len -1 ; j++) {
            unsigned char temp = temp_bytes[j];
            if (temp == i && allSame(temp_bytes) == 0) {
                l++;
                k = 1;
                //n_unique = 0;
            }
            else if (temp != i && allSame(temp_bytes) == 0) {
                k = j;
                break;
            }
            else if (allSame(temp_bytes) == 1) {
                k = len;
            }

        }

        unsigned char byte_ch = static_cast<unsigned char>(temp_bytes[0]);

        //if k=1 (only one unique symbol)
        if (k == 1) {
            n_unique++;
            output_unique.push_back(byte_ch);
        }
        else {
            n_unique = 0;
        }

        unsigned char L;
        if (n_unique == 0) L = 128 + l - 2;
        else if (1 <= n_unique <= 128) L = n_unique - 1;

        output.push_back(L);
        
        if (n_unique == 0) {
            output.push_back(byte_ch);
            output_unique.clear();
        }
        else if (1 <= n_unique <= 128) {
            if(n_unique == 1) output.insert(output.end(), output_unique.begin(), output_unique.end());
            else {
                output.resize(output.size()-(n_unique+1));
                output.push_back(L);
                output.insert(output.end(), output_unique.begin(), output_unique.end());
            }
            if (n_unique == 128) {
                n_unique = 0;
                output_unique.clear();
            }
        }

        if (k != 0) temp_bytes.erase(temp_bytes.begin(), temp_bytes.begin() + k); // if more than one same symbols
        
        if (k > 1) n_unique = 0;

    }
    cout << "Coded bytes: " << endl;
    for (unsigned char out : output) {
        cout << static_cast<char>(out);
    }

    /*Writing coded data in the output file*/
    ofstream res(outfile, std::ios::binary);

    if (res.is_open()) {
        res.write(reinterpret_cast<const char*>(output.data()), output.size());
        res.close();
    }
    else {
        cerr << "Error opening file.";
    }

    cout << "Coding is ended." << endl;
}

void RLE_decoder(const std::string& infile, const std::string& outfile) {
    std::vector<unsigned char> bytes = createVectorOfBytesFromFile(infile);

    vector<unsigned char> temp_bytes = bytes;
    vector<unsigned char> output;
    vector<unsigned char> output_unique;

    while (!temp_bytes.empty()) {
        unsigned char byte_ch = static_cast<unsigned char>(temp_bytes[0]);
        unsigned char b;

        unsigned char l = 0;
        if (byte_ch >= 128) {
            if (temp_bytes.size() < 2) {
                cerr << "Error: not enough bytes for decoding" << endl;
                break;
            }
            l = byte_ch - 128 + 2;
            b = static_cast<unsigned char>(temp_bytes[1]);
        }
        else if (0 <= byte_ch <= 127) {
            
            l = byte_ch + 1;
            if (temp_bytes.size() < l+1) {
                cerr << "Error: not enough bytes for decoding" << endl;
                break;
            }
            output_unique.insert(output_unique.end(), temp_bytes.begin() + 1, temp_bytes.begin() + 1 + l);
        }

        if (byte_ch >= 128) {
            output.insert(output.end(), l, b);
        }
        else if (0 <= byte_ch <= 127) {
            output.insert(output.end(), output_unique.begin(), output_unique.end());
            output_unique.clear();
        }


        if (byte_ch >= 128) temp_bytes.erase(temp_bytes.begin(), temp_bytes.begin() + 2);
        else if (0 <= byte_ch <= 127) temp_bytes.erase(temp_bytes.begin(), temp_bytes.begin() + l + 1);
        else break;
    }

    ofstream decod(outfile, std::ios::binary);

    if (decod.is_open()) {
        decod.write(reinterpret_cast<const char*>(output.data()), output.size());
        decod.close();
    }
    else {
        cerr << "Error opening file.";
    }

    cout << "Decoding is ended." << endl;
}
