#pragma once
#include "board.h"
#include <string>
#include <iostream>
#include <mutex>
template<> struct std::hash<std::array<std::array<int, 15>, 15>> {
    size_t operator()(const std::array<std::array<int, 15>, 15>& grid) const {
        size_t res = 0;
        for (auto& row : grid)
            for (int e : row)
                res ^= std::hash<int>{}(e)+0x9e3779b9 + (res << 6) + (res >> 2);
        return res;
    }
};
struct node;
struct nodeToNodeMove {
    std::pair<int, int> move;
    int scoreGain;
    node* nextNode;
    nodeToNodeMove(std::pair<int, int> m, int s, node* n) : move(m), scoreGain(s), nextNode(n) {}
    nodeToNodeMove() {}
};
struct node {
    int bestChildIndex = -1;
    std::vector<nodeToNodeMove> children;
    std::mutex scoreAddLock;
};
std::ostream& operator<< (std::ostream& os, const nodeToNodeMove& n);
void populateMap(board& b, node* n);
void populateMapWorker(board b, node* n, std::vector<std::pair<int, int>> move, int i);
int addScores(node* n);
std::array<std::array<int, 15>, 15> gridFromString(std::string str);