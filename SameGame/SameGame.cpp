//OH BOY I SURE DO LOVE NEEDING TO HAVE EVERYTHING BE IN ONE FILE THAT SURE IS GREAT
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <chrono>//TODO
#include <map>//replace with unordered

using std::cout; using std::endl; using std::pair; using std::vector; using std::cerr; using std::array;
using sq15 = array<array<int, 15>, 15>;
#ifdef _DEBUG
    int solveCount = 0;
#endif // _DEBUG
class board {
    void setConnected(vector<pair<int, int>>& block, int x, int y, int color) {
        block.emplace_back(x, y);
        grid[x][y] = -grid[x][y];
        if (y < 14 && grid[x][y + 1] == color)
			setConnected(block, x, y + 1, color);
        if (x < 14 && grid[x + 1][y] == color)
            setConnected(block, x + 1, y, color);
        if (y > 0 && grid[x][y - 1] == color)
			setConnected(block, x, y - 1, color);
        if (x > 0 && grid[x - 1][y] == color)
            setConnected(block, x - 1, y, color);
    }
public:
    sq15 grid{};
    int pastMaxX = 15, pastMaxY = 15;
    size_t score = 0;
    vector<vector<pair<int, int>>> getConnectedList() {
        vector<vector<pair<int, int>>> res;
        for (int y = 0; y < pastMaxY; ++y) {
            for (int x = 0; x < pastMaxX; ++x) {
                if (grid[x][y] < 1) continue;
                res.emplace_back();
                setConnected(res.back(), x, y, grid[x][y]);
                if (res.back().size() == 1) res.pop_back();
            }
        }
        for (int y = 0; y < pastMaxY; ++y) {
            for (int x = 0; x < pastMaxX; ++x)
                grid[x][y] = -grid[x][y];
        }
        return res;
    }
    void makeMove(vector<pair<int, int>>& move) {
        int domainMin = 14, domainMax = 0, lowestPoint = 14;
        for (auto& p : move) {
            grid[p.first][p.second] = 0;
            if (p.first < domainMin) domainMin = p.first;
            if (p.first > domainMax) domainMax = p.first;
            if (p.second < lowestPoint) lowestPoint = p.second;
        }
        for (int x = domainMin; x <= domainMax; ++x) {//gravity
            int pit = 128;
            for (int y = lowestPoint; y < pastMaxY; ++y) {
                if (!grid[x][y]) {
                    pit = y;
                    break;
                }
            }
            for (int y = pit + 1; y < pastMaxY; ++y) {
                if (grid[x][y]) {
					grid[x][pit++] = grid[x][y];
					grid[x][y] = 0;
				}
            }
        }
        while (pastMaxY--) {//dec pastMaxY; replace with keeping track of height of each column?
            for (int x = 0; x < pastMaxX; ++x) {
                if (grid[x][pastMaxY]) {
                    ++pastMaxY;
                    goto decYBreak;
                }
            }
        }
        decYBreak:
        if (!lowestPoint) {//left shift
            for (int x = domainMin; x < 15; ++x) {
                if (grid[x][0]) {
                    if (x == domainMin) {
                        ++domainMin;
                        continue;
                    }
                    for (int y = 0; y < pastMaxY; ++y) {
                        grid[domainMin][y] = grid[x][y];
                        grid[x][y] = 0;
                    }
                    ++domainMin;
                }
            }
        }
        while (pastMaxX--) {
            if (grid[pastMaxX][0]) {
				++pastMaxX;
				break;
			}
        }
        score += (move.size() - 2) * (move.size() - 2) + (grid[0][0] ? 0 : 1000);
    }
    board() {}
    board(board& other) {
        grid = other.grid;
		score = other.score;
		pastMaxX = other.pastMaxX;
		pastMaxY = other.pastMaxY;
    }
};
std::ostream& operator<<(std::ostream& os, const board& b) {
    int clrs[] = {0, 31, 32, 34, 33, 35};
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            #ifdef _DEBUG
                //os << "\033[" << clrs[abs(b.grid[x][y])] << 'm' << abs(b.grid[x][y]) << ' ';
                os << "\033[" << clrs[b.grid[x][y]] << 'm' << b.grid[x][y] << ' ';
            #else
                os << b.grid[x][y] << ' ';
            #endif // _DEBUG
		}
		os << '\n';
	}
    #ifdef _DEBUG
        os << "\033[0m";
    #endif // _DEBUG
	return os;
}
namespace best {
    vector<pair<int, int>> moves;
    board b;
}
vector<pair<int, int>> moves;
auto startTime = std::chrono::system_clock::now();//implement one thread for timer and printing, one for solving?
int maxTime = 20000;//first turn 20s, dec by?
template<> struct std::hash<sq15>{//TODO to convert map to unordered_map
    size_t operator()(const sq15& x) const{
        return 0;
    }
};
std::map<sq15, pair<size_t, size_t>> transTable;
int solve(board& b) {
    #ifdef _DEBUG
        ++solveCount;
    #endif // _DEBUG
    auto posMoves = b.getConnectedList();
    if (posMoves.empty()){// || !depth) {
        if (b.score > best::b.score) {
            best::moves = moves;
            best::b = b;
            cerr << "new best score: " << best::b.score << endl;
        }
        return b.score;
    }
    int bestBranchScore = 0;
    for (auto& move : posMoves) {
    	board bCopy = b;
		bCopy.makeMove(move);
        auto itr = transTable.find(bCopy.grid);
        if (itr == transTable.end()) {
            itr = transTable.emplace(bCopy.grid, std::move(std::make_pair(bCopy.score, 0))).first;
        }
        else {
            size_t posNewScore = itr->second.second - itr->second.first + bCopy.score;
            if (posNewScore <= best::b.score)
                continue;
            itr->second.first = bCopy.score;//updated expected score by this grid state
            itr->second.second = posNewScore;//update max score by this grid state
        }
        moves.emplace_back(move[0].first, move[0].second);
        itr->second.second = solve(bCopy);
        if (itr->second.second > bestBranchScore)
			bestBranchScore = itr->second.second;
        moves.pop_back();
    }
    return bestBranchScore;
}
int main() {
    board b{};
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            std::cin >> b.grid[x][y]; std::cin.ignore();
            ++b.grid[x][y];//empty cell 0 instead of -1
        }
    }
    cerr << '\n' << b << endl;
    solve(b);
    transTable.clear();
    cerr << "best score: " << best::b.score << endl;
    #ifdef _DEBUG
        cerr << "solveCount: " << solveCount << endl;
    #endif // _DEBUG
    cout << best::moves[0].first << " " << best::moves[0].second << endl;
    for (int i = 1; i < best::moves.size(); ++i) {//delay to max time, 2nd thread for solving in meantime?; longjmp instead of thread? std::move(moves) to temp var, replace moves with empty vec?
        for (int y = 15; y--;) {
            for (int x = 0; x < 15; ++x) {
                #ifndef _DEBUG
                    std::cin >> b.score; std::cin.ignore();
                #endif // !_NDEBUG
            }
        }
        cout << best::moves[i].first << " " << best::moves[i].second << endl;
    }

    return 0;

    maxTime = 50;
    while (true) {
        for (int y = 15; y--;) {
            for (int x = 0; x < 15; ++x) {
                std::cin >> b.score; std::cin.ignore();
            }
        }
        startTime = std::chrono::system_clock::now();
        moves.clear();
        b.score = best::b.score;
        b.pastMaxX = best::b.pastMaxX;
        b.pastMaxY = best::b.pastMaxY;
        b.grid = std::move(best::b.grid);//laid out like this to avoid a grid copy, as opposed to cur::b = best::b;
        solve(b);
        transTable.clear();
        cout << best::moves[0].first << " " << best::moves[0].second << endl;
    }
}