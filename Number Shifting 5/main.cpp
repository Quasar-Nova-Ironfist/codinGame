#pragma once
#include "main.h"
#include <iostream>
#include <fstream>
#include <set>
#include <atomic>
#include <mutex>
#include <thread>

using std::cout; using std::endl; using std::vector; using std::move; using std::ref;
const int dirMults[4] = { 0, 1, 0, -1 };// x: i, y: 3 - i

std::set<std::vector<std::vector<int>>> transTable;
std::atomic_bool stopSearch;
std::mutex transTableMutex;
int resultOffsetX, resultOffsetY;

int main(){
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
        {
            for (size_t x = pwomp.cur.size(); x--;) {
                for (size_t y = pwomp.cur[0].size(); y--;) {
                    if (pwomp.cur[x][y])
                        goto pastRightShave;
                }
                pwomp.cur.pop_back();
            }
        pastRightShave:
            for (size_t y = pwomp.cur[0].size(); y--;) {
                for (size_t x = pwomp.cur.size(); x--;) {
                    if (pwomp.cur[x][y])
                        goto pastBottomShave;
                }
                for (size_t x = pwomp.cur.size(); x--;)
                    pwomp.cur[x].pop_back();
            }
        pastBottomShave:
            for (size_t y = 0; y < pwomp.cur[0].size(); ++y) {
                if (pwomp.cur[0][y])
                    goto pastLeftShave;
            }
            pwomp.cur.erase(pwomp.cur.begin());
            ++resultOffsetX;
            goto pastBottomShave;
        pastLeftShave:
            for (int x = 0; x < pwomp.cur.size(); ++x) {
                if (pwomp.cur[x][0])
                    goto pastShaves;
            }
            for (int x = 0; x < pwomp.cur.size(); ++x)
                pwomp.cur[x].erase(pwomp.cur[x].begin());
            ++resultOffsetY;
            goto pastBottomShave;
        pastShaves:
            pwomp.cur.shrink_to_fit();
            for (int x = 0; x < pwomp.cur.size(); ++x)
                pwomp.cur[x].shrink_to_fit();
            cout << "\nx, y offset : " << resultOffsetX << ", " << resultOffsetY << '\n';
            for (size_t y = 0; y < pwomp.cur[0].size(); ++y) {
                for (size_t x = 0; x < pwomp.cur.size(); ++x) {
                    cout << pwomp.cur[x][y] << ' ';
                }
                cout << endl;
            }
        }
        auto solveThreadLambda = [&] {
            solveState a;
            a.cur = pwomp.cur;
            a.non0s = pwomp.non0s;
            a.solve();
        };
        std::thread solveThread0(solveThreadLambda);
        std::thread solveThread1(solveThreadLambda);
        std::thread solveThread2(solveThreadLambda);
        pwomp.solve();
        solveThread0.join();
        solveThread1.join();
        solveThread2.join();
        transTable.clear();
    }
}

bool solveState::solve() {
    for (int non0sFromIndex = 0; non0sFromIndex < non0s.size(); ++non0sFromIndex) {
        if (stopSearch)
            return true;
        if (checkIfRemovingCardinallyIsolates(cur, non0s[non0sFromIndex].first, non0s[non0sFromIndex].second))
            continue;
        int fromX = non0s[non0sFromIndex].first;
        int fromY = non0s[non0sFromIndex].second;
        non0s[non0sFromIndex] = non0s.back();
        non0s.pop_back();
        int beforeFrom = cur[fromX][fromY];
        cur[fromX][fromY] = 0;

        for (int dir = 0; dir < 4; ++dir) {
            if (stopSearch)
                return true;
            int toX = fromX + beforeFrom * dirMults[dir];
            int toY = fromY + beforeFrom * dirMults[3 - dir];
            if (toX < 0 || toX >= cur.size() || toY < 0 || toY >= cur[0].size() || !cur[toX][toY])
                continue;
 
            int beforeTo = cur[toX][toY];
            for (int times = -1; times < 2; times += 2) {
                if (stopSearch)
                    return true;
                cur[toX][toY] = abs(beforeTo + beforeFrom * times);
                if (!cur[toX][toY] && (non0s.size() == 2 || checkIfRemovingCardinallyIsolates(cur, toX, toY)))
                    continue;
                transTableMutex.lock();
                bool contDownBranch = transTable.insert(cur).second;
                transTableMutex.unlock();
                if (!contDownBranch)
                    continue;

                if (!cur[toX][toY]) {//remove matching entry from non0s
                    for (int i = 0; i < non0s.size(); ++i) {
                        if (non0s[i].first == toX && non0s[i].second == toY) {
                            non0s[i] = non0s.back();
                            non0s.pop_back();
                            break;
                        }
                    }
                }
                moves.push_back({fromX, fromY, dir, times});
                if (!non0s.size()) {//solution found, write to file
                    stopSearch = true;
                    std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
                    outFile << "cout << \"";
                    for (int i = 0; i < moves.size(); ++i) {
                        outFile << moves[i][0] + resultOffsetX << ' ' << moves[i][1] + resultOffsetY << ' ' <<
                            (moves[i][2] ? (moves[i][2] == 1 ? 'R' : (moves[i][2] == 2 ? 'D' : 'L')) : 'U') << ' ' <<
                            (moves[i][3] > 0 ? '+' : '-') << "\\n";
                    }
                    outFile << "\";" << endl;
                    outFile.close();
                    system("taskkill /f /fi \"windowtitle eq output.txt*\"");
                    system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");
                    return true;
                }
                if (solve())
                    return true;
                moves.pop_back();
                if(!cur[toX][toY])
                    non0s.emplace_back(toX, toY);
            }
            cur[toX][toY] = beforeTo;
        }
        non0s.emplace_back(fromX, fromY);
        cur[fromX][fromY] = beforeFrom;
    }
    return false;
}

bool checkIfRemovingCardinallyIsolates(std::vector<std::vector<int>>& cur, int x, int y){//make recursive?
    int found = -1;
    for (int checkY = 0; checkY < cur[0].size(); ++checkY) {
        if (checkY == y) continue;
        if (cur[x][checkY]) {
            if (found != -1)
                goto pastUpDown;
            found = checkY;
        }
    }
    if (found != -1) {
        for (int checkX = 0; checkX < cur.size(); ++checkX) {
            if (cur[checkX][found] && checkX != x)
                goto pastUpDown;
        }
        return true;
    }
pastUpDown:
    found = -1;
    for (int checkX = 0; checkX < cur.size(); ++checkX) {
        if (checkX == x) continue;
        if (cur[checkX][y]) {
            if (found != -1)
                return false;
            found = checkX;
        }
    }
    if (found != -1) {
        for (int checkY = 0; checkY < cur[0].size(); ++checkY) {
            if (cur[found][checkY] && checkY != y)
                return false;
        }
        return true;
    }
    return false;
}