#include <vector>
#include <set>
#include <array>
#include <fstream>

std::set<std::vector<std::vector<int>>> transTable;
std::vector<std::vector<int>> cur;
std::vector<std::array<int, 4>> moves;
const int dirMults[8] = { 0, 1, 0, -1 };// x: i, y: 3 - i; x, y, dir, times
std::ofstream transTableOutFile("D:/Downloads/transpose.txt", std::fstream::app);
int numbersRemaining;

bool solve();