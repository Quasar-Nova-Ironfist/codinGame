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
			if (l.num)
				std::cout << l.a->x << ',' << l.a->y << ',' << l.b->x << ',' << l.b->y << ',' << l.num << ", ";
		std::cout << std::endl;
		while (true)//prevent accidental exit
			std::cin >> hash;
	}
	for (int numLinks = 3; --numLinks;) {
		for (auto& l : links) {
			if (l.num || l.a->t + numLinks > l.a->num || l.b->t + numLinks > l.b->num)
				continue;
			l.num = numLinks;
			hash ^= l.bitStrings[numLinks - 1];
			if (transTable.emplace(getLinkAmounts()).second) {
				l.a->t += numLinks;
				l.b->t += numLinks;
				nodesFull += (l.a->t == l.a->num) + (l.b->t == l.b->num);
				solve();
				nodesFull -= (l.a->t == l.a->num) + (l.b->t == l.b->num);
				l.b->t -= numLinks;
				l.a->t -= numLinks;
			}
			hash ^= l.bitStrings[numLinks - 1];
			l.num = 0;
		}
	}
}
vector<int> getLinkAmounts() {
	vector<int> res;
	res.reserve(links.size());
	for (auto& l : links)
		res.push_back(l.num);
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
					if (n.num != 1 || nPtr->num != 1) {
						links.emplace_back(&n, nPtr, distr(e2), distr(e2));
						n.links[n.linkCount++] = &links.back();
						nPtr->links[nPtr->linkCount++] = &links.back();
					}
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
	{//establish lists of crossing links; o(n^2)
		for (auto& l0 : links) {
			for (auto& l1 : links) {
				if (&l0 == &l1)
					continue;
				bool l0Vert = l0.a->x == l0.b->x;
				if (l0Vert == (l1.a->x == l1.b->x))
					continue;
				if (l0Vert) {
					if (l0.a->y < l1.a->y && l0.b->y > l1.a->y && l1.a->x < l0.a->x && l1.b->x > l1.a->x) {
						l0.crosses[l0.crossCount++] = &l1;
						cout << "l0 crosses l1; " << l0.a->x << ',' << l0.a->y << ' ' << l0.b->x << ',' << l0.b->y << " : " << l1.a->x << ',' << l1.a->y << ' ' << l1.b->x << ',' << l1.b->y << endl;
					}//RIF^
				}
				else {
					if (l1.a->y < l0.a->y && l1.b->y > l0.a->y && l0.a->x < l1.a->x && l0.b->x > l0.a->x) {
						l0.crosses[l0.crossCount++] = &l1;
						cout << "l0 crosses l1; " << l0.a->x << ',' << l0.a->y << ' ' << l0.b->x << ',' << l0.b->y << " : " << l1.a->x << ',' << l1.a->y << ' ' << l1.b->x << ',' << l1.b->y << endl;
					}
				}
			}
		}
	}//establish lists of crossing links

	return 0;

	{//collapse required links
		bool anyFound = false;
	moveToSolved_Start:
		for (auto& n : nodes) {
			int linksAvailableSum = 0;
			for (int i = 0; i < n.linkCount; ++i) {
				int linkAmount = n.links[i]->getOther(&n)->num;
				if (linkAmount > 2)
					linkAmount = 2;
				linksAvailableSum += linkAmount;
			}
			if (n.num == linksAvailableSum) {
				for (int i = 0; i < n.linkCount; ++i) {
					node* oNode = n.links[i]->getOther(&n);
					int linkAmount = oNode->num;
					if (linkAmount > 2)
						linkAmount = 2;
					cout << n.x << ',' << n.y << ',' << oNode->x << ',' << oNode->y << ',' << linkAmount << ", ";
					if (!(oNode->num -= linkAmount)) {
						removeNode(oNode);
						--i;
					}
				}
				removeNode(&n);
				anyFound = true;
			}
		}
		if (anyFound) {
			anyFound = false;
			goto moveToSolved_Start;
		}
	}//collapse required links
	//solve();
}
void removeNode(node* n){
	while(n->linkCount)
		removeLink(n->links[0]);
	for (int i = 0; i < nodes.back().linkCount; ++i) {
		link* l = nodes.back().links[i];
		if (&nodes.back() == l->a)
			l->a = n;
		else
			l->b = n;
	}
	*n = nodes.back();
	nodes.pop_back();
}
void removeLink(link* l){
	for (int i = 0; i < 4; ++i)
		if (l->a->links[i] == l) {
			l->a->links[i] = l->a->links[l->a->linkCount];
			l->a->links[l->a->linkCount--] = nullptr;
			break;
		}
	for (int i = 0; i < 4; ++i)
		if (l->b->links[i] == l) {
			l->b->links[i] = l->b->links[l->b->linkCount];
			l->b->links[l->b->linkCount--] = nullptr;
			break;
		}
	for (int i = 0; i < l->crossCount; ++i) {
		link* l2 = l->crosses[i];
		for (int j = 0; j < l2->crossCount; ++j)
			if (l2->crosses[j] == l) {
				l2->crosses[j] = l2->crosses[l2->crossCount];
				l2->crosses[l2->crossCount--] = nullptr;
				break;
			}
	}
	for (int i = 0; i < 4; ++i)
		if (links.back().a->links[i] == &links.back()) {
			links.back().a->links[i] = l;
			break;
		}
	for (int i = 0; i < 4; ++i)
		if (links.back().b->links[i] == &links.back()) {
			links.back().b->links[i] = l;
			break;
		}
	*l = std::move(links.back());
	links.pop_back();
}