#include <vector>
#include <set>
#include <atomic>
#include <mutex>
bool checkIfRemovingCardinallyIsolates(std::vector<std::vector<int>>& cur, int x, int y);
void trimGrid(std::vector<std::vector<int>>& cur);
bool outputToFileAndReturnTrue(std::vector<std::array<int, 4>>& moves);

extern std::set<std::vector<std::vector<int>>> transTable;
extern std::atomic_bool stopSearch;
extern std::mutex transTableMutex;
extern int resultOffsetX, resultOffsetY;