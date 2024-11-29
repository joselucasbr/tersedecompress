#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include "NonSpackDecompresser.h"
#include "SpackDecompresser.h"
#include "TerseDecompresser.h"

/**
  Copyright Contributors to the TerseDecompress Project.
  SPDX-License-Identifier: Apache-2.0
**/
/*****************************************************************************/
/* Copyright 2018        IBM Corp.                                           */
/*                                                                           */
/*   Licensed under the Apache License, Version 2.0 (the "License");         */
/*   you may not use this file except in compliance with the License.        */
/*   You may obtain a copy of the License at                                 */
/*                                                                           */
/*     http://www.apache.org/licenses/LICENSE-2.0                            */
/*                                                                           */
/*   Unless required by applicable law or agreed to in writing, software     */
/*   distributed under the License is distributed on an "AS IS" BASIS,       */
/*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*/
/*   See the License for the specific language governing permissions and     */
/*   limitations under the License.                                          */
/*****************************************************************************/
/*                                                                           */
/*  For problems and requirements please create a GitHub issue               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  Author: Iain Lewis                          August 2004 (version 3)      */
/*                                                                           */
/*****************************************************************************/
/* Version 4 with editorial changes for publication as open source code      */
/*          Klaus Egeler, Boris Barth  (clientcenter@de.ibm.com)             */
/*****************************************************************************/
/* Version 5: support for variable length binary records                     */
/*          Andrew Rowley, Black Hill Software                               */
/*          Mario Bezzi, Watson Walker                                       */
/*****************************************************************************/


class TerseDecompress {
private:
    static constexpr const char* DetailedHelp =
        "Usage: \"TerseDecompress <input file> <output file> [-b]\"\n\n"
        "This program decompresses a file compressed using the terse program on z/OS.\n"
        "Default mode is text mode, which performs EBCDIC -> ASCII conversion.\n"
        "The -b flag enables binary mode, where no conversion is performed.\n";

    static constexpr const char* Version = "Version 6, Nov 2024";

    void printUsageAndExit() const {
        std::cout << DetailedHelp << "\n" << Version << std::endl;
        exit(0);
    }

    void process(const std::vector<std::string>& args) const {
        std::string inputFileName;
        std::string outputFileName;
        bool textMode = true;

        auto startTime = std::chrono::high_resolution_clock::now();

        if (args.empty()) {
            printUsageAndExit();
        }

        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "-h" || args[i] == "--help") {
                printUsageAndExit();
            } else if (args[i] == "-b") {
                textMode = false;
            } else if (inputFileName.empty()) {
                inputFileName = args[i];
            } else if (outputFileName.empty()) {
                outputFileName = args[i];
            } else {
                printUsageAndExit();
            }
        }

        if (inputFileName.empty() || outputFileName.empty()) {
            printUsageAndExit();
        }

        try {
            std::ifstream inputFile(inputFileName, std::ios::binary);
            if (!inputFile.is_open()) {
                throw std::runtime_error("Failed to open input file: " + inputFileName);
            }

            std::ofstream outputFile(outputFileName, std::ios::binary);
            if (!outputFile.is_open()) {
                throw std::runtime_error("Failed to open output file: " + outputFileName);
            }

            auto decompressor = TerseDecompresser::create(inputFile, outputFile);
            decompressor->TextFlag = textMode;

            std::cout << "Attempting to decompress input file (" << inputFileName
                      << ") to output file (" << outputFileName << ")" << std::endl;

            decompressor->decode();

            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            std::cout << "Time elapsed in milliseconds: " << elapsedTime << std::endl;
            std::cout << "Processing completed." << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

public:
    static void main(const std::vector<std::string>& args) {
        TerseDecompress decompressor;
        decompressor.process(args);
    }
};

// Entry point for the C++ program
int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    TerseDecompress::main(args);
    return 0;
}
