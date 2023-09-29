#include "main.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <q/URI/URI.h>

#define WIDTH 5
#define HEIGHT 5
#define STARTING_GRID {1,2,3,4,5}, {2,3,4,5,6}, {3,4,5,6,7}, {4,5,6,7,8}, {5,6,7,8,9}

using std::cout; using std::endl; using std::vector;

int main(){
	//quasar::URI url("https://chan.sankakucomplex.com/test/path/of/doom?prev=4.01747446788347%2B31354386&tags=young_futanari%20order%3Apopular&page=12#fragmentTest");
	quasar::URI url("https://johnBlandguy:isurehope@chanty.cultists.com:666/test/path/of/doom?prev=4.01747446788347%2B31354386+ratio&tags=sacrificial_iguana%20order%3Apopular&page=12#fragmentTest");
	cout << "whole: " << url << endl;
	cout << "scheme: " << url.scheme << endl;
	cout << "authority: " << url.authority << endl;
	cout << "\tuserInfo: " << url.userInfo << endl;
	cout << "\thost: " << url.host << endl;
	cout << "\tport: " << url.port << endl;
	cout << "path: " << url.path << endl;
	cout << "pathSegments: " << endl;
	for (auto& s : url.pathSegments) {
		cout << '\t' << s << endl;
	}
	cout << "query: " << url.query << endl;
	cout << "querySegments: " << endl;
	for (auto& s : url.querySegments) {
		cout << '\t' << s << endl;
		cout << "\t\t" << s.key << endl;
		cout << "\t\t" << s.value << endl;
		for (auto& v : s.valueSegments)
			cout << "\t\t\t" << v << endl;
	}
	cout << "fragment: " << url.fragment << endl;
	
	
	
	
	
	return 0;
	
	HANDLE hPipe = CreateFile(
		L"\\\\.\\pipe\\numberShiftingPipe", // pipe name 
		GENERIC_READ | GENERIC_WRITE,		// read and write access 
		0,									// no sharing 
		NULL,								// default security attributes
		OPEN_EXISTING,						// opens existing pipe 
		0,									// default attributes 
		NULL);								// no template file 

	int rec = 0;
	ReadFile(hPipe, &rec, sizeof(rec), 0, 0);

	int grid[WIDTH][HEIGHT] = {STARTING_GRID};
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			cout << grid[x][y] << ' ';
		}
		cout << endl;
	}
}