#include "main.h"
#include <iostream>
#include <Windows.h>
#include <fstream>

using std::cout; using std::endl; using std::vector;

HANDLE hPipe;
int xOffset, yOffset;

void atexit_handler() {
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}
int main(){
    std::atexit(atexit_handler);
    hPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\numberShiftingPipe", // pipe name 
        PIPE_ACCESS_DUPLEX,                 // read/write access 
        PIPE_TYPE_MESSAGE |                 // message type pipe 
        PIPE_READMODE_MESSAGE |             // message-read mode 
        PIPE_WAIT,                          // blocking mode 
        PIPE_UNLIMITED_INSTANCES,           // max. instances  
        512,                                // output buffer size 
        512,                                // input buffer size 
        0,                                  // client time-out 
        NULL);                              // default security attribute
    while (true) {
        xOffset = 0; 
        yOffset = 0;
        out();
        in();
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
    }
}
void trimGrid(int& resultOffsetX, int& resultOffsetY, std::vector<std::vector<int>>& cur) {
shaveStart:
    for (size_t x = cur.size(); x--;) {
        for (size_t y = cur[0].size(); y--;) {
            if (cur[x][y])
                goto pastRightShave;
        }
        cur.pop_back();
    }
    goto shaveStart;
pastRightShave:
    for (size_t y = cur[0].size(); y--;) {
        for (size_t x = cur.size(); x--;) {
            if (cur[x][y])
                goto pastBottomShave;
        }
        for (size_t x = cur.size(); x--;)
            cur[x].pop_back();
    }
    goto shaveStart;
pastBottomShave:
    for (size_t y = 0; y < cur[0].size(); ++y) {
        if (cur[0][y])
            goto pastLeftShave;
    }
    cur.erase(cur.begin());
    ++resultOffsetX;
    goto shaveStart;
pastLeftShave:
    for (int x = 0; x < cur.size(); ++x) {
        if (cur[x][0])
            goto pastShaves;
    }
    for (int x = 0; x < cur.size(); ++x)
        cur[x].erase(cur[x].begin());
    ++resultOffsetY;
    goto shaveStart;
pastShaves:
    cur.shrink_to_fit();
    for (int x = 0; x < cur.size(); ++x)
        cur[x].shrink_to_fit();
}
void out(){
    int width, height;
    std::cin >> width >> height; std::cin.ignore();
    vector<vector<int>> cur(width, vector<int>(height, 0));
    vector<std::pair<int, int>> non0s;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cell;
            std::cin >> cell; std::cin.ignore();
            if (cell) {
                cur[x][y] = cell;
                non0s.emplace_back(x, y);
            }
        }
    }
    trimGrid(xOffset, yOffset, cur);

    cout << '\n';
    for (int i = 0; i < cur[0].size(); ++i) {
        for (int j = 0; j < cur.size(); ++j)
            cout << cur[j][i] << ' ';
        cout << endl;
    }
    cout << "\nxOffset, yOffset: " << xOffset << ", " << yOffset << endl;

    ConnectNamedPipe(hPipe, 0);
    int sendBuffer = cur.size();
    WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);//temp
    sendBuffer = cur[0].size();
    WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);//temp
    sendBuffer = non0s.size();
    WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
    for (std::pair<int, int>& p : non0s) {
        sendBuffer = p.first - xOffset;
        WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
        sendBuffer = p.second - yOffset;
        WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
    }
}
void in() {
    std::ofstream outFile("C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt", std::fstream::app);
    outFile << "cout << \"";
    int moveCount = 0;
    ReadFile(hPipe, &moveCount, sizeof(moveCount), 0, 0);
    cout << "moveCount: " << moveCount << endl;
    for (int i = 0; i < moveCount; ++i) {
        int recInt = 0;
        ReadFile(hPipe, &recInt, sizeof(recInt), 0, 0);
        outFile << recInt + xOffset << ' ';
        ReadFile(hPipe, &recInt, sizeof(recInt), 0, 0);
        outFile << recInt + yOffset << ' ';
        ReadFile(hPipe, &recInt, sizeof(recInt), 0, 0);
        switch (recInt) {
            case 0: outFile << "U "; break;
            case 1: outFile << "R "; break;
            case 2: outFile << "D "; break;
            case 3: outFile << "L "; break;
        }
        ReadFile(hPipe, &recInt, sizeof(recInt), 0, 0);
        outFile << (recInt > 0 ? '+' : '-') << "\\n";
    }
    outFile << "\";" << endl;
    outFile.close();
    system("taskkill /f /fi \"windowtitle eq output.txt*\"");
    system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");
}