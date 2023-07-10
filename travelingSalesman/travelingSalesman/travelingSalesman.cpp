#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <cmath>
//#include <chrono>
using std::cerr; using std::cin; using std::cout; using std::endl; using std::vector; using std::ref;

namespace intersections {
    int orientation(std::pair<int, int>& p, std::pair<int, int>& q, std::pair<int, int>& r) {
        int val = (q.second - p.second) * (r.first - q.first) -
            (q.first - p.first) * (r.second - q.second);
        if (val == 0) return 0;// collinear
        return (val > 0) ? 1 : 2;// clock or counterclock wise
    }
    bool intersects(std::pair<int, int>& p1, std::pair<int, int>& q1, std::pair<int, int>& p2, std::pair<int, int>& q2) {
        return (orientation(p1, q1, p2) != orientation(p1, q1, q2) && orientation(p2, q2, p1) != orientation(p2, q2, q1));
    }
    bool intersects(vector<std::pair<int, int>>& nodes, int p1, int q1, int p2, int q2) {
        return intersects(nodes[p1], nodes[q1], nodes[p2], nodes[q2]);
    }
    void uncrossEdges(vector<int>& order, vector<std::pair<int, int>>& nodes) {
        bool more;
        do {
            more = false;
            for (int i = 0; i < order.size() / 2; ++i) {
                for (int j = i + 2; j < order.size() - 2; ++j) {
                    if (intersects(nodes[order[i]], nodes[order[i + 1]], nodes[order[j]], nodes[order[j + 1]])) {
                        //cerr << "ind: " << i << "," << i + 1 << " " << j << "," << j + 1 << endl;
                        //cerr << "val: " << order[i] << "," << order[i + 1] << " " << order[j] << "," << order[j + 1] << endl;
                        //cerr << "1st: " << nodes[order[i]].first << "," << nodes[order[i]].second << " " << nodes[order[i + 1]].first << "," << nodes[order[i + 1]].second << endl;
                        //cerr << "2nd: " << nodes[order[j]].first << "," << nodes[order[j]].second << " " << nodes[order[j + 1]].first << "," << nodes[order[j + 1]].second << '\n' << endl;

                        std::reverse(order.begin() + i + 1, order.begin() + j + 1);
                        more = true;
                    }
                }
            }
        } while (more);
    }
}
double evaluate(vector<int>& order, vector<vector<int>>& costs) {
    int sum = 0;
    for (int i = 0; i < costs.size(); ++i)//costs.size() - 1 == order.size()
        sum += costs[order[i]][order[i + 1]];
    return sum;
}
void anneal(vector<int>& order, vector<vector<int>>& costs, std::ranlux48_base& ranlux) {
    std::uniform_int_distribution<> swappingRNG(1, order.size() - 2);
    std::uniform_real_distribution<> chooseOptionRNG(0, 1);
    double temperature = 1;//what should this initially be?
    int currentCost = evaluate(order, costs);
    for (int i = 0; i < 600000; ++i) {//tradeoff between iterations, initial temp, and temp decay to fit within time constraint = ???
        int b, a = swappingRNG(ranlux);
        do {
            b = swappingRNG(ranlux);
        } while (b == a);
        std::swap(order[a], order[b]);
        int proposedCost = evaluate(order, costs);
        if (chooseOptionRNG(ranlux) < exp((currentCost - proposedCost) / temperature))
            currentCost = proposedCost;
        else
            std::swap(order[a], order[b]);
        temperature *= .99998;
    }
}
namespace setup {
    void getInputAndSetCostMatrices(vector<vector<int>>& costs, vector<std::pair<int, int>>& nodes) {
        int n;
        cin >> n; cin.ignore();
        nodes.reserve(n);
        costs = vector<vector<int>>(n, vector<int>(n));
        for (int i = 0; i < n; ++i) {
            int x, y;
            cin >> x >> y; cin.ignore();
            nodes.emplace_back(x, y);
        }
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                costs[i][j] = sqrt(pow(nodes[i].first - nodes[j].first, 2) + pow(nodes[i].second - nodes[j].second, 2));
                costs[j][i] = costs[i][j];
            }
        }
    }
    vector<int> antIteration(vector<vector<double>>& weights, std::minstd_rand& rand) {
        vector<int> curOrder;
        curOrder.reserve(weights.size() + 1);
        curOrder.push_back(0);
        vector<int> remaining;
        remaining.reserve(weights.size() - 1);
        for (int i = 1; i < weights.size(); ++i)
            remaining.push_back(i);
        while (remaining.size()) {
            int weightSum = 0;
            for (int i = 0; i < remaining.size(); ++i)
                weightSum += weights[curOrder.back()][remaining[i]];
            std::uniform_int_distribution<> distr(0, weightSum);//[,] not [,)
            int rnd = distr(rand);
            for (int i = 0; i < remaining.size(); ++i) {
                if (rnd < weights[curOrder.back()][remaining[i]]) {
                    curOrder.push_back(remaining[i]);
                    remaining[i] = remaining.back();
                    remaining.pop_back();
                    break;
                }
                rnd -= weights[curOrder.back()][remaining[i]];
            }
        }
        curOrder.push_back(0);
        return curOrder;
    }
    void setInitialOrder(vector<int>& bestOrder, vector<vector<int>>& costs, std::minstd_rand& rand) {
        int bestOrderCost = 2147483647;
        vector<vector<double>> pheromones(costs.size(), vector<double>(costs.size(), 1));
        vector<vector<double>> distWeights(costs.size(), vector<double>(costs.size()));
        for (int i = 0; i < costs.size(); ++i) {
            for (int j = i + 1; j < costs.size(); ++j) {
                distWeights[i][j] = pow(1.0 / costs[i][j], 1.0);//alpha value of??
                distWeights[j][i] = distWeights[i][j];
            }
        }
        cerr << "distWeights:" << endl;
        for (int i = 0; i < distWeights.size(); ++i) {
            for (int j = 0; j < distWeights.size(); ++j)
                fprintf(stderr, "%16f", distWeights[i][j]);
            fprintf(stderr, "\n");
        }
        for (int simBatchNum = 0; simBatchNum < 10; ++simBatchNum) {//<-# = ????
            cerr << "simBatchNum: " << simBatchNum << endl;
            vector<vector<double>> weights(costs.size(), vector<double>(costs.size()));
            for (int weightI = 0; weightI < costs.size(); ++weightI) {
                for (int weightJ = 0; weightJ < costs.size(); ++weightJ)
                    weights[weightI][weightJ] = distWeights[weightI][weightJ] * pow(pheromones[weightI][weightJ], 1.0);//beta value of??
            }
            cerr << "pheromones:" << endl;
            for (int i = 0; i < pheromones.size(); ++i) {
                for (int j = 0; j < pheromones.size(); ++j)
                    fprintf(stderr, "%16f", pheromones[i][j]);
                fprintf(stderr, "\n");
            }
            cerr << "weights:" << endl;
            for (int i = 0; i < weights.size(); ++i) {
                for (int j = 0; j < weights.size(); ++j)
                    fprintf(stderr, "%16f", weights[i][j]);
                fprintf(stderr, "\n");
            }
            for (int simNum = 0; simNum < 10; ++simNum) {//<-# = ????
                vector<int> curOrder = antIteration(weights, rand);
                cerr << "curOrder: ";
                for (int i = 0; i < curOrder.size(); ++i)
                    cerr << curOrder[i] << ' ';
                cerr << endl;
                double curOrderCost = evaluate(curOrder, costs);
                for (int i = 1; i < costs.size(); ++i)
                    pheromones[curOrder[i - 1]][curOrder[i]] += (1.0 / curOrderCost);
                if (curOrderCost < bestOrderCost) {//TODO < vs <=
                    bestOrderCost = curOrderCost;
                    bestOrder = std::move(curOrder);
                }
            }
            for (int i = 0; i < pheromones.size(); ++i) {
                for (int j = 0; j < pheromones.size(); ++j)
                    pheromones[i][j] = pow(pheromones[i][j], .98);//<-# = ????
            }
        }
    }
}
int main() {
    vector<int> order;
    vector<vector<int>> costs;
    vector<std::pair<int, int>> nodes;
    setup::getInputAndSetCostMatrices(costs, nodes);
    std::minstd_rand rand(std::random_device{}());
    setup::setInitialOrder(order, costs, rand);
    //intersections::uncrossEdges(order, nodes);
    //anneal(order, costs, rand);
    //intersections::uncrossEdges(order, nodes);
    for (int i = 0; i < order.size() - 1; ++i)
        cout << order[i] << ' ';
    cout << '0' << endl;
}