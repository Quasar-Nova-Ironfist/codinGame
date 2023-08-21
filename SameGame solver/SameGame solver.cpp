#include "main.h"
#include <fstream>
#include <thread>
#include <parallel_hashmap/phmap.h>
#define BS_THREAD_POOL_DISALBE_ERROR_FORWARDING
#define BS_THREAD_POOL_DISABLE_PAUSE
#include <BS_thread_pool.hpp>
#include <Windows.h>

using std::vector; using std::pair; using std::array; using std::cout; using std::endl; using std::string;
using sq15 = array<array<int, 15>, 15>;

phmap::parallel_flat_hash_map <sq15, node*, phmap::Hash<sq15>, phmap::EqualTo<sq15>, std::allocator<std::pair<const sq15, node*>>, 4, std::mutex> transTable;
BS::thread_pool threadPool;

int main() {
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
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
    //cout << "Best score: " << root.addScores() << '\n';
    //fout << "Best score: " << root.addScores() << '\n';
    node* nodePtr = &root;
    while (!nodePtr->children.empty()) {
        cout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
        fout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
        nodePtr = nodePtr->children[nodePtr->bestChildIndex].nextNode;
    }
    cout << endl;
    fout << endl;
    fout.close();
    for (auto& pair : transTable)
		delete pair.second;
}
void populateMap(board& b, node* n) {
    vector<vector<pair<int, int>>> posMoves = b.getConnectedList();
    n->children.resize(posMoves.size());
    vector<std::future<void>> futures;
    futures.reserve(posMoves.size());
    for (int i = 0; i < posMoves.size(); ++i)
        futures.push_back(threadPool.submit_if_available_else_invoke([&b, n, &posMoves, i]() {//TODO finish work on submit_if_available_else_invoke
            board bCopy = b;
            int gain = bCopy.makeMove(posMoves[i]);
            node* nodePtr = new node;
            auto itrBoolPair = transTable.try_emplace(bCopy.grid, nodePtr);
            n->children[i] = { posMoves[i][0], gain, itrBoolPair.first->second };
            if (itrBoolPair.second)
                populateMap(bCopy, itrBoolPair.first->second);
            else
                delete nodePtr;
        }));
    for (std::future<void>& future : futures)
		future.wait();
}
int node::addScores() {//get rid of bestChildIndex, thread split at addScores to do away with the need for the atomic?
    if (children.empty())
        return 0;
    if (bestChildIndex != -1)
        return children[bestChildIndex].scoreGain;
    bestChildIndex = 0;
    for (int i = 0; i < children.size(); ++i) {
        if (children[i].scoreNotYetAddedTo.exchange(false, std::memory_order_acq_rel))//may need to be the default seq_cst
            children[i].scoreGain += children[i].nextNode->addScores();
        if (children[i].scoreGain > children[bestChildIndex].scoreGain)
            bestChildIndex = i;
    }
    return children[bestChildIndex].scoreGain;
}
int addScores(node* n) {
    if (n->children.empty())
        return 0;
    if (n->bestChildIndex != -1)
		return n->children[n->bestChildIndex].scoreGain;
    n->bestChildIndex = 0;
    vector<std::future<int>> futures;
    futures.reserve(n->children.size());
    for (auto& child : n->children)
        futures.push_back(threadPool.submit_if_available_else_invoke(addScores, child.nextNode));
    for (int i = 0; i < n->children.size(); ++i) {
        n->children[i].scoreGain += futures[i].get();
		if (n->children[i].scoreGain > n->children[n->bestChildIndex].scoreGain)
			n->bestChildIndex = i;
    }
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