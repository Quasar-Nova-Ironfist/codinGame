#include "main.h"
#include <string>
#include <random>
#include <parallel_hashmap/phmap.h>
#include <Windows.h>
using std::cout; using std::vector;
using fint = int_fast8_t;

std::vector<link> links;
size_t hash = 0;
std::vector<node> nodes;
fint nodesLeft;
template<> struct std::hash<vector<fint>> { size_t operator()(const vector<fint>& h) { return ::hash; } };
phmap::parallel_flat_hash_set<vector<fint>> transTable;

std::ostream& operator<<(std::ostream& os, const node& n) {
	os << static_cast<int>(n.x) << ',' << static_cast<int>(n.y);
	return os;
}
node::node(int n_, int x_, int y_) : num(n_), x(x_), y(y_) {}
link::link(node* a_, node* b_, size_t b0, size_t b1) : a(a_), b(b_) { bitStrings[0] = b0; bitStrings[1] = b1; }
node* link::getOther(node* n) { return a == n ? b : a; }
void solve() {
	if (!nodesLeft) {
		for (auto& l : links)
			if (l.num)
				cout << *l.a << ',' << *l.b << ',' << static_cast<int>(l.num) << ", ";
		cout << std::endl;
		while (true) 
			Sleep(999999999);
	}
	bool crossesActiveBefore[15]{};
	for (fint numLinks = 3; --numLinks;) {
		for (auto& l : links) {
			if (l.num || l.crossesActive || numLinks > l.a->num || numLinks > l.b->num)
				continue;
			l.num = numLinks;
			hash ^= l.bitStrings[numLinks - 1];
			if (transTable.emplace(getLinkAmounts()).second) {
				l.a->num -= numLinks;
				l.b->num -= numLinks;
				nodesLeft -= (l.a->num == 0) + (l.b->num == 0);
				for (fint i = 0; i < l.crosses.size(); ++i) {
					crossesActiveBefore[i] = l.crosses[i]->crossesActive;
					l.crosses[i]->crossesActive = true;
				}
				solve();
				for (fint i = 0; i < l.crosses.size(); ++i)
					l.crosses[i]->crossesActive = crossesActiveBefore[i];
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
void createLinks(){
	int_fast16_t width, height;
	std::cin >> width; std::cin.ignore(); std::cin >> height; std::cin.ignore();
	vector<vector<node*>> grid(width, vector<node*>(height, nullptr));
	nodes.reserve(width * height);
	for (fint y = 0; y < height; ++y) {
		std::string line;
		getline(std::cin, line);
		for (fint x = 0; x < width; ++x) {
			if (line[x] != '.') {
				nodes.emplace_back(line[x] - '0', x, y);
				grid[x][y] = &nodes.back();
			}
		}
	}
	links.reserve(nodes.size() * 4);
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
}
void listCrosses() {//FIXME
	for (auto& l0 : links) {//establish lists of crossing links
		if (l0.a->y == l0.b->y)//continue on horizontal
			continue;
		for (auto& l1 : links) {
			if (l1.a->x == l1.b->x)//continue on vertical
				continue;
			//l0 vertical, l1 horizontal
			if ((l0.a->y < l1.a->y) && (l0.b->y > l1.a->y) && (l1.a->x < l0.a->x) && (l1.b->x > l0.a->x)) {
				l0.crosses.push_back(&l1);
				l1.crosses.push_back(&l0);
				cout << &l0 - &links[0] << " : " << &l1 - &links[0] << "; " << *l0.a << ' ' << *l0.b << " : " << *l1.a << ' ' << *l1.b << '\n';
			}
		}
	}
	cout << '\n';
}
void collapseRequired() {
	bool goBackToFullNodeLoop = false;
	fullNodeLoop:
	for (fint nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex) {
		node& n = nodes[nodeIndex];
		if (n.linkCount == 1) {
			link* l = n.links[0];
			while (!l->crosses.empty())
				removeLink(l->crosses[0]);
			node* oNode = l->getOther(&n);
			cout << n << ',' << *oNode << ',' << static_cast<int>(n.num) << ", ";
			if (!(oNode->num -= n.num))
				removeNode(oNode);
			removeNode(&n);
			nodeIndex = -1;
			continue;
		}
		fint linksAvailableSum = 0;
		for (fint i = 0; i < n.linkCount; ++i) {
			fint linkAmount = n.links[i]->getOther(&n)->num;
			if (linkAmount > 2)
				linkAmount = 2;
			linksAvailableSum += linkAmount;
		}
		if (n.num == linksAvailableSum) {
			for (fint i = 0; i < n.linkCount; ++i) {
				link* l = n.links[i];
				if (l->getOther(&n)->num < 0)
					continue;
				while (!l->crosses.empty())
					removeLink(l->crosses[0]);
				node* oNode = l->getOther(&n);
				fint linkAmount = oNode->num;
				if (linkAmount > 2)
					linkAmount = 2;
				cout << n << ',' << *oNode << ',' << static_cast<int>(linkAmount) << ", ";
				if (!(oNode->num = linkAmount - oNode->num)) {
					removeNode(oNode);
					i = -1;
				}
			}
			for (fint i = 0; i < n.linkCount; ++i)
				n.links[i]->getOther(&n)->num *= -1;
			removeNode(&n);
			nodeIndex = -1;
			continue;
		}
	}
	for (node& n : nodes) {
		if ((n.linkCount == 2 && n.num > 2) || (n.linkCount == 3 && n.num > 4) || (n.linkCount == 4 && n.num > 6)) {
			for (fint i = 0; i < n.linkCount; ++i) {
				link* l = n.links[i];
				node* oNode = l->getOther(&n);
				oNode->num -= 1;
				cout << n << ',' << *oNode << ',' << static_cast<int>(1) << ", ";
				while (!l->crosses.empty())
					removeLink(l->crosses[0]);
			}
			n.num -= n.linkCount;
			goBackToFullNodeLoop = true;
			break;
		}
	}
	if (goBackToFullNodeLoop) {
		goBackToFullNodeLoop = false;
		goto fullNodeLoop;
	}
}
int main() {
	createLinks();
	listCrosses();
	collapseRequired();
	nodesLeft = nodes.size();
	solve();
}
void removeNode(node* n) {
	while (n->linkCount)
		removeLink(n->links[0]);
	for (fint i = 0; i < nodes.back().linkCount; ++i) {
		link* l = nodes.back().links[i];
		if (&nodes.back() == l->a)
			l->a = n;
		else
			l->b = n;
	}
	*n = std::move(nodes.back());
	nodes.pop_back();
}
void removeLink(link* l){
	{//remove pointers to l
		for (link* l2 : l->crosses)
			for (fint i = 0; i < l2->crosses.size(); ++i)
				if (l2->crosses[i] == l) {
					l2->crosses[i] = l2->crosses.back();
					l2->crosses.pop_back();
					break;
				}
		for (fint i = 0; i < 4; ++i)
			if (l->a->links[i] == l) {
				l->a->links[i] = l->a->links[--l->a->linkCount];
				break;
			}
		for (fint i = 0; i < 4; ++i)
			if (l->b->links[i] == l) {
				l->b->links[i] = l->b->links[--l->b->linkCount];
				break;
			}
	}//remove pointers to l
	{//update all pointers to links.back() to instead point at l
		for (link* l2 : links.back().crosses)
			for (fint i = 0; i < l2->crosses.size(); ++i)
				if (l2->crosses[i] == &links.back()) {
					l2->crosses[i] = l;
					break;
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
	}//update all pointers to links.back() to instead point at l
	*l = std::move(links.back());
	links.pop_back();
}