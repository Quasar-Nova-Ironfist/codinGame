#include "main.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <unordered_set>
#include <atomic>
#include <mutex>
#include <Windows.h>

using std::cout; using std::endl; using std::vector; using std::move; using std::pair;
const int dirMults[4] = { 0, 1, 0, -1 };// x: i, y: 3 - i

std::unordered_set<uint64_t> transTable;
std::atomic_bool stopSearch;
std::mutex transTableMutex;
int resultOffsetX, resultOffsetY;

bool solveState::tryInsert() {
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
    transTableMutex.lock();
    bool contDownBranch = transTable.insert(state).second;
    transTableMutex.unlock();
    return contDownBranch;
}
int main() {
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
    while (true) {
        stopSearch = false;
        resultOffsetX = 0; resultOffsetY = 0;
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
        trimGrid(pwomp.cur);
        for (int i = 0; i < pwomp.non0s.size(); ++i)
            pwomp.non0s[i] = { pwomp.non0s[i].first - resultOffsetX, pwomp.non0s[i].second - resultOffsetY };
        cout << "\nx, y offset : " << resultOffsetX << ", " << resultOffsetY << '\n';
        for (size_t y = 0; y < pwomp.cur[0].size(); ++y) {
            for (size_t x = 0; x < pwomp.cur.size(); ++x) {
                cout << pwomp.cur[x][y] << ' ';
            }
            cout << endl;
        }

        auto solveThreadLambda = [&] {
            solveState a;
            a.cur = pwomp.cur;
            a.non0s = pwomp.non0s;
            a.moves.reserve(pwomp.moves.capacity());
            a.solve();
        };
        vector<std::thread> solveThreads;
        solveThreads.reserve(11);
        for (int i = 0; i < 23; ++i)
            solveThreads.emplace_back(solveThreadLambda);
        pwomp.solve();        
        for (int i = 0; i < solveThreads.size(); ++i)
            solveThreads[i].join();
        cout << "transTable.size(): " << transTable.size() << endl;
        transTable.clear();
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
    ++resultOffsetX;
    goto shaveStart;
pastLeftShave:
    for (int x = 0; x < cur.size(); ++x) {
        if (cur[x][0])
            goto pastShaves;
    }
    for (int x = 0; x < cur.size(); ++x)
        cur[x].erase(cur[x].begin());
    ++resultOffsetY;
    goto shaveStart;
pastShaves:
    cur.shrink_to_fit();
    for (int x = 0; x < cur.size(); ++x)
        cur[x].shrink_to_fit();
}
bool solveState::checkIfRemovingCardinallyIsolates(std::pair<int, int>& pos) {//make recursive?
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
}
bool solveState::outputToFileAndReturnTrue() {
    stopSearch = true;
    std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting 5/output.txt", std::fstream::app);
    outFile << "cout << \"";
    for (int i = 0; i < moves.size(); ++i) {
        outFile << moves[i][0] + resultOffsetX << ' ' << moves[i][1] + resultOffsetY << ' ' <<
            (moves[i][2] ? (moves[i][2] == 1 ? 'R' : (moves[i][2] == 2 ? 'D' : 'L')) : 'U') << ' ' <<
            (moves[i][3] > 0 ? '+' : '-') << "\\n";
    }
    outFile << "\";" << endl;
    outFile.close();
    system("taskkill /f /fi \"windowtitle eq output.txt*\"");
    system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting 5/output.txt\"");
    return true;
}
/*void _getConnectedVertexCountHelper(vector<vector<bool>>& visited, std::vector<std::vector<int>>& cur, int x, int y) {
    visited[x][y] = true;
    for (int ySearch = 0; ySearch < cur[0].size(); ++ySearch) {
        if (ySearch == y)
            continue;
        if (cur[x][ySearch] && !visited[x][ySearch])
            _getConnectedVertexCountHelper(visited, cur, x, ySearch);
    }
    for (int xSearch = 0; xSearch < cur.size(); ++xSearch) {
        if (xSearch == x)
            continue;
        if (cur[xSearch][y] && !visited[xSearch][y])
            _getConnectedVertexCountHelper(visited, cur, xSearch, y);
    }
}
int getConnectedVertexCount(std::vector<std::vector<int>>& cur, int x, int y){
    vector<vector<bool>> visited(cur.size(), vector<bool>(cur[0].size(), false));
    _getConnectedVertexCountHelper(visited, cur, x, y);
    return visited.size();
}*/
bool solveState::solve() {
    vector<std::pair<int, int>> non0sCopy = non0s;
    /*for (int qweplo = 0; qweplo < non0sCopy.size(); ++qweplo) {
        if (non0sCopy.size() > 2 && checkIfRemovingCardinallyIsolates(non0sCopy[qweplo]))
            continue;
        pair<int, int> from = non0sCopy[qweplo];
        for (int i = 0; i < non0s.size(); ++i) {
            if (from == non0s[i]) {
                non0s[i] = non0s.back();
                non0s.pop_back();
                break;
            }
        }
        int beforeFrom = cur[from.first][from.second];
        cur[from.first][from.second] = 0;


        non0s.emplace_back(from.first, from.second);
        cur[from.first][from.second] = beforeFrom;
    }
    return false;
    */
    for (int non0sFromIndex = 0; non0sFromIndex < non0s.size(); ++non0sFromIndex) {//iterate through copy b/c aaaaaaa the mutation it burrrrrns
        if (non0s.size() > 2 && checkIfRemovingCardinallyIsolates(non0s[non0sFromIndex]))
            continue;
        pair<int, int> from = non0s[non0sFromIndex];
        non0s[non0sFromIndex] = non0s.back();
        non0s.pop_back();
        int beforeFrom = cur[from.first][from.second];
        cur[from.first][from.second] = 0;

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
                if ((!cur[to.first][to.second] && (non0s.size() == 2 ||
                    checkIfRemovingCardinallyIsolates(to))) ||
                    !tryInsert())
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
}