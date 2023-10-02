//don't use back-popback b/c non0s_t hash function needs order by index 
#include "main.h"
#include <parallel_hashmap/phmap.h>
#define BS_THREAD_POOL_YEAH_I_KNOW_WHAT_IM_DOING
#include <BS_thread_pool/full.hpp>
using std::vector; using std::cout; using std::pair;
using fint = signed char;
using non0s_t = std::vector<std::pair<short, short>>;//index, tile value

phmap::parallel_flat_hash_set<non0s_t, phmap::Hash<non0s_t>, phmap::EqualTo<non0s_t>, std::allocator<non0s_t>, 4, std::mutex> transTable;
BS::thread_pool pool;
vector<tile> tiles;
namespace handoverToMainThread {
    std::condition_variable solutionFound;
    std::mutex solutionFoundMutex;
    vector<mov> moves;
}

size_t std::hash<non0s_t>::operator()(non0s_t const& v_) const {
    non0s_t v = v_;
    size_t ret = v.size();
    for (int i = 0; i < v.size(); ++i) {
        pair<size_t,size_t> p = v[i];
        p.first = ((p.first >> 16) ^ p.first) * 0x45d9f3b;
		p.first = ((p.first >> 16) ^ p.first) * 0x45d9f3b;
        p.first =  (p.first >> 16) ^ p.first;
		ret ^= p.first + 0x9e3779b9 + (ret << 6) + (ret >> 2);
        p.second = ((p.second >> 16) ^ p.second) * 0x45d9f3b;
        p.second = ((p.second >> 16) ^ p.second) * 0x45d9f3b;
        p.second =  (p.second >> 16) ^ p.second;
        ret ^= p.second + 0x9e3779b9 + (ret << 6) + (ret >> 2);
    }
    return ret;
}
tile::tile(fint x_, fint y_) : x(x_), y(y_) {}
auto tile::operator[](fint i) { return adj[i]; }
std::ostream& operator<<(std::ostream& os, const mov& m){
    os << m.x << ' ' << m.y;
    switch (abs(m.dir)) {
    case 1:
		os << " U";
		break;
    case 2:
        os << " R";
        break;
    case 3:
        os << " D";
        break;
    case 4:
        os << " L";
        break;
    }
    os << (m.dir > 0 ? " +\\n" : " -\\n");
    return os;
}
non0s_t setupTilesAndGetActiveTiles(){
    non0s_t activeTiles;
    size_t width, height;
    std::cin >> width >> height; std::cin.ignore();
    tiles.reserve(width * height);
    activeTiles.reserve(tiles.capacity());
    vector<vector<int>> indexGrid(width, vector<int>(height, -1));
    for (fint y = 0; y < height; y++) {
        for (fint x = 0; x < width; x++) {
            fint cell;
            std::cin >> cell; std::cin.ignore();
            if (!cell)
                continue;
            indexGrid[x][y] = tiles.size();
            activeTiles.emplace_back(tiles.size(), cell);
            tiles.emplace_back(x, y);
            for (fint y2 = y - 1; y2 >= 0; --y2)
                if (indexGrid[x][y2] != -1)
                    tiles.back()[0].push_back(indexGrid[x][y2]);
            for (fint x2 = x - 1; x2 >= 0; --x2)
                if (indexGrid [x2][y] != -1)
					tiles.back()[3].push_back(indexGrid[x2][y]);
        }
    }
    tiles.shrink_to_fit();
    return activeTiles;
}
int main() {
    while (true) {
        solve(setupTilesAndGetActiveTiles(), vector<mov>());
        std::unique_lock<std::mutex> lock(handoverToMainThread::solutionFoundMutex);
        handoverToMainThread::solutionFound.wait(lock);
        pool.pause();
        cout << '\n';
        for (mov& m : handoverToMainThread::moves)
            cout << m;
        cout << std::endl;
        pool.wait_for_tasks();
        pool.tasks_mutex.lock();
        std::queue<std::function<void()>>().swap(pool.tasks);
        pool.tasks_mutex.unlock();
        pool.unpause();
        transTable.clear();
        tiles.clear();
        handoverToMainThread::moves.clear();
        handoverToMainThread::moves.shrink_to_fit();
    }
}
bool graphContinuity::check(std::vector<std::pair<short, short>>& active){
    int sum = 0;
    vector<bool> vis(active.back().first + 1, false);
    verIteration(active, vis, active[0].first, sum);
    horIteration(active, vis, active[0].first, sum);
    return sum == active.size();;
}
void graphContinuity::verIteration(std::vector<std::pair<short, short>>& active, std::vector<bool>& vis, int t, int& n){
    vis[t] = true;
    ++n;
    for (int dir = 0; dir < 4; ++dir) {
        for (int i = 0; i < tiles[t][dir].size(); ++i) {
            int newIndex = tiles[t][dir][i];
            if (vis[newIndex])
			    continue;
            for (int j = 0; j < active.size(); ++j) {
                if (active[j].first == newIndex) {
				    horIteration(active, vis, newIndex, n);
				    break;
			    }
            }
        }
    }
}
void graphContinuity::horIteration(std::vector<std::pair<short, short>>& active, std::vector<bool>& vis, int t, int& n){

}
void solve(non0s_t active, vector<mov> curMoves) {
    if (active.empty()) {
        handoverToMainThread::solutionFoundMutex.lock();
        if (!handoverToMainThread::moves.empty()) {
            handoverToMainThread::solutionFoundMutex.unlock();
			return;
        }
        handoverToMainThread::moves = std::move(curMoves);
        handoverToMainThread::solutionFoundMutex.unlock();
        handoverToMainThread::solutionFound.notify_one();
    }
    




}