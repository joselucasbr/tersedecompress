#include "SpackDecompresser.h"

// Constructor for SpackDecompresser
SpackDecompresser::SpackDecompresser(std::istream& instream, std::ostream& outstream, const TerseHeader& header)
    : TerseDecompresser(instream, outstream, header) {
    Tree.resize(Constants::TREESIZE + 1);
}

// PutChars handles the traversal and decoding of characters based on the tree structure
void SpackDecompresser::PutChars(int X) {
    Stack.Head = 0;

    while (true) {
        // Traverse the tree while the node value is larger than the CODESIZE
        while (X > Constants::CODESIZE) {
            Stack.Head++;
            Stack.Data[Stack.Head] = Tree[X].Right;
            X = Tree[X].Left;
        }
        PutChar(X);

        // If the stack has more elements, continue the process
        if (Stack.Head > 0) {
            X = Stack.Data[Stack.Head];
            Stack.Head--;
        } else {
            break;
        }
    }
}

// TreeInit initializes the tree nodes
void SpackDecompresser::TreeInit() {
    Tree.resize(Constants::TREESIZE + 1);

    // Initialize all tree nodes
    for (int i = 0; i < Constants::TREESIZE; ++i) {
        Tree[i].Left = Constants::NONE;
        Tree[i].Right = Constants::NONE;
        Tree[i].NextCount = i + 1;
    }
    Tree[Constants::TREESIZE].NextCount = Constants::NONE;

    // Initialize base and code size nodes
    int init_index = Constants::BASE;
    while (init_index <= Constants::CODESIZE) {
        Tree[init_index].Right = init_index++;
    }

    // Set references for tree traversal
    Tree[Constants::BASE].NextCount = Constants::BASE;
    Tree[Constants::BASE].Back = Constants::BASE;

    // Initialize other tree nodes
    for (init_index = Constants::CODESIZE + 1; init_index < Constants::TREESIZE; ++init_index) {
        Tree[init_index].NextCount = init_index + 1;
    }
    TreeAvail = Constants::CODESIZE + 1;
}

// GetTreeNode retrieves the next available tree node for use
int SpackDecompresser::GetTreeNode() {
    node = TreeAvail;
    TreeAvail = Tree[node].NextCount;
    return node;
}

// BumpRef increments the reference count for a node and adjusts pointers accordingly
void SpackDecompresser::BumpRef(int bref) {
    if (Tree[bref].NextCount < 0) {
        Tree[bref].NextCount--;
    } else {
        int forwards = Tree[bref].NextCount;
        int prev = Tree[bref].Back;
        Tree[prev].NextCount = forwards;
        Tree[forwards].Back = prev;
        Tree[bref].NextCount = -1;
    }
}

// LruKill removes the least recently used node from the tree
void SpackDecompresser::LruKill() {
    int lru_p = Tree[0].NextCount;
    int lru_q = Tree[lru_p].NextCount;
    int lru_r = Tree[lru_p].Back;
    Tree[lru_q].Back = lru_r;
    Tree[lru_r].NextCount = lru_q;
    DeleteRef(Tree[lru_p].Left);
    DeleteRef(Tree[lru_p].Right);
    Tree[lru_p].NextCount = TreeAvail;
    TreeAvail = lru_p;
}

// DeleteRef deletes a reference to a tree node
void SpackDecompresser::DeleteRef(int dref) {
    if (Tree[dref].NextCount == -1) {
        LruAdd(dref);
    } else {
        Tree[dref].NextCount++;
    }
}

// LruAdd adds a node to the least recently used list
void SpackDecompresser::LruAdd(int lru_next) {
    int lru_back = Tree[Constants::BASE].Back;
    Tree[lru_next].NextCount = Constants::BASE;
    Tree[Constants::BASE].Back = lru_next;
    Tree[lru_next].Back = lru_back;
    Tree[lru_back].NextCount = lru_next;
}

// The decode function to perform decompression using the SPACK algorithm
void SpackDecompresser::decode() {
    TreeInit();
    Tree[Constants::TREESIZE - 1].NextCount = Constants::NONE;

    int H = input->GetBlok();
    if (H != Constants::ENDOFFILE) {
        PutChars(H);

        int G = input->GetBlok();
        while (G != Constants::ENDOFFILE) {
            if (TreeAvail == Constants::NONE) {
                LruKill();
            }

            PutChars(G);
            int N = GetTreeNode();
            Tree[N].Left = H;
            Tree[N].Right = G;
            BumpRef(H);
            BumpRef(G);
            LruAdd(N);

            H = G;
            G = input->GetBlok();
        }
    }
}
