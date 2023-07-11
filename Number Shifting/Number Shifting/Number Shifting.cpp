#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <string>

using std::cin; using std::cout; using std::cerr; using std::endl; using std::vector; using std::pair;
using board = std::map<std::pair<int, int>, int>;

struct move {
    pair<int, int> pos;
    int dir, times;
    move(pair<int, int> pos, int dir, int times) {
        this->pos = pos; this->dir = dir; this->times = times;
    }
};
vector<move> moves;
std::set<board> alreadySearchedBranches;
bool solve(board& cur) {
    for (auto loopIterator = cur.begin(); loopIterator != cur.end(); ++loopIterator) {
        for (int dir = 0; dir < 4; ++dir) {
            for (int times = -1; times < 2; times += 2) {
                pair<int, int> target(
                    loopIterator->first.first + cur.at(loopIterator->first) * (dir == 1 ? 1 : (dir == 3 ? -1 : 0)),
                    loopIterator->first.second + cur.at(loopIterator->first) * (dir == 0 ? -1 : (dir == 2 ? 1 : 0))
                );
                if (!cur.count(target)) {//extra search to avoid unneeded copying of entire map
                    continue;
                }
                board nextMap(cur);//copy for next iteration
                auto nextMapTargetItr = nextMap.find(target);
                nextMapTargetItr->second = abs(nextMapTargetItr->second + nextMap.at(loopIterator->first) * times);
                if (!nextMapTargetItr->second)
                    nextMap.erase(nextMapTargetItr);
                nextMap.erase(loopIterator->first);
                moves.emplace_back(loopIterator->first, dir, times);
                if (!nextMap.size() || (!alreadySearchedBranches.count(nextMap) && solve(nextMap)) )
                    return true;
                moves.pop_back();
                alreadySearchedBranches.insert(nextMap);
            }
        }
    }
    return false;
}
int main() {
    std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
    while (true) {
        int width;
        int height;
        cin >> width >> height; cin.ignore();
        board cur;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                cin >> cell; cin.ignore();
                if (cell) {
                    cur.emplace(std::make_pair(std::make_pair(x, y), cell));
                }
            }
        }
        solve(cur);
        outFile << "cout << \"";
        for (int i = 0; i < moves.size(); ++i) {
            cout << moves[i].pos.first << ' ' << moves[i].pos.second << ' ' <<
                (moves[i].dir ? (moves[i].dir == 1 ? 'R' : (moves[i].dir == 2 ? 'D' : 'L')) : 'U') << ' ' <<
                (moves[i].times > 0 ? '+' : '-') << endl;
            outFile << moves[i].pos.first << ' ' << moves[i].pos.second << ' ' <<
                (moves[i].dir ? (moves[i].dir == 1 ? 'R' : (moves[i].dir == 2 ? 'D' : 'L')) : 'U') << ' ' <<
                (moves[i].times > 0 ? '+' : '-') << "\\n";
        }
        outFile << "\";" << endl;
        system("taskkill /f /fi \"windowtitle eq output.txt*\"");
        system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");
        moves.clear();
    }
}