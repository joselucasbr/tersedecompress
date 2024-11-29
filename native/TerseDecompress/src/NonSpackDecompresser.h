#ifndef NONSPACKDECOMPRESSER_H
#define NONSPACKDECOMPRESSER_H

#include "TerseDecompresser.h"

class NonSpackDecompresser : public TerseDecompresser {
public:
    // Constructor that initializes the decompressor with input stream, output stream, and header
    NonSpackDecompresser(std::istream& instream, std::ostream& outstream, const TerseHeader& header);

    // Override the decode method for NonSpack decompression
    void decode() override;
};

#endif // NONSPACKDECOMPRESSER_H