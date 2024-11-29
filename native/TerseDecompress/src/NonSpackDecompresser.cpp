#include "NonSpackDecompresser.h"

// Constructor for NonSpackDecompresser
NonSpackDecompresser::NonSpackDecompresser(std::istream& instream, std::ostream& outstream, const TerseHeader& header)
    : TerseDecompresser(instream, outstream, header) {}

// The decode function to perform decompression
void NonSpackDecompresser::decode() {
    // Define arrays to hold values for the decompression process
    std::vector<int> Father(Constants::TREESIZE, 0);
    std::vector<int> CharExt(Constants::TREESIZE, 0);
    std::vector<int> Backward(Constants::TREESIZE, 0);
    std::vector<int> Forward(Constants::TREESIZE, 0);

    int H1 = 0, H2 = 0;
    int x = 0, d = 0, y = 0, q = 0, r = 0, e = 0, p = 0, h = 0;

    // Initialize H2 to a default value
    H2 = 1 + Constants::AscToEbcDef[' '];

    // Initialize Father and CharExt arrays
    for (H1 = 258; H1 < Constants::TREESIZE; ++H1) {
        Father[H1] = H2;
        CharExt[H1] = 1 + Constants::AscToEbcDef[' '];
        H2 = H1;
    }

    // Initialize the Backward and Forward arrays
    for (H1 = 258; H1 < Constants::TREESIZE - 1; ++H1) {
        Backward[H1 + 1] = H1;
        Forward[H1] = H1 + 1;
    }

    Backward[0] = Constants::TREESIZE - 1;
    Forward[0] = 258;
    Backward[258] = 0;
    Forward[Constants::TREESIZE - 1] = 0;

    // Start reading blocks from the input stream
    x = 0;
    d = input->GetBlok();

    while (d != Constants::ENDOFFILE) {
        h = 0;
        y = Backward[0];
        q = Backward[y];
        Backward[0] = q;
        Forward[q] = 0;
        h = y;
        p = 0;

        // Main decompression loop
        while (d > 257) {
            q = Forward[d];
            r = Backward[d];
            Forward[r] = q;
            Backward[q] = r;
            Forward[d] = h;
            Backward[h] = d;
            h = d;
            e = Father[d];
            Father[d] = p;
            p = d;
            d = e;
        }

        q = Forward[0];
        Forward[y] = q;
        Backward[q] = y;
        Forward[0] = h;
        Backward[h] = 0;
        CharExt[x] = d;
        PutChar(d);
        x = y;

        // Write characters from Father array
        while (p != 0) {
            e = Father[p];
            PutChar(CharExt[p]);
            Father[p] = d;
            d = p;
            p = e;
        }

        Father[y] = d;
        d = input->GetBlok();
    }
}
