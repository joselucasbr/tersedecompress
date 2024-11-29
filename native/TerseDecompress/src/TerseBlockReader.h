#ifndef TERSEBLOCKREADER_H
#define TERSEBLOCKREADER_H

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include "Constants.h"

class TerseBlockReader {
private:
    std::istream& stream;
    int bitsAvailable = 0;
    int savedBits = 0;
    int red = 0;

public:
    explicit TerseBlockReader(std::istream& instream) : stream(instream) {}

    // Reads in 12 bits of data and returns it as the lowest 12 bits of an integer
    int GetBlok() {
        if (bitsAvailable == 0) {
            int byte1 = stream.get();
            if (byte1 == EOF) {
                return Constants::ENDOFFILE_BLOCK;
            }
            red++;
            int byte2 = stream.get();
            if (byte2 == EOF) {
                throw std::runtime_error("Tried to read 12 bits but found EOF after reading 8 bits.");
            }
            red++;
            // Save the last 4 bits of the second byte
            savedBits = byte2 & 0x0F;
            bitsAvailable = 4;

            return (byte1 << 4) | (byte2 >> 4);
        } else {
            if (bitsAvailable != 4) {
                // Should never happen; indicates an error in logic
                throw std::logic_error("Unexpected count of bits available");
            }

            int byte2 = stream.get();
            if (byte2 == EOF) {
                // Assume the remaining 4 bits are the last real data (EOF handling)
                return Constants::ENDOFFILE_BLOCK;
            }
            red++;
            bitsAvailable = 0;

            return (savedBits << 8) | byte2;
        }
    }

    // Closes the input stream
    void close() {
        // Note: std::istream does not need explicit closure, but you can manage it if using fstreams
        if (dynamic_cast<std::ifstream*>(&stream)) {
            dynamic_cast<std::ifstream*>(&stream)->close();
        }
    }

    // Destructor to ensure cleanup
    ~TerseBlockReader() {
        try {
            close();
        } catch (...) {
            // Suppress any exceptions during destruction
        }
    }
};

#endif // TERSEBLOCKREADER_H
