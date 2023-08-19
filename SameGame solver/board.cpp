#include "board.h"
void board::setConnected(std::vector<std::pair<int, int>>& block, int x, int y, int color) {
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
int board::makeMove(std::vector<std::pair<int, int>>& move) {
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
    return (move.size() - 2) * (move.size() - 2) + (grid[0][0] ? 0 : 1000);
}
std::vector<std::vector<std::pair<int, int>>> board::getConnectedList() {
    std::vector<std::vector<std::pair<int, int>>> res;
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
std::ostream& operator<<(std::ostream& os, const board& b) {
    int clrs[] = { 0, 31, 32, 34, 33, 35 };
    for (int y = 15; y--;) {
        for (int x = 0; x < 15; ++x) {
            os << "\033[" << clrs[b.grid[x][y]] << 'm' << b.grid[x][y] << ' ';
        }
        os << '\n';
    }
    os << "\033[0m";
    return os;
}