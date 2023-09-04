#pragma once
#include <iostream>
#include <vector>
struct board {
	struct link;
	struct node {
		int n, x, y, t = 0, linkCount = 0;
		link* links[4];
		node(int n_, int x_, int y_) : n(n_), x(x_), y(y_) {}
		node(const node& other) : n(other.n), x(other.x), y(other.y), t(other.t), linkCount(other.linkCount) {
			links[0] = other.links[0];
			links[1] = other.links[1];
			links[2] = other.links[2];
			links[3] = other.links[3];
		}
	};
	struct link {
		node* a, * b;
		int n = 0;
		size_t bitStrings[2];
		link(node* a_, node* b_, size_t b0, size_t b1) : a(a_), b(b_) {
			bitStrings[0] = b0;
			bitStrings[1] = b1;
		}
		link(const link& other) : a(other.a), b(other.b), n(other.n) {
			bitStrings[0] = other.bitStrings[0];
			bitStrings[1] = other.bitStrings[1];
		}
	};
	std::vector<link> links;
	size_t hash = 0;
	std::vector<node> nodes;
	int nodesFull = 0;
	void solve();
	std::pair<size_t, std::vector<int>> getHashKeyPair();
	void printLinks();
	board() = default;
	board(board& o);
};
void removeLinkFromNode(board& b, board::node* n, board::link* l);
void moveLinkToSolved(board& b, board::node& n, int linkIndex, int cutAmount);