#pragma once
#include "main.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <fstream>

using std::cout; using std::endl; using std::vector;

int main(){
    while (true) {
        int width, height;
        std::cin >> width >> height; std::cin.ignore();
        vector<vector<int>> cur(width, vector<int>(height, 0));
        vector<std::pair<int, int>> non0s;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                std::cin >> cell; std::cin.ignore();
                if (cell) {
                    non0s.emplace_back(x, y);
                    cur[x][y] = cell;
                }
            }
        }
        non0s.shrink_to_fit();

        int xOffset = 0;
        int yOffset = 0;
        {
            for (size_t x = cur.size(); x--;) {
                for (size_t y = cur[0].size(); y--;) {
                    if (cur[x][y])
                        goto pastRightShave;
                }
                cur.pop_back();
            }
        pastRightShave:
            for (size_t y = cur[0].size(); y--;) {
                for (size_t x = cur.size(); x--;) {
                    if (cur[x][y])
                        goto pastBottomShave;
                }
                for (size_t x = cur.size(); x--;)
                    cur[x].pop_back();
            }
        pastBottomShave:
            for (size_t y = 0; y < cur[0].size(); ++y) {
                if (cur[0][y])
                    goto pastLeftShave;
            }
            cur.erase(cur.begin());
            ++xOffset;
            goto pastBottomShave;
        pastLeftShave:
            for (int x = 0; x < cur.size(); ++x) {
                if (cur[x][0])
                    goto pastShaves;
            }
            for (int x = 0; x < cur.size(); ++x)
                cur[x].erase(cur[x].begin());
            ++yOffset;
            goto pastBottomShave;
        pastShaves:
            cur.shrink_to_fit();
            for (int x = 0; x < cur.size(); ++x)
                cur[x].shrink_to_fit();
        }

        cout << '\n';
        for (int i = 0; i < cur[0].size(); ++i) {
            for (int j = 0; j < cur.size(); ++j)
                cout << cur[j][i] << ' ';
            cout << endl;
        }
        cout << "\n xOffset, yOffset: " << xOffset << ", " << yOffset << endl;

        HANDLE hPipe = CreateNamedPipe(
            L"\\.\\pipe\\numberShiftingPipe",   // pipe name 
            PIPE_ACCESS_DUPLEX,                 // read/write access 
            PIPE_TYPE_MESSAGE |                 // message type pipe 
            PIPE_READMODE_MESSAGE |             // message-read mode 
            PIPE_WAIT,                          // blocking mode 
            PIPE_UNLIMITED_INSTANCES,           // max. instances  
            512,                                // output buffer size 
            512,                                // input buffer size 
            0,                                  // client time-out 
            NULL);                              // default security attribute 
        #pragma warning( push )
        #pragma warning( disable : 6031)
        ConnectNamedPipe(hPipe, 0);
        #pragma warning( push )
        #pragma warning( disable : 4267)
        int sendBuffer = cur.size();
        WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
        WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
        sendBuffer = cur[0].size();
        #pragma warning( pop ) 
        WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
        for (size_t y = 0; y < cur[0].size(); ++y) {
            for (size_t x = 0; x < cur.size(); ++x) {
                sendBuffer = cur[x][y];
                WriteFile(hPipe, &sendBuffer, sizeof(sendBuffer), 0, 0);
            }
        }
        cur.clear();
        cur.shrink_to_fit();

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
            case 0:
                outFile << "U ";
                break;
            case 1:
                outFile << "R ";
                break;
            case 2:
                outFile << "D ";
                break;
            case 3:
                outFile << "L ";
                break;
            }
            ReadFile(hPipe, &recInt, sizeof(recInt), 0, 0);
            outFile << (recInt > 0 ? '+' : '-') << "\\n";
        }
        #pragma warning( pop ) 
        outFile << "\";" << endl;
        outFile.close();
        system("taskkill /f /fi \"windowtitle eq output.txt*\"");
        system("start notepad \"C:/Users/Quasar/source/repos/codinGame/Number Shifting/output.txt\"");

        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }
}