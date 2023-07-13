#include <iostream>
#include <vector>
//#include <unordered_map>
//#include <unordered_set>
#include <map>
#include <set>
#include <fstream>
#include <string>

//fuck everything, redo it all, beam search


using std::cin; using std::cout; using std::cerr; using std::endl; using std::vector; using std::pair; using std::tuple; using std::get;
using board = std::map<std::pair<int, int>, int>;
const int dirMults[8] = {0, 1, 0, -1};// x: i, y: 3 - i
std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
std::ofstream transposeOut("D:/Downloads/transpose.txt", std::fstream::app);
std::set<board> alreadySearchedBranches;

struct move {
    pair<int, int> pos;
    int dir, times;
    move(pair<int, int> pos, int dir, int times) {
        this->pos = pos; this->dir = dir; this->times = times;
    }
};
vector<move> moves;
void writeMapToFile(board& cur) {
    transposeOut << cur.size() << ' ';
    for (auto itr = cur.begin(); itr != cur.end(); ++itr) {
        transposeOut << itr->first.first << ' ' << itr->first.second << ' ' << itr->second << ' ';
    }
    transposeOut << endl;
}
bool solve(board& cur) {
    for (auto loopItr = cur.begin(); loopItr != cur.end(); ++loopItr) {
        for (int dir = 0; dir < 4; ++dir) {
            for (int times = -1; times < 2; times += 2) {
                /*int loopItrVal = loopItr->second;//having this up here maybe means faster b/c less indirection?
                pair<int, int> target(//pair instead of just ints for move.emplace_back and cur[target]
                    loopItr->first.first + loopItrVal * (dir == 1 ? 1 : (dir == 3 ? -1 : 0)),//replace with switch statement or vals from const array?
                    loopItr->first.second + loopItrVal * (dir == 0 ? -1 : (dir == 2 ? 1 : 0))
                );
                auto targetItr = cur.find(target);
                if (targetItr == cur.end())
                    continue;
                cerr << "a0" << endl;
                int targetItrVal = targetItr->second;
                targetItr->second = abs(targetItrVal + loopItrVal * times);
                if (!targetItr->second)
                    cur.erase(targetItr);
                cerr << "a1" << endl;
                cur.erase(loopItr);//replace with setting to 0? int for num remaining passed between functions? vector of indexes of other vector containing pairs as keys to map?
                cerr << "a2" << endl;
                moves.emplace_back(loopItr->first, dir, times);
                cerr << "a3" << endl;
                if (!alreadySearchedBranches.count(cur)) {
                    cerr << "duplicate " << endl;
                }

                if (!cur.size() || (!alreadySearchedBranches.count(cur) && solve(cur)) )
                    return true;
                
                cur[loopItr->first] = loopItrVal;
                cur[target] = targetItrVal;
                moves.pop_back();
                alreadySearchedBranches.insert(cur);
                */
 
                pair<int, int> target(
                    loopItr->first.first + loopItr->second * dirMults[dir],
                    loopItr->first.second + loopItr->second * dirMults[3-dir]
                );
                if (!cur.count(target)) {//extra search to avoid unneeded copying of entire map
                    continue;
                }
                board nextMap(cur);//copy for next iteration
                auto nextMapTargetItr = nextMap.find(target);
                nextMapTargetItr->second = abs(nextMapTargetItr->second + loopItr->second * times);
                if (!nextMapTargetItr->second)
                    nextMap.erase(nextMapTargetItr);
                nextMap.erase(loopItr->first);
                moves.emplace_back(loopItr->first, dir, times);

                //if (alreadySearchedBranches.count(nextMap)) {
                //    cerr << "duplicate " << endl;
                //}

                if (!nextMap.size() || (!alreadySearchedBranches.count(nextMap) && solve(nextMap)) )
                    return true;
                moves.pop_back();
                if(alreadySearchedBranches.insert(nextMap).second)
                    writeMapToFile(nextMap);
            }
        }
    }
    return false;
}
/*bool solve(board& cur) {
    vector<tuple<int, board, move>> nextMapStateTuples;
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
                nextMapStateTuples.emplace_back(board(cur), move{loopIterator->first, dir, times}, 0);
                auto nextMapTargetItr = get<1>(nextMapStateTuples.back()).find(target);
                nextMapTargetItr->second = abs(nextMapTargetItr->second + get<1>(nextMapStateTuples.back()).at(loopIterator->first) * times);
                if (!nextMapTargetItr->second)
                    get<1>(nextMapStateTuples.back()).erase(nextMapTargetItr);
                get<1>(nextMapStateTuples.back()).erase(loopIterator->first);
                if (alreadySearchedBranches.count(get<1>(nextMapStateTuples.back()))) {
                    return false;
                }
                if (!get<1>(nextMapStateTuples.back()).size()) {
                    moves.push_back(get<2>(nextMapStateTuples.back()));
                    return true;
                }

                /*board nextMap(cur);//copy for next iteration
                auto nextMapTargetItr = nextMap.find(target);
                nextMapTargetItr->second = abs(nextMapTargetItr->second + nextMap.at(loopIterator->first) * times);
                if (!nextMapTargetItr->second)
                    nextMap.erase(nextMapTargetItr);
                nextMap.erase(loopIterator->first);
                moves.emplace_back(loopIterator->first, dir, times);
                if (!nextMap.size() || (!alreadySearchedBranches.count(nextMap) && solve(nextMap)) )
                    return true;
                moves.pop_back();
                alreadySearchedBranches.insert(nextMap);*
            }
        }
    }
    for (int i = 0; i < nextMapStateTuples.size(); ++i) {
        int sum = 0;
        for (auto itr = get<1>(nextMapStateTuples[i]).begin(); itr != get<1>(nextMapStateTuples[i]).end(); ++itr)
            sum += itr->second;
        get<0>(nextMapStateTuples[i]) = sum * get<1>(nextMapStateTuples[i]).size();
    }
    std::sort(nextMapStateTuples.begin(), nextMapStateTuples.end());
    for (int i = 0; i < nextMapStateTuples.size(); ++i) {
        moves.push_back(get<2>(nextMapStateTuples[i]));
        if(solve(get<1>(nextMapStateTuples[i])))
            return true;
        moves.pop_back();
        alreadySearchedBranches.insert(get<1>(nextMapStateTuples[i]));
    }
    return false;
}*/
void readTranspositions() {
    std::ifstream fileIn("D:/Downloads/transpose.txt");
    int size;
    while (fileIn >> size) {
        //cerr << "size: " << size << endl;
        board transEntry;
        for (int i = 0; i < size; ++i) {
            int x, y, val;
            fileIn >> x >> y >> val;
            //cerr << x << ", " << y << ", " << val << endl;
            transEntry.emplace(std::make_pair(std::make_pair(x, y), val));
        }
        alreadySearchedBranches.insert(transEntry);
    }
    cerr << "trans size " << alreadySearchedBranches.size() << endl;
}
int main() {
    readTranspositions();
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