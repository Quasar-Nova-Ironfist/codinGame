#include "solveState.h"
#include "main.h"

#include <iostream>

using std::vector; using std::move;
const int dirMults[4] = { 0, 1, 0, -1 };// x: i, y: 3 - i

bool solveState::solve() {
    for (int non0sFromIndex = 0; non0sFromIndex < non0s.size(); ++non0sFromIndex) {
        if (stopSearch)
            return true;
        if (non0s.size() > 2 && checkIfRemovingCardinallyIsolates(cur, non0s[non0sFromIndex].first, non0s[non0sFromIndex].second))
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
                moves.push_back({ fromX, fromY, dir, times });
                if ((!non0s.size() && outputToFileAndReturnTrue(moves)) || solve())
                    return true;
                moves.pop_back();
                if (!cur[toX][toY])
                    non0s.emplace_back(toX, toY);
            }
            cur[toX][toY] = beforeTo;
        }
        non0s.emplace_back(fromX, fromY);
        cur[fromX][fromY] = beforeFrom;
    }
    return false;
}