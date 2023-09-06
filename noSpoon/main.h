#pragma once
#include <iostream>
#include <vector>
struct link;
struct node {
	int num, x, y, t = 0, linkCount = 0;
	link* links[4] = {};
	node(int n_, int x_, int y_) : num(n_), x(x_), y(y_) {}
};
struct link {
	node* a, * b;
	int num = 0, crossCount = 0;
	size_t bitStrings[2];
	link* crosses[10] = {};
	link(node* a_, node* b_, size_t b0, size_t b1) : a(a_), b(b_) {
		bitStrings[0] = b0;
		bitStrings[1] = b1;
	}
	node* getOther(node* n) {
		if (a == n)
			return b;
		return a;
	}
};
void solve();
std::vector<int> getLinkAmounts();
void removeLink(link* l);