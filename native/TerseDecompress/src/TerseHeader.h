#ifndef TERSEHEADER_H
#define TERSEHEADER_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include "Constants.h"

class TerseHeader {
public:
    int VersionFlag = 0;
    int VariableFlag = 0;
    int RecordLen1 = 0;
    int Flags = 0;
    int Ratio = 0;
    int BlockSize = 0;
    int RecordLen2 = 0;

    int RecordLength = 0;

    bool RecfmV = false;
    bool TextFlag = true;
    bool HostFlag = true;
    bool SpackFlag = true;

    TerseHeader() = default;

    std::string toString() const {
        std::ostringstream oss;
        oss << "\nVersion flag is " << VersionFlag << "\n"
            << "Variable Flag is " << VariableFlag << "\n"
            << "RecordLen1 is " << RecordLen1 << "\n"
            << "Flags are " << Flags << "\n"
            << "Ratio is " << Ratio << "\n"
            << "Block Size is " << BlockSize << "\n"
            << "RecordLen2 is " << RecordLen2 << "\n";
        return oss.str();
    }

    static TerseHeader CheckHeader(std::istream& datastream) {
        TerseHeader header;

        header.VersionFlag = datastream.get();
        if (header.VersionFlag == EOF) {
            throw std::runtime_error("Unexpected EOF while reading version flag");
        }

        switch (header.VersionFlag) {
            case 0x01: // native binary mode, 4-byte header
            case 0x07:
                {
                    int byte2 = datastream.get();
                    int byte3 = datastream.get();
                    int byte4 = datastream.get();
                    header.RecordLen1 = static_cast<uint16_t>(datastream.get() << 8 | datastream.get());
                    
                    if (byte2 != 0x89 || byte3 != 0x69 || byte4 != 0xA5) {
                        throw std::runtime_error("Invalid header validation flags");
                    }
                    header.HostFlag = false;
                    header.TextFlag = false;
                }
                break;

            case 0x02: // host PACK compatibility mode, 12-byte header
            case 0x05: // host SPACK compatibility mode, 12-byte header
                {
                    header.VariableFlag = datastream.get();
                    header.RecordLen1 = static_cast<uint16_t>(datastream.get() << 8 | datastream.get());
                    header.Flags = datastream.get();
                    header.Ratio = datastream.get();
                    header.BlockSize = static_cast<uint16_t>(datastream.get() << 8 | datastream.get());
                    datastream.read(reinterpret_cast<char*>(&header.RecordLen2), sizeof(header.RecordLen2));

                    if (header.RecordLen2 < 0) {
                        throw std::runtime_error("Record length exceeds maximum value");
                    }

                    header.SpackFlag = (header.VersionFlag == 0x05);

                    if (header.VariableFlag != 0x00 && header.VariableFlag != 0x01) {
                        throw std::runtime_error("Record format flag not recognized");
                    }

                    if (header.RecordLen1 == 0 && header.RecordLen2 == 0) {
                        throw std::runtime_error("Record length is 0");
                    }

                    if (header.RecordLen1 != 0 && header.RecordLen2 != 0 &&
                        header.RecordLen1 != header.RecordLen2) {
                        throw std::runtime_error("Ambiguous record length");
                    }

                    header.RecordLength = header.RecordLen1 != 0 ? header.RecordLen1 : header.RecordLen2;
                    header.RecfmV = (header.VariableFlag == 0x01);

                    if ((header.Flags & Constants::FLAGMVS) == 0) {
                        if (header.Flags != 0 || header.Ratio != 0 || header.BlockSize != 0) {
                            throw std::runtime_error("Flags, Ratio, or BlockSize specified for non-MVS");
                        }
                    }

                    header.HostFlag = true;
                }
                break;

            default:
                throw std::runtime_error("Terse header version not recognized");
        }

        return header;
    }
};

#endif // TERSEHEADER_H
