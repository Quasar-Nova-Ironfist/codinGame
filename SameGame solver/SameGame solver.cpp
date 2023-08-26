#include "main.h"
#include "threadPool.h"
#include <fstream>
#include <parallel_hashmap/phmap.h>
#include <chrono>

using std::vector; using std::pair; using std::array; using std::cout; using std::endl; using std::string;
using sq15 = array<array<int, 15>, 15>;

phmap::parallel_flat_hash_map <sq15, node*, phmap::Hash<sq15>, phmap::EqualTo<sq15>, std::allocator<std::pair<const sq15, node*>>, 4, std::mutex> transTable;
threadPool pool(12);

int main() {
	board b{};
	string input;
	std::getline(std::cin, input);
	b.grid = gridFromString(input);
	std::ofstream fout("D:/Downloads/same game solver output.txt");
	cout << '\n' << b << endl;
	fout << input << '\n';
	for (int y = 15; y--;) {
		for (int x = 0; x < 15; ++x) {
			fout << b.grid[x][y] << ' ';
		}
		fout << '\n';
	}
	node root{};
	auto start = std::chrono::steady_clock::now();
	populateMap(b, &root);
	pool.wait_for_tasks();
	auto end = std::chrono::steady_clock::now();
	cout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds\n";
	fout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds\n";
	cout << "Table size: " << transTable.size() << '\n';
	fout << "Table size: " << transTable.size() << '\n';
	cout << "Best score: " << addScores(&root) << '\n';
	fout << "Best score: " << addScores(&root) << '\n';

	node* nodePtr = &root;
	while (!nodePtr->children.empty()) {
		cout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
		fout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
		nodePtr = nodePtr->children[nodePtr->bestChildIndex].nextNode;
	}
	cout << std::flush;
	fout.close();
	for (auto& pair : transTable)
		delete pair.second;
}
bool supercavitating(board& b, std::vector<std::pair<int, int>>& move) {//revise into more general check for if the move affects only the tiles of the move and no others
	if (move.size() == 2) {
		if (move[0].first == move[1].first) {
			if (move[0].second > move[1].second)
				std::swap(move[0].second, move[1].second);
			if (!move[0].second)
				return false;
			if (move[1].second == 14 || !b.grid[move[1].first][move[1].second + 1])
				return true;
		}
		else {
			if (!move[0].second)
				return false;
			if (move[0].second == 14 || (!b.grid[move[0].first][move[0].second + 1] && !b.grid[move[1].first][move[0].second + 1]))
				return true;
		}
	}
	return false;
}
bool individualTilePreventsSupercavitation(board& b, std::pair<int, int> tile) {//not working
	if (!tile.second)
		return true;
	int color = b.grid[tile.first][tile.second];
	for (int y = tile.second + 1; y < 15; ++y) {
		if (!b.grid[tile.first][y])
			return false;
		if (b.grid[tile.first][y] != color)
			return true;
	}
	return false;
}
void populateMap(board& b, node* n) {
	vector<vector<pair<int, int>>> posMoves = b.getConnectedList();
	n->children.resize(posMoves.size());
	for (int i = 0; i < n->children.size(); ++i) {
		if (i && supercavitating(b, posMoves[i])) {
			n->children.pop_back();
			--i;
			continue;
		}
		/*if (i) {
			for (auto& pair : posMoves[i])
				if (individualTilePreventsSupercavitation(b, pair))
					goto doesNotSupercavitate;
			n->children.pop_back();
			--i;
			continue;
		}*/
		doesNotSupercavitate:
		board bCopy = b;
		pool.tasks_mutex.lock();
		pool.tasks.emplace_back(std::move(bCopy), n, std::move(posMoves[i]), i);
		pool.tasks_mutex.unlock();
		pool.task_available_cv.notify_one();
	}
}
void populateMapWorker(populateMapWorkerArgs& args) {
	int gain = args.b.makeMove(args.move);
	node* nodePtr = new node;
	auto itrBoolPair = transTable.try_emplace(args.b.grid, nodePtr);
	args.n->children[args.i] = { args.move[0], gain, itrBoolPair.first->second };
	if (itrBoolPair.second) {
		populateMap(args.b, itrBoolPair.first->second);
	}
	else
		delete nodePtr;
}
int addScores(node* n) {
	if (n->children.empty())
		return 0;
	if (n->bestChildIndex != -1)
		return n->children[n->bestChildIndex].scoreGain;
	n->bestChildIndex = 0;
	for (auto& child : n->children) {
		child.scoreGain += addScores(child.nextNode);
		if (child.scoreGain > n->children[n->bestChildIndex].scoreGain)
			n->bestChildIndex = &child - &n->children[0];
	}
	return n->children[n->bestChildIndex].scoreGain;
}
std::ostream& operator<<(std::ostream& os, const nodeToNodeMove& n) {
	os << n.move.first << ',' << n.move.second;
	return os;
}
sq15 gridFromString(std::string str) {
	sq15 res{};
	int index = 0;
	for (int y = 15; y--;) {
		for (int x = 0; x < 15; ++x) {
			while (str[index] < '0' || str[index] > '9')
				++index;
			res[x][y] = str[index++] - '0' + 1;
		}
	}
	return res;
}