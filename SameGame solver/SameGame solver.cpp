#include "main.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <parallel_hashmap/phmap.h>

using std::vector; using std::pair; using std::array; using std::cout; using std::endl; using std::string;
using sq15 = array<array<int, 15>, 15>;

phmap::parallel_node_hash_map<sq15, node, phmap::Hash<sq15>, phmap::EqualTo<sq15>, std::allocator<std::pair<const sq15, node>>, 4, std::mutex> transTable;
//convert to flat hash map with pointers to nodes on heap rather than nodes as map values?
int main() {
    board b{};
    string input;
    std::getline(std::cin, input);
    b.grid = gridFromString(input);
    cout << '\n' << b << endl;
    std::ofstream fout("D:/Downloads/same game solver output.txt");
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            fout << b.grid[x][y] << ' ';
        }
        fout << '\n';
    }
    node root{};
    populateMap(b, &root);
    cout << "Table size: " << transTable.size() << '\n';
    cout << "Best score: " << root.addScores() << '\n';
    fout << "Best score: " << root.addScores() << '\n';
    node* nodePtr = &root;
    while (!nodePtr->children.empty()) {
        cout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
        fout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
        nodePtr = nodePtr->children[nodePtr->bestChildIndex].nextNode;
    }
    cout << endl;
    fout << endl;
    fout.close();
}
void populateMap(board& b, node* n) {
    auto posMoves = b.getConnectedList();
    for (auto& move : posMoves) {
        board bCopy = b;
        int gain = bCopy.makeMove(move);
        auto itrBoolPair = transTable.try_emplace(bCopy.grid, node());
        node* ptr = &itrBoolPair.first->second;
        n->children.emplace_back(move[0], gain, ptr);
        if (!itrBoolPair.second)
            continue;
        populateMap(bCopy, ptr);
    }
}
int node::addScores() {
    return 0;
    if (children.empty())
        return 0;
    for (int i = 0; i < children.size(); ++i) {
        if (!children[i].scoreAlreadyAddedTo) {
            children[i].scoreGain += children[i].nextNode->addScores();
            children[i].scoreAlreadyAddedTo = true;
        }
        if (children[i].scoreGain > children[bestChildIndex].scoreGain)
            bestChildIndex = i;
    }
    return children[bestChildIndex].scoreGain;
}
std::ostream& operator<<(std::ostream& os, const nodeToNodeMove& n) {
    os << n.move.first << ',' << n.move.second;
    return os;
}
sq15 gridFromString(std::string str) {
    sq15 res{};
    int index = 0;
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            while (str[index] < '0' || str[index] > '9')
                ++index;
            res[x][y] = str[index++] - '0' + 1;
        }
    }
    return res;
}