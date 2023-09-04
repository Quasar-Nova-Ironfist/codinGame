#include "main.h"
#include <string>
#include <random>
#include <parallel_hashmap/phmap.h>
using std::cout; using std::endl; using std::vector;

std::vector<link> links;
size_t hash = 0;
std::vector<node> nodes;
int nodesFull = 0;

template<> struct std::hash<vector<int>> { size_t operator()(const vector<int>& h) { return ::hash; } };
phmap::parallel_flat_hash_set<vector<int>> transTable;

void solve() {
	if (nodesFull == nodes.size()) {
		for (auto& l : links)
			if (l.n)
				std::cout << l.a->x << ',' << l.a->y << ',' << l.b->x << ',' << l.b->y << ',' << l.n << ", ";
		std::cout << std::endl;
		while (true)//prevent accidental exit
			std::cin >> hash;
	}
	for (int numLinks = 3; --numLinks;) {
		for (auto& l : links) {
			if (l.n || l.a->t + numLinks > l.a->n || l.b->t + numLinks > l.b->n)
				continue;
			l.n = numLinks;
			hash ^= l.bitStrings[numLinks - 1];
			if (transTable.emplace(getLinkAmounts()).second) {
				l.a->t += numLinks;
				l.b->t += numLinks;
				nodesFull += (l.a->t == l.a->n) + (l.b->t == l.b->n);
				solve();
				nodesFull -= (l.a->t == l.a->n) + (l.b->t == l.b->n);
				l.b->t -= numLinks;
				l.a->t -= numLinks;
			}
			hash ^= l.bitStrings[numLinks - 1];
			l.n = 0;
		}
	}
}
vector<int> getLinkAmounts() {
	vector<int> res;
	res.reserve(links.size());
	for (auto& l : links)
		res.push_back(l.n);
	return res;
}
int main() {
	{//create links
		int width, height;
		std::cin >> width; std::cin.ignore(); std::cin >> height; std::cin.ignore();
		vector<vector<node*>> grid(height, vector<node*>(width, nullptr));
		for (int y = 0; y < height; ++y) {
			std::string line;
			getline(std::cin, line);
			for (int x = 0; x < width; ++x) {
				if (line[x] != '.') {
					nodes.emplace_back(line[x] - '0', x, y);
					grid[x][y] = &nodes.back();
				}
			}
		}
		nodes.shrink_to_fit();
		links.reserve(nodes.size());
		std::mt19937_64 e2;
		std::uniform_int_distribution<size_t> distr(1, size_t(-2));
		for (auto& n : nodes) {
			for (int nextX = n.x + 1; nextX < grid.size(); ++nextX) {
				if (grid[nextX][n.y] != nullptr) {
					node* nPtr = grid[nextX][n.y];
					links.emplace_back(&n, nPtr, distr(e2), distr(e2));
					n.links[n.linkCount++] = &links.back();
					nPtr->links[nPtr->linkCount++] = &links.back();
					break;
				}
			}
			for (int nextY = n.y + 1; nextY < grid[0].size(); ++nextY) {
				if (grid[n.x][nextY] != nullptr) {
					node* nPtr = grid[n.x][nextY];
					links.emplace_back(&n, nPtr, distr(e2), distr(e2));
					n.links[n.linkCount++] = &links.back();
					nPtr->links[nPtr->linkCount++] = &links.back();
					break;
				}
			}
		}
		links.shrink_to_fit();
	}//create links
	{//move applicable links to solvedLinks
		bool anyFound = false;
	moveToSolved_Start:
		for (int i = 0; i < nodes.size(); ++i) {
			if (nodes[i].n == nodes[i].linkCount * 2) {
				for (int j = 0; j < nodes[i].linkCount; ++i)
					moveLinkToSolved(nodes[i], j, 2);
				nodes[i] = nodes.back();
				nodes.pop_back();
				anyFound = true;
			}
			else if (nodes[i].linkCount == 1) {
				moveLinkToSolved(nodes[i], 0, 1);
				nodes[i] = nodes.back();
				nodes.pop_back();
				anyFound = true;
			}
		}
		if (anyFound) {
			anyFound = false;
			goto moveToSolved_Start;
		}
	}//move applicable links to solvedLinks
	solve();
}
void removeLinkFromNode(node* n, link* l) {
	for (int i = 0; i < 4; ++i)
		if (n->links[i] == l) {
			n->links[i] = n->links[n->linkCount];
			n->links[n->linkCount--] = nullptr;
			return;
		}
}
void moveLinkToSolved(node& n, int linkIndex, int linkAmount) {
	link* l = n.links[linkIndex];
	node* otherNode = (l->a == &n ? l->b : l->a);
	std::cout << n.x << ',' << n.y << ',' << otherNode->x << ',' << otherNode->y << ',' << linkAmount << ", ";
	removeLinkFromNode(otherNode, l);
	links[linkIndex] = links.back();
	links.pop_back();
	if (!(otherNode->n -= linkAmount)) {
		for (int i = 0; i < otherNode->linkCount; ++i)
			removeLinkFromNode(otherNode->links[i]->a == otherNode ? otherNode->links[i]->b : otherNode->links[i]->a, otherNode->links[i]);
		nodes[otherNode - &nodes[0]] = nodes.back();
		nodes.pop_back();
	}
}