#include <iostream>
#include <thread>
#include <atomic>
#include <set>
#include <map>
#include <vector>
#include <shared_mutex>
#include <fstream>
#include <algorithm>

constexpr int MAX_SEARCH_THREADS = 4;

using std::cerr; using std::cout; using std::endl; using std::vector; using std::pair; using std::shared_mutex; using std::mutex; using std::ref;
using board = std::map<std::pair<int, int>, int>;
const int dirMults[4]{ 0, 1, 0, -1 };// x: i, y: 3 - i

std::ofstream solutionsFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
std::atomic_bool continueSearch;
std::atomic_int searchingThreadCount;
namespace transpose {
    std::set<board> table;
    shared_mutex mut;
}

struct move {
    pair<int, int> pos;
    int dir, times;
    move(pair<int, int> pos, int dir, int times) {
        this->pos = pos; this->dir = dir; this->times = times;
    }
};
struct malice {
    int cost = 0;
    board cur;
    vector<move> moves;
    bool go() {
        vector<malice> potentia;
        for(pair<const pair<int, int>, int>& curPair : cur){
            for (int times = -1; times < 2; times += 2) {
                for (int dir = 0; dir < 4; ++dir) {
                    if (!continueSearch)
                        return true;
                    //check if valid move
                    pair<int, int> target(
                        curPair.first.first + curPair.second * dirMults[dir],
                        curPair.first.second + curPair.second * dirMults[3 - dir]
                    );
                    if (!cur.count(target))
                        continue;
                    //modify board of potential next iteration
                    potentia.emplace_back();
                    potentia.back().cur = cur;
                    auto targetItr = potentia.back().cur.find(target);
                    targetItr->second = abs(targetItr->second + curPair.second * times);
                    if (!targetItr->second)
                        potentia.back().cur.erase(targetItr);
                    potentia.back().cur.erase(curPair.first);
                    //check if potential next branch has aleady been searched
                    transpose::mut.lock_shared();
                    if (transpose::table.count(potentia.back().cur)) {
                        transpose::mut.unlock_shared();
                        potentia.pop_back();
                        continue;
                    }
                    transpose::mut.unlock_shared();
                    //add move
                    potentia.back().moves = moves;
                    potentia.back().moves.emplace_back(curPair.first, dir, times);
                    //check win condition
                    if (!potentia.back().cur.size()) {
                        continueSearch = false;
                        solutionsFile << "cout << \"";
                        for (int i = 0; i < potentia.back().moves.size(); ++i) {
                            solutionsFile << potentia.back().moves[i].pos.first << ' ' << potentia.back().moves[i].pos.second << ' ' <<
                                (potentia.back().moves[i].dir ? (potentia.back().moves[i].dir == 1 ? 'R' : (potentia.back().moves[i].dir == 2 ? 'D' : 'L')) : 'U') << ' ' <<
                                (potentia.back().moves[i].times > 0 ? '+' : '-') << "\\n";
                        }
                        solutionsFile << "\";" << endl;
                        system("taskkill /f /fi \"windowtitle eq output.txt*\"");
                        system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");
                        return true;
                    }
                    //set cost
                    for (auto potCurItr = potentia.back().cur.begin(); potCurItr != potentia.back().cur.end(); ++potCurItr)
                        potentia.back().cost += potCurItr->second;
                    potentia.back().cost *= potentia.back().cur.size();
                }
            }
        }
        std::sort(potentia.begin(), potentia.end());
        vector<std::thread> pool;//thread pool specific to each thread so as to avoid having another mutex
        for(malice& pot : potentia) {
            if (!continueSearch)
                return true;
            if (searchingThreadCount < 4) {
                pool.push_back(std::move( std::thread([pot]() {pot.go(); }) ));//is that doing copy or ref?, object has incompatible type qualifier const malice
                ++searchingThreadCount;
            }
            else
                pot.go();
        }
        for (int i = 0; i < potentia.size(); ++i) {
            if (!continueSearch)
                return true;
            if (searchingThreadCount < 4) {
                pool.push_back(std::move( std::thread(potentia[i].go) ));//no constructor instance matches (bool()), pointer to bound function may only be used to call function
                ++searchingThreadCount;
            }
            else
                potentia[i].go();
        }
        --searchingThreadCount;
        for (int i = 0; i < pool.size(); ++i)
            pool[i].join();
        if (!continueSearch)
            return true;
        transpose::mut.lock();
        for (int i = 0; i < potentia.size(); ++i)
            transpose::table.insert(std::move(potentia[i].cur));
        transpose::mut.unlock();
    }
    bool operator < (const malice& rhs) const { return cost < rhs.cost; }
};

int main() {
    while (true) {
        continueSearch = true;
        searchingThreadCount = 1;
        cerr << "Trans size: " << transpose::table.size() << endl;
        malice alpe;
        int width;
        int height;
        std::cin >> width >> height; std::cin.ignore();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                std::cin >> cell; std::cin.ignore();
                if (cell) {
                    alpe.cur.emplace(std::make_pair(std::make_pair(x, y), cell));
                }
            }
        }
        alpe.go();
        transpose::table.clear();
    }
}