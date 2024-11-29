#ifndef TERSEDECOMPRESSER_H
#define TERSEDECOMPRESSER_H

#include <iostream>
#include <vector>
#include <memory>
#include "TerseBlockReader.h"
#include "TerseHeader.h"
#include "Constants.h"

// Forward declarations to avoid circular dependency
class SpackDecompresser;
class NonSpackDecompresser;

class TerseDecompresser {
protected:
    std::unique_ptr<TerseBlockReader> input;  // Handles input stream reading
    std::vector<char> record;                // Buffer for output records
    std::ostream& stream;                    // Output stream

    bool HostFlag = false;                   // Host compatibility flag

    bool VariableFlag = false;               // Variable record format flag
    int RecordLength = 0;                    // Host perspective record length

    std::vector<char> lineseparator;         // Line separator for text mode

    // Writes the current record to the output and clears the buffer
    void endRecord();

    // Writes a character to the output record
    void PutChar(int X);

public:
    // Constructor
    TerseDecompresser(std::istream& instream, std::ostream& outstream, const TerseHeader& header);

    // Virtual destructor
    virtual ~TerseDecompresser() = default;

    // Pure virtual method for decoding, to be implemented by derived classes
    virtual void decode() = 0;

    bool TextFlag = false;                   // Text mode flag

    // Factory method to create the appropriate decompressor
    static std::unique_ptr<TerseDecompresser> create(std::istream& inputStream, std::ostream& outputStream);

    // Finalize output processing
    void close();
};

#endif // TERSEDECOMPRESSER_H
