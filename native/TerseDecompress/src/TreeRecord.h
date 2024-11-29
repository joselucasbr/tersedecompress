#ifndef TREERECORD_H
#define TREERECORD_H

class TreeRecord {
public:
    int Left;
    int Right;
    int Back;
    int NextCount;

    TreeRecord() : Left(0), Right(0), Back(0), NextCount(0) {}
};

#endif // TREERECORD_H
