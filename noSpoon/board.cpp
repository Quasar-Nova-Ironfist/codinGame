#include "main.h"
#include <parallel_hashmap/phmap.h>
using std::vector; using hashKeyPair = std::pair<size_t, vector<int>>;

template<> struct std::hash<hashKeyPair> { size_t operator()(const hashKeyPair& h) { return h.first; } };

phmap::parallel_flat_hash_set<hashKeyPair> transTable;

board::board(board& o) {
	nodesFull = o.nodesFull;
	hash = o.hash;
	nodes = o.nodes;
	links = o.links;
	for (int i = 0; i < links.size(); ++i) {
		links[i].a = &nodes[o.links[i].a - &o.nodes[0]];
		links[i].b = &nodes[o.links[i].b - &o.nodes[0]];
	}
	for (int i = 0; i < nodes.size(); ++i)
		for (int j = 0; j < nodes[i].linkCount; ++j)
			nodes[i].links[j] = &links[o.nodes[i].links[j] - &o.links[0]];
}
hashKeyPair board::getHashKeyPair() {
	vector<int> res;
	res.reserve(links.size());
	for (auto& l : links)
		res.push_back(l.n);
	return std::make_pair(hash, res);
}
void board::printLinks() {
	for (auto& l : links)
		if (l.n)
			std::cout << l.a->x << ',' << l.a->y << ',' << l.b->x << ',' << l.b->y << ',' << l.n << ", ";
	std::cout << std::endl;
	while (true)//prevent accidental exit
		std::cin >> hash;
}
void board::solve() {
	if (nodesFull == nodes.size())
		printLinks();
	for (int numLinks = 3; --numLinks;) {
		for (auto& l : links) {
			if (l.n || l.a->t + numLinks > l.a->n || l.b->t + numLinks > l.b->n)
				continue;
			l.n = numLinks;
			hash ^= l.bitStrings[numLinks - 1];//have additional entry in bitStrings so as to not need the - 1? Malloc the bitStrings and dec pointer to them?
			if (transTable.emplace(getHashKeyPair()).second) {
				l.a->t += numLinks;
				l.b->t += numLinks;
				nodesFull += (l.a->t == l.a->n) + (l.b->t == l.b->n);
				solve();
				nodesFull -= (l.a->t == l.a->n) + (l.b->t == l.b->n);
				l.b->t -= numLinks;
				l.a->t -= numLinks;
			}
			hash ^= l.bitStrings[numLinks - 1];
			l.n = 0;
		}
	}
}