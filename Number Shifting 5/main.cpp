#include "main.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <parallel_hashmap/phmap.h>
//#include <chrono>

using std::cout; using std::endl; using std::vector; using std::move; using std::pair;
const int dirMults[4] = { 0, 1, 0, -1 };// x: i, y: 3 - i

phmap::parallel_flat_hash_set<uint64_t, phmap::Hash<uint64_t>, phmap::EqualTo<uint64_t>, std::allocator<uint64_t>, 6, std::mutex> transTable;//TODO benchmark N
std::atomic_bool stopSearch;
pair<int, int> resultOffsets;

bool solveState::tryInsert() {//no idea how terrible this is
    uint64_t state = 0xcbf29ce484222325;
    int space = 0;
    for (int y = 0; y < cur[0].size(); ++y) {
        for (int x = 0; x < cur.size(); ++x) {
            if (cur[x][y]) {
                state ^= space;
                state *= 0x100000001b3;
                state ^= cur[x][y];
                state *= 0x100000001b3;
                space = 0;
                continue;
            }
            ++space;
        }
    }
    return transTable.insert(state).second;//hash grid to uint64_t then table hashes that uint64_t, probably not great in terms of speed
}
solveState::solveState(solveState& other){
    this->cur = other.cur;
    this->moves = other.moves;//this is unneeded when this is called in main, but I'm including this here just so I don't forget not to if the copy constructor is later called in solve()
    this->moves.reserve(other.moves.capacity());
    this->non0s = other.non0s;
    this->_isolationCheckVisisted = other._isolationCheckVisisted;
}
int main() {
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
    while (true) {
        stopSearch = false;
        resultOffsets = std::make_pair(0, 0);
        int width, height;
        std::cin >> width >> height; std::cin.ignore();
        solveState pwomp;
        pwomp.cur.assign(width, vector<int>(height, 0));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                std::cin >> cell; std::cin.ignore();
                if (cell) {
                    pwomp.non0s.emplace_back(x, y);
                    pwomp.cur[x][y] = cell;
                }
            }
        }
        pwomp.non0s.shrink_to_fit();
        pwomp.moves.reserve(pwomp.non0s.size() - 1);
        pwomp._isolationCheckVisisted.assign(width, vector<char>(height, 0));
        trimGrid(pwomp.cur);
        for (int i = 0; i < pwomp.non0s.size(); ++i)
            pwomp.non0s[i] = { pwomp.non0s[i].first - resultOffsets.first, pwomp.non0s[i].second - resultOffsets.second};
        cout << "\nx, y offset : " << resultOffsets.first << ", " << resultOffsets.second << '\n';
        for (size_t y = 0; y < pwomp.cur[0].size(); ++y) {
            for (size_t x = 0; x < pwomp.cur.size(); ++x) {
                cout << pwomp.cur[x][y] << ' ';
            }
            cout << endl;
        }

        //auto timeStart = std::chrono::steady_clock::now();
        vector<std::thread> solveThreads;
        solveThreads.reserve(11);//TODO benchmark different thread counts
        for (int i = 0; i < solveThreads.capacity(); ++i)
            solveThreads.emplace_back([&pwomp] {solveState(pwomp).solve(); });
        pwomp.solve();
        for (int i = 0; i < solveThreads.size(); ++i)
            solveThreads[i].join();
        cout << "transTable.size(): " << transTable.size() << endl;
        transTable.clear();
        //cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - timeStart).count() << endl;
    }
}

void trimGrid(vector<vector<int>>& cur) {
shaveStart:
    for (size_t x = cur.size(); x--;) {
        for (size_t y = cur[0].size(); y--;) {
            if (cur[x][y])
                goto pastRightShave;
        }
        cur.pop_back();
    }
    goto shaveStart;
pastRightShave:
    for (size_t y = cur[0].size(); y--;) {
        for (size_t x = cur.size(); x--;) {
            if (cur[x][y])
                goto pastBottomShave;
        }
        for (size_t x = cur.size(); x--;)
            cur[x].pop_back();
    }
    goto shaveStart;
pastBottomShave:
    for (size_t y = 0; y < cur[0].size(); ++y) {
        if (cur[0][y])
            goto pastLeftShave;
    }
    cur.erase(cur.begin());
    ++resultOffsets.first;
    goto shaveStart;
pastLeftShave:
    for (int x = 0; x < cur.size(); ++x) {
        if (cur[x][0])
            goto pastShaves;
    }
    for (int x = 0; x < cur.size(); ++x)
        cur[x].erase(cur[x].begin());
    ++resultOffsets.second;
    goto shaveStart;
pastShaves:
    cur.shrink_to_fit();
    for (int x = 0; x < cur.size(); ++x)
        cur[x].shrink_to_fit();
}
/*bool solveState::checkIfRemovingCardinallyIsolates(pair<int, int>& pos) {//make recursive?
    int found = -1;
    for (int checkY = 0; checkY < cur[0].size(); ++checkY) {
        if (checkY == pos.second) continue;
        if (cur[pos.first][checkY]) {
            if (found != -1)
                goto pastUpDown;
            found = checkY;
        }
    }
    if (found != -1) {
        for (int checkX = 0; checkX < cur.size(); ++checkX) {
            if (cur[checkX][found] && checkX != pos.first)
                goto pastUpDown;
        }
        return true;
    }
pastUpDown:
    found = -1;
    for (int checkX = 0; checkX < cur.size(); ++checkX) {
        if (checkX == pos.first) continue;
        if (cur[checkX][pos.second]) {
            if (found != -1)
                return false;
            found = checkX;
        }
    }
    if (found != -1) {
        for (int checkY = 0; checkY < cur[0].size(); ++checkY) {
            if (cur[found][checkY] && checkY != pos.second)
                return false;
        }
        return true;
    }
    return false;
}*/
bool solveState::outputToFileAndReturnTrue() {
    stopSearch = true;
    std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting 5/output.txt", std::fstream::app);
    outFile << "cout << \"";
    for (int i = 0; i < moves.size(); ++i) {
        outFile << moves[i][0] + resultOffsets.first << ' ' << moves[i][1] + resultOffsets.second << ' ' <<
            (moves[i][2] ? (moves[i][2] == 1 ? 'R' : (moves[i][2] == 2 ? 'D' : 'L')) : 'U') << ' ' <<
            (moves[i][3] > 0 ? '+' : '-') << "\\n";
    }
    outFile << "\";" << endl;
    outFile.close();
    system("taskkill /f /fi \"windowtitle eq output.txt*\"");
    system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting 5/output.txt\"");
    return true;
}
void solveState::_isolationIterationUpDown(int x){
    ++_isolationCheckFoundCount;
    for (int ySearch = 0; ySearch < cur[0].size(); ++ySearch) {
        if (cur[x][ySearch] && !_isolationCheckVisisted[x][ySearch]) {//ySearch != y not needed b/c vis[x][y] true
            _isolationCheckVisisted[x][ySearch] = 1;
            _isolationIterationLeftRight(ySearch);
        }
    }
}
void solveState::_isolationIterationLeftRight(int y){
    ++_isolationCheckFoundCount;
    for (int xSearch = 0; xSearch < cur.size(); ++xSearch) {
        if (cur[xSearch][y] && !_isolationCheckVisisted[xSearch][y]) {
            _isolationCheckVisisted[xSearch][y] = 1;
            _isolationIterationUpDown(xSearch);
        }
    }
}
bool solveState::isolates(pair<int, int> posRemoved){
    _isolationCheckFoundCount = -2;//+1 per iteration below, shadow +1 b/c count != non0s.size() - 1 -> count != non0s.size() 
    for (pair<int, int>& pos : non0s)//reset only relevant ones rather than looping through entire 2d vector
        _isolationCheckVisisted[pos.first][pos.second] = 0;
    if (posRemoved == non0s[0]) {
        _isolationCheckVisisted[non0s[1].first][non0s[1].second] = 1;
        _isolationIterationUpDown(non0s[1].first);
        _isolationIterationLeftRight(non0s[1].second);
    }
    else {
        _isolationCheckVisisted[non0s[0].first][non0s[0].second] = 1;
        _isolationIterationUpDown(non0s[0].first);
        _isolationIterationLeftRight(non0s[0].second);
    }  
    return _isolationCheckFoundCount != non0s.size();
}
bool solveState::solve() {
    vector<pair<int, int>> non0sCopy = non0s;
    for (int qweplo = 0; qweplo < non0sCopy.size(); ++qweplo) {
        pair<int, int> from = non0sCopy[qweplo];
        int beforeFrom = cur[from.first][from.second];
        cur[from.first][from.second] = 0;
        if (isolates(from)) {
            cur[from.first][from.second] = beforeFrom;
            continue;
        }
        for (int i = 0; i < non0s.size(); ++i) {
            if (from == non0s[i]) {
                non0s[i] = non0s.back();
                non0s.pop_back();
                break;
            }
        }
        
        for (int dir = 0; dir < 4; ++dir) {
            pair<int, int> to = {
                from.first + beforeFrom * dirMults[dir],
                from.second + beforeFrom * dirMults[3 - dir]
            };
            if (to.first < 0 || to.first >= cur.size() || to.second < 0 || to.second >= cur[0].size() || !cur[to.first][to.second])
                continue;

            int beforeTo = cur[to.first][to.second];
            for (int times = -1; times < 2; times += 2) {
                cur[to.first][to.second] = abs(beforeTo + beforeFrom * times);
                if ((!cur[to.first][to.second] && (non0s.size() == 2 || isolates(to))) || !tryInsert())
                    continue;
                if (!cur[to.first][to.second]) {//remove matching entry from non0s
                    for (int i = 0; i < non0s.size(); ++i) {
                        if (non0s[i].first == to.first && non0s[i].second == to.second) {
                            non0s[i] = non0s.back();
                            non0s.pop_back();
                            break;
                        }
                    }
                }
                moves.push_back({ from.first, from.second, dir, times });
                if (stopSearch.load(std::memory_order_relaxed) || (!non0s.size() && outputToFileAndReturnTrue()) || solve())
                    return true;
                moves.pop_back();
                if (!cur[to.first][to.second])
                    non0s.emplace_back(to.first, to.second);
            }
            cur[to.first][to.second] = beforeTo;
        }
        non0s.emplace_back(from.first, from.second);
        cur[from.first][from.second] = beforeFrom;
    }
    return false;
    /*
    for (int non0sFromIndex = 0; non0sFromIndex < non0s.size(); ++non0sFromIndex) {//iterate through copy b/c aaaaaaa the mutation it burrrrrns
        if (non0s.size() > 2 && checkIfRemovingCardinallyIsolates(non0s[non0sFromIndex]))
            continue;
        pair<int, int> from = non0s[non0sFromIndex];
        non0s[non0sFromIndex] = non0s.back();
        non0s.pop_back();
        int beforeFrom = cur[from.first][from.second];
        cur[from.first][from.second] = 0;

        for (int dir = 0; dir < 4; ++dir) {
        }
        non0s.emplace_back(from.first, from.second);
        cur[from.first][from.second] = beforeFrom;
    }
    return false;*/
}