#include "main.h"
#include "solveState.h"
#include <iostream>
#include <fstream>
#include <thread>

using std::cout; using std::endl; using std::vector; using std::move; using std::ref;

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
        trimGrid(pwomp.cur);
        for (int i = 0; i < pwomp.non0s.size(); ++i)
            pwomp.non0s[i] = {pwomp.non0s[i].first - resultOffsetX, pwomp.non0s[i].second - resultOffsetY};
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
            a.solve();
        };
        std::thread solveThread0(solveThreadLambda);
        std::thread solveThread1(solveThreadLambda);
        std::thread solveThread2(solveThreadLambda);
        pwomp.solve();
        solveThread0.join();
        solveThread1.join();
        solveThread2.join();
        cout << "transTable.size(): " << transTable.size() << endl;
        transTable.clear();
    }
}

void trimGrid(std::vector<std::vector<int>>& cur) {
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
bool outputToFileAndReturnTrue(std::vector<std::array<int, 4>>& moves) {
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