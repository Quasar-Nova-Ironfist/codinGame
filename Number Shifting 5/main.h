#pragma once
#include <vector>
#include <array>
struct solveState {
	std::vector<std::vector<int>> cur;
	std::vector<std::array<int, 4>> moves;//x, y, dir, times
	std::vector<std::pair<int, int>> non0s;
	bool solve();
	bool checkIfRemovingCardinallyIsolates(std::pair<int, int>& pos);
	int isolates(int x, int y);
	void _isolationIteration(std::vector<std::vector<char>>& vis, int& found, int x, int y);
	bool outputToFileAndReturnTrue();
	bool tryInsert();
	solveState() {}
	solveState(solveState& other);
};
void trimGrid(std::vector<std::vector<int>>& cur);