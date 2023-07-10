#include <iostream>
#include <map>
#include <array>
#include <vector>
int coordToIndex(int row, int col) { return row + col * 3; }
bool settingMakes3(std::array<int, 9>& b, int row, int col, int val) {
    b[coordToIndex(row, col)] = val;
    bool ret = false;
    if (
        (
            b[coordToIndex(row, 0)] == val && b[coordToIndex(row, 1)] == val && b[coordToIndex(row, 2)] == val
        ) ||
        (
            b[coordToIndex(0, col)] == val && b[coordToIndex(1, col)] == val && b[coordToIndex(2, col)] == val
        ) ||
        (
            b[coordToIndex(0, 0)] == val && b[coordToIndex(1, 1)] == val && b[coordToIndex(2, 2)] == val
        ) ||
        (
            b[coordToIndex(2, 0)] == val && b[coordToIndex(1, 1)] == val && b[coordToIndex(0, 2)] == val
        )
       ) {
        ret = true;
    }
    b[coordToIndex(row, col)] = 0;
    return ret;
}
int main(){ try {
    std::map<std::array<int, 9>, std::pair<int,int>> moveList = {
        {{0,0,0,0,0,0,0,0,0},{0,0}},
        {{1,2,0,0,0,0,0,0,0},{0,1}},
        {{1,2,0,1,0,0,2,0,0},{1,1}},
        {{1,0,2,0,0,0,0,0,0},{0,1}},
        {{1,0,2,1,0,0,2,0,0},{1,1}},
        {{1,0,0,2,0,0,0,0,0},{1,0}},
        {{1,1,2,2,0,0,0,0,0},{1,1}},
        {{1,0,0,0,2,0,0,0,0},{0,1}},
        {{1,0,0,1,2,0,2,0,0},{2,0}},
        {{1,2,1,1,2,0,2,0,0},{1,2}},
        {{1,0,0,0,0,2,0,0,0},{0,2}},
        {{1,0,0,2,0,2,1,0,0},{1,1}},
        {{1,0,0,0,0,0,2,0,0},{1,0}},
        {{1,1,2,0,0,0,2,0,0},{1,1}},
        {{1,0,0,0,0,0,0,2,0},{1,1}},
        {{1,0,0,0,1,0,0,2,2},{0,2}},
        {{1,0,0,0,0,0,0,0,2},{0,2}},
        {{1,0,0,2,0,0,1,0,2},{2,0}}
    };
    int oppRow, oppCol;
    std::array<int, 9> currentBoard = { 0,0,0,0,0,0,0,0,0 };
    int valMoveRow, valMoveCol, validActionCount;
    std::vector<std::pair<int, int>> valMoves;
    bool moveSelected;
    std::pair<int, int> move;
    while (true) {
        std::cin >> oppRow >> oppCol; std::cin.ignore();//note opponent's move
        if (oppRow != -1) {
            currentBoard[coordToIndex(oppRow, oppCol)] = 2;
        }

        moveSelected = false;
        valMoves.clear();
        std::cin >> validActionCount; std::cin.ignore();
        for (int i = 0; i < validActionCount; i++) {//look for move that wins
            std::cin >> valMoveRow >> valMoveCol; std::cin.ignore();
            if (settingMakes3(currentBoard, valMoveRow, valMoveCol, 1)) {
                moveSelected = true;
                break;
            }
            valMoves.push_back({valMoveRow, valMoveCol});
        }
        if (!moveSelected) {
            for (int i = 0; i < validActionCount; i++) {//look for move that prevents immediate loss
                valMoveRow = valMoves[i].first;
                valMoveCol = valMoves[i].second;
                if (settingMakes3(currentBoard, valMoveRow, valMoveCol, 1)) {
                    moveSelected = true;
                    break;
                }
            }
            if (!moveSelected) {
                auto itr = moveList.find(currentBoard);
                if (itr != moveList.end()) {//look for current board state in list, else just go with whatever the last entered valid move is
                    valMoveRow = itr->second.first;
                    valMoveCol = itr->second.second;
                }
            }
        }
        currentBoard[coordToIndex(valMoveRow, valMoveCol)] = 1;
        std::cout << valMoveRow << ' ' << valMoveCol << std::endl;
    }
}
catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;;
} }