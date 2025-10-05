#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
using namespace std;

/*simple function: copy nbits from one int to another*/
int CopyBits(const int number, int nbits) {
    int c = 0;
    for (int j = 0; j < nbits; j++) {
        c = c | number & (1 << j);
    }
    return c;
}

class BitStream {
private:
    int stream = 0;//state of BitStream
    int lenstream = 0;//length of the BitStream
    int position = 0;//for reading bits from file
public:
    int WriteBitSequence(const int a, int len);
    int ReadBitSequence(const std::string& filename, int len);
    int getStream() {
        return stream;
    }
    // length of BitStream in bytes
    int getLenStream() {
        int byteLenght;
        double len = lenstream/8.0;
        byteLenght = ceil(len);
        return byteLenght;
    }
};

int BitStream::WriteBitSequence(const int a, int len) {
    if (stream == 0) {
        stream = CopyBits(a, len);
        lenstream = len;
    }
    else {
        stream = stream | (CopyBits(a, len) << lenstream);
        lenstream += len;
    }
    return stream;
}

int BitStream::ReadBitSequence(const std::string& filename, int len)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    vector<uint8_t> outputBytes(size);


    file.read(reinterpret_cast<char*>(outputBytes.data()), size);

    lenstream += len;
    //cout << "lenstream =" << std::dec << lenstream << ' ';
    if (getLenStream() > size) {
        std::cerr << "Error: unsufficient size of file." << endl;
        stream = 0;
        return 1;
    }

    int bytes = ceil(len / 8.0);


    int loc_position = 0;
    int loc_position_bits = 0;
    int position_bits = 0;
    int position_bytes = 0;
    int result = 0;


    while (loc_position < len) {
        position_bits = position % 8;
        loc_position_bits = loc_position % 8;
        position_bytes = position / 8;

        int bit = (outputBytes[position_bytes] >> position_bits) & 1;
        result = result | (bit << loc_position_bits);

        loc_position++;
        position++;
        if (loc_position % 8 == 0 && loc_position != len && loc_position != 0) {
            result = result << 8;
        }
    }

    stream = result;
    return stream;
}

int main() {
    BitStream A, A1;
    A.WriteBitSequence(481, 9);
    A.WriteBitSequence(238, 9);

    int res = A.getStream();


    ofstream output("D:\\test1.bin", std::ios::binary);

    if (output.is_open()) {
        output.write(reinterpret_cast<char*>(&res), A.getLenStream());
        output.close();
    }
    else {
        cerr << "Error opening file." << endl;
    }

    /*Reading BitStream from file*/
    BitStream B;
    B.ReadBitSequence("D:\\test1.bin", 11);
    cout << "First 11 bits from file: " << std::hex << B.getStream() << endl;
    B.ReadBitSequence("D:\\test1.bin", 7);
    cout << "Second 7 bits from file: " << std::hex << B.getStream() << endl;


    cin.get();
    return 0;
}
