//word_count: 9935
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using std::cerr; using std::endl;

int main() {
    int word_count;
    std::cin >> word_count; std::cin.ignore();
    for (int i = 0; i < word_count; i++) {
        std::string word;
        std::cin >> word; std::cin.ignore();
    }
    std::string searchOrder = "ETAOINSHRDLCUMWFGYPBVKJXQZ";
    int searchIndex = 0;
    std::string lastTurn;
    int toSearch[6][26]{};
    int found[6]{-1,-1,-1,-1,-1,-1};
    int foundCount = 0;

    while (true) {
        for (int i = 0; i < 6; ++i) {
            int state;
            std::cin >> state; std::cin.ignore();
            if (state == 2) {
                int c = lastTurn[i] - 65;
                for (int j = 0; j < 6; j++) {
                    if (!toSearch[j][c])
                        toSearch[j][c] = 1;
                }
            }
            else if (state == 3 && found[i] == -1) {
                found[i] = lastTurn[i];
                ++foundCount;
            }
        }

        std::string out = "******";
        for (int i = 0; i < 6; ++i) {
            out[i] = 0;
            if (found[i] != -1)
                continue;
            int searchIndex = -1;
            for (int j = 0; j < 26; ++j) {
                if (toSearch[i][j] == 1) {
                    searchIndex = j;
                    toSearch[i][j] = 2;
                    break;
                }
            }
            if (searchIndex != -1) {
                out[i] = searchIndex + 65;
            }
        }
        for (int i = 0; i < 6; ++i) {
            if (foundCount == 6) {
                out[i] = found[i];
            } else if (!out[i]) { 
                out[i] = searchOrder[searchIndex];
                if (++searchIndex == 26)
                    searchIndex = 0;
            }
        }

        lastTurn = out;
        std::cout << out << endl;
    }
}