#include "main.h"
#include <iostream>

using std::cout; using std::endl; using std::vector;
using board = std::vector<std::vector<int>>;

int main() {
    /*std::ifstream fileIn("D:/Downloads/transpose.txt");
    int width, height;
    while (fileIn >> width) {
        fileIn >> height;
        board transIn(width, vector<int>(height, 0));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x)
                fileIn >> transIn[x][y];
        }
        transTable.insert(std::move(transIn));
    }
    fileIn.close();*/
    while (true) {
        numbersRemaining = 0;
        int width, height;
        cout << "transTable.size(): " << transTable.size() << endl;
        std::cin >> width >> height; std::cin.ignore();
        cur.assign(width, vector<int>(height, 0));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                std::cin >> cell; std::cin.ignore();
                cur[x][y] = cell;
                if (cell)
                    ++numbersRemaining;
            }
        }
        solve();
        std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
        outFile << "cout << \"";
        for (int i = 0; i < moves.size(); ++i) {
            outFile << moves[i][0] << ' ' << moves[i][1] << ' ' <<
                (moves[i][2] ? (moves[i][2] == 1 ? 'R' : (moves[i][2] == 2 ? 'D' : 'L')) : 'U') << ' ' <<
                (moves[i][3] > 0 ? '+' : '-') << "\\n";
        }
        outFile << "\";" << endl;
        outFile.close();
        system("taskkill /f /fi \"windowtitle eq output.txt*\"");
        system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");
        moves.clear();
    }
}
bool solve(){
    for (int x = 0; x < cur.size(); ++x) {
        for (int y = 0; y < cur[0].size(); ++y) {
            if (!cur[x][y])
                continue;
            bool cardinalFound = false;
            for (int xInner = 0; xInner < cur.size(); ++xInner)
                if (cur[xInner][y])
                    cardinalFound = true;
            for (int yInner = 0; yInner < cur[0].size(); ++yInner)
                if (cur[x][yInner])
                    cardinalFound = true;
            if (!cardinalFound)
                return false;

            for (int dir = 0; dir < 4; ++dir) {
                int targetX = x + cur[x][y] * dirMults[dir];
                int targetY = y + cur[x][y] * dirMults[dir];
                if (targetX < 0 || targetX >= cur.size() || targetY < 0 || targetY >= cur[0].size() || !cur[x][y])
                    continue;

                for (int times = -1; times < 2; times += 2) {
                    int beforeFrom = cur[x][y];
                    int beforeTo = cur[targetX][targetY];//abs = 0??
                    cur[targetX][targetY] = abs(cur[targetX][targetY] + cur[x][y] * times);
                    cur[x][y] = 0;
                    --numbersRemaining;
                    if (!cur[targetX][targetY])
                        --numbersRemaining;
                    cout << numbersRemaining << endl;
                    if (numbersRemaining == 1)
                        goto woloomp;
                    moves.push_back({ x, y, dir, times });
                    if (!numbersRemaining || (!transTable.count(cur) && solve()))
                        return true;
                    
                    moves.pop_back();
                    woloomp:
                    transTable.insert(cur);
                    /*transTableOutFile << cur.size() << ' ' << cur[0].size() << ' ';
                    for (int yInner = 0; yInner < cur[0].size(); ++yInner)
                       for (int xInner = 0; xInner < cur.size(); ++xInner)
                            transTableOutFile << cur[xInner][yInner] << ' ';
                    transTableOutFile << endl;*/
                    if (!cur[targetX][targetY])
                        ++numbersRemaining;
                    cur[targetX][targetY] = beforeTo;
                    cur[x][y] = beforeFrom;
                    ++numbersRemaining;
                }
            }
        } 
    }
    return false;
}