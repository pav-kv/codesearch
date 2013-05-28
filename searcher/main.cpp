#include <base/types.h>
#include <util/code.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <trigram>\n";
        return 1;
    }

    string trigram;
    cin >> trigram;
    TTrigram tri = TByte(trigram[0]) | (TByte(trigram[1]) << 8) | (TByte(trigram[2]) << 16);

    ifstream idxInput("index.idx");
    ifstream datInput("index.dat");
    vector<char> idxBuffer(1 << 13);
    vector<char> datBuffer(1 << 13);
    idxInput.rdbuf()->pubsetbuf(&idxBuffer[0], idxBuffer.size());
    datInput.rdbuf()->pubsetbuf(&datBuffer[0], datBuffer.size());

    TDocId N = 0;
    Read(idxInput, N);
    idxInput.seekg(N * sizeof(TOffset), ios_base::cur);
    ifstream::pos_type pos = idxInput.tellg();
    idxInput.seekg(tri * sizeof(TOffset), ios_base::cur);
    TOffset offset, nextOffset;
    Read(idxInput, offset);
    Read(idxInput, nextOffset);
    if (offset > nextOffset) {
        cerr << "Error\n";
        return 0;
    }
    if (offset == nextOffset) {
        cerr << "Not found\n";
        return 0;
    }

    datInput.seekg(offset);
    TEncoder* encoder = new TSimpleEncoder();
    TPostingList list;
    encoder->Decode(datInput, list);

    ++list[0];
    idxInput.seekg(sizeof(TDocId));
    for (size_t i = 0; i < list.size(); ++i) {
        idxInput.seekg((list[i] - 1) * sizeof(TOffset), ios_base::cur);
        Read(idxInput, offset);
        datInput.seekg(offset);
        TOffset size = 0;
        Read(datInput, size);
        vector<char> str(size);
        datInput.read(&str[0], size);
        str.push_back(0);
        puts(&str[0]);
    }

    return 0;
}

