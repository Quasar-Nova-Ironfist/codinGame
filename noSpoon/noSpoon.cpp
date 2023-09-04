#include "main.h"
#include <string>
#include <random>
using std::cout; using std::endl; using std::vector;

int main() {
	board b;
	{//create links
		int width, height;
		std::cin >> width; std::cin.ignore(); std::cin >> height; std::cin.ignore();
		vector<vector<board::node*>> grid(height, vector<board::node*>(width, nullptr));
		for (int y = 0; y < height; ++y) {
			std::string line;
			getline(std::cin, line);
			for (int x = 0; x < width; ++x) {
				if (line[x] != '.') {
					b.nodes.emplace_back(line[x] - '0', x, y);
					grid[x][y] = &b.nodes.back();
				}
			}
		}
		b.nodes.shrink_to_fit();
		b.links.reserve(b.nodes.size());
		std::mt19937_64 e2;
		std::uniform_int_distribution<size_t> distr(1, size_t(-2));
		for (auto& n : b.nodes) {
			for (int nextX = n.x + 1; nextX < grid.size(); ++nextX) {
				if (grid[nextX][n.y] != nullptr) {
					board::node* nPtr = grid[nextX][n.y];
					b.links.emplace_back(&n, nPtr, distr(e2), distr(e2));
					n.links[n.linkCount++] = &b.links.back();
					nPtr->links[nPtr->linkCount++] = &b.links.back();
					break;
				}
			}
			for (int nextY = n.y + 1; nextY < grid[0].size(); ++nextY) {
				if (grid[n.x][nextY] != nullptr) {
					board::node* nPtr = grid[n.x][nextY];
					b.links.emplace_back(&n, nPtr, distr(e2), distr(e2));
					n.links[n.linkCount++] = &b.links.back();
					nPtr->links[nPtr->linkCount++] = &b.links.back();
					break;
				}
			}
		}
		b.links.shrink_to_fit();
	}//create links
	{//move applicable links to solvedLinks
		bool anyFound = false;
	moveToSolved_Start:
		for (int i = 0; i < b.nodes.size(); ++i) {
			if (b.nodes[i].n == b.nodes[i].linkCount * 2) {
				for (int j = 0; j < b.nodes[i].linkCount; ++i)
					moveLinkToSolved(b, b.nodes[i], j, 2);
				b.nodes[i] = b.nodes.back();
				b.nodes.pop_back();
				anyFound = true;
			}
			else if (b.nodes[i].linkCount == 1) {
				moveLinkToSolved(b, b.nodes[i], 0, 1);
				b.nodes[i] = b.nodes.back();
				b.nodes.pop_back();
				anyFound = true;
			}
		}
		if (anyFound) {
			anyFound = false;
			goto moveToSolved_Start;
		}
	}//move applicable links to solvedLinks
	b.solve();
}
void removeLinkFromNode(board& b, board::node* n, board::link* l) {
	for (int i = 0; i < 4; ++i)
		if (n->links[i] == l) {
			n->links[i] = n->links[n->linkCount];
			n->links[n->linkCount--] = nullptr;
			return;
		}
}
void moveLinkToSolved(board& b, board::node& n, int linkIndex, int linkAmount) {
	board::link* l = n.links[linkIndex];
	board::node* otherNode = (l->a == &n ? l->b : l->a);
	std::cout << n.x << ',' << n.y << ',' << otherNode->x << ',' << otherNode->y << ',' << linkAmount << ", ";
	removeLinkFromNode(b, otherNode, l);
	b.links[linkIndex] = b.links.back();
	b.links.pop_back();
	if (!(otherNode->n -= linkAmount)) {
		for (int i = 0; i < otherNode->linkCount; ++i)
			removeLinkFromNode(b, otherNode->links[i]->a == otherNode ? otherNode->links[i]->b : otherNode->links[i]->a, otherNode->links[i]);
		b.nodes[otherNode - &b.nodes[0]] = b.nodes.back();
		b.nodes.pop_back();
	}
}