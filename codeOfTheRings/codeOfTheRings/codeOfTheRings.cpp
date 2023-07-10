/*lengths:


24: 383

validators:

<380
*/

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

using std::cerr; using std::endl;

//declarations
#pragma warning( push )
#pragma warning( disable : 26495 )
struct outChar {
    int c;// , partOf;//potentially unneeded, related calls kept in comment for now
    outChar* next = nullptr;
    int curRune;
    int r[30];
    ~outChar() { delete next; }
    outChar(const outChar& x) = delete;//RIF, here only to act as compiler warning
    outChar& operator=(const outChar&) = delete;//RIF, here only to act as compiler warning
};
struct outList {
    int size = 0;//not including head
    outChar *head, *tail;
    outList() {
        head = new outChar{};
        tail = head;
    }
    void append(int cIn, int curRuneIn, int* runeArrayIn) {
    //void append(int cIn, int partOfIn, int curRuneIn, int* runeArrayIn) {
        //tail->next = new outChar{ cIn, partOfIn, nullptr };
        tail->next = new outChar{ cIn, nullptr };
        tail = tail->next;
        //fillOutRuneArray(tail->r);
        for (int i = 0; i < 30; ++i)
            tail->r[i] = runeArrayIn[i];
            //tail->r[i].c = runeArrayIn[i].c;
        tail->curRune = curRuneIn;
        //tail->curRune = &tail->r[curRuneIn];
        ++size;
    }
    ~outList() { delete head; }
    outList(const outList& x) = delete;//RIF, here only to act as compiler warning
    outList& operator=(const outList&) = delete;//RIF, here only to act as compiler warning
};
#pragma warning ( pop )
int findWrap(int, int, int);//from, to, exclusive max
int findBestRune(int*, int, int, double, double, double);//working set, from rune, to char, move cost mult, scroll cost mult, mult if space (not multiplicative w/ scroll mult)
void modificationLoopFunction(outList&, std::atomic_bool&);

int main() {
    auto timeStart = std::chrono::steady_clock::now();

    std::vector<int> str;
    {//convert string to ints
        std::string out;
        std::getline(std::cin, out);
        str.reserve(out.length());
        for (int i = 0; i < out.size(); ++i) {
            str.push_back(out[i] - 64);
            if (str[i] == -32)
                str[i] = 0;
        }
    }

    outList out;
    int r[30] = {0};
    int cur = 0;
    for (int i = 0; i < str.size(); ++i) {//loop through int-ized string
        {//move to correct rune
            int moveTo = findBestRune(r, cur, str[i], 1, 1.5, 1);
            int moveBy = findWrap(cur, moveTo, 30);
            for (int j = 0; j < abs(moveBy); ++j)
                //out.append(moveBy > 0, str[i], cur, r);
                out.append(moveBy > 0, cur, r);
            cur = moveTo;//may need to move inside loop for purposes of rune id recording
        }
        {//scroll to correct character
            int scrollBy = findWrap(cur, str[i], 27);
            for (int j = 0; j < abs(scrollBy); ++j)
                //out.append(scrollBy > 0 ? 2 : 3, str[i], cur, r);
                out.append(scrollBy > 0 ? 2 : 3, cur, r);
            cur = str[i];
        }
        {//select char
            //out.append(6, str[i], cur, r);
            out.append(6, cur, r);
        }
    }

    cerr << "Pre: " << endl;
    std::string moveSet = "><+-[].";
    out.tail = out.head->next;
    while (out.tail != nullptr) {
        std::cerr << moveSet[out.tail->c];
        out.tail = out.tail->next;
    }
    std::cerr << std::endl;

    std::atomic_bool exitModificationLoop = false;
    std::thread modificationLoopThread(&modificationLoopFunction, std::ref(out), std::ref(exitModificationLoop));
    std::this_thread::sleep_until(timeStart + std::chrono::milliseconds(2000));
    //exitModificationLoop = true;
    modificationLoopThread.join();
    
    out.tail = out.head->next;
    while (out.tail != nullptr) {
        std::cout << moveSet[out.tail->c];
        out.tail = out.tail->next;
    }
    std::cout << std::endl;
    std::cin >> moveSet;
    return 0;
}

void modificationLoopFunction(outList& out, std::atomic_bool& begone) {
ohBoyHereIGoCountingAgain:
    outChar* oneBeforeStart = out.head;
    int sectionStartPos = 0;
    while (oneBeforeStart->next != nullptr) {//from the left
        if (begone)
            return;
        cerr << "out.size, sectionStartPos: " << out.size << ", " << sectionStartPos << endl;
        for (int sectionSize = 1; sectionSize < out.size - sectionStartPos - 7; ++sectionSize) {//from the left
            outChar* curOutChar = oneBeforeStart->next;
            if (begone)
                return;
            std::vector<int> section;
            section.reserve(sectionSize);
            bool runeUsedInSection[30] = { 0 };
            for (int i = 0; i < sectionSize; ++i) {//record one strip of data
                if (begone)
                    return;
                section.push_back(curOutChar->c);
                if(curOutChar->c > 1)
                    runeUsedInSection[section[i]] = true;
                curOutChar = curOutChar->next;
            }
            int sectionIndex = 0, count = 0;
            while (curOutChar != nullptr) {//compare to further strips
                if (begone)
                    return;
                if (section[sectionIndex] != curOutChar->c)
                    break;
                if (sectionIndex == section.size()) {//make it discrete strips instead of a continuous blob
                    sectionIndex = 0;
                }
                ++count;
                curOutChar = curOutChar->next;
            }
            if (count == 1)
                continue;
            if (count > 26)
                count = 26;
            curOutChar = oneBeforeStart->next;
            int moveCountBefore, plusMinusCount, scrollCountBefore, scrollCountAfter, minCostIndex, cost = 999;
            {
                for (int i = 0; i < 30; ++i) {
                    if (runeUsedInSection[i])
                        continue;

                    int moveCountBeforeInternal = findWrap(curOutChar->curRune, i, 30);

                    int plusMinusCountInternal = 1;//plus if scrollCountBefore > 0
                    int scrollCountBeforeInternal = findWrap(curOutChar->r[i], count, 27); //from char of current rune to Z - (26-count) which just equals count.
                    for (int j = 2; j < 5; ++j) {//j < ???
                        if (count % j)
                            continue;
                        int temp = findWrap(curOutChar->r[j], j * count, 27);
                        if (scrollCountBeforeInternal > temp + j - 1) {
                            plusMinusCountInternal = j;
                            scrollCountBeforeInternal = temp;
                        }
                    }
                    int scrollCountAfterInternal = findWrap(0, curOutChar->r[i], 27);//reset cost; check if rune is used again?

                    int costInternal = 2 + abs(moveCountBeforeInternal) * 2 + plusMinusCountInternal + abs(scrollCountBeforeInternal) + abs(scrollCountAfterInternal);
                    if (costInternal < cost) {
                        cost = costInternal; minCostIndex = i;
                        moveCountBefore = moveCountBeforeInternal; plusMinusCount = plusMinusCountInternal; scrollCountBefore = scrollCountBeforeInternal; scrollCountAfter = scrollCountAfterInternal;
                    }
                }
                cerr << "cost " << cost << endl;
                if (cost >= sectionSize * count)
                    continue;

                cerr << "moveCountBefore " << moveCountBefore << endl;
                cerr << "plusMinusCount " << plusMinusCount << endl;
                cerr << "scrollCountBefore " << scrollCountBefore << endl;
                cerr << "scrollCountAfter " << scrollCountAfter << endl;
                cerr << "minCostIndex " << minCostIndex << endl;
            }

            
            //iterate through nodes, replace chars
            
            /*
            //save pointer to removed branch
            outChar* firstToBeDeleted = curOutChar->next;
            outChar* sacrifice = firstToBeDeleted->next;
            for (int i = 0; i < sectionSize - cost - 2; ++i) {//2???
                sacrifice = sacrifice->next;
            }
            curOutChar->next = sacrifice->next;
            sacrifice->next = nullptr;
            delete firstToBeDeleted;
            */
            //goto ohBoyHereIGoCountingAgain;
        }
        oneBeforeStart = oneBeforeStart->next;
        ++sectionStartPos;
    }
    cerr << "Modification loop actually didn't run out of time" << endl;
}

int findWrap(int from, int to, int max){//max is not inclusive
    if (from == to)
        return 0;
    if (from < to) {
        if (to - from < from + max - to)
            return to - from;
        return -(from + max - to);
    }
    return -findWrap(to, from, max);
}

int findBestRune(int* set, int fromRune, int toChar, double moveMult, double scrollMult, double spaceMult) {
    if (set[fromRune] == toChar)
        return fromRune;
    double minCost = 99;
    int minCostIndex;
    for (int i = 0; i < 30; ++i) {
        //double cost = moveMult * abs(findWrap(fromRune, i, 30)) + (set.r[i].c ? scrollMult : spaceMult) * abs(findWrap(set.r[i].c, toChar, 27));//cost of moving to rune then cost of scrolling that rune
        double cost = findWrap(set[i], toChar, 27);//TODO: experiment w/ fitness formulas
        cost *= cost * .5;
        cost += abs(findWrap(fromRune, i, 30));
        
        if (cost < minCost) {
            minCost = cost;
            minCostIndex = i;
        }
    }
    return minCostIndex;
}

/*int wrap(int i, int lower, int upper) {
    auto helper = [](int j, int n) {
        if (j < 0) { return (n - 1) - (-1 - j) % n; } // -1-i is >=0
        if (j >= n) { return j % n; }
        return j; // In range, no mod
    };
    return lower + helper(i - lower, 1 + upper - lower);
}*/