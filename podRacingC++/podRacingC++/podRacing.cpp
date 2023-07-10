#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

using std::cout;using std::endl;using std::cin;using std::vector;using std::cerr;
constexpr auto checkpointPosOffsetRadius = -600;
#define nc checkList[nextCheckpoint]

struct checkpoint {
    int x, y;
};
vector<checkpoint> checkList = { {-1, -1} };
int nextCheckpoint = 1;
int checkpointPosOfLastIteration[2];
bool allCheckpointsFound = false;

double checkpointToPod[3];

double thrust;
bool boostUsed = false, boostNow = false;

int pc[2];//pod cordinates
int tc[2];//target cordinates
int ncd; // distance to the next checkpoint
int nca; // angle between your pod orientation and the direction of the next checkpoint
int ncc[2];//next checkpoint coordinates
int opponent_x;
int opponent_y;

int main() {
    cin >> pc[0] >> pc[1] >> ncc[0] >> ncc[1] >> ncd >> nca; cin.ignore();
    cin >> opponent_x >> opponent_y; cin.ignore();
    checkList.push_back({ncc[0], ncc[1]});
    checkpointPosOfLastIteration[0] = ncc[0];
    checkpointPosOfLastIteration[1] = ncc[1];
    goto skipSecondInputQueryOnFirstMove;
while (1) {
    cin >> pc[0] >> pc[1] >> ncc[0] >> ncc[1] >> ncd >> nca; cin.ignore();
    cin >> opponent_x >> opponent_y; cin.ignore();
    skipSecondInputQueryOnFirstMove:
    if (checkpointPosOfLastIteration[0] != ncc[0] || checkpointPosOfLastIteration[1] != ncc[1]) {//checkpoint changed
        ++nextCheckpoint;
        if (allCheckpointsFound) {
            if (nextCheckpoint == checkList.size())
                nextCheckpoint = 0;
        }
        else {
            if (ncc[0] == checkList[1].x && ncc[1] == checkList[1].y) {
                cerr << "a0" << endl;
                checkList[0] = checkList[checkList.size()-1];
                cerr << "a1" << endl;
                checkList.pop_back();
                cerr << "a2" << endl;
                allCheckpointsFound = true;
                cerr << "a3" << endl;
                nextCheckpoint = 1;
            }
            else {
                checkList.push_back({ncc[0],ncc[1]});
            }
        }
        checkpointPosOfLastIteration[0] = ncc[0];
        checkpointPosOfLastIteration[1] = ncc[1];
    }//checkList[nextCheckpoint] hereonwards
    cerr << "Current: " << nextCheckpoint << ", " << nc.x << ", " << nc.y << endl;
    for (int i = 0; i < checkList.size(); i++) {
        cerr << i << ',' << checkList[i].x << ',' << checkList[i].y << ' ';
    }
    cerr << endl;

    checkpointToPod[0] = nc.x - pc[0];
    checkpointToPod[1] = nc.y - pc[1];
    checkpointToPod[2] = atan(checkpointToPod[1] / checkpointToPod[0]);
    cerr << "Angle to pod: " << checkpointToPod[2] << endl;

    tc[0] = nc.x - cos(checkpointToPod[2]) * checkpointPosOffsetRadius;
    tc[1] = nc.y - sin(checkpointToPod[2]) * checkpointPosOffsetRadius;
    cerr << "Target: " << tc[0] << ", " << tc[1] << endl;

    ncd = sqrt(pow(pc[0] - tc[0], 2) + pow(pc[1] - tc[1], 2));
    thrust = 5 * ncd;
    //thrust = 100;
    if (abs(nca) > 75)
        thrust /= 5;
    if (abs(nca) > 80)
        thrust = 0;

    if (thrust > 100)
        thrust = 100;
    if (thrust < 0)
        thrust = 0;
    cerr << "Dist: " << ncd << endl;
    cerr << "Thrust: " << thrust << endl;

    if (!boostUsed && abs(nca) < 1 && ncd > 5000) {
        boostNow = true;
        boostUsed = true;
    }
    if (boostNow) {
        cout << tc[0] << ' ' << tc[1] << " BOOST" << endl;
        boostNow = false;
    }
    else {
        cout << tc[0] << ' ' << tc[1] << ' ' << thrust << endl;
    }
}
}