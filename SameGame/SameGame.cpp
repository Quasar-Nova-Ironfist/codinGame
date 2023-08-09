#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <chrono>//TODO
#include <unordered_set>

using std::cout; using std::endl; using std::pair; using std::vector; using std::cerr;
#define sq15 std::array<std::array<int, 15>, 15>
std::ostream& operator<<(std::ostream& os, const sq15& grid) {
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
			os << grid[x][y] << ' ';
		}
		os << '\n';
	}
	return os;
}
template<> struct std::hash<pair<int, sq15&>> {
    size_t operator()(const pair<int, sq15&> grid) const {
        size_t res = grid.first;
        for (int y = 0; y < 15; ++y) {
            for (int x = 0; x < 15; ++x) {
                res = res * 31 + grid.second[x][y];
            }
        }
        return res;
    }
};
std::unordered_set<pair<int, sq15&>> transTable;
namespace best {
    vector<pair<int, int>> moves;
    int pastMaxX, pastMaxY;
    unsigned long long score = 0;
    sq15 grid;
}
namespace cur {//this could be moved into each solve() but best:: is probably best as global anyway and this means less passing around references, I think.
    vector<pair<int, int>> moves;
    int pastMaxX = 15, pastMaxY = 15;
    unsigned long long score = 0;
    sq15 grid{};
}
auto startTime = std::chrono::system_clock::now();//implement one thread for timer and printing, one for solving?
int maxTime = 20000;//first turn 20s, dec by?

void setConnected(vector<pair<int, int>>& res, sq15& grid, size_t x, size_t y, int color) {
    grid[x][y] = 0;
    if (y < 14 && grid[x][y + 1] == color) {
        res.emplace_back(x, y + 1);
        setConnected(res, grid, x, y + 1, color);
    }
    if (x < 14 && grid[x + 1][y] == color) {
        res.emplace_back(x + 1, y);
        setConnected(res, grid, x + 1, y, color);
    }
    if (y > 0 && grid[x][y - 1] == color) {
		res.emplace_back(x, y - 1);
		setConnected(res, grid, x, y - 1, color);   
	}
    if (x > 0 && grid[x - 1][y] == color) {
		res.emplace_back(x - 1, y);
		setConnected(res, grid, x - 1, y, color);
	}
}

vector<vector<pair<int,int>>> getConnectedList(sq15 grid) {
    vector<vector<pair<int, int>>> res;
    for (int y = 0; y < cur::pastMaxY; ++y) {
        for (int x = 0; x < cur::pastMaxX; ++x) {
			if (!grid[x][y]) continue;
			vector<pair<int, int>> connected;
            res.emplace_back();
            res.back().emplace_back(x, y);
            setConnected(res.back(), grid, x, y, grid[x][y]);
            if (res.back().size() == 1)
                res.pop_back();
		}
    }
    return res;
}

void solve(/*int depth*/) {
    auto posMoves = getConnectedList(cur::grid);//sort by size?
    if (posMoves.empty() && cur::score > best::score) {
        best::score = cur::score;
        best::moves = cur::moves;
        best::grid = cur::grid;
        best::pastMaxX = cur::pastMaxX;
        best::pastMaxY = cur::pastMaxY;
    }
    int beforeScore = cur::score, beforePastMaxX = cur::pastMaxX, beforePastMaxY = cur::pastMaxY;
    sq15 beforeGrid = std::move(cur::grid);//avoid a copy
    for (auto& move : posMoves) {
        cur::grid = beforeGrid;
        cur::pastMaxX = beforePastMaxX;
        cur::pastMaxY = beforePastMaxY;

        int moveDomainMin = 14, moveDomainMax = 0, moveLowestPoint = 14;
        for (auto& p : move) {
            cur::grid[p.first][p.second] = 0;
			if (p.first < moveDomainMin) moveDomainMin = p.first;
			if (p.first > moveDomainMax) moveDomainMax = p.first;
            if (p.second < moveLowestPoint) moveLowestPoint = p.second;
		}

        for (int x = moveDomainMin; x <= moveDomainMax; ++x) {//gravity
            int pit = moveLowestPoint;
            for (int y = moveLowestPoint + 1; y < cur::pastMaxY; ++y) {
                if (cur::grid[x][y]) {
                    cur::grid[x][pit++] = cur::grid[x][y];
                    cur::grid[x][y] = 0;
				}
			}
        }
        while (cur::pastMaxY--) {//dec pastMaxY
            for (int x = 0; x < cur::pastMaxX; ++x) {
                if (cur::grid[x][cur::pastMaxY]) {
                    ++cur::pastMaxY;
                    goto decYBreak;
                }
            }
        }
        decYBreak:

        if (!moveLowestPoint) {//left shift
            for (int x = moveDomainMin; x < cur::pastMaxX; ++x) {
                if (cur::grid[x][0]) {
                    ++moveDomainMin;
                    continue;
                }
                for (int y = 0; y < cur::pastMaxY; ++y) {
                    cur::grid[moveDomainMin][y] = cur::grid[x][y];
                    cur::grid[x][y] = 0;
                }
                ++moveDomainMin;
                --cur::pastMaxX;
            }
        }

        cur::score = beforeScore + (move.size() - 2) * (move.size() - 2) + (cur::grid[0][0] ? 0 : 1000);
        if(!transTable.emplace(cur::score, cur::grid).second) continue;//if already seen this state, skip further processing
        cur::moves.emplace_back(move[0].first, move[0].second);
        solve(); //if (depth) solve(depth - 1);
        cur::moves.pop_back();
    }
}
int main() {
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            std::cin >> cur::grid[x][y]; std::cin.ignore();
            ++cur::grid[x][y];//empty cell 0 instead of -1
        }
    }
    cerr << cur::grid << endl;
    solve();
    transTable.clear();
    cerr << "score: " << best::score << endl;
    cout << best::moves[0].first << " " << best::moves[0].second << endl;
    for (int i = 1; i < best::moves.size(); ++i) {//delay to max time, 2nd thread for solving in meantime?; longjmp instead of thread? std::move(moves) to temp var, replace moves with empty vec?
        for (int y = 15; y--; y) {
            for (int x = 0; x < 15; ++x) {
                //std::cin >> best::score; std::cin.ignore();
            }
        }
        cout << best::moves[i].first << " " << best::moves[i].second << endl;
    }
    maxTime = 50;

    return 0;

    while (true) {
        for (int y = 15; y--;) {
            for (int x = 0; x < 15; ++x) {
                std::cin >> best::score; std::cin.ignore();
            }
        }
        startTime = std::chrono::system_clock::now();
        best::score = 0;
        cur::score = 0;
        cur::moves.clear();
        cur::pastMaxX = best::pastMaxX;
        cur::pastMaxY = best::pastMaxY;
        cur::grid = std::move(best::grid);
        solve();
        transTable.clear();
        cout << best::moves[0].first << " " << best::moves[0].second << endl;
    }
}