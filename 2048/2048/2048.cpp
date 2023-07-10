#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

using std::cin; using std::cerr; using std::cout; using std::endl; using std::vector;

struct board {
    /*
    0  1  2  3
    4  5  6  7
    8  9  10 11
    12 13 14 15
    */
    long seed;
    int nums[16]{};
    board() {};
    board(long seedIn, int p0, int v0, int p1, int v1) {
        seed = seedIn;
        nums[p0] = v0;
        nums[p1] = v1;
    }
    board(board& b) {
        seed = b.seed;
        for (int i = 0; i < 16; ++i)
            nums[i] = b.nums[i];
    }
    int collapse(int& n0, int& n1, int& n2, int& n3, bool& anyMoved) {//to the left
        bool merged[3]{};
        return 0;
    }
    int makeMove(int dir) {
        bool anyMoved = false;
        int moveScoreSum;
        switch (dir) {
        case 0://up
            moveScoreSum = collapse(nums[0], nums[4], nums[8], nums[12], anyMoved)//these could be values from a vector or a map, but, this should be faster as they are constant?
                         + collapse(nums[1], nums[5], nums[9], nums[13], anyMoved)
                         + collapse(nums[2], nums[6], nums[10], nums[14], anyMoved)
                         + collapse(nums[3], nums[7], nums[11], nums[15], anyMoved);
            break;
        case 1://right
            moveScoreSum = collapse(nums[3], nums[2], nums[1], nums[0], anyMoved)
                         + collapse(nums[7], nums[6], nums[5], nums[4], anyMoved)
                         + collapse(nums[11], nums[10], nums[9], nums[8], anyMoved)
                         + collapse(nums[15], nums[14], nums[13], nums[12], anyMoved);
            break;
        case 2://down
            moveScoreSum = collapse(nums[12], nums[8], nums[4], nums[0], anyMoved)
                         + collapse(nums[13], nums[9], nums[5], nums[1], anyMoved)
                         + collapse(nums[14], nums[10], nums[6], nums[2], anyMoved)
                         + collapse(nums[15], nums[11], nums[7], nums[3], anyMoved);
            break;
        case 3://left
            moveScoreSum = collapse(nums[0], nums[1], nums[2], nums[3], anyMoved)
                         + collapse(nums[4], nums[5], nums[6], nums[7], anyMoved)
                         + collapse(nums[8], nums[9], nums[10], nums[11], anyMoved)
                         + collapse(nums[12], nums[13], nums[14], nums[15], anyMoved);
            break;
        }
        if (anyMoved) {
            vector<int> freeCells(8);
            for (int x = 0; x < 4; x++) {
                for (int y = 0; y < 4; y++) {
                    if (!nums[x + y * 4]) {
                        //cerr << x << ", " << y << endl;
                        freeCells.push_back(x + y * 4);
                    }
                }
            }
            nums[freeCells[(int)seed % freeCells.size()]]//choose available spot to spawn according to game's algorithm
                = (seed & 0x10) == 0 ? 2 : 4;//set cell value to 2 or 4, depending
            seed = seed * seed % 50515093L;
            return moveScoreSum;
        }
        return -9999;
    }
    long getSeed() { return seed; }//RIF?
    friend std::ostream& operator<<(std::ostream& os, const board& b);
};
std::ostream& operator<<(std::ostream& os, const board& b){
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y)
            os << std::setw(5) << b.nums[x + y * 4] << ' ';
        os << '\n';
    } 
    return os;
}
void outputMoves(board& current, int time) {
    auto startTime = std::chrono::steady_clock::now();//replace with now() + time for checking if past that time
    current.makeMove(0);
    cout << "U" << endl;
}
void setup(board& current) {
    long seed;
    cin >> seed; cin.ignore();
    int score;
    cin >> score; cin.ignore();
    vector<int> initCells;
    initCells.reserve(4);
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            int cell;
            cin >> cell; cin.ignore();
            if (cell) {
                initCells.push_back(x + y * 4);
                initCells.push_back(cell);
            }
        }
    }
    current =  board(seed, initCells[0], initCells[1], initCells[2], initCells[3]);//test
}
int main() {//passed seed probably needs to be advanced twice in setup
    board current;
    setup(current);
    outputMoves(current, 990);
    while (true) {
        cerr << current << endl;
        long seed;
        cin >> seed; cin.ignore();
        cerr << "Current seed: " << current.getSeed() << endl;;
        int score;
        cin >> score; cin.ignore();
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                int cell;
                cin >> cell; cin.ignore();
            }
        }
        outputMoves(current, 45);
    }
}