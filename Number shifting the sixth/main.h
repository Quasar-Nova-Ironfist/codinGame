#pragma once
#include <vector>
#include <ostream>
struct tile {
	signed char x, y;
	std::vector<int> adj[4]{}; // 0 = up, 1 = right, 2 = down, 3 = left
	auto operator[](signed char i);
	tile(signed char x_, signed char y_);
};
struct mov {
	signed char x, y, dir;
};
std::ostream& operator<<(std::ostream& os, const mov& m);
std::vector<std::pair<short, short>> setupTilesAndGetActiveTiles();
template<> struct std::hash<std::vector<std::pair<short, short>>> {
	std::size_t operator()(std::vector<std::pair<short, short>> const& v) const;
};
void solve(std::vector<std::pair<short, short>> active, std::vector<mov> curMoves);
namespace graphContinuity {
	bool check(std::vector<std::pair<short, short>>& active);
	void verIteration(std::vector<std::pair<short, short>>& active, std::vector<bool>& vis, int t, int& n);
	void horIteration(std::vector<std::pair<short, short>>& active, std::vector<bool>& vis, int t, int& n);
}