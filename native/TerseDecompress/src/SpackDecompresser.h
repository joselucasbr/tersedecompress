#ifndef SPACKDECOMPRESSER_H
#define SPACKDECOMPRESSER_H

#include "TerseDecompresser.h"
#include "TreeRecord.h"

class SpackDecompresser : public TerseDecompresser {
private:
    int node;              // Current tree node
    int TreeAvail;         // Next available tree node
    std::vector<TreeRecord> Tree; // Tree structure for decompression

    // Stack for traversal during decoding
    struct StackType {
        int Head;          // Stack head pointer
        std::vector<int> Data;

        StackType() : Data(Constants::TREESIZE + 1, 0) {}
    } Stack;

    // Outputs characters based on tree traversal
    void PutChars(int X);

    // Initializes the tree structure
    void TreeInit();

    // Gets the next available tree node
    int GetTreeNode();

    // Increments reference count for a tree node
    void BumpRef(int bref);

    // Removes the least recently used node
    void LruKill();

    // Deletes a reference to a tree node
    void DeleteRef(int dref);

    // Adds a node to the least recently used list
    void LruAdd(int lru_next);

public:
    // Constructor
    SpackDecompresser(std::istream& instream, std::ostream& outstream, const TerseHeader& header);

    // Decode function for SPACK decompression
    void decode() override;
};

#endif // SPACKDECOMPRESSER_H
