#pragma once
#include <iostream>
#include <vector>
#include <stdint.h>
struct link;
struct node {
	int_fast8_t num, x, y, linkCount = 0;
	link* links[4];
	node(int n_, int x_, int y_);
};
struct link {
	node* a, * b;
	int_fast8_t num = 0;
	size_t bitStrings[2];
	std::vector<link*> crosses;
	bool crossesActive = false;
	link(node* a_, node* b_, size_t b0, size_t b1);
	node* getOther(node* n);
};
std::vector<int_fast8_t> getLinkAmounts();
void solve();
void removeNode(node* n);
void removeLink(link* l);