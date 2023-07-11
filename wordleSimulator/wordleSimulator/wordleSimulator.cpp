#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <unordered_map>

using std::endl; using std::cout; using std::cerr; using std::string; using std::vector; using std::cin; using std::list;

int main(){
	std::ifstream wordFile("C:\\Users\\Quasar\\source\\repos\\codinGame\\wordleSimulator\\wordListEntirety.txt");
	vector<string> wordList;
	string line;
	std::ofstream outFile("C:\\Users\\Quasar\\source\\repos\\codinGame\\wordleSimulator\\wordList.txt", std::fstream::trunc);
	while (wordFile >> line) {
		if(line[0] != 'Z')
			outFile << line << '\n';
	}
	outFile.close();
		//for (int i = 0; i < 500; ++i, wordFile >> line)
		//wordList.push_back(line);
	
	//for (int i = 0; i < wordList.size(); ++i) {
		//string secret = wordList[i];






	//}
	//cout << wordList.size() << endl;

	/*
	srand(time(0));
	line = wordList[rand() % 9935];
	string input;
	cout << "00-00-00-00-00-00" << endl;
	cout << line << endl;
	for(int turn = 0; turn < 10; ++turn) {
		cin >> input; cin.ignore();
		std::transform(input.begin(), input.end(), input.begin(), ::toupper);
		int state[6]{};
		for (int i = 0; i < 6; ++i) {
			cerr << i ;
			if (line[i] == input[i]) {
				state[i] = 3;
				continue;
			}
			for (int j = 0; j < 6; ++j) {
				if (line[j] == input[i]) {
					state[i] = 2;
					break;
				}
			}
			if (!state[i])
				state[i] = 1;
			
			cout << input[i] << state[i] << ' ';
		}
		cout << endl;
		bool win = true;
		for (int i = 0; i < 6; ++i) {
			if (input[i] != line[i])
				win = false;
		}
		if (win) {
			cout << "yay" << endl;
		}
	}
	*/
}