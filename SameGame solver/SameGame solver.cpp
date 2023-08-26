#include "main.h"
#include <fstream>
#include <parallel_hashmap/phmap.h>
#define BS_THREAD_POOL_DISABLE_PAUSE
#define BS_THREAD_POOL_DSiABLE_ERROR_FORWARDING
#define BS_THREAD_POOL_YEAH_I_KNOW_WHAT_IM_DOING
#include <BS_thread_pool/full.hpp>
#include <future>
#include <Windows.h>

#include <chrono>

using std::vector; using std::pair; using std::array; using std::cout; using std::endl; using std::string;
using sq15 = array<array<int, 15>, 15>;

phmap::parallel_flat_hash_map <sq15, node*, phmap::Hash<sq15>, phmap::EqualTo<sq15>, std::allocator<std::pair<const sq15, node*>>, 4, std::mutex> transTable;
BS::thread_pool threadPool;

int main() {
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
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
	fout << endl;
	node root{};
	auto start = std::chrono::steady_clock::now();
	//populateMap(b, &root);
	threadPool.push_task(populateMap, b, &root);
	threadPool.wait_for_tasks();
	auto end = std::chrono::steady_clock::now();
	cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
	cout << "Table size: " << transTable.size() << '\n';

	populateMap(b, &root);
	cout << "Table size: " << transTable.size() << '\n';


	start = std::chrono::steady_clock::now();
	threadPool.tasks.push(std::bind(addScores, &root));
	threadPool.task_available_cv.notify_one();
	threadPool.wait_for_tasks();
	end = std::chrono::steady_clock::now();
	cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
	cout << "Best score: " << addScores(&root) << '\n';//test if remains same
	cout << "Best score: " << addScores(&root) << '\n';
	cout << "Best score: " << addScores(&root) << '\n';
	node* nodePtr = &root;
	while (!nodePtr->children.empty()) {
		cout << nodePtr->children.size() << endl;
		cout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
		fout << nodePtr->children[nodePtr->bestChildIndex] << ", ";
		nodePtr = nodePtr->children[nodePtr->bestChildIndex].nextNode;
	}
	cout << std::flush;
	fout.close();
	for (auto& pair : transTable)
		delete pair.second;
}


void populateMap(board& b, node* n) {
	vector<vector<pair<int, int>>> posMoves = b.getConnectedList();
	n->children.reserve(posMoves.size());
	for (int i = 0; i < posMoves.size(); ++i) {
		board bCopy = b;
		int gain = bCopy.makeMove(posMoves[i]);
		node* nodePtr = new node;
		auto itrBoolPair = transTable.try_emplace(bCopy.grid, nodePtr);
		n->children.emplace_back(posMoves[i][0], gain, itrBoolPair.first->second);
		if (itrBoolPair.second)
			threadPool.push_task(populateMap, std::ref(bCopy), itrBoolPair.first->second);
		else
			delete nodePtr;//delete required b/c entire map operation needs to be done in single try_emplace so that synchronization can be handled by map object on submap level
	}
}

void populateMap2(board& b, node* n) {
	vector<vector<pair<int, int>>> posMoves = b.getConnectedList();
	n->children.resize(posMoves.size());
	//threadPool.tasks_mutex.lock();
	for (int i = 0; i < posMoves.size(); ++i) {
		//threadPool.tasks.push(std::bind(populateMapWorker, b, n, std::move(posMoves[i]), i));
		//auto task = std::bind(populateMapWorker, b, n, std::move(posMoves[i]), i);
		//std::function<void()> taskF = std::bind(&populateMapWorker, b, n, std::move(posMoves[i]), i);
		//populateMapWorker(b, n, std::move(posMoves[i]), i);
		//threadPool.task_available_cv.notify_one();
	}
	//threadPool.tasks_mutex.unlock();
	//for (int i = 0; i < posMoves.size(); ++i)
		//threadPool.task_available_cv.notify_one();
	////threadPool.task_available_cv.notify_all();
}
void populateMapWorker(board b, node* n, vector<pair<int, int>> move, int i) {
	int gain = b.makeMove(move);
	node* nodePtr = new node;
	auto itrBoolPair = transTable.try_emplace(b.grid, nodePtr);
	n->children[i] = { move[0], gain, itrBoolPair.first->second };
	if (itrBoolPair.second)
		populateMap(b, itrBoolPair.first->second);
	else
		delete nodePtr;//delete required b/c entire map operation needs to be done in single try_emplace so that synchronization can be handled by map object on submap level
}
/*int addScores(node* n) {//this may be faster than the multithreaded version
	if (n->children.empty())
		return 0;
	if (n->bestChildIndex != -1)
		return n->children[n->bestChildIndex].scoreGain;
	n->bestChildIndex = 0;
	for (auto& child : n->children)
		child.scoreGain += addScores(child.nextNode);
	for (int i = 1; i < n->children.size(); ++i)
		if (n->children[i].scoreGain > n->children[n->bestChildIndex].scoreGain)
			n->bestChildIndex = i;
	return n->children[n->bestChildIndex].scoreGain;
}*/
int addScores(node* n) {
	if (n->children.empty())
		return 0;
	n->scoreAddLock.lock();
	if (n->bestChildIndex != -1) {
		n->scoreAddLock.unlock();
		return n->children[n->bestChildIndex].scoreGain;
	}
	n->bestChildIndex = 0;
	vector<std::future<void>> futures;
	futures.reserve(n->children.size());//repace with clearing of vector assigned by thread id?
	for (auto& child : n->children) {
		threadPool.tasks_mutex.lock();
		if (threadPool.tasks_running + threadPool.tasks.size() < 11) {
			std::shared_ptr<std::promise<void>> task_promise = std::make_shared<std::promise<void>>();
			threadPool.tasks.push([&child, task_promise] {
				child.scoreGain += addScores(child.nextNode);
				task_promise->set_value();
				});
			threadPool.tasks_mutex.unlock();
			threadPool.task_available_cv.notify_one();
			futures.push_back(task_promise->get_future());
		}
		else {
			threadPool.tasks_mutex.unlock();
			child.scoreGain += addScores(child.nextNode);
		}
	}
	for (std::future<void>& future : futures)
		future.wait();
	for (int i = 1; i < n->children.size(); ++i)
		if (n->children[i].scoreGain > n->children[n->bestChildIndex].scoreGain)
			n->bestChildIndex = i;
	n->scoreAddLock.unlock();
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