#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <chrono>

using std::cout; using std::cerr; using std::endl; using std::vector;

struct guessStruct {//replace with pair<pair<int,int>, vector<int>>?
    vector<int> g;
    int matches, wrongPos;
    int operator[](int i) const { return g[i]; }
    int& operator[](int i) { return g[i]; }
    friend std::ostream& operator<< (std::ostream& stream, const guessStruct& guess);
};
std::ostream& operator<< (std::ostream& stream, const guessStruct& guess) {
	for (int i = 0; i < guess.g.size(); ++i)
		stream << guess.g[i];
	return stream;
}
void setGuess(guessStruct& guess, vector<int>& guessIndices, vector<vector<int>>& nums) {
    vector<int> temp{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    for (int i = 0; i < nums.size(); ++i) {
        for (int j = 0; j < nums[i].size(); ++j) {
            auto seRes = std::find(temp.begin(), temp.end(), nums[i][j]);
            if (seRes != temp.end()) {
                guessIndices[i] = j;
                guess[i] = *seRes;
                *seRes = temp.back();
                temp.pop_back();
                break;
			}
        }
    }
}

int main() {
    int numLen;
    std::cin >> numLen; std::cin.ignore();
    vector<vector<int>> nums(numLen, vector<int>());
    for (int i = 0; i < nums.size(); ++i)
        nums[i].assign({ 1,2,3,4,5,6,7,8,9,0 });
    nums[0].pop_back();//remove 0 from first digit pool
    guessStruct guess;
    guess.g.assign(numLen, 0);
    std::cin >> guess.matches>> guess.wrongPos; std::cin.ignore();//-1, -1 for first turn
    vector<int> guessIndices(numLen, 0);
    vector<guessStruct> guesses;
    setGuess(guess, guessIndices, nums);
    while (true) {
        std::cin >> guess.matches >> guess.wrongPos; std::cin.ignore();
        if (!guess.matches) {
            if (!guess.wrongPos) {//remove each guess digit from all pools
                for (int i = 0; i < numLen; ++i) {
                    for (int j = 0; j < nums[i].size(); ++j) {
                        if (nums[i][j] == guess[i]) {
							nums[i][j] = nums[i].back();
							nums[i].pop_back();
						}
                    }
                }
			}
            else {//remove relevant numbers per digit pool
                guesses.push_back(guess);//inside this statement b/c probably not needed when !guess.wrongPos; if it is move statement above outer if so as to avoid duplicate statement
                for (int i = 0; i < numLen; ++i) {
                    nums[i][guessIndices[i]] = nums[i].back();
                    nums[i].pop_back();
                }
            }
            setGuess(guess, guessIndices, nums);
            continue;
        }
        guesses.push_back(guess);



        setGuess(guess, guessIndices, nums);
    }
}