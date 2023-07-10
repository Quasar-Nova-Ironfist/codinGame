#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using std::cout; using std::cerr; using std::cin; using std::endl; using std::vector;
#define hi hums[i]
#define zi zoms[i]
#define hj hums[j]
#define zj zoms[j]

struct npc {
    double id, x, y, nx, ny;
    npc* closestZom;
    double distToZom, ttnz, ttsr, distToMe;
};
double dist(double x1, double y1, double x2, double y2) { return sqrt(pow(x2-x1,2)+pow(y2-y1,2)); }
double dist(npc& a, npc& b) { return sqrt(pow(a.nx-b.nx, 2) + pow(a.ny-b.ny, 2)); }
int main() {
    double npcInput[6];//num, id, x, y, nx, ny
    double x, y;
    vector<npc> hums;
    vector<npc> zoms;
    npc *closestZom, *endangeredHum;
    while (true) {
        hums.clear();
        zoms.clear();
        cin >> x >> y; cin.ignore();
        cin >> npcInput[0]; cin.ignore();
        for (int i = 0; i < npcInput[0]; i++) {
            cin >> npcInput[1] >> npcInput[2] >> npcInput[3]; cin.ignore();
            hums.push_back({ npcInput[1], npcInput[2], npcInput[3], npcInput[2], npcInput[3] });
        }
        cin >> npcInput[0]; cin.ignore();
        for (int i = 0; i < npcInput[0]; i++) {
            cin >> npcInput[1] >> npcInput[2] >> npcInput[3] >> npcInput[4] >> npcInput[5]; cin.ignore();
            zoms.push_back({ npcInput[1], npcInput[2], npcInput[3], npcInput[4], npcInput[5] });
        }
        closestZom = &zoms[0];
        endangeredHum = &hums[0];

        for (int i = 1; i < zoms.size(); ++i) {
            zi.distToMe = dist(x, y, zi.x, zi.y);
            if (closestZom->distToMe > zi.distToMe) {
                closestZom = &zi;
            }
        }
        for (int i = 0; i < hums.size(); ++i) {
            hi.distToZom = 20000;
            for (int j = 0; j < zoms.size(); ++j) {
                double temp = dist(hi, zj);
                if (temp < hi.distToZom) {
                    hi.distToZom = temp;
                    hi.closestZom = &zj;
                }
            }
            hi.ttnz = hi.distToZom / 400;
            hi.ttsr = (dist(hi.x, hi.y, x, y) / 1000) -3;
            if(hi.ttsr > hi.ttnz){
                hums.erase(hums.begin() + i);
                --i;
                continue;
            }
            if (hi.ttnz < endangeredHum->ttnz) {
                endangeredHum = &hi;
            }
        }

        if (endangeredHum->distToZom < 2000) {
            cout << endangeredHum->x << ' ' << endangeredHum->y << endl;
        }
        else {
            cout << closestZom->nx << ' ' << closestZom->ny << endl;
        }
    }
}