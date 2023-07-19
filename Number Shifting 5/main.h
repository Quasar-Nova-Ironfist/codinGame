#pragma once
#include <vector>
#include <array>
struct solveState {
	std::vector<std::vector<int>> cur;
	std::vector<std::array<int, 4>> moves;//x, y, dir, times
	std::vector<std::pair<int, int>> non0s;
	bool solve();
};
bool checkIfRemovingCardinallyIsolates(std::vector<std::vector<int>>& cur, int x, int y);
void trimGrid(std::vector<std::vector<int>>& cur);
bool outputToFileAndReturnTrue(std::vector<std::array<int, 4>>& moves);
bool tryInsert(std::vector<std::vector<int>>& cur, size_t non0s);