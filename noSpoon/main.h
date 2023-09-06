#pragma once
#include <iostream>
#include <vector>
#include <stdint.h>
struct link;
struct node {
	int_fast8_t num, x, y, linkCount = 0;
	link* links[4] = {};
	node(int n_, int x_, int y_) : num(n_), x(x_), y(y_) {}
};
struct link {
	node* a, * b;
	int_fast8_t num = 0, crossCount = 0;
	size_t bitStrings[2];
	link* crosses[8] = {};
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
bool crossesActive(link& l);
std::vector<int_fast8_t> getLinkAmounts();
void solve();
void removeNode(node* n);
void removeLink(link* l);