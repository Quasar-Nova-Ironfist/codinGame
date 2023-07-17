#include "main.h"
#include <iostream>
#include <vector>
#include <Windows.h>

#define WIDTH 5
#define HEIGHT 5
#define STARTING_GRID {1,2,3,4,5}, {2,3,4,5,6}, {3,4,5,6,7}, {4,5,6,7,8}, {5,6,7,8,9}

using std::cout; using std::endl; using std::vector;

int main(){
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