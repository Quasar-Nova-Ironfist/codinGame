#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <random>
using std::cerr; using std::endl; using std::vector;
struct link;
struct node { int n, x, y, t = 0, linkCount = 0; link* links[4]; };
struct link { node* a, * b; int n = 0; size_t bitStrings[2]; };
vector<link> links;
vector<link> solvedLinks;
vector<node> nodes;
size_t hash = 0;
template<>struct std::hash<vector<link>> { size_t operator()(const vector<link>& z) { return ::hash; } };
std::unordered_set<vector<link>> transTable;
int nodesFull = 0;
void solve() {
    if (nodesFull == nodes.size()) {
        for (auto& l : links)
            if (l.n)
                std::cout << l.a->x << ' ' << l.a->y << ' ' << l.b->x << ' ' << l.b->y << ' ' << l.n << std::endl;
        for (auto & l : solvedLinks)
            if (l.n)
				std::cout << l.a->x << ' ' << l.a->y << ' ' << l.b->x << ' ' << l.b->y << ' ' << l.n << std::endl;
        exit(0);
    }
    for (int numLinks = 3; --numLinks;) {
        for (auto& l : links) {
            if (l.n || l.a->t + numLinks > l.a->n || l.b->t + numLinks > l.b->n)
                continue;
            l.n = numLinks;
            hash ^= l.bitStrings[numLinks - 1];
            if (transTable.emplace(links).second) {
                l.a->t += numLinks;
                if (l.a->t == l.a->n)
                    ++nodesFull;
                l.b->t += numLinks;
                if (l.b->t == l.b->n)
                    ++nodesFull;
                solve();
                if (l.b->t == l.b->n)
                    --nodesFull;
                l.b->t -= numLinks;
                if (l.a->t == l.a->n)
                    --nodesFull;
                l.a->t -= numLinks;
            }
            hash ^= l.bitStrings[numLinks - 1];
            l.n = 0;
        }
    }
}
void setupLinks() {
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
    links.reserve(nodes.size() * 4);
    std::mt19937_64 e2;
    std::uniform_int_distribution<size_t> distr(1, size_t(-2));
    for (auto& n : nodes) {
        auto& p = grid[n.x][n.y];
        for (int nextX = n.x + 1; nextX < grid.size(); ++nextX) {
            if (grid[nextX][n.y] != nullptr) {
                node* nPtr = grid[nextX][n.y];
                links.emplace_back(&n, nPtr);
                links.back().bitStrings[0] = distr(e2);
                links.back().bitStrings[1] = distr(e2);
                n.links[n.linkCount++] = &links.back();
                nPtr->links[nPtr->linkCount++] = &links.back();
                break;
            }
        }
        for (int nextY = n.y + 1; nextY < grid[0].size(); ++nextY) {
            if (grid[n.x][nextY] != nullptr) {
                node* nPtr = grid[n.x][nextY];
                links.emplace_back(&n, nPtr);
                links.back().bitStrings[0] = distr(e2);
                links.back().bitStrings[1] = distr(e2);
                n.links[n.linkCount++] = &links.back();
                nPtr->links[nPtr->linkCount++] = &links.back();
                break;
            }
        }
    }
    links.shrink_to_fit();
}
void cutLinkToNode(node& n, int linkIndex, int linkAmount) {
    link* l = n.links[linkIndex];
    l->n = linkAmount;
    node* otherNode = (l->a == &n ? l->b : l->a);
    for (int i = 0; i < 4; ++i)
        if (otherNode->links[i] == l) {
            otherNode->links[i] = otherNode->links[otherNode->linkCount--];
            break;
        }
    if (linkAmount) {
        otherNode->t += n.n;
        if (otherNode->t == otherNode->n) {
            ++nodesFull;
            for (int i = 0; i < otherNode->linkCount; ++i)
			    cutLinkToNode(*otherNode, i, 0);
            otherNode->linkCount = 0;
        }
    }
    for (int i = 0; i < links.size(); ++i) {
        if (&links[i] == l) {
            solvedLinks.push_back(std::move(links[i]));
			links[i] = links.back();
			links.pop_back();
			break;
		}
    }
}
void firstSolvePhase() {
    bool anyFound = false;
    start:
    for (auto& n : nodes) {
        if (n.linkCount == 1) {
            n.t = n.n;
            ++nodesFull;
            cutLinkToNode(n, 0, n.n - n.t);
            n.linkCount = 0;
            anyFound = true;
            continue;
        }
        if (n.linkCount == 2 && n.n - n.t == 4) {
            n.t += 4;
            ++nodesFull;
            cutLinkToNode(n, 0, 2);
            cutLinkToNode(n, 1, 2);
            n.linkCount = 0;
            anyFound = true;
            continue;
        }
        if (n.linkCount == 3 && n.n - n.t == 6) {
			n.t += 6;
			++nodesFull;
			cutLinkToNode(n, 0, 2);
			cutLinkToNode(n, 1, 2);
			cutLinkToNode(n, 2, 2);
			n.linkCount = 0;
			anyFound = true;
			continue;		
		}
        if (n.linkCount == 4 && n.n == 8) {
            n.t = 8;
            ++nodesFull;
            cutLinkToNode(n, 0, 2);
            cutLinkToNode(n, 1, 2);
            cutLinkToNode(n, 2, 2);
            cutLinkToNode(n, 3, 2);
            n.linkCount = 0;
            anyFound = true;
            continue;
        }
    }
    if (anyFound) {
        anyFound = false;
        goto start;
    }
}
int main() {
    setupLinks();
    firstSolvePhase();
    solve();
}