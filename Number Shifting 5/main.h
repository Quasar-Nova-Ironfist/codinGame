#pragma once
#include <vector>
#include <array>
struct solveState {
	std::vector<std::vector<int>> cur;
	std::vector<std::array<int, 4>> moves;//x, y, dir, times
	std::vector<std::pair<int, int>> non0s;
	bool solve();
	bool checkIfRemovingCardinallyIsolates(int x, int y);
	bool outputToFileAndReturnTrue();
	bool tryInsert();
};
void trimGrid(std::vector<std::vector<int>>& cur);