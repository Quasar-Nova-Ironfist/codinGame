#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>

using std::cerr; using std::cout; using std::endl; using std::vector; using std::pair;
using board = std::map<std::pair<int, int>, int>;
const int dirMults[4]{ 0, 1, 0, -1 };// x: i, y: 3 - i

struct move {
    pair<int, int> pos;
    int dir, times;
    move(pair<int, int> pos, int dir, int times) {
        this->pos = pos; this->dir = dir; this->times = times;
    }
    #pragma warning( push )
    #pragma warning( disable : 26495)
    move() {}
    #pragma warning( pop ) 
};
std::set<board> transTable;
vector<move> moves;
struct malice {
    int cost = 0;
    board cur;
    move mov;
    bool go() {
        vector<malice> potentia;
        for (pair<const pair<int, int>, int>& curPair : cur) {
            for (int times = -1; times < 2; times += 2) {
                for (int dir = 0; dir < 4; ++dir) {
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
                    auto targetItr = potentia.back().cur.find(std::move(target));
                    targetItr->second = abs(targetItr->second + curPair.second * times);
                    if (!targetItr->second)
                        potentia.back().cur.erase(targetItr);
                    potentia.back().cur.erase(curPair.first);
                    //check if potential next branch has aleady been searched
                    if (transTable.count(potentia.back().cur) || potentia.back().cur.size() == 1) {
                        potentia.pop_back();
                        continue;
                    }
                    //add move
                    potentia.back().mov = { curPair.first, dir, times };
                    //check win condition
                    if (!potentia.back().cur.size()) {
                        moves.push_back(std::move(potentia.back().mov));
                        return true;
                    }
                    //set cost
                    for (pair<const pair<int, int>, int>& almri : potentia.back().cur)
                        potentia.back().cost += almri.second;
                    #pragma warning( push )
                    #pragma warning( disable : 4267)
                    potentia.back().cost *= potentia.back().cur.size();
                    #pragma warning( pop ) 
                }
            }
        }
        cur.clear();
        std::sort(potentia.begin(), potentia.end());
        for (int i = 0; i < potentia.size(); ++i) {
            moves.push_back(std::move(potentia[i].mov));
            if (potentia[i].go())
                return true;
            moves.pop_back();
            transTable.insert(std::move(potentia[i].cur));
        }
        return false;
    }
    bool operator < (const malice& rhs) const { return cost < rhs.cost; }
};

int main() {
    while (true) {
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
        std::ofstream solutionsFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
        cerr << "moves.size() " << moves.size() << endl;
        solutionsFile << "cout << \"";
        for (int i = 0; i < moves.size(); ++i) {
            solutionsFile << moves[i].pos.first << ' ' << moves[i].pos.second << ' ' <<
                (moves[i].dir ? (moves[i].dir == 1 ? 'R' : (moves[i].dir == 2 ? 'D' : 'L')) : 'U') << ' ' <<
                (moves[i].times > 0 ? '+' : '-') << "\\n";
        }
        solutionsFile << "\";" << endl;
        system("taskkill /f /fi \"windowtitle eq output.txt*\"");
        system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");
        solutionsFile.close();
        transTable.clear();
    }
}