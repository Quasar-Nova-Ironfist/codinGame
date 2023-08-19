//OH BOY I SURE DO LOVE NEEDING TO HAVE EVERYTHING BE IN ONE FILE THAT SURE IS GREAT
//https://www.codingame.com/ide/puzzle/samegame
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <chrono>//TODO
#include <unordered_map>

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
    void makeMove(pair<int, int>& pos) {
        vector<pair<int, int>> move;
        setConnected(move, pos.first, pos.second, grid[pos.first][pos.second]);
        makeMove(move);
    }
    board() {}
    board(board& other) {
        grid = other.grid;
        score = other.score;
        pastMaxX = other.pastMaxX;
        pastMaxY = other.pastMaxY;
    }
    bool operator==(const board& other) const { return grid == other.grid; }
};
std::ostream& operator<<(std::ostream& os, const board& b) {
    int clrs[] = { 0, 31, 32, 34, 33, 35 };
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
#ifdef _DEBUG
            //os << "\033[" << clrs[abs(b.grid[x][y])] << 'm' << abs(b.grid[x][y]) << ' ';
            os << "\033[" << clrs[b.grid[x][y]] << 'm' << b.grid[x][y] << ' ';
#else
            os << b.grid[x][y] - 1 << ' ';
#endif // _DEBUG
        }
        os << '\n';
    }
#ifdef _DEBUG
    os << "\033[0m";
#endif // _DEBUG
    return os;
}
size_t bestScore = 0;
vector<pair<int, int>> bestMoves;
vector<pair<int, int>> moves;
//auto startTime = std::chrono::system_clock::now();//implement one thread for timer and printing, one for solving?
int maxTime = 20000;//first turn 20s, dec by?
template<> struct std::hash<sq15> {//TODO to convert map to unordered_map
    size_t operator()(const sq15& grid) const {
        size_t res = 0;
        for (auto& row : grid)
            for (int e : row)
                res ^= std::hash<int>{}(e)+0x9e3779b9 + (res << 6) + (res >> 2);
        return res;
    }
};
std::unordered_map<sq15, pair<size_t, size_t>> transTable;
int solve(board& b) {
#ifdef _DEBUG
    ++solveCount;
#endif // _DEBUG
    auto posMoves = b.getConnectedList();
    if (b.score > bestScore) {
        bestMoves = moves;
        bestScore = b.score;
        //cerr << "new best score: " << bestScore << endl;//RIF
        cerr << "new best score: " << bestScore;
        for (auto& p : bestMoves)
            cerr << ' ' << p.first << ',' << p.second;
        cerr << endl;
    }
    if (posMoves.empty())
        return b.score;
    int bestBranchScore = 0;
    for (auto& move : posMoves) {
        board bCopy = b;
        bCopy.makeMove(move);
        auto itrBoolPair = transTable.try_emplace(bCopy.grid, std::make_pair(bCopy.score, 0));
        if (!itrBoolPair.second && itrBoolPair.first->second.second - itrBoolPair.first->second.first + bCopy.score <= bestScore)
            continue;
        moves.emplace_back(move[0].first, move[0].second);
        itrBoolPair.first->second.second = solve(bCopy);
        if (itrBoolPair.first->second.second > bestBranchScore)
            bestBranchScore = itrBoolPair.first->second.second;
        moves.pop_back();
        /*auto itr = transTable.find(bCopy.grid);
        if (itr == transTable.end()) {
            itr = transTable.try_emplace(bCopy.grid, std::make_pair(bCopy.score, 0)).first;
        }
        else {
            if (itr->second.second - itr->second.first + bCopy.score <= bestScore)
                continue;
        }
        moves.emplace_back(move[0].first, move[0].second);
        itr->second.second = solve(bCopy);
        if (itr->second.second > bestBranchScore)
            bestBranchScore = itr->second.second;
        moves.pop_back();*/
    }
    return bestBranchScore;
}

#include <string>
sq15 gridFromString(std::string str) {
    sq15 res{};
    int index = 0;
    for (int y = 15; y--;) {
		for (int x = 0; x < 15; ++x) {
            res[x][y] = str[index] - '0' + 1;
            index += 2;
		}
	}
    return res;
}

void disregardInput() {
    int discard;
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
			std::cin >> discard; std::cin.ignore();
		}
	}
}

int main() {



    board b{};
    b.grid = gridFromString("3 1 1 4 1 0 4 0 4 4 1 1 0 2 3\n3 3 2 0 4 4 1 3 1 2 0 0 4 0 4\n0 2 3 4 3 0 3 0 0 3 4 4 1 1 1\n2 3 4 0 2 3 0 2 4 4 4 3 0 2 3\n1 2 1 3 1 2 0 1 2 1 0 3 4 0 1\n0 4 4 3 0 3 4 2 2 2 0 2 3 4 0\n2 4 3 4 2 3 1 1 1 3 4 1 0 3 1\n1 0 0 4 0 3 1 2 1 0 4 1 3 3 1\n1 3 3 2 0 4 3 1 3 0 4 1 0 0 3\n0 3 3 4 2 3 0 0 2 1 2 3 4 0 1\n0 4 1 2 0 1 3 4 3 3 4 1 4 0 4\n2 2 3 1 0 4 0 1 2 4 1 3 3 0 1\n3 3 0 2 3 2 1 4 3 1 3 0 2 1 3\n1 0 3 2 1 4 4 4 4 0 4 2 1 3 4\n1 0 1 0 1 1 2 2 1 0 0 1 4 3 2");
    cerr << b << endl;



    return 0;


    //board b{};
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            std::cin >> b.grid[x][y]; std::cin.ignore();
            ++b.grid[x][y];//empty cell 0 instead of -1
        }
    }

#ifndef _DEBUG
    if (b.grid ==
        gridFromString("3 1 1 4 1 0 4 0 4 4 1 1 0 2 3\n3 3 2 0 4 4 1 3 1 2 0 0 4 0 4\n0 2 3 4 3 0 3 0 0 3 4 4 1 1 1\n2 3 4 0 2 3 0 2 4 4 4 3 0 2 3\n1 2 1 3 1 2 0 1 2 1 0 3 4 0 1\n0 4 4 3 0 3 4 2 2 2 0 2 3 4 0\n2 4 3 4 2 3 1 1 1 3 4 1 0 3 1\n1 0 0 4 0 3 1 2 1 0 4 1 3 3 1\n1 3 3 2 0 4 3 1 3 0 4 1 0 0 3\n0 3 3 4 2 3 0 0 2 1 2 3 4 0 1\n0 4 1 2 0 1 3 4 3 3 4 1 4 0 4\n2 2 3 1 0 4 0 1 2 4 1 3 3 0 1\n3 3 0 2 3 2 1 4 3 1 3 0 2 1 3\n1 0 3 2 1 4 4 4 4 0 4 2 1 3 4\n1 0 1 0 1 1 2 2 1 0 0 1 4 3 2")
        || b.grid ==
        gridFromString("0 3 3 1 3 4 1 4 1 1 3 3 4 2 0\n0 0 2 4 1 1 3 0 3 2 4 4 1 4 1\n4 2 0 1 0 4 0 4 4 0 1 1 3 3 3\n2 0 1 4 2 0 4 2 1 1 1 0 4 2 0\n3 2 3 0 3 2 4 3 2 3 4 0 1 4 3\n4 1 1 0 4 0 1 2 2 2 4 2 0 1 4\n2 1 0 1 2 0 3 3 3 0 1 3 4 0 3\n3 4 4 1 4 0 3 2 3 4 1 3 0 0 3\n3 0 0 2 4 1 0 3 0 4 1 3 4 4 0\n4 0 0 1 2 0 4 4 2 3 2 0 1 4 3\n4 1 3 2 4 3 0 1 0 0 1 3 1 4 1\n2 2 0 3 4 1 4 3 2 1 3 0 0 4 3\n0 0 4 2 0 2 3 1 0 3 0 4 2 3 0\n3 4 0 2 3 1 1 1 1 4 1 2 3 0 1\n3 4 3 4 3 3 2 2 3 4 4 3 1 0 2")
        ) {
        cerr << "test case 1" << endl;
    }
    /*if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 2" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 3" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 4" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 5" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 6" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 7" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 8" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 9" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 10" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 11" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 12" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 13" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 14" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 15" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 16" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 17" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 18" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 19" << endl;
    }
    if (b.grid ==
        gridFromString()
        || b.grid ==
        gridFromString()
        ) {
        cerr << "test case 20" << endl;
    }*/
#endif // !_DEBUG







    cerr << '\n' << b << endl;
    board beforeSolve = b;
    solve(b);
    cerr << "best score: " << bestScore << endl;
#ifdef _DEBUG
    cerr << "solveCount: " << solveCount << endl;
#endif // _DEBUG
    cout << bestMoves[0].first << " " << bestMoves[0].second << endl;
    b = std::move(beforeSolve);
    b.makeMove(bestMoves[0]);
    for (int i = 1; i < bestMoves.size(); ++i) {//delay to max time, 2nd thread for solving in meantime?; longjmp instead of thread? std::move(moves) to temp var, replace moves with empty vec?
        disregardInput();
        cout << bestMoves[i].first << " " << bestMoves[i].second << endl;
        b.makeMove(bestMoves[i]);
    }
    cerr << '\n' << b << endl;

    while (true) {
        std::string yeem;
        std::cin >> yeem; std::cin.ignore();
        cerr << yeem;//input might be optimized out otherwise not sure
    }

    /*return 0;
    
    maxTime = 50;
    while (true) {
        disregardInput();
        startTime = std::chrono::system_clock::now();
        moves.clear();
        transTable.clear();
        beforeSolve = b;
        solve(b);
        b = std::move(beforeSolve);
        cout << bestMoves[0].first << " " << bestMoves[0].second << endl;
        b.makeMove(bestMoves[0]);
    }*/
}