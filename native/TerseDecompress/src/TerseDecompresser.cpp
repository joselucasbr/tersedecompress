#include "TerseDecompresser.h"
#include "SpackDecompresser.h"
#include "NonSpackDecompresser.h"

// Constructor for TerseDecompresser
TerseDecompresser::TerseDecompresser(std::istream& instream, std::ostream& outstream, const TerseHeader& header)
    : stream(outstream), RecordLength(header.RecordLength), HostFlag(header.HostFlag),
      VariableFlag(header.RecfmV) {
    input = std::make_unique<TerseBlockReader>(instream);
    record.reserve(RecordLength);  // Reserve space for the record buffer
    lineseparator = {'\n'};        // Default line separator
}

// Factory method to create the appropriate decompressor
std::unique_ptr<TerseDecompresser> TerseDecompresser::create(std::istream& inputStream, std::ostream& outputStream) {
    TerseHeader header = TerseHeader::CheckHeader(inputStream);

    if (!header.SpackFlag) {
        return std::make_unique<NonSpackDecompresser>(inputStream, outputStream, header);
    } else {
        return std::make_unique<SpackDecompresser>(inputStream, outputStream, header);
    }
}

// endRecord writes the current record to the output and clears the buffer
void TerseDecompresser::endRecord() {
    if (VariableFlag && !TextFlag) {
        // Add a Record Descriptor Word (RDW)
        int recordLength = record.size() + 4;
        int rdw = recordLength << 16;
        stream.write(reinterpret_cast<const char*>(&rdw), sizeof(rdw));
    }

    // Write the record content to the output stream
    stream.write(record.data(), record.size());
    record.clear();

    // Add line separator if in text mode
    if (TextFlag) {
        stream.write(lineseparator.data(), lineseparator.size());
    }
}

// PutChar writes a single character to the current record
void TerseDecompresser::PutChar(int X) {
    if (X == 0) {
        if (HostFlag && TextFlag && VariableFlag) {
            endRecord();
        }
    } else {
        if (HostFlag && TextFlag) {
            if (VariableFlag) {
                if (X == Constants::RECORDMARK) {
                    endRecord();
                } else {
                    record.push_back(Constants::EbcToAsc[X - 1]);
                }
            } else {
                record.push_back(Constants::EbcToAsc[X - 1]);
                if (record.size() == static_cast<size_t>(RecordLength)) {
                    endRecord();
                }
            }
        } else {
            if (X == Constants::RECORDMARK) {
                if (VariableFlag) {
                    endRecord();
                }
            } else {
                record.push_back(X - 1);
            }
        }
    }
}

// Finalize output processing and ensure all data is written
void TerseDecompresser::close() {
    if (!record.empty() || (TextFlag && VariableFlag)) {
        endRecord();
    }
    stream.flush();  // Ensure all output is written
}
