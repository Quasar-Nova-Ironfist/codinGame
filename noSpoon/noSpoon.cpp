#include "main.h"
#include <string>
#include <random>
#include <parallel_hashmap/phmap.h>
using std::cout; using std::endl; using std::vector;
using fint = int_fast8_t;

std::vector<link> links;
size_t hash = 0;
std::vector<node> nodes;
fint nodesLeft = 0;

template<> struct std::hash<vector<fint>> { size_t operator()(const vector<fint>& h) { return ::hash; } };
phmap::parallel_flat_hash_set<vector<fint>> transTable;

bool crossesActive(link& l){
	for (fint i = 0; i < l.crossCount; ++i)
		if (l.crosses[i]->num)
			return true;
	return false;
}
void solve() {
	if (!nodesLeft) {
		for (auto& l : links)
			if (l.num)
				std::cout << l.a->x << ',' << l.a->y << ',' << l.b->x << ',' << l.b->y << ',' << l.num << ", ";
		std::cout << std::endl;
		while (true)//prevent accidental exit
			std::cin >> hash;
	}
	for (fint numLinks = 3; --numLinks;) {
		for (auto& l : links) {
			if (l.num || numLinks > l.a->num || numLinks > l.b->num || crossesActive(l))
				continue;
			l.num = numLinks;
			hash ^= l.bitStrings[numLinks - 1];
			if (transTable.emplace(getLinkAmounts()).second) {
				l.a->num -= numLinks;
				l.b->num -= numLinks;
				nodesLeft -= (l.a->num == 0) + (l.b->num == 0);
				solve();
				nodesLeft += (l.a->num == 0) + (l.b->num == 0);
				l.b->num += numLinks;
				l.a->num += numLinks;
			}
			hash ^= l.bitStrings[numLinks - 1];
			l.num = 0;
		}
	}
}
vector<fint> getLinkAmounts() {
	vector<fint> res;
	res.reserve(links.size());
	for (auto& l : links)
		res.push_back(l.num);
	return res;
}
int main() {
	{//create links
		fint width, height;
		std::cin >> width; std::cin.ignore(); std::cin >> height; std::cin.ignore();
		vector<vector<node*>> grid(height, vector<node*>(width, nullptr));
		for (fint y = 0; y < height; ++y) {
			std::string line;
			getline(std::cin, line);
			for (fint x = 0; x < width; ++x) {
				if (line[x] != '.') {
					nodes.emplace_back(line[x] - '0', x, y);
					grid[x][y] = &nodes.back();
					++nodesLeft;
				}
			}
		}
		nodes.shrink_to_fit();
		links.reserve(nodes.size());
		std::mt19937_64 e2;
		std::uniform_int_distribution<size_t> distr(1, size_t(-2));
		for (auto& n : nodes) {
			for (fint nextX = n.x + 1; nextX < grid.size(); ++nextX) {
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
			for (fint nextY = n.y + 1; nextY < grid[0].size(); ++nextY) {
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
			fint linksAvailableSum = 0;
			for (fint i = 0; i < n.linkCount; ++i) {
				fint linkAmount = n.links[i]->getOther(&n)->num;
				if (linkAmount > 2)
					linkAmount = 2;
				linksAvailableSum += linkAmount;
			}
			if (n.num == linksAvailableSum) {
				for (fint i = 0; i < n.linkCount; ++i) {
					node* oNode = n.links[i]->getOther(&n);
					fint linkAmount = oNode->num;
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
void removeNode(node* n) {
	for (fint i = 0; i < n->linkCount; ++i) {
		link* l = n->links[i];
		{//update all pointers to links.back() to instead point at l
			for (fint j = 0; j < links.back().crossCount; ++j) {
				link* l2 = links.back().crosses[j];
				for (fint k = 0; k < l2->crossCount; ++k)
					if (l2->crosses[k] == &links.back()) {
						l2->crosses[k] = l;
						break;
					}
			}
			for (fint j = 0; j < 4; ++j)
				if (links.back().a->links[j] == &links.back()) {
					links.back().a->links[j] = l;
					break;
				}
			for (fint j = 0; j < 4; ++j)
				if (links.back().b->links[j] == &links.back()) {
					links.back().b->links[j] = l;
					break;
				}
		}//update all pointers to links.back() to instead point at l
		{//set pointers to l to null, except for those changed above
			for (fint j = 0; j < l->crossCount; ++j) {
				link* l2 = l->crosses[j];
				for (fint k = 0; k < l2->crossCount; ++k)
					if (l2->crosses[k] == l) {
						l2->crosses[k] = l2->crosses[l2->crossCount];
						l2->crosses[l2->crossCount--] = nullptr;
						break;
					}
			}
			node* oNode = l->getOther(n);
			for (fint j = 0; j < 4; ++j)
				if (oNode->links[j] == l) {
					oNode->links[j] = oNode->links[oNode->linkCount];
					oNode->links[oNode->linkCount--] = nullptr;
					break;
				}
		}//set pointers to l to null, except for those changed above
		*l = std::move(links.back());
		links.pop_back();
	}
	for (fint i = 0; i < nodes.back().linkCount; ++i) {
		link* l = nodes.back().links[i];
		if (&nodes.back() == l->a)
			l->a = n;
		else
			l->b = n;
	}
	*n = std::move(nodes.back());
	nodes.pop_back();
	--nodesLeft;
}
/*void removeLink(link* l){
	for (fint i = 0; i < 4; ++i)
		if (l->a->links[i] == l) {
			l->a->links[i] = l->a->links[l->a->linkCount];
			l->a->links[l->a->linkCount--] = nullptr;
			break;
		}
	for (fint i = 0; i < 4; ++i)
		if (l->b->links[i] == l) {
			l->b->links[i] = l->b->links[l->b->linkCount];
			l->b->links[l->b->linkCount--] = nullptr;
			break;
		}
	for (fint i = 0; i < l->crossCount; ++i) {
		link* l2 = l->crosses[i];
		for (fint j = 0; j < l2->crossCount; ++j)
			if (l2->crosses[j] == l) {
				l2->crosses[j] = l2->crosses[l2->crossCount];
				l2->crosses[l2->crossCount--] = nullptr;
				break;
			}
	}
	for (fint i = 0; i < 4; ++i)
		if (links.back().a->links[i] == &links.back()) {
			links.back().a->links[i] = l;
			break;
		}
	for (fint i = 0; i < 4; ++i)
		if (links.back().b->links[i] == &links.back()) {
			links.back().b->links[i] = l;
			break;
		}
	*l = std::move(links.back());
	links.pop_back();
}*/