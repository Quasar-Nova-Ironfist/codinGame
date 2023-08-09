#include "main.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <parallel_hashmap/phmap.h>

using std::cout; using std::endl; using std::vector; using std::move; using std::pair; using std::array;
const int dirMults[4] = { 0, 1, 0, -1 };// x: i, y: 3 - i

phmap::parallel_flat_hash_set<uint64_t, phmap::Hash<uint64_t>, phmap::EqualTo<uint64_t>, std::allocator<uint64_t>, 6, std::mutex> transTable;//TODO benchmark N
std::atomic_bool stopSearch;
pair<int, int> resultOffsets;

int main(){
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
    while (true) {
        stopSearch = false;
        resultOffsets = std::make_pair(0, 0);
        int width, height;
        std::cin >> width >> height; std::cin.ignore();
        solveState pwomp;
        pwomp.cur.assign(width, vector<array<int, 5>>(height, array<int, 5>()));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                std::cin >> cell; std::cin.ignore();
                if (cell) {
                    pwomp.tiles.emplace_back(x, y);
                    pwomp.cur[x][y][0] = cell;
                }
            }
        }
        pwomp.tiles.shrink_to_fit();
        pwomp.moves.reserve(pwomp.tiles.size() - 1);
        pwomp._isolationCheckVisisted.assign(width, vector<char>(height, 0));
        trimGrid(pwomp.cur);
        for (int i = 0; i < pwomp.tiles.size(); ++i)
            pwomp.tiles[i] = { pwomp.tiles[i].first - resultOffsets.first, pwomp.tiles[i].second - resultOffsets.second };
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