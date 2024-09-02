#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <map>

#define MAX_ITERS 100

uint64_t make_number(std::string word) {
	uint64_t number = 0;
	uint64_t multiplier = 1;
	for (char c : word) {
		number += (c - 'a') * multiplier;
		multiplier *= 256;
	}
	return number;
}

std::string make_word(uint64_t number, size_t wordLength) {
	std::string word;
	for (int i = 0; i < wordLength; ++i) {
		word += (number & 0xFF) + 'a';
		number >>= 8;
	}
	return word;
}

std::unordered_map<size_t, std::vector<uint64_t>> readDictionary(std::string file_path)
{
	std::unordered_set<char> annotations{ ':', '&', '#', '=', '<', '^', '~', '+' };
	std::unordered_map<size_t, std::vector<uint64_t>> dictionaryMap;

	std::ifstream dictIn(file_path, std::ios_base::in);
	if (!dictIn) {
		std::cerr << "Error: Could not open the file at " << file_path << std::endl;
		return dictionaryMap;
	}

	for (std::string word; std::getline(dictIn, word);) {
		size_t wordLength = word.length();
		if (!word.empty() && annotations.find(word.back()) != annotations.end())
			word.pop_back();
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		dictionaryMap[wordLength].push_back(std::move(make_number(word)));
	}

	return dictionaryMap;
};

void addPairTable(std::unordered_map<size_t, std::unordered_set<uint64_t>> *pairTables, size_t wordLength) {
	std::unordered_set<uint64_t> pairTable;
	for (int i = 0; i < wordLength; ++i) {
		for (int j = 0; j < 32; ++j) {
			pairTable.insert(static_cast<uint64_t>(j) << (i * 8));
		}
	}
	(*pairTables)[wordLength] = pairTable;
}

void mainLoop() {
	std::unordered_map<size_t, std::vector<uint64_t>> dictionaries = readDictionary("dictionary.txt");

	std::unordered_map<size_t, std::unordered_set<uint64_t>> pairTables;

	std::cout << "From word: ";

	std::string fromWord;

	std::getline(std::cin, fromWord);
	std::transform(fromWord.begin(), fromWord.end(), fromWord.begin(), ::tolower);
	uint64_t fromNumber = make_number(fromWord);

	std::cout << "To word: ";

	std::string toWord;

	std::getline(std::cin, toWord);
	std::transform(toWord.begin(), toWord.end(), toWord.begin(), ::tolower);
	uint64_t toNumber = make_number(toWord);

	if (fromWord.length() != toWord.length()) {
		std::cout << "Uncorresponding lengths\n";
		return;
	}

	size_t wordLength = fromWord.length();

	auto currentDictionary = dictionaries[wordLength];

	if (!pairTables.count(wordLength)) addPairTable(&pairTables, wordLength);

	auto currentPairTable = pairTables[wordLength];

	if (fromNumber != 0 && std::find(currentDictionary.begin(), currentDictionary.end(), fromNumber) == currentDictionary.end()) {
		std::cout << "No connections to " + fromWord + "\n";
		return;
	}

	if (std::find(currentDictionary.begin(), currentDictionary.end(), toNumber) == currentDictionary.end()) {
		std::cout << "No connections to " + toWord + "\n";
		return;
	}

	std::map<uint64_t, uint64_t> connections;
	std::map<uint64_t, int> dist;
	for (uint64_t word : currentDictionary) {
		dist[word] = -1;
	}
	dist[toNumber] = 0;

	bool wordFound = false;
	for (int iter = 0; iter < MAX_ITERS; ++iter) {
		bool madeChanges = false;
		for (uint64_t word1 : currentDictionary) {
			if (dist[word1] != iter) {
				if (wordFound) break;
				continue;
			};
			for (uint64_t word2 : currentDictionary) {
				if (dist[word2] != -1) continue;
				if (currentPairTable.find(word1 ^ word2) == currentPairTable.end()) continue;
				dist[word2] = iter + 1;
				connections[word2] = word1;
				madeChanges = true;
				if (word2 == fromNumber) {
					std::cout << "Found!\n";
					wordFound = true;
				}
			}
		}
		if (wordFound || (!madeChanges)) break;
	}

	if (fromNumber != 0) {
		if (connections.find(fromNumber) == connections.end()) {
			std::cout << "Cannot connect.\n";
		}
		else {
			uint64_t word = fromNumber;
			while (true) {
				std::cout << make_word(word, wordLength) << "\n";
				if (word == toNumber) break;
				word = connections[word];
			}
			std::cout << dist[fromNumber] << " steps\n";
		}
	}
	else {
		for (uint64_t word : currentDictionary) {
			if (dist[word] > 0) {
				std::cout << make_word(word, wordLength) << " in " << dist[word] << " steps\n";
			}
		}
	}
}

int main()
{
	while (true) mainLoop();
	return 0;
}	