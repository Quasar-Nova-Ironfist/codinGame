#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using std::cout; using std::cerr; using std::cin; using std::endl; using std::vector; using std::pair;

int main() {
    vector<pair<int, int>> checkpoints; {
        int checkpointsNum; // Count of checkpoints to read, times 3 laps
        cin >> checkpointsNum; cin.ignore();
        checkpoints.reserve(checkpointsNum);
        for (int i = 0; i < checkpointsNum; i++) {
            int checkpoint_x; // Position X
            int checkpoint_y; // Position Y
            cin >> checkpoint_x >> checkpoint_y; cin.ignore();
            checkpoints.emplace_back(checkpoint_x, checkpoint_y);
        }
    }
    while (true) {
        int checkpointIndex; // Index of the checkpoint to lookup in the checkpoints input, initially 0
        pair<int, int> curPos;
        pair<int, int> curVel;// horizontal speed. Positive is right; vertical speed. Positive is downwards
        int angle; // facing angle of this car, clockwise, 0 degrees is (1,0)
        cin >> checkpointIndex >> curPos.first >> curPos.second >> curVel.first >> curVel.second >> angle; cin.ignore();
        
        pair<int, int> checkRelPos{ checkpoints[checkpointIndex].first - curPos.first, checkpoints[checkpointIndex].second - curPos.second };
        double angleCarPosToCheck = atan(checkRelPos.second * 1.0 / checkRelPos.first) * 180 / 3.14159265358979323846 + 180;
        int angleChange = angle - angleCarPosToCheck;
        int thrust = 200 - 2 * abs(angleChange);
        if (thrust < 0)
            thrust = 0;
        cout << "EXPERT " << std::max(std::min(angleChange, 18), -18) << ' ' << thrust << endl;
        //cout << checkpoints[checkpointIndex].first << ' ' << checkpoints[checkpointIndex].second << ' ' << thrust << endl; // X Y THRUST MESSAGE
    }
}