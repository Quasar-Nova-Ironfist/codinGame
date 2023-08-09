#pragma once
#include <vector>
#include <array>
struct solveState {
	std::vector<std::vector<std::array<int, 5>>> cur;
	std::vector<std::array<int, 4>> moves;//x, y, dir, times
	std::vector<std::pair<int, int>> tiles;
	int non0s;
	bool solve();
	bool isolates(std::pair<int, int> pos);
	std::vector<std::vector<char>> _isolationCheckVisisted;//avoid allocating all this space every time it checks if removing a tile isolates others
	int _isolationCheckFoundCount;//might as well have this here since ^ anyway, rather than passing refferences to it
	void _isolationIterationUpDown(int x);
	void _isolationIterationLeftRight(int y);
	bool outputToFileAndReturnTrue();
	bool tryInsert();
	solveState() {}
	solveState(solveState& other);
};
void trimGrid(std::vector<std::vector<int>>& cur);