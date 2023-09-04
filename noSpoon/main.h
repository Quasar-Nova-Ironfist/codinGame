#pragma once
#include <iostream>
#include <vector>
struct link;
struct node {
	int n, x, y, t = 0, linkCount = 0;
	link* links[4];
	node(int n_, int x_, int y_) : n(n_), x(x_), y(y_) {}
};
struct link {
	node* a, * b;
	int n = 0;
	size_t bitStrings[2];
	link(node* a_, node* b_, size_t b0, size_t b1) : a(a_), b(b_) {
		bitStrings[0] = b0;
		bitStrings[1] = b1;
	}
};
void solve();
std::vector<int> getLinkAmounts();
void removeLinkFromNode(node* n, link* l);
void moveLinkToSolved(node& n, int linkIndex, int cutAmount);