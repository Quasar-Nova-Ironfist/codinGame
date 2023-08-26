#pragma once
#include <vector>
#include <array>
#include <ostream>
class board {
    void setConnected(std::vector<std::pair<int, int>>& block, int x, int y, int color);
public:
    std::array<std::array<int, 15>, 15> grid{};
    int pastMaxX = 15, pastMaxY = 15;
    std::vector<std::vector<std::pair<int, int>>> getConnectedList();
    int makeMove(std::vector<std::pair<int, int>>& move);
    board() {}
    board(board& other) {
        grid = other.grid;
        pastMaxX = other.pastMaxX;
        pastMaxY = other.pastMaxY;
    }
    board(board&& other) {
        grid = std::move(other.grid);
		pastMaxX = other.pastMaxX;
		pastMaxY = other.pastMaxY;
    }
    board& operator=(const board& other) {
		grid = other.grid;
		pastMaxX = other.pastMaxX;
		pastMaxY = other.pastMaxY;
		return *this;
	}
};
std::ostream& operator<<(std::ostream& os, const board& b);